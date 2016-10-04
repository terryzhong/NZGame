// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileGameController.h"


void UNZMobileGameController::Initialize(UPlayerInput* InPlayerInput)
{
	check(InPlayerInput);
	CachedPlayerInput = InPlayerInput;

    if (ViewClass)
    {
        View = NewObject<class UNZMobileGameView>(this, ViewClass);
        if (View)
        {
            View->Initialize(this);
        }
    }
}

void UNZMobileGameController::Update()
{
	if (View)
	{
		View->Update();
	}
}
