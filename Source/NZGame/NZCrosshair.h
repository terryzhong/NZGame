// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZCrosshair.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NZGAME_API UNZCrosshair : public UObject
{
	GENERATED_BODY()
	
public:
	UNZCrosshair();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crosshair)
	FCanvasIcon CrosshairIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crosshair)
	FText CrosshairName;
	
	UFUNCTION(BlueprintNativeEvent)
	void DrawCrosshair(UCanvas* Canvas, class ANZWeapon* Weapon, float DeltaTime, float Scale, FLinearColor Color);

	UFUNCTION(BlueprintNativeEvent)
	void DrawPreviewCrosshair(UCanvas* Canvas, class ANZWeapon* Weapon, float DeltaTime, float Scale, FLinearColor Color);
};
