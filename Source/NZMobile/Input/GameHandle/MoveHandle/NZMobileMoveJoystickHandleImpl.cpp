// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileMoveJoystickHandleImpl.h"


FVector2D UNZMobileMoveJoystickHandleImpl::CalcMovementVector(FMobileInputData& InputData)
{
	FVector2D MovementVector = FVector2D::ZeroVector;

	if (InputData.IsValid())
	{
		MovementVector = (InputData.Location - InputData.BeginLocation);
		// 坐标系上小下大，所以需要取反
		MovementVector.Y = -MovementVector.Y;

		MovementVector.X = (MovementVector.X < MIN_MOVEMENT_DELTA && MovementVector.X > MINUS_MIN_MOVEMENT_DELTA) ? 0.f : MovementVector.X;
		MovementVector.Y = (MovementVector.Y < MIN_MOVEMENT_DELTA && MovementVector.Y > MINUS_MIN_MOVEMENT_DELTA) ? 0.f : MovementVector.Y;
		MovementVector.X = FMath::Clamp(MovementVector.X / MAX_MOVEMENT_DELTA, -1.f, 1.f);
		MovementVector.Y = FMath::Clamp(MovementVector.Y / MAX_MOVEMENT_DELTA, -1.f, 1.f);
	}

	return MovementVector;
}

bool UNZMobileMoveJoystickHandleImpl::InMovementArea(FVector2D Pos)
{
	UNZMobileGameHandle* OuterGameHandle = Cast<UNZMobileGameHandle>(GetOuter());
	check(OuterGameHandle && OuterGameHandle->GetMovementButton());
	return OuterGameHandle->GetMovementButton()->InButtonRange(Pos);
}

void UNZMobileMoveJoystickHandleImpl::ProcessStaticWalking()
{

}

void UNZMobileMoveJoystickHandleImpl::UpdateMovementAcceleration()
{
	UNZMobileGameHandle* OuterGameHandle = Cast<UNZMobileGameHandle>(GetOuter());
	check(OuterGameHandle);

	FVector2D Accel = OuterGameHandle->GetMovementVector();
	float Angle = FMath::Abs(FQuat::FindBetween(FVector(OuterGameHandle->GetMovementVector(), 0.0f).GetSafeNormal(), FVector(0.f, 1.f, 0.f).GetSafeNormal()).Rotator().Yaw);

	if (Angle <= 32.f || Angle >= 148.f)
	{
		Accel.X = 0;
	}

	Angle = FMath::Abs(FQuat::FindBetween(FVector(OuterGameHandle->GetMovementVector(), 0.0f).GetSafeNormal(), FVector(1.f, 0.f, 0.f).GetSafeNormal()).Rotator().Yaw);

	if (Angle <= 32.f || Angle >= 148.f)
	{
		Accel.Y = 0;
	}

	MovementAccel = Accel;
}


