// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileGameHandle.h"
#include "NZMobilePlayerInput.h"


void UNZMobileGameHandle::Initialize()
{
	if (MovementButton == NULL)
	{
		MovementButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("MovementButton")));
	}

	if (FireButton == NULL)
	{
		FireButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("FireButton")));
	}

	if (SecondaryFireButton == NULL)
	{
		SecondaryFireButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("SecondaryFireButton")));
	}

	if (RotationButtion == NULL)
	{
		RotationButtion = NewObject<UNZMobileInputButton>(this, FName(TEXT("RotationButtion")));
	}

	if (JumpButton == NULL)
	{
		JumpButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("JumpButton")));
	}

	if (LeftJumpButton == NULL)
	{
		LeftJumpButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("LeftJumpButton")));
	}

	if (CrouchButton == NULL)
	{
		CrouchButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("CrouchButton")));
	}

	if (StaticWalkButton == NULL)
	{
		StaticWalkButton = NewObject<UNZMobileInputButton>(this, FName(TEXT("StaticWalkButton")));
	}
}

void UNZMobileGameHandle::InitFireButton(FVector2D Pos, FVector2D Size)
{
	if (FireButton)
	{
		FireButton->Position = Pos;
	}
}

void UNZMobileGameHandle::InitMovementButton(FVector2D Pos, FVector2D Size)
{
	MoveHandleImpl->Pos = Pos;
	MoveHandleImpl->MovementSize = Size;
}

void UNZMobileGameHandle::InitJumpButton(FVector2D Pos, FVector2D Size)
{
	if (JumpButton)
	{
		JumpButton->Position = Pos;
		JumpButton->Size = Size * 0.5f;
	}
}

void UNZMobileGameHandle::InitLeftJumpButton(FVector2D Pos, FVector2D Size)
{
	if (LeftJumpButton)
	{
		LeftJumpButton->Position = Pos;
		LeftJumpButton->Size = Size * 0.5f;
	}
}

void UNZMobileGameHandle::InitCrouchButton(FVector2D Pos, FVector2D Size)
{
	if (CrouchButton)
	{
		CrouchButton->Position = Pos;
		CrouchButton->Size = Size * 0.5f;
	}
}

void UNZMobileGameHandle::InitStaticWalkButton(FVector2D Pos, FVector2D Size)
{
	if (StaticWalkButton)
	{
		StaticWalkButton->Position = Pos;
		StaticWalkButton->Size = Size * 0.5f;
	}
}

void UNZMobileGameHandle::InitSecondaryAttackButton(FVector2D Pos, FVector2D Size)
{
	if (SecondaryFireButton)
	{
		SecondaryFireButton->Position = Pos;
		SecondaryFireButton->Size = Size * 0.5f;
	}
}

