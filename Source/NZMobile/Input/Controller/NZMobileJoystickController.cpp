// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileJoystickController.h"


void UNZMobileJoystickController::Init()
{
	if (JoystickViewClass != NULL && JoystickView == NULL)
	{
		JoystickView = NewObject<UNZMobileJoystickView>(this, JoystickViewClass);
	}
	InitFromConfig();
}

void UNZMobileJoystickController::InitFromConfig()
{

}

