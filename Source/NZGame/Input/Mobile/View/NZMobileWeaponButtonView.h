// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/Mobile/View/NZMobileGameView.h"
#include "NZMobileWeaponButtonView.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZMobileWeaponButtonView : public UNZMobileGameView
{
	GENERATED_BODY()
	
public:
	UNZMobileWeaponButtonView();

	virtual void Initialize(class UNZMobileGameController* InController);

	virtual void Update();

	UFUNCTION(BlueprintNativeEvent)
	void PreviousWeaponButtonOnClick();

	UFUNCTION(BlueprintNativeEvent)
	void CurrentWeaponButtonOnClick();

	UFUNCTION(BlueprintNativeEvent)
	void NextWeaponButtonOnClick();

protected:
	UWidget* CachedPreviousWeapon;
	UWidget* CachedCurrentWeapon;
	UWidget* CachedNextWeapon;
};
