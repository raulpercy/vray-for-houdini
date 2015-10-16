//
// Copyright (c) 2015, Chaos Software Ltd
//
// V-Ray For Houdini
//
// Andrei Izrantcev <andrei.izrantcev@chaosgroup.com>
//
// All rights reserved. These coded instructions, statements and
// computer programs contain unpublished information proprietary to
// Chaos Software Ltd, which is protected by the appropriate copyright
// laws and may not be disclosed to third parties or copied or
// duplicated, in whole or in part, without prior written consent of
// Chaos Software Ltd.
//

#include "vop_GeomStaticSmoothedMesh.h"


using namespace VRayForHoudini;


void VOP::GeomStaticSmoothedMesh::setPluginType()
{
	pluginType = "GEOMETRY";
	pluginID   = "GeomStaticSmoothedMesh";
}


OP::VRayNode::PluginResult VOP::GeomStaticSmoothedMesh::asPluginDesc(Attrs::PluginDesc &pluginDesc, VRayExporter *exporter, OP_Node *parent)
{
	PRINT_WARN("OP::GeomStaticSmoothedMesh::asPluginDesc()");

	return OP::VRayNode::PluginResultContinue;
}


void VOP::GeomStaticSmoothedMesh::getCode(UT_String &codestr, const VOP_CodeGenContext &)
{
}