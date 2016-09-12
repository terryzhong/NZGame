// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZPlayerInput.h"
#include "NZMobileGameHandle.h"
#include "NZMobileGameController.h"
#include "NZMobilePlayerInput.generated.h"


/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobilePlayerInput : public UNZPlayerInput
{
	GENERATED_BODY()
	
public:
	TArray<struct FMobileInputData> MobileInputDataList;
	
	void Initialize();

	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);

	void Tick(float DeltaTime);
	void UpdatePlayerInputOnMobile();
	void UpdatePlayerInputOnPC();
	void UpdateJoystick();

	ENZMobileStaticWalkMode StaticWalkMode;

protected:
	class UNZMobileGameHandle* GameHandle;
    class UNZMobileGameController* GameController;
};
