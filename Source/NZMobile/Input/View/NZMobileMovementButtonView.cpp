// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileMovementButtonView.h"


UNZMobileMovementButtonView::UNZMobileMovementButtonView()
{
    static ConstructorHelpers::FObjectFinder<UClass> MovementButtonClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_MovementButton.BP_MobileHUD_MovementButton_C'"));
    CacheWidgetClass = MovementButtonClassRef.Object;
}

