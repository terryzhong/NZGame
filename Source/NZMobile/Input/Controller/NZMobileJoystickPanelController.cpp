// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileJoystickPanelController.h"
#include "NZMobileMovementButtonController.h"
#include "NZMobileFireButtonController.h"


void UNZMobileJoystickPanelController::Initialize()
{
    Super::Initialize();
    
    check(MovementButtonController == NULL);
    MovementButtonController = NewObject<UNZMobileMovementButtonController>(this, UNZMobileMovementButtonController::StaticClass(), TEXT("MovementButtonController"));
    if (MovementButtonController)
    {
        MovementButtonController->Initialize();
    }
    
    check(FireButtonController == NULL);
    FireButtonController = NewObject<UNZMobileFireButtonController>(this, UNZMobileFireButtonController::StaticClass(), TEXT("FireButtonController"));
    if (FireButtonController)
    {
        FireButtonController->Initialize();
    }
    
    
}

