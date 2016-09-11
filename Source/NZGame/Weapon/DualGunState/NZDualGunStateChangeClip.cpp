// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDualGunStateChangeClip.h"



void UNZDualGunStateChangeClip::BeginState(const UNZWeaponState* PrevState)
{
	if (GetOuterANZDualGun()->GetWorldTimerManager().IsTimerActive(ChangeClipFinishedHandle))
	{
		return;
	}

	Super::BeginState(PrevState);
}

void UNZDualGunStateChangeClip::EndState()
{
	//GetOuterANZDualGun()->GetWorldTimerManager().ClearTimer(ChangeClipFinishedHandle);
}

bool UNZDualGunStateChangeClip::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	if (FireModeNum == 0 && GetOuterANZDualGun()->FiringState.IsValidIndex(FireModeNum))
	{
		if (GetOuterANZDualGun()->HasAmmo(FireModeNum))
		{
			GetOuterANZDualGun()->CurrentFireMode = FireModeNum;
			GetOuterANZDualGun()->GotoState(GetOuterANZWeapon()->FiringState[FireModeNum]);
			return true;
		}
		else
		{
			// Try and play equip sound if out of ammo when firing
			if (GetOuterANZDualGun()->FireEmptySound != NULL)
			{
				UNZGameplayStatics::NZPlaySound(GetWorld(), GetOuterANZDualGun()->FireEmptySound, GetNZOwner(), SRT_None);
			}
		}
	}
	return false;
}
