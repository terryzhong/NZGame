// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon_Enforcer.h"
#include "NZWeaponStateEquipping_Enforcer.h"
#include "NZWeaponStateUnequippingEnforcer.h"

const FName NAME_EnforcerKills(TEXT("EnforcerKills"));
const FName NAME_EnforcerDeaths(TEXT("EnforcerDeaths"));
const FName NAME_EnforcerHits(TEXT("EnforcerHits"));
const FName NAME_EnforcerShots(TEXT("EnforcerShots"));

ANZWeapon_Enforcer::ANZWeapon_Enforcer()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultGroup = 2;
	Ammo = 20;
	MaxAmmo = 40;
	LastFireTime = 0.f;
	SpreadResetIncrease = 1.f;
	SpreadIncrease = 0.03f;
	MaxSpread = 0.12f;
	BringUpTime = 0.28f;
	DualBringUpTime = 0.36f;
	PutDownTime = 0.2f;
	DualPutDownTime = 0.3f;
	StoppingPower = 30000.f;
	BaseAISelectRating = 0.4f;
	FireCount = 0;
	ImpactCount = 0;
	bDualEnforcerMode = false;
	bBecomeDual = false;
	bCanThrowWeapon = false;
	bFireLeftSide = false;
	FOVOffset = FVector(0.7f, 1.f, 1.f);
	MaxTracerDist = 2500.f;

	LeftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left"));
	LeftMesh->SetOnlyOwnerSee(true);
	LeftMesh->AttachParent = RootComponent;
	LeftMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	LeftMesh->bSelfShadowOnly = true;
	LeftMesh->bHiddenInGame = true;
	FirstPLeftMeshOffset = FVector(0.f);

	EnforcerEquippingState = CreateDefaultSubobject<UNZWeaponStateEquipping_Enforcer>(TEXT("EnforcerEquippingState"));
	EnforcerUnequippingState = CreateDefaultSubobject<UNZWeaponStateUnequippingEnforcer>(TEXT("EnforcerUnequippingState"));

	KillStatsName = NAME_EnforcerKills;
	DeathStatsName = NAME_EnforcerDeaths;
	HitsStatsName = NAME_EnforcerHits;
	ShotsStatsName = NAME_EnforcerShots;

	DisplayName = NSLOCTEXT("NZWeapon_Enforcer", "DisplayName", "Enforcer");
}

void ANZWeapon_Enforcer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

float ANZWeapon_Enforcer::GetBringUpTime()
{
	return bDualEnforcerMode ? DualBringUpTime : BringUpTime;
}

float ANZWeapon_Enforcer::GetPutDownTime()
{
	return bDualEnforcerMode ? DualPutDownTime : PutDownTime;
}

void ANZWeapon_Enforcer::UpdateViewBob(float DeltaTime)
{
	Super::UpdateViewBob(DeltaTime);

	if (LeftMesh != NULL && LeftMesh->AttachParent != NULL && NZOwner != NULL && NZOwner->GetWeapon() == this && ShouldPlay1PVisuals() && GetWeaponHand() != HAND_Hidden)
	{
		if (FirstPLeftMeshOffset.IsZero())
		{
			FirstPLeftMeshOffset = LeftMesh->GetRelativeTransform().GetLocation();
			FirstPLeftMeshRotation = LeftMesh->GetRelativeTransform().Rotator();
		}
		LeftMesh->SetRelativeLocation(FirstPLeftMeshOffset);
		LeftMesh->SetWorldLocation(LeftMesh->GetComponentLocation() + NZOwner->GetWeaponBobOffset(0.0f, this));
		LeftMesh->SetRelativeRotation(Mesh->RelativeRotation - FirstPMeshRotation + FirstPLeftMeshRotation);
	}
}

void ANZWeapon_Enforcer::PlayFiringEffects()
{

}

void ANZWeapon_Enforcer::FireInstantHit(bool bDealDamage, FHitResult* OutHit)
{

}

bool ANZWeapon_Enforcer::StackPickup_Implementation(ANZInventory* ContainedInv)
{
	return 0.f;
}

void ANZWeapon_Enforcer::BringUp(float OverflowTime)
{
	if (LeftBringUpAnim != NULL)
	{
		UAnimInstance* AnimInstance = LeftMesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(LeftBringUpAnim, LeftBringUpAnim->SequenceLength / EnforcerEquippingState->EquipTime);
		}
	}

	Super::BringUp(OverflowTime);
}

