// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Tracy : ModuleRules
{
	public Tracy(ReadOnlyTargetRules Target) : base(Target)
	{
		if (Target.Configuration != UnrealTargetConfiguration.Shipping &&
			Target.Configuration != UnrealTargetConfiguration.Test &&
			(
				Target.Type == TargetType.Game ||
				Target.Type == TargetType.Client ||
				Target.Type == TargetType.Server ||
				Target.Type == TargetType.Editor)
			)
		{
			// TRACY_ENABLE must NOT be defined to disable tracy. Comment this line to disable tracy.
			PublicDefinitions.Add("TRACY_ENABLE=1");
		}

		PublicDefinitions.Add("TRACY_EXPORTS=1");
		PublicDefinitions.Add("TRACY_ON_DEMAND=1");
		//PublicDefinitions.Add("TRACY_NO_CALLSTACK=1");
		PublicDefinitions.Add("TRACY_CALLSTACK=1");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"TracyLib",
			}
		);
	}
}
