// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobilePlayerInput.h"
#include "NZMobileGameView.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZMobileGameView : public UObject
{
	GENERATED_BODY()

	friend class UNZMobileGameController;

public:
	virtual void Initialize(class UNZMobileGameController* InController);

	virtual void Update();

    class UNZMobileGameController* GetController() { return Controller; }
    
protected:
	class UPlayerInput* GetPlayerInput();

	TSubclassOf<class UUserWidget> CachedWidgetClass;

	UPROPERTY()
	class UUserWidget* CachedWidget;

	class UNZMobileGameController* Controller;
};
