// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileCrouchButtonView.h"


UNZMobileCrouchButtonView::UNZMobileCrouchButtonView()
{
	static ConstructorHelpers::FObjectFinder<UClass> CrouchButtonClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_CrouchButton.BP_MobileHUD_CrouchButton_C'"));
	CachedWidgetClass = CrouchButtonClassRef.Object;
}

