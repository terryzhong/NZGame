// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileGameView.h"


void UNZMobileGameView::Initialize(UNZMobileGameController* InController)
{
    check(InController);
    Controller = InController;
    
	if (CacheWidgetClass != NULL && CacheWidget == NULL)
	{
		CacheWidget = CreateWidget<UUserWidget>(GWorld->GetGameInstance(), CacheWidgetClass);
        if (CacheWidget)
        {
            CacheWidget->AddToViewport();
        }
	}
}

