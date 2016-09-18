// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileInputConfig.generated.h"

USTRUCT()
struct FInputButtonConfig
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

	FInputButtonConfig()
	{
		Width = 0;
		Height = 0;
		Type = 0;
		bFixed = false;
	}
};

USTRUCT()
struct FJoystickButtonConfig
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
	bool bLeftSide;

	UPROPERTY()
	bool bUpSide;

	UPROPERTY()
	bool bHorizontalLimit;

	UPROPERTY()
	bool bVerticalLimit;

	FJoystickButtonConfig()
	{
		X = 0;
		Y = 0;
		Scale = 1.0;
		MinScale = 0.6;
		MaxScale = 2.0;
		Alpha = 1.0;
		bLeftSide = true;
		bUpSide = true;
		bHorizontalLimit = false;
		bVerticalLimit = false;
	}
};

USTRUCT()
struct FInputConfig
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FInputButtonConfig MovementButton;

	UPROPERTY()
	FInputButtonConfig RotationButton;

	UPROPERTY()
	FInputButtonConfig FireButton;

	UPROPERTY()
	FInputButtonConfig JumpButton;

	UPROPERTY()
	FInputButtonConfig CrouchButton;
};

USTRUCT()
struct FJoystickConfig
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FJoystickButtonConfig MovementButton;

	UPROPERTY()
	FJoystickButtonConfig RotationButton;

	UPROPERTY()
	FJoystickButtonConfig FixedFireButton;

	UPROPERTY()
	FJoystickButtonConfig FollowFireButton;

	UPROPERTY()
	FJoystickButtonConfig MoveFireButton;

	UPROPERTY()
	FJoystickButtonConfig JumpButton;

	UPROPERTY()
	FJoystickButtonConfig CrouchButton;

	UPROPERTY()
	FJoystickButtonConfig RightHand;
};

/**
 * 
 */
UCLASS(Config = MobileInput)
class NZMOBILE_API UNZMobileInputConfig : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)
	FJoystickConfig DefaultJoystickConfig;

	UPROPERTY(Config)
	FJoystickConfig CustomJoystickConfig;

	UPROPERTY(Config)
	TArray<FInputConfig> InputConfigList;
	
};
