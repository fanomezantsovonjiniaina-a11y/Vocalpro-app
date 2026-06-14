using UnrealBuildTool;

public class SurvivalMadagascar : ModuleRules
{
	public SurvivalMadagascar(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"NavigationSystem",
			"AIModule",
			"UMG",
			"Slate",
			"SlateCore",
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"NetCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Metasound",
			"PhysicsCore"
		});
	}
}
