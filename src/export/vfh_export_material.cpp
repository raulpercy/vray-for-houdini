//
// Copyright (c) 2015-2016, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#include "vfh_exporter.h"
#include "vop/material/vop_mtl_def.h"

#include <SHOP/SHOP_Node.h>
#include <VOP/VOP_ParmGenerator.h>
#include <OP/OP_Options.h>


using namespace VRayForHoudini;


void VRayExporter::RtCallbackSurfaceShop(OP_Node *caller, void *callee, OP_EventType type, void *data)
{
	VRayExporter &exporter = *reinterpret_cast<VRayExporter*>(callee);

	Log::getLog().info("RtCallbackSurfaceShop: %s from \"%s\"",
					   OPeventToString(type), caller->getName().buffer());

	if (type == OP_INPUT_REWIRED && caller->error() < UT_ERROR_ABORT) {
		UT_String inputName;
		const int idx = reinterpret_cast<long>(data);
		caller->getInputName(inputName, idx);

		if (inputName.equal("Material")) {
			SHOP_Node *shop_node = caller->getParent()->castToSHOPNode();
			if (shop_node) {
				UT_String shopPath;
				shop_node->getFullPath(shopPath);

				exporter.exportMaterial(*shop_node);
			}
		}
	}
	else if (type == OP_NODE_PREDELETE) {
		exporter.delOpCallback(caller, VRayExporter::RtCallbackSurfaceShop);
	}
}


VRay::Plugin VRayExporter::exportMaterial(SHOP_Node &shop_node)
{
	VRay::Plugin material;
	UT_ValArray<OP_Node *> mtlOutList;
	if ( shop_node.getOpsByName("vray_material_output", mtlOutList) ) {
		// there is at least 1 "vray_material_output" node so take the first one
		VOP::MaterialOutput *mtlOut = static_cast< VOP::MaterialOutput * >( mtlOutList(0) );
		addOpCallback(mtlOut, VRayExporter::RtCallbackSurfaceShop);

		if (mtlOut->error() < UT_ERROR_ABORT ) {
			Log::getLog().info("Exporting material output \"%s\"...",
							   mtlOut->getName().buffer());

			const int idx = mtlOut->getInputFromName("Material");
			OP_Node *inpNode = mtlOut->getInput(idx);
			if (inpNode) {
				VOP_Node *vopNode = inpNode->castToVOPNode();
				if (vopNode) {
					switch (mtlOut->getInputType(idx)) {
						case VOP_SURFACE_SHADER: {
							material = exportVop(vopNode);
							break;
						}
						case VOP_TYPE_BSDF: {
							VRay::Plugin pluginBRDF = exportVop(vopNode);

							// Wrap BRDF into MtlSingleBRDF for RT GPU to work properly
							Attrs::PluginDesc mtlPluginDesc(VRayExporter::getPluginName(vopNode, "Mtl"), "MtlSingleBRDF");
							mtlPluginDesc.addAttribute(Attrs::PluginAttr("brdf", pluginBRDF));

							material = exportPlugin(mtlPluginDesc);
							break;
						}
						default:
							Log::getLog().error("Unsupported input type for node \"%s\", input %d!",
												mtlOut->getName().buffer(), idx);
					}

					if (material && isIPR()) {
						// Wrap material into MtlRenderStats to always have the same material name
						// Used when rewiring materials when running interactive RT session
						Attrs::PluginDesc pluginDesc(VRayExporter::getPluginName(&shop_node, "Mtl"), "MtlRenderStats");

						pluginDesc.addAttribute(Attrs::PluginAttr("base_mtl", material));
						material = exportPlugin(pluginDesc);
					}
				}
			}
		}
	}
	else {
		Log::getLog().error("Can't find \"V-Ray Material Output\" operator under \"%s\"!",
							shop_node.getName().buffer());
	}

	if ( NOT(material) ) {
		material = exportDefaultMaterial();
	}

	return material;
}


VRay::Plugin VRayExporter::exportDefaultMaterial()
{
	Attrs::PluginDesc brdfDesc("BRDFDiffuse@Clay", "BRDFDiffuse");
	brdfDesc.addAttribute(Attrs::PluginAttr("color", 0.5f, 0.5f, 0.5f));

	Attrs::PluginDesc mtlDesc("Mtl@Clay", "MtlSingleBRDF");
	mtlDesc.addAttribute(Attrs::PluginAttr("brdf", exportPlugin(brdfDesc)));

	return exportPlugin(mtlDesc);
}


