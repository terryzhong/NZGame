// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateInactive.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateInactive : public UNZWeaponState
{
	GENERATED_BODY()
	
public:
    virtual void BeginState(const UNZWeaponState* PrevState);
	
    virtual void BringUp(float OverflowTime);
};
