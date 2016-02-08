// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWorldSettings.h"




bool ANZWorldSettings::EffectIsRelevant(AActor* RelevantActor, const FVector& SpawnLocation, bool bSpawnNearSelf, bool bIsLocallyOwnedEffect, float CullDstance, float AlwaysSpawnDist, bool bForceDedicated)
{
    return true;
}
