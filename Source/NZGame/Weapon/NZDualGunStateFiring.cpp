// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDualGunStateFiring.h"



bool UNZDualGunStateFiring::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	if (GetOuterANZDualGun()->FiringState.IsValidIndex(FireModeNum))
	{
		if (GetOuterANZDualGun()->HasAmmo(FireModeNum))
		{
			if (GetOuterANZDualGun()->CurrentFireMode != FireModeNum)
			{
				GetOuterANZDualGun()->CurrentFireMode = FireModeNum;
				GetOuterANZDualGun()->GotoState(GetOuterANZDualGun()->FiringState[FireModeNum]);
				return true;
			}
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
