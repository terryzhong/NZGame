// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZGunStateChangeClip.h"
#include "NZDualGunStateChangeClip.generated.h"

/**
 * 
 */
UCLASS(Within = NZDualGun)
class NZGAME_API UNZDualGunStateChangeClip : public UNZGunStateChangeClip
{
	GENERATED_BODY()
	
public:

	virtual void BeginState(const UNZWeaponState* PrevState) override;

	virtual void EndState() override;

	virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired);

};
