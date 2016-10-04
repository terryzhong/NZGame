// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileJumpButtonView.h"


UNZMobileJumpButtonView::UNZMobileJumpButtonView()
{
	static ConstructorHelpers::FObjectFinder<UClass> JumpButtonClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_JumpButton.BP_MobileHUD_JumpButton_C'"));
	CachedWidgetClass = JumpButtonClassRef.Object;
}

