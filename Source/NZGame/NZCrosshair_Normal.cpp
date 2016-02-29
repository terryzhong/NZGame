// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCrosshair_Normal.h"
#include "NZWeapon.h"
#include "NZWeaponViewKickComponent.h"


void UNZCrosshair_Normal::DrawCrosshair_Implementation(UCanvas* Canvas, ANZWeapon* Weapon, float DeltaTime, float Scale, FLinearColor Color)
{
    if (Weapon != NULL && Weapon->ViewKickComponent && Weapon->ViewKickComponent->DrawCrosshair_Normal(this, Canvas, DeltaTime, Scale, Color))
    {
        return;
    }
    
    Super::DrawCrosshair_Implementation(Canvas, Weapon, DeltaTime, Scale, Color);
}
