// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "NZWorldSettings.generated.h"


USTRUCT()
struct FTimedImpactEffect
{
    GENERATED_BODY()
    
    /** The component */
    UPROPERTY()
    USceneComponent* EffectComp;
    
    /** Time component was added */
    UPROPERTY()
    float CreationTime;
    
    /** Life time scaling */
    UPROPERTY()
    float LifetimeScaling;
    
    UPROPERTY()
    float FadeMultiplier;
    
    FTimedImpactEffect()
    {}
    FTimedImpactEffect(USceneComponent* InComp, float InTime, float InScaling)
        : EffectComp(InComp)
        , CreationTime(InTime)
        , LifetimeScaling(InScaling)
        , FadeMultiplier(1.0f)
    {}
};

/**
 * 
 */
UCLASS()
class NZGAME_API ANZWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = World)
    static ANZWorldSettings* GetWorldSettings(UObject* WorldContextObject);
    
    UPROPERTY()
    TArray<FTimedImpactEffect> TimedEffects;
    
    UPROPERTY()
    TArray<FTimedImpactEffect> FadingEffects;
    
    /** 156 */
    virtual void NotifyBeginPlay() override;
    
    /** Add an impact effect that will be managed by the timing system */
    virtual void AddImpactEffect(class USceneComponent* NewEffect, float LifeScaling = 1.f);
	
    virtual bool EffectIsRelevant(AActor* RelevantActor, const FVector& SpawnLocation, bool bSpawnNearSelf, bool bIsLocallyOwnedEffect, float CullDstance, float AlwaysSpawnDist, bool bForceDedicated = false);
};
