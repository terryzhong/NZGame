// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon/NZWeaponStateFiring.h"
#include "NZDualGunStateFiring.generated.h"

/**
 * 
 */
UCLASS(Within = NZDualGun)
class NZGAME_API UNZDualGunStateFiring : public UNZWeaponStateFiring
{
	GENERATED_BODY()
	
public:
	virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired) override;
	
};
