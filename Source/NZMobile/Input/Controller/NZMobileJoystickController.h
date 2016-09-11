// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileJoystickView.h"
#include "NZMobileJoystickController.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileJoystickController : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Init();

protected:
	virtual void InitFromConfig();

	TSubclassOf<class UNZMobileJoystickView> JoystickViewClass;
	class UNZMobileJoystickView* JoystickView;	
	
};
