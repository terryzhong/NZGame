// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileMovementButtonController.h"
#include "NZMobileMovementButtonView.h"


UNZMobileMovementButtonController::UNZMobileMovementButtonController()
{
    ViewClass = UNZMobileMovementButtonView::StaticClass();
}

void UNZMobileMovementButtonController::Initialize(UPlayerInput* InPlayerInput)
{
    Super::Initialize(InPlayerInput);
}
