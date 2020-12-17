/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

using System.IO;
using UnrealBuildTool;

public class EOSLibrary : ModuleRules
{
	public EOSLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

        string EOSPath = ModuleDirectory;

        PublicIncludePaths.Add(Path.Combine(EOSPath, "Include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            string EOSLibPath = Path.Combine(EOSPath, "Lib");
            PublicAdditionalLibraries.Add(Path.Combine(EOSLibPath, "EOSSDK-Win64-Shipping.lib"));

            PublicDelayLoadDLLs.Add("EOSSDK-Win64-Shipping.dll");

            string EOSDLLPath = Path.Combine(EOSPath, "Bin", "EOSSDK-Win64-Shipping.dll");
            RuntimeDependencies.Add(EOSDLLPath);
        }
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
            string SoPath = Path.Combine(EOSPath, "Bin");
            string SoName = Path.Combine(EOSPath, "Bin", "libEOSSDK-Linux-Shipping.so");

            PrivateRuntimeLibraryPaths.Add(SoPath);

            PublicAdditionalLibraries.Add(SoName);

            RuntimeDependencies.Add(SoName);
        }
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
            string DylibName = Path.Combine(EOSPath, "Bin", "libEOSSDK-Mac-Shipping.dylib");

            PublicDelayLoadDLLs.Add(DylibName);

            RuntimeDependencies.Add(DylibName);
        }
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
            AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(EOSPath, "LibEOSCore_APL_IOS.xml"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicAdditionalLibraries.Add(Path.Combine(EOSPath, "Lib", "arm64-v8a", "libEOSSDK.so"));
            PublicAdditionalLibraries.Add(Path.Combine(EOSPath, "Lib", "armeabi-v7a", "libEOSSDK.so"));

            AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(EOSPath, "LibEOSCore_APL.xml")));
        }
	}
}
