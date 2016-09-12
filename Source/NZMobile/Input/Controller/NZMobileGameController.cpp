// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileGameController.h"


UNZMobileGameController::UNZMobileGameController()
{
	if (ViewClass)
	{
		View = NewObject<class UNZMobileGameView>(this, ViewClass);
		if (View)
		{
			View->Controller = this;
		}
	}
}
