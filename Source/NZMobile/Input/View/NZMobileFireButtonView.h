// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/View/NZMobileJoystickView.h"
#include "NZMobileFireButtonView.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileFireButtonView : public UNZMobileJoystickView
{
	GENERATED_BODY()
	
public:
	UNZMobileFireButtonView();

	virtual void Initialize(class UNZMobileGameController* InController);

	virtual void Update();

protected:
	virtual void UpdateFixedFire(UNZMobilePlayerInput* PlayerInput);

	virtual void UpdateFollowFire(UNZMobilePlayerInput* PlayerInput);

	UWidget* CachedFireButtonBG;
	UWidget* CachedFireButtonDirection;
	UWidget* CachedFireButtonTopDirectionButton;
	UWidget* CachedFireButtonBullet;
};
