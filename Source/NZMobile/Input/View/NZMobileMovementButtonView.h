// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/View/NZMobileJoystickView.h"
#include "NZMobileMovementButtonView.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileMovementButtonView : public UNZMobileJoystickView
{
	GENERATED_BODY()
	
public:
    UNZMobileMovementButtonView();
	
	virtual void Initialize(class UNZMobileGameController* InController);

	virtual void Update();

protected:
	virtual void UpdateJoystick(UNZMobilePlayerInput* PlayerInput);

	virtual void UpdateClassics(UNZMobilePlayerInput* PlayerInput);

	UWidget* CachedJoystickBG;
	UWidget* CachedJoystickDirection;
	UWidget* CachedJoystickTopDirectionButton;
	UWidget* CachedJoystickFrontArrow;
	UWidget* CachedJoystickBackArrow;
	UWidget* CachedJoystickLeftArrow;
	UWidget* CachedJoystickRightArrow;

};
