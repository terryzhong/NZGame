// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon/NZGunStateActive.h"
#include "NZDualGunStateActive.generated.h"

/**
 * 
 */
UCLASS(Within = NZDualGun)
class NZGAME_API UNZDualGunStateActive : public UNZGunStateActive
{
	GENERATED_BODY()
	
public:
	virtual void BeginState(const UNZWeaponState* PrevState) override;
	
};
