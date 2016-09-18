// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileJoystickPanelController.h"
#include "NZMobileMovementButtonController.h"
#include "NZMobileRightHandController.h"
#include "NZMobileFireButtonController.h"
#include "NZMobileJumpButtonController.h"
#include "NZMobileCrouchButtonController.h"


void UNZMobileJoystickPanelController::Initialize(UPlayerInput* InPlayerInput)
{
    Super::Initialize(InPlayerInput);
    
    check(MovementButtonController == NULL);
    MovementButtonController = NewObject<UNZMobileMovementButtonController>(this, UNZMobileMovementButtonController::StaticClass(), TEXT("MovementButtonController"));
    if (MovementButtonController)
    {
        MovementButtonController->Initialize(InPlayerInput);
    }
    
	check(RightHandController == NULL);
	RightHandController = NewObject<UNZMobileRightHandController>(this, UNZMobileRightHandController::StaticClass(), TEXT("RightHandController"));
	if (RightHandController)
	{
		RightHandController->Initialize(InPlayerInput);
	}
	
	check(FireButtonController == NULL);
    FireButtonController = NewObject<UNZMobileFireButtonController>(this, UNZMobileFireButtonController::StaticClass(), TEXT("FireButtonController"));
    if (FireButtonController)
    {
        FireButtonController->Initialize(InPlayerInput);
    }
    
	check(JumpButtonController == NULL);
	JumpButtonController = NewObject<UNZMobileJumpButtonController>(this, UNZMobileJumpButtonController::StaticClass(), TEXT("JumpButtonController"));
	if (JumpButtonController)
	{
		JumpButtonController->Initialize(InPlayerInput);
	}

	check(CrouchButtonController == NULL);
	CrouchButtonController = NewObject<UNZMobileCrouchButtonController>(this, UNZMobileCrouchButtonController::StaticClass(), TEXT("CrouchButtonController"));
	if (CrouchButtonController)
	{
		CrouchButtonController->Initialize(InPlayerInput);
	}
}

void UNZMobileJoystickPanelController::Update()
{
	if (MovementButtonController)
	{
		MovementButtonController->Update();
	}

	if (RightHandController)
	{
		RightHandController->Update();
	}

	if (FireButtonController)
	{
		FireButtonController->Update();
	}

	if (JumpButtonController)
	{
		JumpButtonController->Update();
	}

	if (CrouchButtonController)
	{
		CrouchButtonController->Update();
	}
}
