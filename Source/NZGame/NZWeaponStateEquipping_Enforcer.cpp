// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateEquipping_Enforcer.h"
#include "NZWeapon_Enforcer.h"


void UNZWeaponStateEquipping_Enforcer::StartEquip(float OverflowTime)
{
	EquipTime -= OverflowTime;
	if (EquipTime <= 0.0f)
	{
		BringUpFinished();
	}
	else
	{
		GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(BringUpFinishedHandle, this, &UNZWeaponStateEquipping_Enforcer::BringUpFinished, EquipTime);
		ANZWeapon_Enforcer* OuterWeapon = Cast<ANZWeapon_Enforcer>(GetOuterANZWeapon());
		if (OuterWeapon->BringUpAnim != NULL)
		{
			UAnimInstance* AnimInstance = OuterWeapon->Mesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(OuterWeapon->BringUpAnim, OuterWeapon->BringUpAnim->SequenceLength / EquipTime);
			}
		}

		if (OuterWeapon->LeftBringUpAnim != NULL && OuterWeapon->bDualEnforcerMode)
		{
			UAnimInstance* AnimInstance = OuterWeapon->LeftMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(OuterWeapon->LeftBringUpAnim, OuterWeapon->LeftBringUpAnim->SequenceLength / EquipTime);
			}
		}
	}
}

