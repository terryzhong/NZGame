// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateReloading.h"



void UNZWeaponStateReloading::BeginState(const UNZWeaponState* PrevState)
{
	ReloadTime = GetOuterANZWeapon()->GetReloadTime();
	ReloadTimeElapsed = 0.0f;
	if (ReloadTime <= 0.0f)
	{
		ReloadFinished();
	}
	else
	{
		GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(ReloadFinishedHandle, this, &UNZWeaponStateReloading::ReloadFinished, ReloadTime);
		GetOuterANZWeapon()->PlayWeaponAnim(GetOuterANZWeapon()->ReloadAnim, GetOuterANZWeapon()->ReloadAnimHands,
			GetAnimLengthForScaling(GetOuterANZWeapon()->ReloadAnim, GetOuterANZWeapon()->ReloadAnimHands) / ReloadTime);
	}
}

void UNZWeaponStateReloading::EndState()
{
	GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(ReloadFinishedHandle);
}

void UNZWeaponStateReloading::ReloadFinished()
{
	GetOuterANZWeapon()->GotoActiveState();
}

void UNZWeaponStateReloading::Tick(float DeltaTime)
{
	ReloadTimeElapsed += DeltaTime;
	if (ReloadTimeElapsed > ReloadTime)
	{
		ReloadFinished();
	}
}

