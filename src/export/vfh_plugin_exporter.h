//
// Copyright (c) 2015-2016, Chaos Software Ltd
//
// V-Ray For Houdini
//
// ACCESSIBLE SOURCE CODE WITHOUT DISTRIBUTION OF MODIFICATION LICENSE
//
// Full license text: https://github.com/ChaosGroup/vray-for-houdini/blob/master/LICENSE
//

#ifndef VRAY_FOR_HOUDINI_PLUGIN_EXPORTER_H
#define VRAY_FOR_HOUDINI_PLUGIN_EXPORTER_H

#include "vfh_defines.h"
#include "vfh_vray.h"
#include "vfh_log.h"
#include "vfh_plugin_attrs.h"

#include <boost/function.hpp>

namespace VRayForHoudini {

typedef boost::function<void (void)>                                                          CbVoid;
typedef boost::function<void (VRay::VRayRenderer&)>                                           CbOnRendererClose;
typedef boost::function<void (VRay::VRayRenderer&)>                                           CbOnImageReady;
typedef boost::function<void (VRay::VRayRenderer&, VRay::VRayImage*)>                         CbOnRTImageUpdated;
typedef boost::function<void (VRay::VRayRenderer&, int, int, int, int, const char*)>          CbOnBucketInit;
typedef boost::function<void (VRay::VRayRenderer&, int, int, int, int, const char*)>          CbOnBucketFailed;
typedef boost::function<void (VRay::VRayRenderer&, int, int, const char*, VRay::VRayImage*)>  CbOnBucketReady;
typedef boost::function<void (VRay::VRayRenderer&, const char*, int)>                         CbOnDumpMessage;
typedef boost::function<void (VRay::VRayRenderer&, const char*, int, int)>                    CbOnProgress;


template <typename CbT>
struct CbBase
{
	CbBase() {}

	typedef std::vector<CbVoid>  CbVoidArray;
	typedef std::vector<CbT>     CbTypeArray;

	void add(CbT cb)    { m_cbTyped.push_back(cb); }
	void add(CbVoid cb) { m_cbVoid.push_back(cb);  }

	void clear() {
		m_cbVoid.clear();
		m_cbTyped.clear();
	}

	CbVoidArray m_cbVoid;
	CbTypeArray m_cbTyped;

	VfhDisableCopy(CbBase)
};

typedef CbBase<CbOnRendererClose>   CbSetOnRendererClose;
typedef CbBase<CbOnImageReady>      CbSetOnImageReady;
typedef CbBase<CbOnRTImageUpdated>  CbSetOnRTImageUpdated;
typedef CbBase<CbOnBucketInit>      CbSetOnBucketInit;
typedef CbBase<CbOnBucketFailed>    CbSetOnBucketFailed;
typedef CbBase<CbOnBucketReady>     CbSetOnBucketReady;
typedef CbBase<CbOnDumpMessage>     CbSetOnDumpMessage;
typedef CbBase<CbOnProgress>        CbSetOnProgress;


struct CbCollection {
	CbCollection() {}

	void clear() {
		m_cbOnRendererClose.clear();
		m_cbOnImageReady.clear();
		m_cbOnRTImageUpdated.clear();
		m_cbOnBucketInit.clear();
		m_cbOnBucketFailed.clear();
		m_cbOnBucketReady.clear();
		m_cbOnDumpMessage.clear();
		m_cbOnProgress.clear();
	}

	CbSetOnRendererClose   m_cbOnRendererClose;
	CbSetOnImageReady      m_cbOnImageReady;
	CbSetOnRTImageUpdated  m_cbOnRTImageUpdated;
	CbSetOnBucketInit      m_cbOnBucketInit;
	CbSetOnBucketFailed    m_cbOnBucketFailed;
	CbSetOnBucketReady     m_cbOnBucketReady;
	CbSetOnDumpMessage     m_cbOnDumpMessage;
	CbSetOnProgress        m_cbOnProgress;

