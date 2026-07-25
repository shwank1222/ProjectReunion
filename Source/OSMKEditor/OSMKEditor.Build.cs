using UnrealBuildTool;

public class OSMKEditor : ModuleRules
{
	public OSMKEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"OSMK"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"Blutility",
			"UMG",
			"Slate",
			"SlateCore",
			"ScriptableEditorWidgets",
			"DeveloperSettings",
			"PropertyEditor",
			"InputCore"
		});
	}
}
