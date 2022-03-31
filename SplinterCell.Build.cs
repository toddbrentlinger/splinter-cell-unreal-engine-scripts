// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SplinterCell : ModuleRules
{
	public SplinterCell(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule" });
	}
}
