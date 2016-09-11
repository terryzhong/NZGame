// Fill out your copyright notice in the Description page of Project Settings.

#include "NZMobile.h"
#include "NZMobileFixedFireGameHandle.h"



void UNZMobileFixedFireGameHandle::InitFireButton(FVector2D Pos, FVector2D Size)
{
	Super::InitFireButton(Pos, Size);

	if (FireButton)
	{
		FireButton->Size = Size * 0.5f;
	}
}

