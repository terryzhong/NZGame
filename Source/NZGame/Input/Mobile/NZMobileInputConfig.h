// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileInputConfig.generated.h"


enum EMobileButtonType
{
	MobileButtonType_Circle,
	MobileButtonType_Rectangle,
	MobileButtonType_LeftRightButton,
};

USTRUCT()
struct FMobileButtonStyle
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float Width;

	UPROPERTY()
	float Height;

	UPROPERTY()
	uint8 Type;

	UPROPERTY()
	bool bFixed;

	FMobileButtonStyle()
	{
		Width = 0;
		Height = 0;
		Type = 0;
		bFixed = false;
	}
};

USTRUCT()
struct FMobileButton
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float X;

	UPROPERTY()
	float Y;

	UPROPERTY()
	float Scale;

	UPROPERTY()
	float MinScale;

	UPROPERTY()
	float MaxScale;

	UPROPERTY()
	float Alpha;

	UPROPERTY()
	uint8 AnchorType;

	FMobileButton()
	{
		X = 0;
		Y = 0;
		Scale = 1.0;
		MinScale = 0.6;
		MaxScale = 2.0;
		Alpha = 1.0;
		AnchorType = 0;
	}
};

USTRUCT()
struct FMobileButtonStyleConfig
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FMobileButtonStyle MovementButton;

	UPROPERTY()
	FMobileButtonStyle RotationButton;

	UPROPERTY()
	FMobileButtonStyle FireButton;

	UPROPERTY()
	FMobileButtonStyle JumpButton;

	UPROPERTY()
	FMobileButtonStyle CrouchButton;
};

USTRUCT()
struct FMobileButtonConfig
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FMobileButton MovementButton;

	UPROPERTY()
	FMobileButton RotationButton;

	UPROPERTY()
	FMobileButton FixedFireButton;

	UPROPERTY()
	FMobileButton FollowFireButton;

	UPROPERTY()
	FMobileButton MoveFireButton;

	UPROPERTY()
	FMobileButton JumpButton;

	UPROPERTY()
	FMobileButton CrouchButton;

	UPROPERTY()
	FMobileButton RightHand;
};

/**
 * 
 */
UCLASS(Config = Input)
class NZGAME_API UNZMobileInputConfig : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)
	FMobileButtonStyleConfig ButtonStyleConfig;

	UPROPERTY(Config)
	FMobileButtonConfig DefaultButtonConfig;

	UPROPERTY(Config)
	FMobileButtonConfig CustomButtonConfig;
	
};
