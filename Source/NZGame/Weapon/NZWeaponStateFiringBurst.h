// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponStateFiring.h"
#include "NZWeaponStateFiringBurst.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateFiringBurst : public UNZWeaponStateFiring
{
	GENERATED_BODY()
	
public:
    UNZWeaponStateFiringBurst();
    
    /** Number of shots in a burst */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Burst)
    int32 BurstSize;
    
    /** Interval between shots in a burst */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Burst)
    float BurstInterval;
    
    /** How much spread increases for each shot */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Burst)
    float SpreadIncrease;
    
    /** Current shot since we started firing */
    int32 CurrentShot;
    
    virtual void BeginState(const UNZWeaponState* PrevState) override;
    virtual void UpdateTiming() override;
    
    /** Called after the refire delay to see what we should do next (generally, fire or go back to active state) */
    virtual void RefireCheckTimer();
    
    virtual void Tick(float DeltaTime) override;
    
    /** Sets NextShotTime for the next shot, taking into whether in burst or not */
    virtual void IncrementShotTimer();
    
    /** Allow putdown if between bursts */
    virtual void PutDown() override;
    
protected:
    
    /**
     * Time untill next shot will occur - We have to manually time via Tick() instead of a timer to properly handle overflow with the changing shot intervals as we can't retrieve the overflow from the timer system and changing the interval will clobber that overflow
     */
    float ShotTimeRemaining;
	
	
};
