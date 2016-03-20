// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateActive.h"
#include "NZWeaponStateFiring.h"



void UNZWeaponStateActive::BeginState(const UNZWeaponState* PrevState)
{
	if (GetOuterANZWeapon()->GetNZOwner()->GetPendingWeapon() == NULL || !GetOuterANZWeapon()->PutDown())
	{
		for (uint8 i = 0; i < GetOuterANZWeapon()->GetNumFireModes(); i++)
		{
			if (GetOuterANZWeapon()->GetNZOwner()->IsPendingFire(i) && GetOuterANZWeapon()->HasAmmo(i))
			{
				GetOuterANZWeapon()->CurrentFireMode = i;
				GetOuterANZWeapon()->GotoState(GetOuterANZWeapon()->FiringState[i]);
				return;
			}
		}
	}
    
    GetOuterANZWeapon()->PlayWeaponAnim(GetOuterANZWeapon()->IdleAnim, GetOuterANZWeapon()->IdleAnimHands, 1.0);
}

bool UNZWeaponStateActive::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	if (GetOuterANZWeapon()->FiringState.IsValidIndex(FireModeNum))
	{
        if (GetOuterANZWeapon()->HasAmmo(FireModeNum))
        {
            GetOuterANZWeapon()->CurrentFireMode = FireModeNum;
            GetOuterANZWeapon()->GotoState(GetOuterANZWeapon()->FiringState[FireModeNum]);
            return true;
        }
        else
        {
            // Try and play equip sound if out of ammo when firing
            if (GetOuterANZWeapon()->FireEmptySound != NULL)
            {
                UNZGameplayStatics::NZPlaySound(GetWorld(), GetOuterANZWeapon()->FireEmptySound, GetNZOwner(), SRT_None);
            }
        }
	}
	return false;
}

bool UNZWeaponStateActive::WillSpawnShot(float DeltaTime)
{
	ANZPlayerController* NZPC = Cast<ANZPlayerController>(GetOuterANZWeapon()->GetNZOwner()->GetController());
	return NZPC && NZPC->HasDeferredFireInputs();
}

