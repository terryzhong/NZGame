// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileMoveHandleImpl.generated.h"


/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileMoveHandleImpl : public UObject
{
	GENERATED_BODY()
	
public:
	virtual ENZMobileMoveHandle GetHandleImplType() { return NZMobileMoveHandle_None; }
	virtual FVector2D CalcMovementVector(FMobileInputData& InputData) { return FVector2D::ZeroVector; }
	virtual bool InMovementArea(FVector2D Pos) { return false; }
	virtual void ProcessStaticWalking() {}
	virtual void UpdateMovementAcceleration() {}

	FVector2D MovementAccel;
	FVector2D Pos;
	FVector2D MovementSize;
	bool bIsStaticWalking;
};
