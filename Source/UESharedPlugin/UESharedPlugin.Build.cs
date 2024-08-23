// © Rollingmedia 2016 all rights reserved

using UnrealBuildTool;
using System.IO;

public class UESharedPlugin : ModuleRules
{
    private string ModulePath { get { return ModuleDirectory; } }
	
	private string ThirdPartyPath { get { return Path.GetFullPath( Path.Combine( ModulePath, "../ThirdParty") ); } }
	
	public UESharedPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        //bUseRTTI = true;

        PrivateIncludePaths.Add("UESharedPlugin/Private");
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "ApplicationCore",
            "Engine",
            "InputCore",
            "InputDevice",
            "SlateCore",
            "UMG",
            "Projects",
            "ProceduralMeshComponent",
        }) ;

        if ((Target.Platform == UnrealTargetPlatform.Win64) )//|| (Target.Platform == UnrealTargetPlatform.Win32))
        {
            //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            //string LibrariesPath = Path.Combine(ThirdPartyPath, "OpenMesh-8.1", "lib");
            //string DllPath = Path.Combine( ThirdPartyPath, "OpenMesh-8.1");

            //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "OpenMeshCore.lib"));
            //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "OpenMeshTools.lib"));
           
            //RuntimeDependencies.Add(Path.Combine(DllPath, "OpenMeshCore.dll"));
            //RuntimeDependencies.Add(Path.Combine(DllPath, "OpenMeshTools.dll"));
            //// Let the engine know to defer loading of the SDL DLL, as it will be performed explicitly during module initialisation.
            //// By manually loading the DLL from the plugin's folder later, the need to copy the DLL into the binaries directories is removed.
            //PublicDelayLoadDLLs.Add("OpenMeshCore.dll");
            //PublicDelayLoadDLLs.Add("OpenMeshTools.dll");

            //PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "OpenMesh-8.1", "include"));

            //Definitions.Add("OPENMESH_DLL_PATH=" + "\"" + DllPath.Replace(@"\", @"/") + "\"");
        }
        else
        {

        }
    }
}
