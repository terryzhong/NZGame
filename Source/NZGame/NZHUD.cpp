// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUD.h"
#include "NZHUDWidget.h"
#include "NZBasePlayerController.h"



EInputMode::Type ANZHUD::GetInputMode_Implementation() const
{
    return EInputMode::EIM_None;
}
