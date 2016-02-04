// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponStateUnequipping.h"
#include "NZWeaponStateUnequippingEnforcer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateUnequippingEnforcer : public UNZWeaponStateUnequipping
{
	GENERATED_BODY()
	
public:
	virtual void BeginState(const UNZWeaponState* PrevState) override;	
};
