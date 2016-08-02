// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZHUDWidget.h"
#include "NZHUDWidget_WeaponCrosshair.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZHUDWidget_WeaponCrosshair : public UNZHUDWidget
{
	GENERATED_BODY()
	
public:
	UNZHUDWidget_WeaponCrosshair();

	virtual bool ShouldDraw_Implementation(bool bShowScores);
		
	virtual void Draw_Implementation(float DeltaTime);
		
};
