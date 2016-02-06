// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateFiring.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class NZGAME_API UNZWeaponStateFiring : public UNZWeaponState
{
	GENERATED_BODY()
	
public:
    UNZWeaponStateFiring();
    
	FTimerHandle RefireCheckHandle;

	inline uint8 GetFireMode() { return GetOuterANZWeapon()->GetCurrentFireMode(); }

    virtual bool IsFiring() const override;

    /** Returns true if weapon will fire a shot this frame - used to network synchronization */
    virtual bool WillSpawnShot(float DeltaTime);
    
    /** Called to fire the shot and consume ammo */
    virtual void FireShot();
    
    virtual void BeginState(const UNZWeaponState* PrevState) override;
    virtual void EndState() override;
    virtual void UpdateTiming() override;
    
	/** Called after the refire delay to see what we should do next (generally, fire or go back to active state) */
	virtual void RefireCheckTimer();
    
    virtual void PutDown() override;
    
    virtual void Tick(float DeltaTime) override;
    
    /** If bDelayedShot, fire */
    virtual void HandleDelayedShot();

    /** Pending fire mode on server when equip completes */
    int32 PendingFireSequence;
    
    /** Delay shot one frame to synch with client */
    bool bDelayShot;
    
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired) override;
    
    /**
     * Called when the owner starts a pending fire for this mode (pressed button but may or may not move to this state yet)
     * This function is called even if another fire mode is in use
     * NOTE: the weapon is *not* in this state! This is intended for modes that can do something while another mode is active (e.g. zooming)
     */
    virtual void PendingFireStarted() {}
	
    virtual void PendingFireStopped() {}
    
    virtual void WeaponBecameInactive() {}
    
    /**
     * Draw additional HUD displays; Called for all the active weapon's firemodes regardless of firing or not
     * Return whether the weapon's standard crosshair should still be drawn
     */
    virtual bool DrawHUD(class UNZHUDWidget* WeaponHudWidget);
    
    /** Activates or deactivates looping firing effects (sound/anim/etc) that are played for duration of firing state (if weapon has any set) */
    virtual void ToggleLoopingEffects(bool bNowOn);
};
