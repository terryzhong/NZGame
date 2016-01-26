// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZWeapon.h"
#include "NZWeaponState.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, CustomConstructor, Within = NZWeapon)
class NZGAME_API UNZWeaponState : public UObject
{
	GENERATED_BODY()
	
public:
    // Accessor for convenience
    inline class ANZCharacter* GetNZOwner()
    {
        return GetOuterANZWeapon()->GetNZOwner();
    }
	
	virtual UWorld* GetWorld() const override
    {
        return GetOuterANZWeapon()->GetWorld();
    }
    
    virtual void BeginState(const UNZWeaponState* PrevState) {}
    virtual void EndState() {}
    
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
    {
        return false;
    }
    virtual void EndFiringSequence(uint8 FireModeNum)
    {
    }
    
    /** Returns true if weapon will fire a shot this frame - used for network synchronization */
    virtual bool WillSpawnShot(float DeltaTime)
    {
        return false;
    }
    
    /**
     * Handle request to bring up
     * Only inactive and equipping states should ever receive this function
     */
    virtual void BringUp(float OverflowTime)
    {
    }
    
    /**
     * Handle request to put down weapon
     * Note that the state can't outright reject putdown (weapon has that authority)
     * but it can delay it until it returns to active by overriding this
     */
    FTimerHandle PutDownHandle;
    virtual void PutDown()
    {
        GetOuterANZWeapon()->UnEquip();
    }
    virtual bool IsFiring() const
    {
        return false;
    }
    /** Update any timers and such due to a weapon timing change; for example, a powerup that changes firing speed */
    virtual void UpdateTiming()
    {
    }
    virtual void Tick(float DeltaTime)
    {
    }
};
