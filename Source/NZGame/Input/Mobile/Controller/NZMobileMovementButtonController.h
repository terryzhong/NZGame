// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZMobileGameController.h"
#include "NZMobileMovementButtonController.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZMobileMovementButtonController : public UNZMobileGameController
{
	GENERATED_BODY()
	
public:
    UNZMobileMovementButtonController();
	
    virtual void Initialize(UPlayerInput* InPlayerInput);
};
