// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class WifiDirect : ModuleRules
{
	public WifiDirect(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "UMG", "AndroidPermission" });
        
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
	        PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
	        AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "AndroidUPL.xml"));
        }
    }
}
