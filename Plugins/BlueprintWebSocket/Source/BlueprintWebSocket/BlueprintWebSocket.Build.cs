// Copyright Charmillot Clement 2020. All Rights Reserved.

using UnrealBuildTool;

using System.IO;

public class BlueprintWebSocket : ModuleRules
{
	public BlueprintWebSocket(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"WebSockets"
			}
		);
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
	}
}
