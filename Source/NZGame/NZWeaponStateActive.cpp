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
}

bool UNZWeaponStateActive::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	if (GetOuterANZWeapon()->FiringState.IsValidIndex(FireModeNum) && GetOuterANZWeapon()->HasAmmo(FireModeNum))
	{
		GetOuterANZWeapon()->CurrentFireMode = FireModeNum;
		GetOuterANZWeapon()->GotoState(GetOuterANZWeapon()->FiringState[FireModeNum]);
		return true;
	}
	return false;
}

bool UNZWeaponStateActive::WillSpawnShot(float DeltaTime)
{
	ANZPlayerController* NZPC = Cast<ANZPlayerController>(GetOuterANZWeapon()->GetNZOwner()->GetController());
	return NZPC && NZPC->HasDeferredFireInputs();
}

