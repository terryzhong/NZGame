// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileGameView.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileGameView : public UObject
{
	GENERATED_BODY()

	friend class UNZMobileGameController;

public:
	virtual void Initialize(class UNZMobileGameController* InController);

    class UNZMobileGameController* GetController() { return Controller; }
    
protected:
	TSubclassOf<class UUserWidget> CacheWidgetClass;
	class UUserWidget* CacheWidget;

	class UNZMobileGameController* Controller;
};
