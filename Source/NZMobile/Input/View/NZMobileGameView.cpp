// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileGameView.h"


void UNZMobileGameView::Initialize(UNZMobileGameController* InController)
{
    check(InController);
    Controller = InController;
    
	if (CachedWidgetClass != NULL && CachedWidget == NULL)
	{
		CachedWidget = CreateWidget<UUserWidget>(GWorld->GetGameInstance(), CachedWidgetClass);
        if (CachedWidget)
        {
			CachedWidget->AddToViewport();
        }
	}
}

void UNZMobileGameView::Update()
{

}

UPlayerInput* UNZMobileGameView::GetPlayerInput()
{
	check(Controller);
	return Controller->GetPlayerInput();
}
