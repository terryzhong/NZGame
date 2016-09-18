// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileGameController.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileGameController : public UObject
{
	GENERATED_BODY()
	
	friend class UNZMobileGameView;

public:
    virtual void Initialize(UPlayerInput* InPlayerInput);
    
	virtual void Update();

	UPlayerInput* GetPlayerInput() { return CachedPlayerInput; }

	class UNZMobileGameView* GetView() { return View; }
    
protected:
	UPROPERTY()
	UPlayerInput* CachedPlayerInput;

	UPROPERTY()
    TSubclassOf<UNZMobileGameView> ViewClass;
    
	UPROPERTY()
    UNZMobileGameView* View;
};
