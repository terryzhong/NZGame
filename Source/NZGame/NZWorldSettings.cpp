// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWorldSettings.h"




ANZWorldSettings* ANZWorldSettings::GetWorldSettings(UObject* WorldContextObject)
{
    UWorld* World = (WorldContextObject != NULL) ? WorldContextObject->GetWorld() : NULL;
    return (World != NULL) ? Cast<ANZWorldSettings>(World->GetWorldSettings()) : NULL;
}

void ANZWorldSettings::NotifyBeginPlay()
{
    Super::NotifyBeginPlay();
}

void ANZWorldSettings::AddImpactEffect(class USceneComponent* NewEffect, float LifeScaling)
{
    bool bNeedsTiming = true;
    
    // Do some checks for components we can assume will go away on their own
    UParticleSystemComponent* PSC = Cast<UParticleSystemComponent>(NewEffect);
    if (PSC != NULL)
    {
        if (PSC->bAutoDestroy && PSC->Template != NULL && !IsLoopingParticleSystem(PSC->Template))
        {
            bNeedsTiming = false;
        }
    }
    else
    {
        UAudioComponent* AC = Cast<UAudioComponent>(NewEffect);
        if (AC != NULL)
        {
            if (AC->bAutoDestroy && AC->Sound != NULL && AC->Sound->GetDuration() < INDEFINITELY_LOOPING_DURATION)
            {
                bNeedsTiming = false;
            }
        }
    }
    
    if (bNeedsTiming)
    {
        new(TimedEffects) FTimedImpactEffect(NewEffect, GetWorld()->TimeSeconds, LifeScaling);
    }
}


bool ANZWorldSettings::EffectIsRelevant(AActor* RelevantActor, const FVector& SpawnLocation, bool bSpawnNearSelf, bool bIsLocallyOwnedEffect, float CullDstance, float AlwaysSpawnDist, bool bForceDedicated)
{
    return true;
}
