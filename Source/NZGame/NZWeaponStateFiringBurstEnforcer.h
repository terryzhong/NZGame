// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponStateFiringBurst.h"
#include "NZWeaponStateFiringBurstEnforcer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateFiringBurstEnforcer : public UNZWeaponStateFiringBurst
{
	GENERATED_BODY()
	
public:
    UNZWeaponStateFiringBurstEnforcer();
    
    virtual void BeginState(const UNZWeaponState* PrevState) override;
    virtual void EndState() override;
    virtual void IncrementShotTimer() override;
    virtual void UpdateTiming() override;
    virtual void RefireCheckTimer() override;
    virtual void PutDown() override;
    virtual float GetRemainingCooldownTime();
    virtual void Tick(float DeltaTime) override {}
    virtual void ResetTiming();

    bool bFirstVolleyComplete;
    bool bSecondVolleyComplete;
    
private:
    float LastFiredTime;	
	
};
