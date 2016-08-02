// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDualGunStateActive.h"



void UNZDualGunStateActive::BeginState(const UNZWeaponState* PrevState)
{
	if (GetOuterANZDualGun()->Ammo == 0 && GetOuterANZDualGun()->CarriedAmmo > 0)
	{
		GetOuterANZDualGun()->ChangeClip();
	}
	
	if (GetOuterANZDualGun()->LeftHandAmmo == 0 && GetOuterANZDualGun()->LeftHandCarriedAmmo > 0)
	{
		GetOuterANZDualGun()->LeftHandChangeClip();
	}

	if (GetOuterANZDualGun()->IdleAnim || GetOuterANZDualGun()->IdleAnimHands)
	{
		GetOuterANZDualGun()->PlayWeaponAnim(GetOuterANZDualGun()->IdleAnim, GetOuterANZDualGun()->IdleAnimHands, 1.0);
	}
}

