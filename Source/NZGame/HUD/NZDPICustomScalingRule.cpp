// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDPICustomScalingRule.h"


float UNZDPICustomScalingRule::GetDPIScaleBasedOnSize(FIntPoint Size) const
{
#if PLATFORM_WINDOWS
	return 1.0;
#else
	return Super::GetDPIScaleBasedOnSize(Size);
#endif
}

