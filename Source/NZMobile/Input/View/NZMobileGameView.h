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
	
public:
	virtual void Init();

protected:
	TSubclassOf<class UUserWidget> CacheWidgetClass;
	class UUserWidget* CacheWidget;

};
