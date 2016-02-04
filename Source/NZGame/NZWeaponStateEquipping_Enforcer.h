// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponStateEquipping.h"
#include "NZWeaponStateEquipping_Enforcer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZWeaponStateEquipping_Enforcer : public UNZWeaponStateEquipping
{
	GENERATED_BODY()
	
public:
	virtual void StartEquip(float OverflowTime) override;	
};
