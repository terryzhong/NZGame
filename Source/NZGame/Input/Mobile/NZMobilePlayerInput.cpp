// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerController.h"
#include "NZMobilePlayerInput.h"
#include "NZMobileFixedFireGameHandle.h"
#include "NZMobileJoystickPanelController.h"


void UNZMobilePlayerInput::Initialize()
{
	ANZPlayerController* OuterPC = Cast<ANZPlayerController>(GetOuter());
	check(OuterPC);
	if (OuterPC->bPCSimulateMobile)
	{
		OuterPC->bShowMouseCursor = true;
		OuterPC->DefaultMouseCursor = EMouseCursor::Crosshairs;
	}

	check(InputConfig == NULL);
	InputConfig = NewObject<UNZMobileInputConfig>(this, UNZMobileInputConfig::StaticClass(), TEXT("InputConfig"));
	//if (InputConfig)
	//{
	//	InputConfig->Initialize();
	//}

    check(GameHandle == NULL);
	GameHandle = NewObject<UNZMobileFixedFireGameHandle>(this, UNZMobileFixedFireGameHandle::StaticClass(), TEXT("FixedFireGameHandle"));
    if (GameHandle)
    {
        GameHandle->Initialize();
    }
    
    check(GameController == NULL);
    GameController = NewObject<UNZMobileJoystickPanelController>(this, UNZMobileJoystickPanelController::StaticClass(), TEXT("JoystickPanelController"));
    if (GameController)
    {
        GameController->Initialize(this);
    }
}

void UNZMobilePlayerInput::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D Location2D = FVector2D(Location.X, Location.Y);
	FMobileInputData* InputDataPtr = MobileInputDataList.FindByKey(FingerIndex);
	if (InputDataPtr)
	{
		InputDataPtr->BeginLocation = Location2D;
		InputDataPtr->Location = Location2D;
		InputDataPtr->BeginTime = GWorld->GetTimeSeconds();
	}
	else
	{
		MobileInputDataList.Add(FMobileInputData(FingerIndex, Location2D, Location2D, GWorld->GetTimeSeconds()));
	}
}

void UNZMobilePlayerInput::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	for (int32 Index = 0; Index < MobileInputDataList.Num(); Index++)
	{
		if (MobileInputDataList[Index].FingerIndex == FingerIndex)
		{
			MobileInputDataList.RemoveAt(Index);
			return;
		}
	}
}

void UNZMobilePlayerInput::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D Location2D = FVector2D(Location.X, Location.Y);
	FMobileInputData* InputData = MobileInputDataList.FindByKey(FingerIndex);
	//if (InputData == NULL)
	//{
	//	int32 Index = MobileInputDataList.Add(FMobileInputData(FingerIndex, Location2D, GWorld->GetTimeSeconds()));
	//	InputData = &(MobileInputDataList[Index]);
	//	InputData->BeginLocation = Location2D;
	//	InputData->BeginTime = GWorld->GetTimeSeconds();
	//}
	if (InputData)
	{
		InputData->DeltaMove = Location2D - InputData->Location;
		InputData->DeltaTime = GWorld->GetDeltaSeconds();
		InputData->Location = Location2D;
	}
}

void UNZMobilePlayerInput::UpdatePlayerInput(float DeltaTime)
{
#if (PLATFORM_IOS || PLATFORM_ANDROID)
	UpdatePlayerInputOnMobile();
#else
	UpdatePlayerInputOnPC();
#endif

	FSlateApplication::Get().SetAllUserFocusToGameViewport();

	FVector2D MovementAccel = GetMovementAccel();
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogX, 0, MovementAccel.X);
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogY, 0, MovementAccel.Y);

	FVector2D Rotation = GetRotation();
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::RightAnalogX, 0, Rotation.X);
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::RightAnalogY, 0, -Rotation.Y);

	if (IsFireButtonDown())
	{
		FSlateApplication::Get().OnControllerButtonPressed(FGamepadKeyNames::RightTriggerThreshold, 0, false);
	}
	else if (IsFireButtonUp())
	{
		FSlateApplication::Get().OnControllerButtonReleased(FGamepadKeyNames::RightTriggerThreshold, 0, false);
	}

	if (IsJumpButtonDown())
	{
		FSlateApplication::Get().OnControllerButtonPressed(FGamepadKeyNames::FaceButtonTop, 0, false);
	}
	else if (IsJumpButtonUp())
	{
		FSlateApplication::Get().OnControllerButtonReleased(FGamepadKeyNames::FaceButtonTop, 0, false);
	}
}

void UNZMobilePlayerInput::UpdatePlayerInputOnMobile()
{
	UpdateJoystick();
}

void UNZMobilePlayerInput::UpdatePlayerInputOnPC()
{
	UpdateJoystick();
}

void UNZMobilePlayerInput::UpdateJoystick()
{
	if (GameHandle)
	{
		GameHandle->UpdateJoystick(MobileInputDataList);
	}

	if (GameController)
	{
		GameController->Update();
	}
}

FVector2D UNZMobilePlayerInput::GetMovementAccel()
{
	check(GameHandle);
	return GameHandle->GetMovementAccel();
}

FVector2D UNZMobilePlayerInput::GetRotation()
{
	check(GameHandle);
	return GameHandle->GetRotationVector();
}

bool UNZMobilePlayerInput::IsFireButtonDown()
{
	check(GameHandle);
	return GameHandle->IsFireButtonDown();
}

bool UNZMobilePlayerInput::IsFireButtonUp()
{
	check(GameHandle);
	return GameHandle->IsFireButtonUp();
}

bool UNZMobilePlayerInput::IsJumpButtonDown()
{
	check(GameHandle);
	return GameHandle->IsJumpButtonDown();
}

bool UNZMobilePlayerInput::IsJumpButtonUp()
{
	check(GameHandle);
	return GameHandle->IsJumpButtonUp();
}

bool UNZMobilePlayerInput::IsCrouchButtonDown()
{
	check(GameHandle);
	return GameHandle->IsCrouchButtonDown();
}

bool UNZMobilePlayerInput::IsCrouchButtonUp()
{
	check(GameHandle);
	return GameHandle->IsCrouchButtonUp();
}

ENZMobileMoveHandle UNZMobilePlayerInput::GetMoveHandleType()
{
	check(GameHandle);
	return GameHandle->GetMoveHandleType();
}

FMobileInputData UNZMobilePlayerInput::GetMovementInputData()
{
	check(GameHandle);
	return GameHandle->GetMovementInputData();
}

FMobileInputData UNZMobilePlayerInput::GetRotationInputData()
{
	check(GameHandle);
	return GameHandle->GetRotationInputData();
}

bool UNZMobilePlayerInput::IsFixed()
{
	return true;
}
