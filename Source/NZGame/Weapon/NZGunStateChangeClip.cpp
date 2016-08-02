// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGunStateChangeClip.h"



void UNZGunStateChangeClip::BeginState(const UNZWeaponState* PrevState)
{
    ChangeClipTime = GetOuterANZGun()->GetChangeClipTime();
    ChangeClipTimeElapsed = 0.0f;
    if (ChangeClipTime <= 0.0f)
    {
        ChangeClipFinished();
    }
    else
    {
        GetOuterANZGun()->GetWorldTimerManager().SetTimer(ChangeClipFinishedHandle, this, &UNZGunStateChangeClip::ChangeClipFinished, ChangeClipTime);
        GetOuterANZGun()->PlayChangeClipAnim();
    }
}

void UNZGunStateChangeClip::EndState()
{
    GetOuterANZGun()->GetWorldTimerManager().ClearTimer(ChangeClipFinishedHandle);
}

void UNZGunStateChangeClip::ChangeClipFinished()
{
    GetOuterANZGun()->ChangeClipFinished();
    GetOuterANZGun()->GotoActiveState();
}

void UNZGunStateChangeClip::Tick(float DeltaTime)
{
	ChangeClipTimeElapsed += DeltaTime;
	if (ChangeClipTimeElapsed > ChangeClipTime)
	{
		ChangeClipFinished();
	}
}
