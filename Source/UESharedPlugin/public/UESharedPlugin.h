// © Rollingmedia 2016 all rights reserved

#pragma once

#include "Modules/ModuleManager.h"


class FGeometrySystem;
class UESHAREDPLUGIN_API FUESharedPlugin : public IModuleInterface
{

	/** Called right after the module DLL has been loaded and the module object has been created **/
	virtual void StartupModule() override;

	/** Called before the module is unloaded, right before the module object is destroyed. **/
	virtual void ShutdownModule() override;



	TSharedPtr<class FGeometrySystem, ESPMode::ThreadSafe>   m_geometrySystem;
	void* m_OpenMeshCoreHandle;
	void* m_OpenMeshToolsHandle;





public:
	static FUESharedPlugin* Get();

	class FGeometrySystem* GetGeometrySystem() { return m_geometrySystem.Get(); }



	
};