// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCrosshair.h"



UNZCrosshair::UNZCrosshair()
{
	CrosshairName = NSLOCTEXT("UNZCrosshair", "NoName", "No Name");
}

void UNZCrosshair::DrawCrosshair_Implementation(UCanvas* Canvas, ANZWeapon* Weapon, float DeltaTime, float Scale, FLinearColor Color)
{
	float X = FMath::RoundToFloat((Canvas->SizeX * 0.5f) - (CrosshairIcon.UL * Scale * 0.5f));
	float Y = FMath::RoundToFloat((Canvas->SizeY * 0.5f) - (CrosshairIcon.VL * Scale * 0.5f));

	Canvas->DrawColor = Color.ToFColor(false);
	Canvas->DrawIcon(CrosshairIcon, X, Y, Scale);
}

void UNZCrosshair::DrawPreviewCrosshair_Implementation(UCanvas* Canvas, class ANZWeapon* Weapon, float DeltaTime, float Scale, FLinearColor Color)
{
	DrawCrosshair(Canvas, Weapon, DeltaTime, Scale, Color);
}
