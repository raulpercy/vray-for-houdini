//
// Copyright (c) 2015-2016, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_PRM_JSON_H
#define VRAY_FOR_HOUDINI_PRM_JSON_H

#include "vfh_plugin_info.h"

#include <PRM/PRM_Template.h>

namespace VRayForHoudini {
namespace Parm {

VRayPluginInfo *generatePluginInfo(const std::string &pluginID);
PRMTmplList    *generatePrmTemplate(const std::string &pluginID, const std::string &prefix="");

PRM_Template   *getPrmTemplate(const std::string &pluginID);

} // namespace Parm
} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_PRM_JSON_H
