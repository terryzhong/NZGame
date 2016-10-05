// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileInputButton.h"
#include "NZMobileInputConfig.h"
#include "NZMobileMoveHandleImpl.h"
#include "NZMobileGameHandle.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZMobileGameHandle : public UObject
{
	GENERATED_BODY()
	
public:
	UNZMobileGameHandle();

	virtual void Initialize();

	virtual void UpdateJoystick(TArray<FMobileInputData>& InputDataList);

	UNZMobileInputButton* GetMovementButton() { return MovementButton; }

	ENZMobileMoveHandle GetMoveHandleType();
	FMobileInputData GetMovementInputData();
	FMobileInputData GetRotationInputData();

	FVector2D GetMovementAccel();
	FVector2D GetMovementVector();
	FVector2D GetRotationVector();
	bool IsFireButtonDown() { return bIsFireButtonDown; }
	bool IsFireButtonUp() { return bIsFireButtonUp; }
	bool IsJumpButtonDown() { return bIsJumpButtonDown; }
	bool IsJumpButtonUp() { return bIsJumpButtonUp; }
	bool IsCrouchButtonDown() { return bIsCrouchButtonDown; }
	bool IsCrouchButtonUp() { return bIsCrouchButtonUp; }

protected:
	bool NeedInitializeButtons();
	void InitializeButtons();

	bool IsJumpButtonPressed() { return IsRightJumpButtonPressed() || IsLeftJumpButtonPressed(); }
	bool IsLeftJumpButtonPressed() { return bLeftJumpButtonState; }
	bool IsRightJumpButtonPressed() { return bRightJumpButtonState; }

	bool InMovementArea(FVector2D Pos);
	bool InRotationArea(FVector2D Pos);
	bool InCrouchArea(FVector2D Pos);
	bool InStaticWalkArea(FVector2D Pos);
	bool InJumpArea(FVector2D Pos);
	bool InLeftJumpArea(FVector2D Pos);
	bool InRightJumpArea(FVector2D Pos);
	bool InFireArea(FVector2D Pos);
	bool InSecondaryFireArea(FVector2D Pos);

	bool IsPointFire(FMobileInputData& InFireInput);

	void CalcMovementVector();
	void ProcessRightInput(bool IsFiring);
	void ProcessRightInput();
	void ProcessRotation();
	void ProcessEmptyRightInput();
	void ProcessStaticWalking();
	void UpdateMovementAcceleration();

protected:
	//class UNZMobileInputConfig* InputConfig;

	UPROPERTY()
	class UNZMobileInputButton* MovementButton;

	UPROPERTY()
	class UNZMobileInputButton* RotationButton;

	UPROPERTY()
	class UNZMobileInputButton* FireButton;

	UPROPERTY()
	class UNZMobileInputButton* SecondaryFireButton;

	UPROPERTY()
	class UNZMobileInputButton* JumpButton;

	UPROPERTY()
	class UNZMobileInputButton* LeftJumpButton;

	UPROPERTY()
	class UNZMobileInputButton* CrouchButton;

	UPROPERTY()
	class UNZMobileInputButton* StaticWalkButton;

	UPROPERTY()
	class UNZMobileMoveHandleImpl* MoveHandleImpl;

	bool bIsButtonsInitialized;
	
	bool bIsCrouchButtonDown;
	bool bIsCrouchButtonUp;
	bool bRightJumpButtonState;
	bool bLeftJumpButtonState;
	bool bIsJumpButtonDown;
	bool bIsJumpButtonUp;
	bool bIsFiring;
	bool bCrouchButtonState;
	bool bStaticWalkButtonState;
	bool bEnableStaticWalk;
	bool bEnableSecondaryFireButton;
	bool bIsFireButtonDown;
	bool bIsFireButtonUp;
	bool bIsSecondaryFiring;
	bool bIsSecondaryFireButtonDown;
	bool bIsSecondaryFireButtonUp;

	bool bIsLockMoveForwardDir;
	bool bIsLockFire;
	bool bIsLockMovement;
	bool bIsLockRotation;
	bool bIsLockJump;
	bool bIsLockCrouch;
	bool bAutoHideFireBtn;

	FMobileInputData MovementInput;
	FMobileInputData RotationInput;
	FVector2D LastRotateDelta;
	FMobileInputData FireInput;
	FMobileInputData LastRotationInput;

	float ResolutionResetScale;

	FVector2D MovementVector;
	FVector2D RotationVector;

	float MovementSensitity;
	float RotationSensitity;
	
	float RotationSensitityFactor;

	float SmoothRotationSpeed;

	float MaxAlphaForJoysitck;

	//FVector2D MovementAccel;

	bool bHasRotationInputAfterLoadInputConfig;

	bool bEnableDoubleClickJumping;

	float DoubleClickMaxDeltaTime;

	EMobileInputPhase LastRotationInputPhase;

	float RotationSensitityForAccurateAiming;
};
