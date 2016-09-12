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

protected:
	class UNZMobileMovementButtonController* MovementButtonController;
	class UNZMobileFireButtonController* FireButtonController;
	class UNZMobileJumpButtonController* JumpButtonController;
	class UNZMobileCrouchButtonController* CrouchButtonController;
	
};
