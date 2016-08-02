// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon/NZGunStateChangeClip.h"
#include "NZDualGunStateLeftHandChangeClip.generated.h"

/**
 * 
 */
UCLASS(Within = NZDualGun)
class NZGAME_API UNZDualGunStateLeftHandChangeClip : public UNZGunStateChangeClip
{
	GENERATED_BODY()
	
public:

	virtual void BeginState(const UNZWeaponState* PrevState) override;

	virtual void EndState() override;

	void ChangeClipFinished();

	virtual void Tick(float DeltaTime) override;

	virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired);

};
