// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/Controller/NZMobileGameController.h"
#include "NZMobileJoystickView.h"
#include "NZMobileJoystickController.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileJoystickController : public UNZMobileGameController
{
	GENERATED_BODY()

public:
    virtual void Initialize();

protected:
	virtual void InitFromConfig();

};
