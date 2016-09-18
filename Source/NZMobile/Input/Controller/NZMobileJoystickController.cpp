// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileJoystickController.h"


void UNZMobileJoystickController::Initialize(UPlayerInput* InPlayerInput)
{
    Super::Initialize(InPlayerInput);
    
    InitFromConfig();
}

void UNZMobileJoystickController::InitFromConfig()
{

}

