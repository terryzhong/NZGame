// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileInputButton.generated.h"


enum ENZMobileInputButtonType
{
	NZMobileInputButtonType_Circle,
	NZMobileInputButtonType_Rectangle,
	NZMobileInputButtonType_LeftRightButton,
};

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileInputButton : public UObject
{
	GENERATED_BODY()
	
public:
	bool InButtonRange(FVector2D Point)
	{
		switch (Type)
		{
		case NZMobileInputButtonType_Circle:
			break;

		case NZMobileInputButtonType_Rectangle:
			break;

		case NZMobileInputButtonType_LeftRightButton:
			break;

		}
		return false;
	}
	
	FVector2D Position;
	FVector2D Size;
	ENZMobileInputButtonType Type;
	bool bFixed;
	bool bEnabled;
	
};
