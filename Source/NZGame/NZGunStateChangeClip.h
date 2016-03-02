// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZGunStateChangeClip.generated.h"

/**
 * 
 */
UCLASS(Within = NZGun)
class NZGAME_API UNZGunStateChangeClip : public UNZWeaponState
{
	GENERATED_BODY()
	
public:
    
    float ChangeClipTimeElapsed;
    
    float ChangeClipTime;
    
    virtual void BeginState(const UNZWeaponState* PrevState) override;
    
    virtual void EndState() override;
    
    FTimerHandle ChangeClipFinishedHandle;
    
    void ChangeClipFinished();
    
    virtual void Tick(float DeltaTime) override;
	
};
