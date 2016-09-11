// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileMoveJoystickHandleImpl.h"


FVector2D UNZMobileMoveJoystickHandleImpl::CalcMovementVector(FMobileInputData& TouchData)
{
	FVector2D MovementVector = FVector2D::ZeroVector;

	MovementVector = (TouchData.Location - TouchData.BeginLocation);
	MovementVector.X = (MovementVector.X < MIN_MOVEMENT_DELTA && MovementVector.X > MINUS_MIN_MOVEMENT_DELTA) ? 0.f : MovementVector.X;
	MovementVector.Y = (MovementVector.Y < MIN_MOVEMENT_DELTA && MovementVector.Y > MINUS_MIN_MOVEMENT_DELTA) ? 0.f : MovementVector.Y;
	MovementVector.X = FMath::Clamp(MovementVector.X / MAX_MOVEMENT_DELTA, -1.f, 1.f);
	MovementVector.Y = FMath::Clamp(MovementVector.Y / MAX_MOVEMENT_DELTA, -1.f, 1.f);

	return MovementVector;
}

bool UNZMobileMoveJoystickHandleImpl::InMovementArea(FMobileInputData& TouchData)
{
	return false;
}

void UNZMobileMoveJoystickHandleImpl::ProcessStaticWalking()
{

}

void UNZMobileMoveJoystickHandleImpl::UpdateMovementAcceleration()
{

}


