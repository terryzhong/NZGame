// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileRightHandView.h"


UNZMobileRightHandView::UNZMobileRightHandView()
{
	static ConstructorHelpers::FObjectFinder<UClass> RightHandClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_RightHand.BP_MobileHUD_RightHand_C'"));
	CachedWidgetClass = RightHandClassRef.Object;
}

