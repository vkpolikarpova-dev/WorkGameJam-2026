// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class gamejam2 : ModuleRules
{
	public gamejam2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"gamejam2",
			"gamejam2/Variant_Platforming",
			"gamejam2/Variant_Platforming/Animation",
			"gamejam2/Variant_Combat",
			"gamejam2/Variant_Combat/AI",
			"gamejam2/Variant_Combat/Animation",
			"gamejam2/Variant_Combat/Gameplay",
			"gamejam2/Variant_Combat/Interfaces",
			"gamejam2/Variant_Combat/UI",
			"gamejam2/Variant_SideScrolling",
			"gamejam2/Variant_SideScrolling/AI",
			"gamejam2/Variant_SideScrolling/Gameplay",
			"gamejam2/Variant_SideScrolling/Interfaces",
			"gamejam2/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
