// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class NZGameServerTarget : TargetRules
{
	public NZGameServerTarget(TargetInfo Target)
	{
		Type = TargetType.Server;
    }

    //
    // TargetRules interface.
    //
    public override void SetupBinaries(
        TargetInfo Target,
        ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
        ref List<string> OutExtraModuleNames
        )
    {
        base.SetupBinaries(Target, ref OutBuildBinaryConfigurations, ref OutExtraModuleNames);
        OutExtraModuleNames.Add("NZGame");
        //string LibPath = UEBuildConfiguration.UEThirdPartySourceDirectory + "MCPP/mcpp-2.7.2/lib/";

        //if (Target.Platform == UnrealTargetPlatform.Linux)
        //{
        //    LibPath += ("Win64/VS" + WindowsPlatform.GetVisualStudioCompilerVersionName());
        //    PublicLibraryPaths.Add(LibPath);
        //    PublicAdditionalLibraries.Add("mcpp_64.lib");
        //}
    }

	/*
    public override bool GetSupportedPlatforms(ref List<UnrealTargetPlatform> OutPlatforms)
    {
        // It is valid for only server platforms
        return UnrealBuildTool.UnrealBuildTool.GetAllServerPlatforms(ref OutPlatforms, false);
    }

    public override List<UnrealTargetPlatform> GUBP_GetPlatforms_MonolithicOnly(UnrealTargetPlatform HostPlatform)
    {
        if (HostPlatform == UnrealTargetPlatform.Mac)
        {
            return new List<UnrealTargetPlatform>();
        }
        return new List<UnrealTargetPlatform> { HostPlatform, UnrealTargetPlatform.Win32, UnrealTargetPlatform.Linux };
    }

    public override List<UnrealTargetConfiguration> GUBP_GetConfigs_MonolithicOnly(UnrealTargetPlatform HostPlatform, UnrealTargetPlatform Platform)
    {
        return new List<UnrealTargetConfiguration> { UnrealTargetConfiguration.Development };
    }
	*/
}
