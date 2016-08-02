// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDualGunStateLeftHandChangeClip.h"




void UNZDualGunStateLeftHandChangeClip::BeginState(const UNZWeaponState* PrevState)
{
	if (GetOuterANZDualGun()->GetWorldTimerManager().IsTimerActive(ChangeClipFinishedHandle))
	{
		return;
	}

	ChangeClipTime = GetOuterANZDualGun()->GetChangeClipTime();
	ChangeClipTimeElapsed = 0.0f;
	if (ChangeClipTime <= 0.0f)
	{
		ChangeClipFinished();
	}
	else
	{
		GetOuterANZDualGun()->GetWorldTimerManager().SetTimer(ChangeClipFinishedHandle, this, &UNZDualGunStateLeftHandChangeClip::ChangeClipFinished, ChangeClipTime);
		GetOuterANZDualGun()->LeftHandPlayChangeClipAnim();
	}
}

void UNZDualGunStateLeftHandChangeClip::EndState()
{
	//GetOuterANZDualGun()->GetWorldTimerManager().ClearTimer(ChangeClipFinishedHandle);
}

void UNZDualGunStateLeftHandChangeClip::ChangeClipFinished()
{
	GetOuterANZDualGun()->LeftHandChangeClipFinished();
	GetOuterANZDualGun()->GotoActiveState();
}

void UNZDualGunStateLeftHandChangeClip::Tick(float DeltaTime)
{
	ChangeClipTimeElapsed += DeltaTime;
	if (ChangeClipTimeElapsed > ChangeClipTime)
	{
		ChangeClipFinished();
	}
}

bool UNZDualGunStateLeftHandChangeClip::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	if (FireModeNum == 1 && GetOuterANZDualGun()->FiringState.IsValidIndex(FireModeNum))
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
