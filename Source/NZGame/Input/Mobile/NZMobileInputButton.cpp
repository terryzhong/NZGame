// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileInputButton.h"


void UNZMobileInputButton::InitButtonStyle(FVector2D InSize, uint8 InType, bool bInFixed)
{
	HalfSize = InSize * 0.5;
	Type = (EMobileButtonType)InType;
	bFixed = bInFixed;
}

void UNZMobileInputButton::InitButtonPosition(FVector2D InPosition, uint8 AnchorType, float Scale)
{
	UGameViewportClient* ViewportClient = GWorld->GetGameViewport();
	check(ViewportClient);
	FVector2D ViewportSize;
	ViewportClient->GetViewportSize(ViewportSize);

	HalfSize *= Scale;
	FVector2D Dock(0.5, 0.5);
	switch (AnchorType)
	{
	case 1: Dock = FVector2D(0.0, 0.0); break;
	case 2: Dock = FVector2D(0.5, 0.0); break;
	case 3: Dock = FVector2D(1.0, 0.0); break;
	case 4: Dock = FVector2D(0.0, 0.5); break;
	case 5: Dock = FVector2D(0.5, 0.5); break;
	case 6: Dock = FVector2D(1.0, 0.5); break;
	case 7: Dock = FVector2D(0.0, 1.0); break;
	case 8: Dock = FVector2D(0.5, 1.0); break;
	case 9: Dock = FVector2D(1.0, 1.0); break;
	default:Dock = FVector2D(0.5, 0.5); break;
	}
	Position = ViewportSize * Dock + InPosition;
}

bool UNZMobileInputButton::InButtonRange(FVector2D Point)
{
	switch (Type)
	{
	case MobileButtonType_Circle:
	{
		float Dist = (Point - Position).Size();
		if (Dist <= HalfSize.Y)
		{
			return true;
		}
	}
	break;
	case MobileButtonType_Rectangle:
	case MobileButtonType_LeftRightButton:
	{
		if (FMath::Abs(Point.X - Position.X) <= HalfSize.X &&
			FMath::Abs(Point.Y - Position.Y) <= HalfSize.Y)
		{
			return true;
		}
	}
	break;
	}
	return false;
}
