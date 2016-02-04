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
	FTimerHandle RefireCheckHandle;

	inline uint8 GetFireMode() { return GetOuterANZWeapon()->GetCurrentFireMode(); }

	virtual bool IsFiring() const override { return true; }

	/** Called after the refire delay to see what we should do next (generally, fire or go back to active state) */
	virtual void RefireCheckTimer();

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