void ANZWeapon_Enforcer::PlayImpactEffects(const FVector& TargetLoc, uint8 FireMode, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{

}

void ANZWeapon_Enforcer::UpdateOverlays()
{
	UpdateOverlaysShared(this, GetNZOwner(), Mesh, OverlayEffectParams, OverlayMesh);
	if (bBecomeDual)
	{
		UpdateOverlaysShared(this, GetNZOwner(), LeftMesh, OverlayEffectParams, LeftOverlayMesh);
	}
}

void ANZWeapon_Enforcer::SetSkin(UMaterialInterface* NewSkin)
{

}

void ANZWeapon_Enforcer::GotoEquippingState(float OverflowTime)
{
	GotoState(EnforcerEquippingState);
	if (CurrentState == EnforcerEquippingState)
	{
		EnforcerEquippingState->StartEquip(OverflowTime);
	}
}

void ANZWeapon_Enforcer::FireShot()
{
	Super::FireShot();

	// todo:
}

void ANZWeapon_Enforcer::StateChanged()
{
	if (!FiringState.Contains(Cast<UNZWeaponStateFiring>(CurrentState)))
	{
		FireCount = 0;
		ImpactCount = 0;
	}

	Super::StateChanged();
}

void ANZWeapon_Enforcer::UpdateWeaponHand()
{
	Super::UpdateWeaponHand();

	// todo:
}

TArray<UMeshComponent*> ANZWeapon_Enforcer::Get1PMeshes_Implementation() const
{
	TArray<UMeshComponent*> Result = Super::Get1PMeshes_Implementation();
	Result.Add(LeftMesh);
	Result.Add(LeftOverlayMesh);
	return Result;
}

void ANZWeapon_Enforcer::BecomeDual()
{
	bBecomeDual = true;

	if (EnforcerEquippingState->EquipTime == 0.0f)
	{
		EnforcerEquippingState->EquipTime = GetBringUpTime();
	}

	// Pick up the second enforcer
	AttachLeftMesh();

	// The UnequippingState needs to be undated so that both guns are lowered during weapon switch
	UnequippingState = EnforcerUnequippingState;

	BaseAISelectRating = FMath::Max<float>(BaseAISelectRating, 0.6f);

	// Setup a timer to fire once the equip animation finishes
	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, this, &ANZWeapon_Enforcer::DualEquipFinished, EnforcerEquippingState->EquipTime);
	MaxAmmo *= 2;
}

float ANZWeapon_Enforcer::GetImpartedMomentumMag(AActor* HitActor)
{
	return 0.f;
}

void ANZWeapon_Enforcer::DetachFromOwner_Implementation()
{
	if (LeftMesh != NULL && LeftMesh->SkeletalMesh != NULL)
	{
		LeftMesh->DetachFromParent();
	}
	Super::DetachFromOwner_Implementation();
}

void ANZWeapon_Enforcer::AttachToOwner_Implementation()
{
	if (NZOwner == NULL)
	{
		return;
	}

	if (bBecomeDual && !bDualEnforcerMode)
	{
		DualEquipFinished();
	}

	// Attach left mesh
	if (LeftMesh != NULL && LeftMesh->SkeletalMesh != NULL && bDualEnforcerMode)
	{
		LeftMesh->SetHiddenInGame(false);
		LeftMesh->AttachTo(NZOwner->FirstPersonMesh);
		if (ShouldPlay1PVisuals())
		{
			LeftMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
			LeftMesh->LastRenderTime = GetWorld()->TimeSeconds;
			LeftMesh->bRecentlyRendered = true;
			if (LeftOverlayMesh != NULL)
			{
				LeftOverlayMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
				LeftOverlayMesh->LastRenderTime = GetWorld()->TimeSeconds;
				LeftOverlayMesh->bRecentlyRendered = true;
			}
		}
	}

	if (bDualEnforcerMode)
	{
		AttachmentType = DualWieldAttachmentType;
		if (NZOwner != NULL)
		{
			GetNZOwner()->SetWeaponAttachmentClass(AttachmentType);
		}
	}

	Super::AttachToOwner_Implementation();
}

void ANZWeapon_Enforcer::DualEquipFinished()
{
	if (!bDualEnforcerMode)
	{
		bDualEnforcerMode = true;
		FireInterval = FireIntervalDualWield;

		// Reset the FireRate timer
		if (Cast<UNZWeaponStateFiring>(CurrentState) != NULL)
		{
			((UNZWeaponStateFiring*)CurrentState)->UpdateTiming();
		}

		// Update the animation since the stance has changed
		// Change the weapon attachment
		AttachmentType = DualWieldAttachmentType;

		if (NZOwner != NULL && NZOwner->GetWeapon() == this)
		{
			GetNZOwner()->SetWeaponAttachmentClass(AttachmentType);
			if (ShouldPlay1PVisuals())
			{
				UpdateWeaponHand();
			}
		}

		if (Role == ROLE_Authority)
		{
			OnRep_AttachmentType();
		}
	}
}

void ANZWeapon_Enforcer::FiringInfoUpdated_Implementation(uint8 InFireMode, uint8 FlashCount, FVector InFlashLocation)
{

}

void ANZWeapon_Enforcer::AttachLeftMesh()
{
	if (NZOwner == NULL)
	{
		return;
	}

	if (LeftMesh != NULL && LeftMesh->SkeletalMesh != NULL)
	{
		LeftMesh->SetHiddenInGame(false);
		LeftMesh->AttachTo(NZOwner->FirstPersonMesh);
		if (Cast<APlayerController>(NZOwner->Controller) != NULL && NZOwner->IsLocallyControlled())
		{
			LeftMesh->LastRenderTime = GetWorld()->TimeSeconds;
			LeftMesh->bRecentlyRendered = true;
		}

		if (LeftBringUpAnim != NULL)
		{
			UAnimInstance* AnimInstance = LeftMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(LeftBringUpAnim, LeftBringUpAnim->SequenceLength / EnforcerEquippingState->EquipTime);
			}
		}

		if (NZOwner != NULL && NZOwner->GetWeapon() == this && GetNetMode() != NM_DedicatedServer)
		{
			UpdateOverlays();
		}
	}
}
