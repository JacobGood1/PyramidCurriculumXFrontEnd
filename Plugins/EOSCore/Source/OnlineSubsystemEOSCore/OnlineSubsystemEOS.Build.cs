// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class OnlineSubsystemEOS : ModuleRules
{
	public OnlineSubsystemEOS(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/OnlineSubsystemEOSPrivatePCH.h";

        PrivateDefinitions.Add("ONLINESUBSYSTEMEOS_PACKAGE=1");

        PublicDependencyModuleNames.AddRange(
			new string[] {
				"OnlineSubsystemUtils"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Sockets",
				"OnlineSubsystem",
				"Json",
                "EOSLibrary",
                "Projects"
			}
		);

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
        }

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        if (Target.Version.MinorVersion >= 26)
        {
            PrivateDependencyModuleNames.Add("DeveloperSettings");
        }
    }
}
