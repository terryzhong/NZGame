// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobilePlayerInput.h"
#include "NZMobileFixedFireGameHandle.h"


void UNZMobilePlayerInput::Initialize()
{
	GameHandle = NewObject<UNZMobileFixedFireGameHandle>(this, UNZMobileFixedFireGameHandle::StaticClass(), TEXT("FixedFireGameHandle"));
}

void UNZMobilePlayerInput::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D Location2D = FVector2D(Location.X, Location.Y);
	FMobileInputData* TouchDataPtr = MobileInputDataList.FindByKey(FingerIndex);
	if (TouchDataPtr)
	{
		TouchDataPtr->BeginLocation = Location2D;
		TouchDataPtr->BeginTime = GWorld->GetTimeSeconds();
	}
	else
	{
		MobileInputDataList.Add(FMobileInputData(FingerIndex, Location2D, GWorld->GetTimeSeconds()));
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
	FMobileInputData* TouchData = MobileInputDataList.FindByKey(FingerIndex);
	if (TouchData == NULL)
	{
		int32 Index = MobileInputDataList.Add(FMobileInputData(FingerIndex, Location2D, GWorld->GetTimeSeconds()));
		TouchData = &(MobileInputDataList[Index]);
		TouchData->BeginLocation = Location2D;
		TouchData->BeginTime = GWorld->GetTimeSeconds();
	}
	if (TouchData)
	{
		TouchData->DeltaMove = Location2D - TouchData->Location;
		TouchData->DeltaTime = GWorld->GetDeltaSeconds();
		TouchData->Location = Location2D;
	}
}

void UNZMobilePlayerInput::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if (PLATFORM_IOS || PLATFORM_ANDROID)
	UpdatePlayerInputOnMobile();
#else
	UpdatePlayerInputOnPC();
#endif
}

void UNZMobilePlayerInput::UpdatePlayerInputOnMobile()
{
	UpdateJoystick();
}

void UNZMobilePlayerInput::UpdatePlayerInputOnPC()
{

}

void UNZMobilePlayerInput::UpdateJoystick()
{
	if (GameHandle)
	{
		GameHandle->UpdateJoystick(MobileInputDataList);
	}
}