void UNZMobileGameHandle::UpdateJoystick(TArray<FMobileInputData>& InputDataList)
{
	bool LastFireButtonState = bIsFiring;
	bool LastSecondaryFireButtonState = bIsSecondaryFiring;
	bool LastJumpButtonState = IsJumpButtonPressed();
	bool LastCrouchButtonState = bCrouchButtonState;
	bool LastStaticWalkButtonState = bStaticWalkButtonState;

	MovementVector = FVector2D::ZeroVector;
	RotationVector = FVector2D::ZeroVector;
	bIsFiring = false;
	bIsSecondaryFiring = false;
	bRightJumpButtonState = false;
	bLeftJumpButtonState = false;
	bCrouchButtonState = false;
	bStaticWalkButtonState = false;
	bool bFoundDoubleClickInRotationRegion = false;

	int32 LastMoveInputFingerIndex = MovementInput.FingerIndex;
	LastRotateDelta = RotationInput.DeltaMove;

	float RotationDelta = 0;

	for (int32 Index = 0; Index < InputDataList.Num(); Index++)
	{
		FVector2D InitPos = InputDataList[Index].BeginLocation;

		if (bIsLockCrouch && InCrouchArea(InitPos))
		{
			bCrouchButtonState = true;
		}

		if (InStaticWalkArea(InitPos))
		{
			bStaticWalkButtonState = true;
		}

		if (!bIsLockMovement && InMovementArea(InputDataList[Index]) && !InLeftJumpArea(InputDataList[Index].BeginLocation))
		{
			if (MovementInput.FingerIndex != LastMoveInputFingerIndex)
			{
				MovementInput = InputDataList[Index];

				if (bIsLockMoveForwardDir)
				{
					MovementInput.Location.X = MovementInput.BeginLocation.X;
					MovementInput.Location.Y = MovementInput.Location.Y > MovementInput.BeginLocation.Y ? MovementInput.Location.Y : MovementInput.BeginLocation.Y;
				}
			}
		}
		else if (InRotationArea(InputDataList[Index].BeginLocation))
		{
			if (!bIsLockRotation && (bIsLockJump || !InJumpArea(InitPos)))
			{
				float TempRotDelta = InputDataList[Index].DeltaMove.Size();
				if (TempRotDelta > RotationDelta)
				{
					RotationInput = InputDataList[Index];
					RotationDelta = TempRotDelta;
				}
				bHasRotationInputAfterLoadInputConfig = true;
			}

			if (!InFireArea(InputDataList[Index].BeginLocation) &&
				LastRotationInput.BeginTime != InputDataList[Index].BeginTime &&
				(LastRotationInput.BeginLocation - InputDataList[Index].BeginLocation).Size() < JumpButton->Size.Y &&
				(InputDataList[Index].BeginTime - LastRotationInput.BeginTime) < DoubleClickMaxDeltaTime)
			{
				bFoundDoubleClickInRotationRegion = true;
			}
			LastRotationInput = InputDataList[Index];
		}

		if (!bIsLockFire && InFireArea(InitPos))
		{
			FireInput = InputDataList[Index];
			bIsFiring = true;
		}

		if (!bIsLockFire && InSecondaryFireArea(InitPos))
		{
			bIsSecondaryFiring = true;
		}

		if (!bIsLockJump)
		{
			if (InRightJumpArea(InitPos) || (bEnableDoubleClickJumping && bFoundDoubleClickInRotationRegion))
			{
				bRightJumpButtonState = true;
			}
			if (InLeftJumpArea(InitPos))
			{
				bLeftJumpButtonState = true;
			}
		}
	}

	bIsFireButtonDown = (!LastFireButtonState) && bIsFiring;
	bIsFireButtonUp = LastFireButtonState && (!bIsFiring);

	bIsSecondaryFireButtonDown = (!LastSecondaryFireButtonState) && bIsSecondaryFiring;
	bIsSecondaryFireButtonUp = LastSecondaryFireButtonState && !bIsSecondaryFiring;

	bIsJumpButtonDown = (!LastJumpButtonState) && IsJumpButtonPressed();
	bIsJumpButtonUp = LastJumpButtonState && !IsJumpButtonPressed();

	bIsCrouchButtonDown = (!LastCrouchButtonState) && bCrouchButtonState;
	bIsCrouchButtonUp = LastCrouchButtonState && (!bCrouchButtonState);

	CalcMovementVector();
	ProcessRightInput(bIsFiring);

	UNZMobilePlayerInput* OuterPlayerInput = Cast<UNZMobilePlayerInput>(GetOuter());
	if (OuterPlayerInput && OuterPlayerInput->StaticWalkMode == NZMobileStaticWalkMode_Click && LastStaticWalkButtonState && !bStaticWalkButtonState)
	{
		bEnableStaticWalk = !bEnableStaticWalk;
	}

	ProcessStaticWalking();
	UpdateMovementAcceleration();
}

bool UNZMobileGameHandle::InMovementArea(FMobileInputData InputData)
{
	return MoveHandleImpl && MoveHandleImpl->InMovementArea(InputData);
}

