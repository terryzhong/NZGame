// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponStateFiring.h"
#include "NZWeaponStateFiring_Enforcer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateFiring_Enforcer : public UNZWeaponStateFiring
{
	GENERATED_BODY()
	
public:
	virtual void BeginState(const UNZWeaponState* PrevState) override;
	virtual void EndState() override;

	virtual void UpdateTiming() override;
	
	virtual void PutDown() override;
};
