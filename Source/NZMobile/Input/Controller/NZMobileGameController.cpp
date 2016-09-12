// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileGameController.h"


void UNZMobileGameController::Initialize()
{
    if (ViewClass)
    {
        View = NewObject<class UNZMobileGameView>(this, ViewClass);
        if (View)
        {
            View->Initialize(this);
        }
    }
}
