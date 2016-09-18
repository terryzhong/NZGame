// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZPlayerInput.h"
#include "NZMobileInputConfig.h"
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
	void Initialize();

	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);

	void UpdatePlayerInput(float DeltaTime);
	void UpdatePlayerInputOnMobile();
	void UpdatePlayerInputOnPC();
	void UpdateJoystick();

	FVector2D GetMovementAccel();
	ENZMobileMoveHandle GetMoveHandleType();
	FMobileInputData GetMovementInputData();
	FMobileInputData GetRotationInputData();
	bool IsFixed();

	ENZMobileStaticWalkMode StaticWalkMode;

protected:
	UPROPERTY()
	class UNZMobileInputConfig* InputConfig;

	UPROPERTY()
	class UNZMobileGameHandle* GameHandle;

	UPROPERTY()
    class UNZMobileGameController* GameController;

	TArray<struct FMobileInputData> MobileInputDataList;
};
