// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUD.h"
#include "NZHUDWidget.h"
#include "NZBasePlayerController.h"



EInputMode::Type ANZHUD::GetInputMode_Implementation() const
{
    return EInputMode::EIM_None;
}

float ANZHUD::GetCrosshairScale()
{
    // Apply pickup scaling
    float PickupScale = 1.f;
    const float WorldTime = GetWorld()->GetTimeSeconds();
    if (LastPickupTime > WorldTime - 0.3f)
    {
        if (LastPickupTime > WorldTime - 0.15f)
        {
            PickupScale = (1.f + 5.f * (WorldTime - LastPickupTime));
        }
        else
        {
            PickupScale = (1.f + 5.f * (LastPickupTime + 0.3f - WorldTime));
        }
    }
    
    if (Canvas != NULL)
    {
        PickupScale = PickupScale * Canvas->ClipX / 1920.f;
    }
    
    return PickupScale;
}

FLinearColor ANZHUD::GetCrosshairColor(FLinearColor CrosshairColor) const
{
    float TimeSinceHit = GetWorld()->TimeSeconds - LastConfirmedHitTime;
    if (TimeSinceHit < 0.4f)
    {
        CrosshairColor = FMath::Lerp<FLinearColor>(FLinearColor::Red, CrosshairColor, FMath::Lerp<float>(0.f, 1.f, FMath::Pow((GetWorld()->TimeSeconds - LastConfirmedHitTime) / 0.4f, 2.0f)));
    }
    return CrosshairColor;
}
