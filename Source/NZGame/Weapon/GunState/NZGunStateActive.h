// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponStateActive.h"
#include "NZGunStateActive.generated.h"

/**
 * 
 */
UCLASS(Within = NZGun)
class NZGAME_API UNZGunStateActive : public UNZWeaponStateActive
{
	GENERATED_BODY()
	
public:
    virtual void BeginState(const UNZWeaponState* PrevState) override;
};
