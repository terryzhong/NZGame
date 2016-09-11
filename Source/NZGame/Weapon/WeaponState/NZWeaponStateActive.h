// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateActive.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateActive : public UNZWeaponState
{
	GENERATED_BODY()
	
public:
	virtual void BeginState(const UNZWeaponState* PrevState) override;
	virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired) override;
	virtual bool WillSpawnShot(float DeltaTime) override;
};

