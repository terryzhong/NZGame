// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/View/NZMobileGameView.h"
#include "NZMobileJoystickView.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileJoystickView : public UNZMobileGameView
{
	GENERATED_BODY()
	
public:
	virtual FVector2D GetJoystickSize();

	virtual FVector2D GetJoystickPos();
	
protected:
	virtual void SetInputConfig();
};
