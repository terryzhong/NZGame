// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZMobileMoveHandleImpl.h"
#include "NZMobileMoveJoystickHandleImpl.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileMoveJoystickHandleImpl : public UNZMobileMoveHandleImpl
{
	GENERATED_BODY()
	
public:
	virtual ENZMobileMoveHandle GetHandleImplType() { return NZMobileMoveHandle_Joystick; }

	virtual FVector2D CalcMovementVector(FMobileInputData& InputData);
	virtual bool InMovementArea(FVector2D Pos);
	virtual void ProcessStaticWalking();
	virtual void UpdateMovementAcceleration();

};
