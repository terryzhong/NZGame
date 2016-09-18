// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/Controller/NZMobileGameController.h"
#include "NZMobileJoystickPanelController.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileJoystickPanelController : public UNZMobileGameController
{
	GENERATED_BODY()
	
public:
    virtual void Initialize(UPlayerInput* InPlayerInput);

	virtual void Update();

protected:
	UPROPERTY()
	class UNZMobileMovementButtonController* MovementButtonController;

	UPROPERTY()
	class UNZMobileRightHandController* RightHandController;

	UPROPERTY()
	class UNZMobileFireButtonController* FireButtonController;

	UPROPERTY()
	class UNZMobileJumpButtonController* JumpButtonController;

	UPROPERTY()
	class UNZMobileCrouchButtonController* CrouchButtonController;
	
};
