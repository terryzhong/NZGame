// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZCrosshair.h"
#include "NZCrosshair_Normal.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZCrosshair_Normal : public UNZCrosshair
{
	GENERATED_BODY()
	
public:
    void DrawCrosshair_Implementation(UCanvas* Canvas, ANZWeapon* Weapon, float DeltaTime, float Scale, FLinearColor Color) override;
	
};
