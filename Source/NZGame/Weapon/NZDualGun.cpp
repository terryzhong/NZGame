// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDualGun.h"
#include "NZDualGunStateFiring.h"
#include "NZDualGunStateChangeClip.h"
#include "NZDualGunStateLeftHandChangeClip.h"
#include "NZDualGunStateActive.h"


ANZDualGun::ANZDualGun(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UNZDualGunStateActive>(TEXT("StateActive"))
		.SetDefaultSubobjectClass<UNZDualGunStateFiring>(TEXT("FiringState0"))
		.SetDefaultSubobjectClass<UNZDualGunStateFiring>(TEXT("FiringState1"))
		.SetDefaultSubobjectClass<UNZDualGunStateChangeClip>(TEXT("StateChangeClip")))
{
	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetOnlyOwnerSee(true);
	LeftHandMesh->SetupAttachment(RootComponent);
	LeftHandMesh->bCastInsetShadow = true;
	//LeftHandMesh->bSelfShadowOnly = true;
	LeftHandMesh->bReceivesDecals = false;
	LeftHandMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	FirstPLeftHandMeshOffset = FVector(0.f);

	LeftHandChangeClipState = ObjectInitializer.CreateDefaultSubobject<UNZDualGunStateLeftHandChangeClip>(this, TEXT("StateLeftHandChangeClip"));
}

void ANZDualGun::AttachToOwner_Implementation()
{
	if (NZOwner == NULL)
	{
		return;
	}

	if (LeftHandMesh != NULL && LeftHandMesh->SkeletalMesh != NULL)
	{
		LeftHandMesh->SetHiddenInGame(false);
		//LeftHandMesh->AttachTo(NZOwner->FirstPersonMesh, (GetWeaponHand() != HAND_Hidden) ? HandsAttachSocket : NAME_None);
		LeftHandMesh->AttachToComponent(NZOwner->FirstPersonMesh, FAttachmentTransformRules::KeepRelativeTransform, (GetWeaponHand() != HAND_Hidden) ? HandsAttachSocket : NAME_None);
		if (ShouldPlay1PVisuals())
		{
			LeftHandMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
			LeftHandMesh->LastRenderTime = GetWorld()->TimeSeconds;
			LeftHandMesh->bRecentlyRendered = true;
			if (LeftOverlayMesh != NULL)
			{
				LeftOverlayMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
				LeftOverlayMesh->LastRenderTime = GetWorld()->TimeSeconds;
				LeftOverlayMesh->bRecentlyRendered = true;
			}
		}
	}

	Super::AttachToOwner_Implementation();
}

void ANZDualGun::DetachFromOwner_Implementation()
{
	if (LeftHandMesh != NULL && LeftHandMesh->SkeletalMesh != NULL)
	{
		LeftHandMesh->DetachFromParent();
	}

	Super::DetachFromOwner_Implementation();
}

void ANZDualGun::BeginPlay()
{
	LeftHandAmmo = LeftHandClipAmmoCount;
	LeftHandCarriedAmmo = LeftHandMaxCarriedAmmo;

	Super::BeginPlay();
}


FVector ANZDualGun::InstantFireStartTrace()
{
	// VR设备下，射线检测的方向以枪口朝向为准
	if (NZOwner && NZOwner->Controller && NZOwner->Controller->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		FVector OutLocation;
		FRotator OutRotation;
		// 双枪开火模式，左键用LeftHandMesh，右键用Mesh
		USkeletalMeshComponent* FireMesh = (CurrentFireMode == 0 ? LeftHandMesh : Mesh);
		FireMesh->GetSocketWorldLocationAndRotation(TEXT("MuzzleFlash"), OutLocation, OutRotation);
		return OutLocation;
	}
	else
	{
		return Super::InstantFireStartTrace();
	}
}

FVector ANZDualGun::InstantFireEndTrace(FVector StartTrace)
{
	// VR设备下，射线检测的方向以枪口朝向为准
	if (NZOwner && NZOwner->Controller && NZOwner->Controller->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		FVector OutLocation;
		FRotator OutRotation;
		// 双枪开火模式，左键用LeftHandMesh，右键用Mesh
		USkeletalMeshComponent* FireMesh = (CurrentFireMode == 0 ? LeftHandMesh : Mesh);
		FireMesh->GetSocketWorldLocationAndRotation(TEXT("MuzzleFlash"), OutLocation, OutRotation);
		return StartTrace + OutRotation.Vector() * InstantHitInfo[CurrentFireMode].TraceRange;
	}
	else
	{
		const FVector FireDir = ModifyForwardDirection(GetAdjustedAim(StartTrace));
		return StartTrace + FireDir * InstantHitInfo[CurrentFireMode].TraceRange;
	}
}

