// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileGameView.h"


UNZMobileGameView::UNZMobileGameView()
{
	if (CacheWidgetClass != NULL && CacheWidget == NULL)
	{
		CacheWidget = CreateWidget<UUserWidget>(GWorld->GetGameInstance(), CacheWidgetClass);
		CacheWidget->AddToViewport();
	}
}

