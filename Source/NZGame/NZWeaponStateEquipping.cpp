// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateEquipping.h"


static float GetAnimLengthForScaling(UAnimMontage* WeaponAnim, UAnimMontage* HandsAnim)
{
	if (HandsAnim != NULL)
	{
		return HandsAnim->SequenceLength / HandsAnim->RateScale;
	}
	else if (WeaponAnim != NULL)
	{
		return WeaponAnim->SequenceLength / WeaponAnim->RateScale;
	}
	else
	{
		return 0.0f;
	}
}

UNZWeaponStateEquipping::UNZWeaponStateEquipping()
{
	PendingFireSequence = -1;
}

void UNZWeaponStateEquipping::BeginState(const UNZWeaponState* PrevState)
{
    const UNZWeaponStateUnequipping* PrevEquip = Cast<UNZWeaponStateUnequipping>(PrevState);
    
	// If was previously unequipping, pay same amount of time to bring back up
    EquipTime = (PrevEquip != NULL) ? FMath::Min(PrevEquip->PartialEquipTime, GetOuterANZWeapon()->GetBringUpTime()) : GetOuterANZWeapon()->GetBringUpTime();
	EquipTime = FMath::Max(EquipTime, GetOuterANZWeapon()->EarliestFireTime - GetWorld()->GetTimeSeconds());

	PendingFireSequence = -1;
	if (EquipTime <= 0.0f)
	{
		BringUpFinished();
	}
}

bool UNZWeaponStateEquipping::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	// On server, might not be quite done equipping yet when client done, so queue firing
	if (bClientFired)
	{
		PendingFireSequence = FireModeNum;
		GetNZOwner()->NotifyPendingServerFire();
	}
}

void UNZWeaponStateEquipping::StartEquip(float OverflowTime)
{
	EquipTime -= OverflowTime;
	if (EquipTime <= 0.0f)
	{
		BringUpFinished();
	}
	else
	{
		GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(BringUpFinishedHandle, this, &UNZWeaponStateEquipping::BringUpFinished, EquipTime);
		GetOuterANZWeapon()->PlayWeaponAnim(GetOuterANZWeapon()->BringUpAnim, GetOuterANZWeapon()->BringUpAnimHands, 
			GetAnimLengthForScaling(GetOuterANZWeapon()->BringUpAnim, GetOuterANZWeapon()->BringUpAnimHands) / EquipTime);
		if (GetOuterANZWeapon()->GetNetMode() != NM_DedicatedServer && GetOuterANZWeapon()->ShouldPlay1PVisuals())
		{
			GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh->TickAnimation(0.0f, false);
			GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh->RefreshBoneTransforms();
			GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh->UpdateComponentToWorld();
			GetOuterANZWeapon()->Mesh->TickAnimation(0.0f, false);
			GetOuterANZWeapon()->Mesh->RefreshBoneTransforms();
			GetOuterANZWeapon()->Mesh->UpdateComponentToWorld();
			FComponentRecreateRenderStateContext ReregisterContext(GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh);
			FComponentRecreateRenderStateContext ReregisterContext2(GetOuterANZWeapon()->Mesh);
		}
	}
}

void UNZWeaponStateEquipping::BringUpFinished()
{
	GetOuterANZWeapon()->GotoActiveState();
	if (PendingFireSequence >= 0)
	{
		GetOuterANZWeapon()->bNetDelayedShot = (GetOuterANZWeapon()->GetNetMode() == NM_DedicatedServer);
		GetOuterANZWeapon()->BeginFiringSequence(PendingFireSequence, true);
		PendingFireSequence = -1;
		GetOuterANZWeapon()->bNetDelayedShot = false;
	}
}

