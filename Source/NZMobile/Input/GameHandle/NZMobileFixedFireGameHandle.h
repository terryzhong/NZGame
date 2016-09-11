// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZMobileGameHandle.h"
#include "NZMobileFixedFireGameHandle.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileFixedFireGameHandle : public UNZMobileGameHandle
{
	GENERATED_BODY()
	
public:
	virtual void InitFireButton(FVector2D Pos, FVector2D Size);
	
};
