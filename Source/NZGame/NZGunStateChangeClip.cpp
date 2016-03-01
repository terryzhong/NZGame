// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGunStateChangeClip.h"



void UNZGunStateChangeClip::BeginState(const UNZWeaponState* PrevState)
{
 /*	ChangeClipTime = GetOuterANZWeapon()->GetChangeClipTime();
     ChangeClipTimeElapsed = 0.0f;
     if (ChangeClipTime <= 0.0f)
     {
     ChangeClipFinished();
     }
     else
     {
     GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(ChangeClipFinishedHandle, this, &UNZWeaponStateChangeCliping::ChangeClipFinished, ChangeClipTime);
     GetOuterANZWeapon()->PlayWeaponAnim(GetOuterANZWeapon()->ChangeClipAnim, GetOuterANZWeapon()->ChangeClipAnimHands,
     GetAnimLengthForScaling(GetOuterANZWeapon()->ChangeClipAnim, GetOuterANZWeapon()->ChangeClipAnimHands) / ChangeClipTime);
     }*/
}

void UNZGunStateChangeClip::EndState()
{
    GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(ChangeClipFinishedHandle);
}

void UNZGunStateChangeClip::ChangeClipFinished()
{
    GetOuterANZWeapon()->GotoActiveState();
}

void UNZGunStateChangeClip::Tick(float DeltaTime)
{
    ChangeClipTimeElapsed += DeltaTime;
    if (ChangeClipTimeElapsed > ChangeClipTime)
    {
        ChangeClipFinished();
    }
}


