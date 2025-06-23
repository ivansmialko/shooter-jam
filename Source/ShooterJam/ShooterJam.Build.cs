// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShooterJam : ModuleRules
{
	public ShooterJam(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "MultiplayerSessions", "OnlineSubsystem", "OnlineSubsystemSteam" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
