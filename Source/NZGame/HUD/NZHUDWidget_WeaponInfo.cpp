// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUDWidget_WeaponInfo.h"
#include "NZCharacter.h"


UNZHUDWidget_WeaponInfo::UNZHUDWidget_WeaponInfo()
{
    Ammo = 0;
    CarriedAmmo = 0;
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
            Ammo = CurrentWeapon->Ammo;
            CarriedAmmo = CurrentWeapon->CarriedAmmo;
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
    
    Ammo = 0;
    CarriedAmmo = 0;
    AmmoText.GetTextDelegate.BindUObject(this, &UNZHUDWidget_WeaponInfo::GetAmmoText_Implementation);
}

FText UNZHUDWidget_WeaponInfo::GetAmmoText_Implementation()
{
    FString AmmoString = FString::Printf(TEXT("%d/%d"), Ammo, CarriedAmmo);
    return FText::FromString(AmmoString);
}
