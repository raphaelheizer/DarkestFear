// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DarkestFear : ModuleRules
{
	public DarkestFear(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
