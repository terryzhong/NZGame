// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateUnequipping.h"
#include "NZWeaponStateEquipping.h"



void UNZWeaponStateUnequipping::BeginState(const UNZWeaponState* PrevState)
{
	const UNZWeaponStateEquipping* PrevEquip = Cast<UNZWeaponStateEquipping>(PrevState);

	UnequipTime = (PrevEquip != NULL) ? FMath::Min(PrevEquip->PartialEquipTime, GetOuterANZWeapon()->GetPutDownTime()) : GetOuterANZWeapon()->GetPutDownTime();
	UnequipTimeElapsed = 0.0f;
	if (UnequipTime <= 0.0f)
	{
		PutDownFinished();
	}
	else
	{
		GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(PutDownFinishedHandle, this, &UNZWeaponStateUnequipping::PutDownFinished, UnequipTime);
		GetOuterANZWeapon()->PlayWeaponAnim(GetOuterANZWeapon()->PutDownAnim, GetOuterANZWeapon()->PutDownAnimHands,
			GetAnimLengthForScaling(GetOuterANZWeapon()->PutDownAnim, GetOuterANZWeapon()->PutDownAnimHands) / UnequipTime);
	}
}

void UNZWeaponStateUnequipping::EndState()
{
	GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(PutDownFinishedHandle);
}

void UNZWeaponStateUnequipping::PutDownFinished()
{
	float OverflowTime = UnequipTimeElapsed - UnequipTime;
	GetOuterANZWeapon()->DetachFromOwner();
	GetOuterANZWeapon()->GotoState(GetOuterANZWeapon()->InactiveState);
	GetOuterANZWeapon()->GetNZOwner()->WeaponChanged(OverflowTime);
}

void UNZWeaponStateUnequipping::BringUp(float OverflowTime)
{
	PartialEquipTime = FMath::Max<float>(0.001f, GetOuterANZWeapon()->GetWorldTimerManager().GetTimerElapsed(PutDownFinishedHandle));
	GetOuterANZWeapon()->GotoEquippingState(OverflowTime);
}

void UNZWeaponStateUnequipping::Tick(float DeltaTime)
{
    UnequipTimeElapsed += DeltaTime;
    if (UnequipTimeElapsed > UnequipTime)
    {
        PutDownFinished();
    }
}

