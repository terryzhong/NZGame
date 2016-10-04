// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileRotationButtonView.h"


UNZMobileRotationButtonView::UNZMobileRotationButtonView()
{
	static ConstructorHelpers::FObjectFinder<UClass> RightHandClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_RotationButton.BP_MobileHUD_RotationButton_C'"));
	CachedWidgetClass = RightHandClassRef.Object;
}
