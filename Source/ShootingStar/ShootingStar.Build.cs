// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ShootingStar : ModuleRules
{
	public ShootingStar(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "UMG", "AndroidPermission" });
        
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
	        PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
	        AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "AndroidUPL.xml"));
        }
    }
}
