// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LearnUnreal_56_1 : ModuleRules
{
	public LearnUnreal_56_1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"LearnUnreal_56_1",
			"LearnUnreal_56_1/TopDown",
			"LearnUnreal_56_1/Variant_Strategy",
			"LearnUnreal_56_1/Variant_Strategy/UI",
			"LearnUnreal_56_1/Variant_TwinStick",
			"LearnUnreal_56_1/Variant_TwinStick/AI",
			"LearnUnreal_56_1/Variant_TwinStick/Gameplay",
			"LearnUnreal_56_1/Variant_TwinStick/UI",
			"LearnUnreal_56_1/Flappy",
			"LearnUnreal_56_1/Tetris"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