void VRayExporter::setAttrsFromSHOPOverrides(Attrs::PluginDesc &pluginDesc, VOP_Node &vopNode)
{
	OP_Network *creator = vopNode.getCreator();
	if (NOT(creator)) {
		return;
	}

	const Parm::VRayPluginInfo *pluginInfo = Parm::GetVRayPluginInfo( pluginDesc.pluginID );
	if (!pluginInfo) {
		return;
	}

	const fpreal t = m_context.getTime();

	VOP_ParmGeneratorList prmVOPs;
	vopNode.getParmInputs(prmVOPs);
	for (VOP_ParmGenerator *prmVOP : prmVOPs) {
		int inpidx = vopNode.whichInputIs(prmVOP);
		if (inpidx < 0) {
			continue;
		}

		UT_String inpName;
		vopNode.getInputName(inpName, inpidx);
		const std::string attrName = inpName.toStdString();
		// plugin doesn't have such attribute or
		// it has already been exported
		if (   NOT(pluginInfo->attributes.count(attrName))
			|| pluginDesc.contains(attrName) )
		{
			continue;
		}

		UT_String prmToken = prmVOP->getParmNameCache();
		const PRM_Parm *prm = creator->getParmList()->getParmPtr(prmToken);
		// no such parameter on the parent node or
		// parameter is not floating point
		if (   NOT(prm)
			|| NOT(prm->getType().isFloatType()) )
		{
			continue;
		}

		const Parm::AttrDesc &attrDesc = pluginInfo->attributes.at(attrName);
		switch (attrDesc.value.type) {
			case Parm::eBool:
			case Parm::eEnum:
			case Parm::eInt:
			case Parm::eTextureInt:
			{
				Attrs::PluginDesc mtlOverrideDesc(VRayExporter::getPluginName(&vopNode, attrName), "TexUserScalar");
				mtlOverrideDesc.addAttribute(Attrs::PluginAttr("default_value", creator->evalInt(prm, 0, t)));
				mtlOverrideDesc.addAttribute(Attrs::PluginAttr("user_attribute", prm->getToken()));

				VRay::Plugin overridePlg = exportPlugin(mtlOverrideDesc);
				pluginDesc.addAttribute(Attrs::PluginAttr(attrName, overridePlg, "scalar"));

				break;
			}
			case Parm::eFloat:
			case Parm::eTextureFloat:
			{
				Attrs::PluginDesc mtlOverrideDesc(VRayExporter::getPluginName(&vopNode, attrName), "TexUserScalar");
				mtlOverrideDesc.addAttribute(Attrs::PluginAttr("default_value", creator->evalFloat(prm, 0, t)));
				mtlOverrideDesc.addAttribute(Attrs::PluginAttr("user_attribute", prm->getToken()));

				VRay::Plugin overridePlg = exportPlugin(mtlOverrideDesc);
				pluginDesc.addAttribute(Attrs::PluginAttr(attrName, overridePlg, "scalar"));

				break;
			}
			case Parm::eColor:
			case Parm::eAColor:
			case Parm::eTextureColor:
			{
				Attrs::PluginDesc mtlOverrideDesc(VRayExporter::getPluginName(&vopNode, attrName), "TexUserColor");

				Attrs::PluginAttr attr("default_color", Attrs::PluginAttr::AttrTypeAColor);
				for (int i = 0; i < std::min(prm->getVectorSize(), 4); ++i) {
					attr.paramValue.valVector[i] = creator->evalFloat(prm, i, t);
				}
				mtlOverrideDesc.addAttribute(attr);
				mtlOverrideDesc.addAttribute(Attrs::PluginAttr("user_attribute", prm->getToken()));

				VRay::Plugin mtlOverridePlg = exportPlugin(mtlOverrideDesc);
				pluginDesc.addAttribute(Attrs::PluginAttr(attrName, mtlOverridePlg, "color"));

				break;
			}
			case Parm::eVector:
			{
				VRay::Vector v;
				for (int i = 0; i < std::min(3, prm->getVectorSize()); ++i) {
					v[i] = creator->evalFloat(prm->getToken(), i, t);
				}

				pluginDesc.addAttribute(Attrs::PluginAttr(attrName, v));
				break;
			}
			case Parm::eMatrix:
			{
				VRay::Matrix m(1);

				for (int k = 0; k < std::min(9, prm->getVectorSize()); ++k) {
					const int i = k / 3;
					const int j = k % 3;
					m[i][j] = creator->evalFloat(prm->getToken(), k, t);
				}

				pluginDesc.addAttribute(Attrs::PluginAttr(attrName, m));
				break;
			}
			case Parm::eTransform:
			{
				VRay::Transform tm(1);

				for (int k = 0; k < std::min(16, prm->getVectorSize()); ++k) {
					const int i = k / 4;
					const int j = k % 4;
					if (i < 3) {
						if (j < 3) {
							tm.matrix[i][j] = creator->evalFloat(prm->getToken(), k, t);
						}
					}
					else {
						if (j < 3) {
							tm.offset[j] = creator->evalFloat(prm->getToken(), k, t);
						}
					}
				}

				pluginDesc.addAttribute(Attrs::PluginAttr(attrName, tm));
				break;
			}
			default:
			// ignore other types for now
				;
		}
	}
}
