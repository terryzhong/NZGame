// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileMovementButtonController.h"
#include "NZMobileMovementButtonView.h"


UNZMobileMovementButtonController::UNZMobileMovementButtonController()
{
    ViewClass = UNZMobileMovementButtonView::StaticClass();
}

void UNZMobileMovementButtonController::Initialize()
{
    Super::Initialize();
}
