// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUDWidget_WeaponInfo.h"
#include "NZCharacter.h"


UNZHUDWidget_WeaponInfo::UNZHUDWidget_WeaponInfo()
{
    Position = FVector2D(-5.f, -5.f);
    Size = FVector2D(0.f, 0.f);
    ScreenPosition = FVector2D(1.f, 1.f);
    Origin = FVector2D(0.f, 0.f);
    
    AmmoFlashColor = FLinearColor::White;
    WeaponChangeFlashColor = FLinearColor::White;
    AmmoFlashTime = 0.5;
}

void UNZHUDWidget_WeaponInfo::Draw_Implementation(float DeltaTime)
{
    ANZCharacter* NZC = Cast<ANZCharacter>(NZHUDOwner->NZPlayerOwner->GetViewTarget());
    if (NZC)
    {
        UNZHUDWidget_WeaponInfo* DefObj = GetClass()->GetDefaultObject<UNZHUDWidget_WeaponInfo>();
        ANZWeapon* CurrentWeapon = NZC->GetWeapon();
        if (CurrentWeapon)
        {
            if (CurrentWeapon != LastWeapon)
            {
                FlashTimer = AmmoFlashTime;
                AmmoText.RenderColor = WeaponChangeFlashColor;
                LastAmmoAmount = CurrentWeapon->Ammo;
            }
            
            LastWeapon = CurrentWeapon;
        }
        
        if (LastWeapon)
        {
            if (LastAmmoAmount < LastWeapon->Ammo)
            {
                FlashTimer = AmmoFlashTime;
                AmmoText.RenderColor = AmmoFlashColor;
                AmmoText.TextScale = 2.f;
            }
            LastAmmoAmount = LastWeapon->Ammo;
        }
        
        if (FlashTimer > 0.f)
        {
            FlashTimer = FlashTimer - DeltaTime;
            if (FlashTimer < 0.5f * AmmoFlashTime)
            {
                AmmoText.RenderColor = FMath::CInterpTo(AmmoText.RenderColor, DefObj->AmmoText.RenderColor, DeltaTime, (1.f / (AmmoFlashTime > 0.f ? 2.f * AmmoFlashTime : 1.f)));
            }
            AmmoText.TextScale = 1.f + FlashTimer / AmmoFlashTime;
        }
        else
        {
            AmmoText.RenderColor = DefObj->AmmoText.RenderColor;
            AmmoText.TextScale = 1.f;
        }
    }
    
    Super::Draw_Implementation(DeltaTime);
}

bool UNZHUDWidget_WeaponInfo::ShouldDraw_Implementation(bool bShowScores)
{
    ANZCharacter* NZC = Cast<ANZCharacter>(NZHUDOwner->NZPlayerOwner->GetViewTarget());
    return Super::ShouldDraw_Implementation(bShowScores) && (NZC && NZC->GetWeapon() && NZC->GetWeapon()->NeedsAmmoDisplay());
}

void UNZHUDWidget_WeaponInfo::InitializeWidget(ANZHUD* Hud)
{
    Super::InitializeWidget(Hud);
    
    LastWeapon = NULL;
    LastAmmoAmount = 0;
    FlashTimer = 0.f;
    AmmoText.GetTextDelegate.BindUObject(this, &UNZHUDWidget_WeaponInfo::GetAmmoAmount_Implementation);
}

FText UNZHUDWidget_WeaponInfo::GetAmmoAmount_Implementation()
{
    return FText::AsNumber(LastAmmoAmount);
}

