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
class NZGAME_API UNZMobilePlayerInput : public UNZPlayerInput
{
	GENERATED_BODY()
	
public:
	virtual void Initialize();

	virtual void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	virtual void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	virtual void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);

	virtual void UpdatePlayerInput(float DeltaTime);

	ENZMobileMoveHandle GetMoveHandleType();
	FMobileInputData GetMovementInputData();
	FMobileInputData GetRotationInputData();

	class UNZMobileInputConfig* GetInputConfig() { return InputConfig; }

	FVector2D GetMovementAccel();
	FVector2D GetRotation();
	bool IsFireButtonDown();
	bool IsFireButtonUp();
	bool IsJumpButtonDown();
	bool IsJumpButtonUp();
	bool IsCrouchButtonDown();
	bool IsCrouchButtonUp();
	bool IsFixed();

	ENZMobileStaticWalkMode StaticWalkMode;

protected:
	void UpdatePlayerInputOnMobile();
	void UpdatePlayerInputOnPC();
	void UpdateJoystick();

	UPROPERTY()
	class UNZMobileInputConfig* InputConfig;

	UPROPERTY()
	class UNZMobileGameHandle* GameHandle;

	UPROPERTY()
    class UNZMobileGameController* GameController;

	TArray<struct FMobileInputData> MobileInputDataList;
};