	VfhDisableCopy(CbCollection)
};


class VRayPluginRenderer
{
public:
	static bool initialize();

public:
	VRayPluginRenderer();
	~VRayPluginRenderer();

	int                           initRenderer(int hasUI, int reInit);
	void                          freeMem();

	VRay::Plugin                  getPlugin(const char *pluginName);

	VRay::Plugin                  exportPlugin(const Attrs::PluginDesc &pluginDesc);
	void                          exportPluginProperties(VRay::Plugin &plugin, const Attrs::PluginDesc &pluginDesc);
	void                          removePlugin(const Attrs::PluginDesc &pluginDesc);
	void                          removePlugin(const std::string &pluginName);
	void                          commit();

	void                          setAnimation(bool on);
	void                          setCurrentTime(fpreal time);
	void                          setCamera(VRay::Plugin camera);
	void                          setRendererMode(int mode);
	void                          setAutoCommit(const bool enable);
	void                          setImageSize(const int w, const int h);

	void                          showVFB(bool show=true);
	void                          clearFrames(float toTime);

	int                           startRender(int locked=false);
	int                           startSequence(int start, int end, int step, int locked=false);
	void                          stopRender();

	int                           exportScene(const std::string &filepath, VRay::VRayExportSettings *settings);

	void                          addCbOnRendererClose(CbOnRendererClose cb)   { m_callbacks.m_cbOnRendererClose.add(cb); }
	void                          addCbOnRendererClose(CbVoid cb)              { m_callbacks.m_cbOnRendererClose.add(cb); }
	void                          addCbOnImageReady(CbOnImageReady cb)         { m_callbacks.m_cbOnImageReady.add(cb); }
	void                          addCbOnImageReady(CbVoid cb)                 { m_callbacks.m_cbOnImageReady.add(cb); }
	void                          addCbOnRTImageUpdated(CbOnRTImageUpdated cb) { m_callbacks.m_cbOnRTImageUpdated.add(cb); }
	void                          addCbOnRTImageUpdated(CbVoid cb)             { m_callbacks.m_cbOnRTImageUpdated.add(cb); }
	void                          addCbOnBucketInit(CbOnBucketInit cb)         { m_callbacks.m_cbOnBucketInit.add(cb); }
	void                          addCbOnBucketInit(CbVoid cb)                 { m_callbacks.m_cbOnBucketInit.add(cb); }
	void                          addCbOnBucketReady(CbOnBucketReady cb)       { m_callbacks.m_cbOnBucketReady.add(cb); }
	void                          addCbOnBucketReady(CbVoid cb)                { m_callbacks.m_cbOnBucketReady.add(cb); }
	void                          addCbOnBucketFailed(CbOnBucketFailed cb)     { m_callbacks.m_cbOnBucketFailed.add(cb); }
	void                          addCbOnBucketFailed(CbVoid cb)               { m_callbacks.m_cbOnBucketFailed.add(cb); }
	void                          addCbOnDumpMessage(CbOnDumpMessage cb)       { m_callbacks.m_cbOnDumpMessage.add(cb); }
	void                          addCbOnDumpMessage(CbVoid cb)                { m_callbacks.m_cbOnDumpMessage.add(cb); }
	void                          addCbOnProgress(CbOnProgress cb)             { m_callbacks.m_cbOnProgress.add(cb); }
	void                          addCbOnProgress(CbVoid cb)                   { m_callbacks.m_cbOnProgress.add(cb); }

	void                          resetCallbacks();

	VRay::VRayRenderer           &getVRay() { return *m_vray; }

private:
	void                          vfbParent(void *parent);

	VRay::VRayRenderer           *m_vray;
	CbCollection                  m_callbacks;
};

} // namespace VRayForHoudini

#endif // VRAY_FOR_HOUDINI_PLUGIN_EXPORTER_H
