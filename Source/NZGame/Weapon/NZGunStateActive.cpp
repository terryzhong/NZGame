// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGunStateActive.h"


void UNZGunStateActive::BeginState(const UNZWeaponState* PrevState)
{
    Super::BeginState(PrevState);
    
    if (GetOuterANZGun()->Ammo == 0 &&
        GetOuterANZGun()->CarriedAmmo > 0 &&
        !GetOuterANZGun()->IsChangingClip() &&
        !GetOuterANZGun()->IsUnEquipping())
    {
		GetOuterANZGun()->ChangeClip();
	}
}


