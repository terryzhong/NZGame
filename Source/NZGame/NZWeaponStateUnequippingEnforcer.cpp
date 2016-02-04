// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateUnequippingEnforcer.h"
#include "NZWeapon_Enforcer.h"


void UNZWeaponStateUnequippingEnforcer::BeginState(const UNZWeaponState* PrevState)
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
		ANZWeapon_Enforcer* OuterWeapon = Cast<ANZWeapon_Enforcer>(GetOuterANZWeapon());
		GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(PutDownFinishedHandle, this, &UNZWeaponStateUnequippingEnforcer::PutDownFinished, UnequipTime);
		if (OuterWeapon->PutDownAnim != NULL)
		{
			UAnimInstance* AnimInstance = OuterWeapon->Mesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(OuterWeapon->PutDownAnim, OuterWeapon->PutDownAnim->SequenceLength / UnequipTime);
			}

			AnimInstance = OuterWeapon->LeftMesh->GetAnimInstance();
			if (AnimInstance != NULL && OuterWeapon->bDualEnforcerMode)
			{
				AnimInstance->Montage_Play(OuterWeapon->PutDownAnim, OuterWeapon->PutDownAnim->SequenceLength / UnequipTime);
			}
		}
	}
}