/************************************************************************/
/* Display of dual weapons on the VR device.                            */
/************************************************************************/
void ANZDualGun::ProcessHeadMountedDisplay(FVector HeadPosition, FRotator HeadRotation, FVector RightHandPosition, FRotator RightHandRotation, FVector LeftHandPosition, FRotator LeftHandRotation)
{
	if (NZOwner->FirstPersonMesh != NULL && !NZOwner->FirstPersonMesh->bHiddenInGame)
	{
		NZOwner->FirstPersonMesh->SetHiddenInGame(true);
	}

	FRotationTranslationMatrix HeadMatrix = FRotationTranslationMatrix(HeadRotation, HeadPosition);

	FVector OutLocation;
	FRotator OutRotation;
	NZOwner->Controller->GetPlayerViewPoint(OutLocation, OutRotation);
	FRotationTranslationMatrix ViewMatrix = FRotationTranslationMatrix(OutRotation, OutLocation);

	FMatrix RightMeshMatrix = FRotationMatrix(FRotator(0, -90, 0)) * FRotationMatrix(FRotator(-60, 0, 0)) * FTranslationMatrix(FVector(-10, 0, 0)) * FRotationTranslationMatrix(RightHandRotation, RightHandPosition) * HeadMatrix.Inverse() * ViewMatrix;
	FMatrix LeftHandMeshMatrix = FRotationMatrix(FRotator(0, -90, 0)) * FRotationMatrix(FRotator(-60, 0, 0)) * FTranslationMatrix(FVector(-10, 0, 0)) * FRotationTranslationMatrix(LeftHandRotation, LeftHandPosition) * HeadMatrix.Inverse() * ViewMatrix;

	if (Mesh->GetAttachParent() != NULL)
	{
		Mesh->DetachFromParent(true);
	}
	Mesh->SetWorldLocation(LeftHandMeshMatrix.GetOrigin());
	Mesh->SetWorldRotation(LeftHandMeshMatrix.Rotator());

	if (LeftHandMesh->GetAttachParent() != NULL)
	{
		LeftHandMesh->DetachFromParent(true);
	}
	LeftHandMesh->SetWorldLocation(RightMeshMatrix.GetOrigin());
	LeftHandMesh->SetWorldRotation(RightMeshMatrix.Rotator());
}

/************************************************************************/
/* Check whether current fire mode is available.                        */
/* corresponding ammo count should be larger than 0.					*/
/************************************************************************/
bool ANZDualGun::HasAmmo(uint8 FireModeNum)
{
	bool bAmmoLeft = false;

	if (AmmoCost.IsValidIndex(FireModeNum))
	{
		int RealAmmoCost = AmmoCost[FireModeNum];

		bAmmoLeft = FireModeNum == 0 ? (LeftHandAmmo >= RealAmmoCost) : (Ammo >= RealAmmoCost);
	}

	return bAmmoLeft;
}

/************************************************************************/
/* Ammo cost of current dual weapon.									*/
/* 0 - stands for left hand firing.										*/
/* 1 - right hand firing.												*/
/************************************************************************/
void ANZDualGun::ConsumeAmmo(uint8 FireModeNum)
{
	if (Role == ROLE_Authority)
	{
		if (AmmoCost.IsValidIndex(FireModeNum))
		{
			if (FireModeNum == 0)
			{
				LeftHandAmmo = FMath::Clamp<int32>(LeftHandAmmo - AmmoCost[1], 0, ClipAmmoCount);
			}
			else
			{
				Ammo = FMath::Clamp<int32>(Ammo - AmmoCost[0], 0, ClipAmmoCount);
			}
		}
	}
}

// to do: replication functions
void ANZDualGun::OnRep_LeftHandAmmo()
{
}

void ANZDualGun::OnRep_LeftHandCarriedAmmo()
{
}

void ANZDualGun::ChangeClip()
{
	if (!IsPlayingChangeClipAnim())
	{
		Super::ChangeClip();
	}
}

void ANZDualGun::LeftHandChangeClip()
{
	if (!LeftHandIsPlayingChangeClipAnim())
	{
		LeftHandGotoChangeClipState();
	}
}

void ANZDualGun::GotoChangeClipState()
{
	Super::GotoChangeClipState();
}

void ANZDualGun::LeftHandGotoChangeClipState()
{
	GotoState(LeftHandChangeClipState);
}

