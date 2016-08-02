// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateUnequipping.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateUnequipping : public UNZWeaponState
{
	GENERATED_BODY()
	
public:

	/** Set to amount of equip time that elapsed when exiting early, i.e. to go back up */
	float PartialEquipTime;
	
	/** Unequip time so far; When this reaches UnequipTime, the weapon change happens manual timer so that we can track overflow and apply it to the weapon being switched to */
	float UnequipTimeElapsed;

	/** Total time to bring down the weapon */
	float UnequipTime;

	virtual void BeginState(const UNZWeaponState* PrevState) override;

	virtual void EndState() override;

	FTimerHandle PutDownFinishedHandle;

	void PutDownFinished();

	virtual void BringUp(float OverflowTime) override;

	virtual void Tick(float DeltaTime) override;
};
