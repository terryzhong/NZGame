// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateEquipping.h"




void UNZWeaponStateEquipping::BeginState(const UNZWeaponState* PrevState)
{
/*    const UNZWeaponStateEquipping* PrevEquip = Cast<UNZWeaponStateEquipping>(PrevState);
    
    EquipTime = (PrevEquip != NULL) ? FMath::Min(PrevEquip->PartialEquipTime, GetOuterANZWeapon()->GetPutDownTime()) : GetOuterANZWeapon()->GetPutDownTime();
    UnequipTimeElapsed = 0.0f;
    if (UnequipTime <= 0.0f)
    {
        PutDownFinished();
    }
    else
    {
        GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(PutDownFinishedHandle, this, &UNZWeaponStateUnequipping::PutDownFinished, UnequipTime);
        GetOuterANZWeapon()->PlayWeaponAnim(GetOuterANZWeapon()->PutDownAnim, GetOuterANZWeapon()->PutDownAnimHands, GetAnimLengthForScaling(GetOuterANZWeapon()->PutDownAnim, GetOuterANZWeapon()->PutDownAnimHands) / UnequipTime);
    }*/
}

bool UNZWeaponStateEquipping::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
    return false;
}

void UNZWeaponStateEquipping::StartEquip(float OverflowTime)
{
    
}

void UNZWeaponStateEquipping::BringUpFinished()
{
    
}
