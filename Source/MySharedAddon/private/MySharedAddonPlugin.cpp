// © Rollingmedia 2016 all rights reserved
#include "UESharedPlugin.h"
#include "UESharedPluginPrivatePCH.h"
#include"Interfaces\IPluginManager.h"
#include "GeometrySystem.h"

IMPLEMENT_MODULE(FUESharedPlugin, UESharedPlugin)



void FUESharedPlugin::StartupModule()
{
	// Explicitly load the plugin DLL from the plugin directory to save copying it into the binaries directory when packaging.
	//FString path = IPluginManager::Get().FindPlugin("UESharedPlugin")->GetBaseDir() + TEXT("\\") + (OPENMESH_DLL_PATH) + "\\" + TEXT("OpenMeshCore.dll");
	//m_OpenMeshCoreHandle = FPlatformProcess::GetDllHandle(*path);
	//check(m_OpenMeshCoreHandle);

	//path = IPluginManager::Get().FindPlugin("UESharedPlugin")->GetBaseDir() + TEXT("\\") + (OPENMESH_DLL_PATH)+"\\" + TEXT("OpenMeshTools.dll");
	//m_OpenMeshToolsHandle = FPlatformProcess::GetDllHandle(*path);
	//check(m_OpenMeshToolsHandle);



	m_geometrySystem = MakeShareable(new FGeometrySystem());
}


void FUESharedPlugin::ShutdownModule()
{
	/*FPlatformProcess::FreeDllHandle(m_OpenMeshCoreHandle);
	FPlatformProcess::FreeDllHandle(m_OpenMeshToolsHandle);*/
}

FUESharedPlugin* FUESharedPlugin::Get()
{
	static const FName ModuleName = TEXT("UESharedPlugin");
	FUESharedPlugin* OSSModule = FModuleManager::GetModulePtr<FUESharedPlugin>(ModuleName);
	return OSSModule;
}