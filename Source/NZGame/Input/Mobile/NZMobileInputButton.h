// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobile.h"
#include "NZMobileInputButton.generated.h"


/**
 * 
 */
UCLASS()
class NZGAME_API UNZMobileInputButton : public UObject
{
	GENERATED_BODY()
	
public:
	void InitButtonStyle(FVector2D InSize, uint8 InType, bool bInFixed);
	void InitButtonPosition(FVector2D InPosition, uint8 AnchorType = 0, float Scale = 1.0);
	bool InButtonRange(FVector2D Point);
	FVector2D GetSize() { return HalfSize * 2.0; }
	bool IsEnabled() { return bEnabled; }

protected:
	FVector2D Position;
	FVector2D HalfSize;
	EMobileButtonType Type;
	bool bFixed;
	bool bEnabled;
};