bool UNZMobileGameHandle::InRotationArea(FVector2D Pos)
{
	if (RotationButtion)
	{
		return RotationButtion->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::InCrouchArea(FVector2D Pos)
{
	if (CrouchButton)
	{
		return CrouchButton->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::InStaticWalkArea(FVector2D Pos)
{
	UNZMobilePlayerInput* OuterPlayerInput = Cast<UNZMobilePlayerInput>(GetOuter());
	if (OuterPlayerInput && (OuterPlayerInput->StaticWalkMode == NZMobileStaticWalkMode_Press || OuterPlayerInput->StaticWalkMode == NZMobileStaticWalkMode_Click))
	{
		return StaticWalkButton->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::InJumpArea(FVector2D Pos)
{
	return InLeftJumpArea(Pos) || InRightJumpArea(Pos);
}

bool UNZMobileGameHandle::InLeftJumpArea(FVector2D Pos)
{
	if (LeftJumpButton && LeftJumpButton->bEnabled)
	{
		return LeftJumpButton->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::InRightJumpArea(FVector2D Pos)
{
	if (JumpButton/* && JumpButton->bEnabled*/)
	{
		return JumpButton->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::InFireArea(FVector2D Pos)
{
	if (FireButton)
	{
		return FireButton->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::InSecondaryFireArea(FVector2D Pos)
{
	if (bEnableSecondaryFireButton && SecondaryFireButton)
	{
		return SecondaryFireButton->InButtonRange(Pos);
	}
	return false;
}

bool UNZMobileGameHandle::IsPointFire(FMobileInputData& InFireInput)
{
	return InFireInput.IsValid() &&
		(InFireInput.Location - InFireInput.BeginLocation).Size() < 12.f &&
		GWorld->GetTimeSeconds() - InFireInput.BeginTime < 0.4f &&
		!InJumpArea(InFireInput.Location);
}

void UNZMobileGameHandle::CalcMovementVector()
{
	MovementVector = MoveHandleImpl->CalcMovementVector(MovementInput);
	MovementVector *= MovementSensitity;
}

void UNZMobileGameHandle::ProcessRightInput(bool IsFiring)
{
	if (RotationInput.IsValid())
	{
		if (!bIsFiring || !IsPointFire(FireInput))
		{
			ProcessRotation();
		}

		//LastRotationInput
		ProcessRightInput();
	}
	else
	{
		SmoothRotationSpeed = 0;
		ProcessEmptyRightInput();
	}
}

void UNZMobileGameHandle::ProcessRightInput()
{

}

void UNZMobileGameHandle::ProcessRotation()
{
	if (!LastRotateDelta.ContainsNaN())
	{
		RotationVector = LastRotateDelta * 0.3f + RotationInput.DeltaMove * 0.7f;
	}
	else
	{
		RotationVector = RotationInput.DeltaMove;
	}

	RotationVector.X = FMath::Clamp(RotationVector.X / MAX_ROTATION_DELTA, -1.f, 1.f);
	RotationVector.Y = FMath::Clamp(RotationVector.Y / MAX_ROTATION_DELTA, -1.f, 1.f);

	float DpiFactor = 1.f;


	FVector2D DisVector = (RotationInput.Location - RotationInput.BeginLocation - RotationInput.DeltaMove / 2.f) * ResolutionResetScale * DpiFactor;
	FVector2D DeltaVector = RotationInput.DeltaMove * DpiFactor;

	if (InFireArea(RotationInput.Location))
	{
		RotationVector *= RotationSensitityForAccurateAiming;
	}
	else
	{
		RotationVector *= RotationSensitity * RotationSensitityFactor;
	}

	FVector2D DisFactor;
	DisFactor.X = FMath::Abs(DisVector.X / MAX_ROTATION_DELTA);
	DisFactor.Y = FMath::Abs(DisVector.Y / MAX_ROTATION_DELTA);
	DisFactor.X = 1.f + DisFactor.X * ROTATIONEXPONENT;
	DisFactor.Y = 1.f + DisFactor.Y * ROTATIONEXPONENT * 0.8f;
	RotationVector.X *= DisFactor.X;
	RotationVector.Y *= DisFactor.Y;

	if (LastRotationInputPhase == MobileInputPhase_Stationary || RotationInput.Phase == MobileInputPhase_Stationary)
	{
		if (FMath::Abs(RotationVector.X) > 3.f)
		{
			RotationVector.X *= 0.7f;
		}
		if (FMath::Abs(RotationVector.Y) > 3.f)
		{
			RotationVector.Y *= 0.7f;
		}
	}
}

void UNZMobileGameHandle::ProcessEmptyRightInput()
{

}

void UNZMobileGameHandle::ProcessStaticWalking()
{
	UNZMobilePlayerInput* OuterPlayerInput = Cast<UNZMobilePlayerInput>(GetOuter());
	if (OuterPlayerInput->StaticWalkMode == NZMobileStaticWalkMode_Click)
	{
		MoveHandleImpl->bIsStaticWalking = bEnableStaticWalk;
		return;
	}
	else if (OuterPlayerInput->StaticWalkMode == NZMobileStaticWalkMode_Press)
	{
		MoveHandleImpl->bIsStaticWalking = bStaticWalkButtonState;
		return;
	}
	MoveHandleImpl->ProcessStaticWalking();
}

void UNZMobileGameHandle::UpdateMovementAcceleration()
{
	MoveHandleImpl->UpdateMovementAcceleration();
}
