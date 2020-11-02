// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

using UnrealBuildTool;
using Tools.DotNETCommon;

public class WR : ModuleRules
{
    public string Platform
    {
        get
        {
            string Platform;
            if (Target.Platform == UnrealTargetPlatform.Win64)
                Platform = "Win64";
            else if (Target.Platform == UnrealTargetPlatform.PS4)
                Platform = "PS4";
            else
                Platform = "UnKnown";

            return Platform;
        }
    }

    public string EngineSourceDirectory
    {
        get { return DirectoryReference.GetCurrentDirectory().FullName;  }
    }

    public WR(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        Log.TraceInformation("WR Module Platform: {0}", Platform);

        PrivatePCHHeaderFile = "WRPCH.h";

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "UMG",
            "Slate",
            "SlateCore",
            "RenderCore",
            "NavigationSystem",
            "AIModule",
            "GameplayTasks",
            "Json",
            "JsonUtilities",
            "LevelSequence",
            "MovieScene",
            "Landscape",
            "Foliage",
            "Sockets",
            "Networking",
            "MediaAssets",
            "WebSockets",
            "Niagara",
            "AudioMixer",
            "AudioCapture",
            "ProceduralMeshComponent",
            "Http",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Voice",
            "RHI",
            "PhysXVehicles",
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "WRScreenFader", "CableComponent", "WRLevelLoader", "Engine", });

        if (Target.Type == TargetType.Editor)
        {
            Log.TraceInformation("WR Module Editor Build");
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "AnimGraph", "AssetManagerEditor", "EditorScriptingUtilities" });

            PublicIncludePaths.AddRange(new string[] {

                EngineSourceDirectory + "/Editor/DetailCustomizations/Private",

            });
        }

        if (Target.Platform != UnrealTargetPlatform.PS4)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "OculusHMD", "RHI", "OpenVR" });
        }
        else
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Morpheus", "MorpheusFunctionLibrary" });
        }

//         Log.WriteLine(LogEventType.Log, ModuleDirectory);
// 
//         bLegacyPublicIncludePaths = false;
//         PublicIncludePaths.AddRange(new string[] {
// 
//             ModuleDirectory,
//             EngineSourceDirectory + "/Runtime/CoreUObject/Public/UObject",
//             EngineSourceDirectory + "/Runtime/SlateCore/Public/Layout",
//             EngineSourceDirectory + "/Runtime/Core/Public/Containers",
//             EngineSourceDirectory + "/Runtime/Core/Public/Templates",
//             EngineSourceDirectory + "/Runtime/Core/Public/Delegates",
//             EngineSourceDirectory + "/Runtime/Core/Public/Windows",
//             EngineSourceDirectory + "/Runtime/SlateCore/Public/Input",
//             EngineSourceDirectory + "/Runtime/NavigationSystem/Public/NavMesh",
//             EngineSourceDirectory + "/Runtime/Core/Public/Misc",
//             EngineSourceDirectory + "/Runtime/Slate/Public/Widgets/Input",
//             EngineSourceDirectory + "/Runtime/Core/Public/Modules",
//             EngineSourceDirectory + "/Runtime/Core/Public/Math",
// 
//             });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
