// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateReloading.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateReloading : public UNZWeaponState
{
	GENERATED_BODY()
	
public:

	float ReloadTimeElapsed;

	float ReloadTime;

	virtual void BeginState(const UNZWeaponState* PrevState) override;

	virtual void EndState() override;

	FTimerHandle ReloadFinishedHandle;

	void ReloadFinished();

	virtual void Tick(float DeltaTime) override;
		
};
