// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileWeaponButtonView.h"
#include "NZMobileWeaponButtonController.h"


UNZMobileWeaponButtonController::UNZMobileWeaponButtonController()
{
	ViewClass = UNZMobileWeaponButtonView::StaticClass();
}
