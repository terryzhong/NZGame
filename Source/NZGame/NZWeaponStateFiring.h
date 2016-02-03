// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateFiring.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateFiring : public UNZWeaponState
{
	GENERATED_BODY()
	
	
public:
    /** Pending fire mode on server when equip completes */
    int32 PendingFireSequence;
    
    /** Delay shot one frame to synch with client */
    bool bDelayShot;
    
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired) override;
    
    /** 66
     */
    virtual void PendingFireStarted() {}
	
    virtual void PendingFireStopped() {}
    
    virtual void WeaponBecameInactive() {}
};
