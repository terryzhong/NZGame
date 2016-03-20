// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateEquipping.h"
#include "ComponentRecreateRenderStateContext.h"
#include "NZWeaponStateUnequipping.h"



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
    return false;
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
            // Now that the anim is playing, force update first person meshes
            // This is necessary to avoid one frame artifacts since the meshes may have been set to not update while the weapon was down
			GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh->TickAnimation(0.0f, false);
			GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh->RefreshBoneTransforms();
			GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh->UpdateComponentToWorld();
			GetOuterANZWeapon()->Mesh->TickAnimation(0.0f, false);
			GetOuterANZWeapon()->Mesh->RefreshBoneTransforms();
			GetOuterANZWeapon()->Mesh->UpdateComponentToWorld();
			FComponentRecreateRenderStateContext ReregisterContext(GetOuterANZWeapon()->GetNZOwner()->FirstPersonMesh);
			FComponentRecreateRenderStateContext ReregisterContext2(GetOuterANZWeapon()->Mesh);
		}
        // Try and play equip sound if specified
        if (GetOuterANZWeapon()->EquipSound != NULL)
        {
            UNZGameplayStatics::NZPlaySound(GetWorld(), GetOuterANZWeapon()->EquipSound, GetNZOwner(), SRT_None);
        }
	}
}

void UNZWeaponStateEquipping::EndState()
{
    GetOuterANZWeapon()->GetWorldTimerManager().ClearAllTimersForObject(this);
}

void UNZWeaponStateEquipping::Tick(float DeltaTime)
{
    if (!GetOuterANZWeapon()->GetWorldTimerManager().IsTimerActive(BringUpFinishedHandle))
    {
        //UE_LOG(NZ, Warning, TEXT("%s in state Equipping with no equip timer!"), *GetOuterAUTWeapon()->GetName());
        BringUpFinished();
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

void UNZWeaponStateEquipping::PutDown()
{
    PartialEquipTime = FMath::Max(0.001f, GetOuterANZWeapon()->GetWorldTimerManager().GetTimerElapsed(BringUpFinishedHandle));
    GetOuterANZWeapon()->GotoState(GetOuterANZWeapon()->UnequippingState);
}
