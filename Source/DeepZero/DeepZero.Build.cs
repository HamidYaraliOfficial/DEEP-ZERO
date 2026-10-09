using UnrealBuildTool;

public class DeepZero : ModuleRules
{
    public DeepZero(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core","CoreUObject","Engine","InputCore","EnhancedInput",
            "AIModule","NavigationSystem","Slate","SlateCore","UMG",
            "Json","JsonUtilities","Niagara"
        });
        PrivateDependencyModuleNames.AddRange(new[] { "RenderCore","RHI","Projects" });
    }
}
