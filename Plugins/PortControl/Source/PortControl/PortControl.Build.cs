// Some copyright should be here...

using UnrealBuildTool;

public class PortControl : ModuleRules
{
	public PortControl(TargetInfo Target)
	{

        PublicIncludePaths.AddRange(
            new string[] {
                "PortControl/Public"
				
				// ... add public include paths required here ...
			}
            );

        PrivateIncludePaths.AddRange(
			new string[] {
				"PortControl/Private",
				
				// ... add other private include paths required here ...
			}
			);


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Engine",
                "InputCore",
                "Core",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "CoreUObject"
				
				// ... add other public dependencies that you statically link with here ...
			}
            );

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Engine",
                "InputCore",
                "Core",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "CoreUObject"

				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
