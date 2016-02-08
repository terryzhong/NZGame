// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "NZWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
    
	
    virtual bool EffectIsRelevant(AActor* RelevantActor, const FVector& SpawnLocation, bool bSpawnNearSelf, bool bIsLocallyOwnedEffect, float CullDstance, float AlwaysSpawnDist, bool bForceDedicated = false);
};
