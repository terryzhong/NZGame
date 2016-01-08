// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZInventory.h"
#include "NZWeapon.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZWeapon : public ANZInventory
{
	GENERATED_BODY()

public:
	virtual void BringUp(float OverflowTime = 0.0f);

	virtual bool PutDown();
	
};