/************************************************************************/
/* After clip changing.													*/
/* 1. Automatic clip changing, full either weapon with 0 ammo count.	*/
/* 2. Manually clip changing, based on current fire mode.				*/
/************************************************************************/
void ANZDualGun::ChangeClipFinished()
{
	Super::ChangeClipFinished();
}

void ANZDualGun::LeftHandChangeClipFinished()
{
	if (LeftHandAmmo + LeftHandCarriedAmmo > LeftHandClipAmmoCount)
	{
		LeftHandCarriedAmmo -= LeftHandClipAmmoCount - LeftHandAmmo;
		LeftHandAmmo = LeftHandClipAmmoCount;
	}
	else
	{
		LeftHandAmmo = LeftHandAmmo + LeftHandCarriedAmmo;
		LeftHandCarriedAmmo = 0;
	}
}

/************************************************************************/
/* First person firing effect: muzzle flash + hand/weapon animation.    */
/************************************************************************/
void ANZDualGun::PlayFiringEffects()
{
	if (NZOwner != NULL)
	{
		// Try and play the sound if specified
		if (FireSound.IsValidIndex(CurrentFireMode) && FireSound[CurrentFireMode] != NULL)
		{
			UNZGameplayStatics::NZPlaySound(GetWorld(), FireSound[CurrentFireMode], NZOwner, SRT_AllButOwner);
		}

		if (ShouldPlay1PVisuals() && GetWeaponHand() != HAND_Hidden)
		{
			NZOwner->TargetEyeOffset.X = FiringViewKickback;

			// Try and play a firing animation if specified
			PlayWeaponFiringAnim(CurrentFireMode);

			// Muzzle flash
			if (MuzzleFlash.IsValidIndex(CurrentFireMode) && MuzzleFlash[CurrentFireMode] != NULL && MuzzleFlash[CurrentFireMode]->Template != NULL)
			{
				// If we detect a looping particle system, then don't reactivate it
				if (!MuzzleFlash[CurrentFireMode]->bIsActive ||
					MuzzleFlash[CurrentFireMode]->bSuppressSpawning ||
					!IsLoopingParticleSystem(MuzzleFlash[CurrentFireMode]->Template))
				{
					MuzzleFlash[CurrentFireMode]->ActivateSystem();
				}
			}
		}
	}
}

/************************************************************************/
/* Play first person hand & weapon firing animations.                   */
/************************************************************************/
void ANZDualGun::PlayWeaponFiringAnim(uint8 InputFireMode)
{
	if (NZOwner != NULL)
	{
		UAnimMontage* WeaponAnim = GetFiringAnim(InputFireMode, false);
		if (WeaponAnim != NULL)
		{
			UAnimInstance* AnimInstance = InputFireMode == 1 ? Mesh->GetAnimInstance() : LeftHandMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(WeaponAnim);
			}
		}

		UAnimMontage* HandsAnim = GetFiringAnim(InputFireMode, true);
		if (HandsAnim != NULL)
		{
			UAnimInstance* AnimInstance = NZOwner->FirstPersonMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(HandsAnim);
			}
		}
	}
}

/************************************************************************/
/* Rewrite play change animation method.								*/
/* When doing clip change lef thand, try to do different animation.	    */
/************************************************************************/
void ANZDualGun::PlayChangeClipAnim()
{
	Super::PlayChangeClipAnim();
}

void ANZDualGun::LeftHandPlayChangeClipAnim()
{
	float RealChangeClipTime = GetChangeClipTime();

	if (NZOwner != NULL && RealChangeClipTime > 0)
	{
		UAnimMontage* WeaponAnim = LeftHandChangeClipAnim;
		if (WeaponAnim != NULL)
		{
			UAnimInstance* AnimInstance = LeftHandMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(WeaponAnim, WeaponAnim->SequenceLength / WeaponAnim->RateScale / RealChangeClipTime);
			}
		}

		UAnimMontage* HandsAnim = LeftHandChangeClipAnimHands;
		if (HandsAnim != NULL)
		{
			UAnimInstance* AnimInstance = NZOwner->FirstPersonMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(HandsAnim, HandsAnim->SequenceLength / HandsAnim->RateScale / RealChangeClipTime);
			}
		}
	}
}

bool ANZDualGun::IsPlayingChangeClipAnim()
{
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (AnimInstance != NULL)
	{
		return AnimInstance->Montage_IsPlaying(ChangeClipAnim);
	}
	return false;
}

bool ANZDualGun::LeftHandIsPlayingChangeClipAnim()
{
	UAnimInstance* AnimInstance = LeftHandMesh->GetAnimInstance();
	if (AnimInstance != NULL)
	{
		return AnimInstance->Montage_IsPlaying(LeftHandChangeClipAnim);
	}
	return false;
}
