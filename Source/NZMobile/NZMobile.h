// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

#include "UMG.h"

const float MIN_MOVEMENT_DELTA = 0;
const float MINUS_MIN_MOVEMENT_DELTA = 0;
const float MAX_MOVEMENT_DELTA = 300.f;
const float MAX_ROTATION_DELTA = 300.f;

#if PLATFORM_IOS
const float ROTATIONEXPONENT = 5.f;
#else
const float ROTATIONEXPONENT = 3.f;
#endif

enum ENZMobileGameHandle
{
	NZMobileGameHandle_FixedFire,
	NZMobileGameHandle_FollowFire,
	NZMobileGameHandle_MoveFire,
	NZMobileGameHandle_Touch3DFire
};

enum ENZMobileMoveHandle
{
	NZMobileMoveHandle_None,
	NZMobileMoveHandle_Joystick,
	NZMobileMoveHandle_Classics
};

enum ENZMobileRightInputState
{
	NZMobileRightInputState_Rotation,
	NZMobileRightInputState_Fire,
	NZMobileRightInputState_Idle,
	NZMobileRightInputState_TouchBegin
};

enum ENZMobileStaticWalkMode
{
	NZMobileStaticWalkMode_LightPush,
	NZMobileStaticWalkMode_Press,
	NZMobileStaticWalkMode_Click
};

enum EMobileInputPhase
{
	MobileInputPhase_Began,
	MobileInputPhase_Moved,
	MobileInputPhase_Stationary,
	MobileInputPhase_Ended,
	MobileInputPhase_Canceled
}; 

struct FMobileInputData
{
	ETouchIndex::Type FingerIndex;
	EMobileInputPhase Phase;
	FVector2D BeginLocation;
	FVector2D Location;
	FVector2D DeltaMove;
	float BeginTime;
	float DeltaTime;

	FMobileInputData()
	{
		Clear();
	}

	FMobileInputData(ETouchIndex::Type InFingerIndex, FVector2D InBeginLocation, FVector2D InLocation, float InBeginTime)
	{
		Clear();
		FingerIndex = InFingerIndex;
		BeginLocation = InBeginLocation;
		Location = InLocation;
		BeginTime = InBeginTime;
	}

	// to support FindByKey()
	bool operator == (const ETouchIndex::Type& OtherFingerIndex) const
	{
		return (FingerIndex == OtherFingerIndex);
	}

	bool IsValid()
	{
		return FingerIndex != ETouchIndex::Type(INDEX_NONE);
	}

	void Clear()
	{
		FMemory::Memzero(this, sizeof(FMobileInputData));
		FingerIndex = ETouchIndex::Type(INDEX_NONE);
	}
};




#include "NZMobileGameView.h"
#include "NZMobileGameController.h"
#include "NZMobileGameHandle.h"
