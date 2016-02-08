// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon_Enforcer.h"
#include "NZWeaponStateEquipping_Enforcer.h"
#include "NZWeaponStateUnequippingEnforcer.h"
#include "NZWeaponStateFiringBurst.h"
#include "NZWeaponStateFiringBurstEnforcer.h"

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
	SpreadResetInterval = 1.f;
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
    DOREPLIFETIME_CONDITION(ANZWeapon_Enforcer, bBecomeDual, COND_None);
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
    UNZWeaponStateFiringBurstEnforcer* BurstFireMode = Cast<UNZWeaponStateFiringBurstEnforcer>(FiringState[CurrentFireMode]);
    
    if (NZOwner != NULL)
    {
        // Fire on right side by default, unless dual and bFireLeftSide
        if (!bDualEnforcerMode || (BurstFireMode ? (FireCount / BurstFireMode->BurstSize == 0) : !bFireLeftSide))
        {
            if (!BurstFireMode || BurstFireMode->CurrentShot == 0)
            {
                Super::PlayFiringEffects();
            }
            else if (ShouldPlay1PVisuals())
            {
                NZOwner->TargetEyeOffset.X = FiringViewKickback;
                // Muzzle flash
                if (MuzzleFlash.IsValidIndex(CurrentFireMode) &&
                    MuzzleFlash[CurrentFireMode] != NULL &&
                    MuzzleFlash[CurrentFireMode]->Template != NULL)
                {
                    // If we detect a looping particle system, then don't reactivate it
                    if (!MuzzleFlash[CurrentFireMode]->bIsActive || !IsLoopingParticleSystem(MuzzleFlash[CurrentFireMode]->Template))
                    {
                        MuzzleFlash[CurrentFireMode]->ActivateSystem();
                    }
                }
            }
        }
        else
        {
            // Try and play the sound if specified
            if ((!BurstFireMode || BurstFireMode->CurrentShot == 0) &&
                FireSound.IsValidIndex(CurrentFireMode) &&
                FireSound[CurrentFireMode] != NULL)
            {
                UNZGameplayStatics::NZPlaySound(GetWorld(), FireSound[CurrentFireMode], NZOwner, SRT_AllButOwner);
            }
            
            if (ShouldPlay1PVisuals())
            {
                // Try and play a firing animation if specified
                if ((!BurstFireMode || BurstFireMode->CurrentShot == 0) &&
                    FireAnimation.IsValidIndex(CurrentFireMode) &&
                    FireAnimation[CurrentFireMode] != NULL)
                {
                    UAnimInstance* AnimInstance = LeftMesh->GetAnimInstance();
                    if (AnimInstance != NULL)
                    {
                        AnimInstance->Montage_Play(FireAnimation[CurrentFireMode], NZOwner->GetFireRateMultiplier());
                    }
                }
                
                NZOwner->TargetEyeOffset.X = FiringViewKickback;
                // Muzzle flash
                uint8 LeftHandMuzzleFlashIndex = CurrentFireMode + 2;
                if (MuzzleFlash.IsValidIndex(LeftHandMuzzleFlashIndex) &&
                    MuzzleFlash[LeftHandMuzzleFlashIndex] != NULL &&
                    MuzzleFlash[LeftHandMuzzleFlashIndex]->Template != NULL)
                {
                    // If we detect a looping particle system, then don't reactivate it
                    if (!MuzzleFlash[LeftHandMuzzleFlashIndex]->bIsActive ||
                        MuzzleFlash[LeftHandMuzzleFlashIndex]->bSuppressSpawning ||
                        !IsLoopingParticleSystem(MuzzleFlash[LeftHandMuzzleFlashIndex]->Template))
                    {
                        MuzzleFlash[LeftHandMuzzleFlashIndex]->ActivateSystem();
                    }
                }
            }
        }
        if (!BurstFireMode)
        {
            bFireLeftSide = !bFireLeftSide;
        }
    }
}

void ANZWeapon_Enforcer::FireInstantHit(bool bDealDamage, FHitResult* OutHit)
{
    // Burst mode takes care of spread variation itself
    if (!Cast<UNZWeaponStateFiringBurst>(FiringState[GetCurrentFireMode()]))
    {
        float TimeSinceFired = NZOwner->GetWorld()->GetTimeSeconds() - LastFireTime;
        float SpreadScalingOverTime = FMath::Max(0.f, 1.f - (TimeSinceFired - FireInterval[GetCurrentFireMode()]) / (SpreadResetInterval - FireInterval[GetCurrentFireMode()]));
        Spread[GetCurrentFireMode()] = FMath::Min(MaxSpread, Spread[GetCurrentFireMode()] + SpreadIncrease) * SpreadScalingOverTime;
    }

    Super::FireInstantHit(bDealDamage, OutHit);
    
    if (NZOwner)
    {
        LastFireTime = NZOwner->GetWorld()->GetTimeSeconds();
    }
}

bool ANZWeapon_Enforcer::StackPickup_Implementation(ANZInventory* ContainedInv)
{
	if (!bBecomeDual)
    {
        BecomeDual();
    }
    return Super::StackPickup_Implementation(ContainedInv);
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
    UNZWeaponStateFiringBurst* BurstFireMode = Cast<UNZWeaponStateFiringBurst>(FiringState[CurrentFireMode]);
    if (GetNetMode() != NM_DedicatedServer)
    {
        if (bDualEnforcerMode && (BurstFireMode ? (FireCount / BurstFireMode->BurstSize != 0) : bFireLeftSide))
        {
            // Fire effects
            static FName NAME_HitLocation(TEXT("HitLocation"));
            static FName NAME_LocalHitLocation(TEXT("LocalHitLocation"));
            
            // TODO: This is a really ugly solution, what if somebody modifies this later
            // Is the best solution really to split out a separate MuzzleFlash too??
            uint8 LeftHandMuzzleFlashIndex = CurrentFireMode + 2;
            const FVector LeftSpawnLocation = (MuzzleFlash.IsValidIndex(LeftHandMuzzleFlashIndex) && MuzzleFlash[LeftHandMuzzleFlashIndex] != NULL)
                ? MuzzleFlash[LeftHandMuzzleFlashIndex]->GetComponentLocation()
                : NZOwner->GetActorLocation() + NZOwner->GetControlRotation().RotateVector(FireOffset);
            if (FireEffect.IsValidIndex(CurrentFireMode) && FireEffect[CurrentFireMode] != NULL)
            {
                UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect[CurrentFireMode], LeftSpawnLocation, (MuzzleFlash.IsValidIndex(LeftHandMuzzleFlashIndex) && MuzzleFlash[LeftHandMuzzleFlashIndex] != NULL) ? MuzzleFlash[LeftHandMuzzleFlashIndex]->GetComponentRotation() : (TargetLoc - SpawnLocation).Rotation(), true);
                FVector AdjustedTargetLoc = ((TargetLoc - LeftSpawnLocation).SizeSquared() > 4000000.f)
                    ? LeftSpawnLocation + MaxTracerDist * (TargetLoc - LeftSpawnLocation).GetSafeNormal()
                    : TargetLoc;
                PSC->SetVectorParameter(NAME_HitLocation, AdjustedTargetLoc);
                PSC->SetVectorParameter(NAME_LocalHitLocation, PSC->ComponentToWorld.InverseTransformPosition(AdjustedTargetLoc));
            }
            // Perhaps the muzzle flash also contains hit effect (constant beam, etc) so set the parameter on it instead
            else if (MuzzleFlash.IsValidIndex(LeftHandMuzzleFlashIndex) && MuzzleFlash[LeftHandMuzzleFlashIndex] != NULL)
            {
                MuzzleFlash[LeftHandMuzzleFlashIndex]->SetVectorParameter(NAME_HitLocation, TargetLoc);
                MuzzleFlash[LeftHandMuzzleFlashIndex]->SetVectorParameter(NAME_LocalHitLocation, MuzzleFlash[LeftHandMuzzleFlashIndex]->ComponentToWorld.InverseTransformPositionNoScale(TargetLoc));
            }
            
            if ((TargetLoc - LastImpactEffectLocation).Size() >= ImpactEffectSkipDistance ||
                GetWorld()->TimeSeconds - LastImpactEffectTime >= MaxImpactEffectSkipTime)
            {
                if (ImpactEffect.IsValidIndex(CurrentFireMode) && ImpactEffect[CurrentFireMode] != NULL)
                {
                    FHitResult ImpactHit = GetImpactEffectHit(NZOwner, LeftSpawnLocation, TargetLoc);
                    if (!CancelImpactEffect(ImpactHit))
                    {
                        ImpactEffect[CurrentFireMode].GetDefaultObject()->SpawnEffect(GetWorld(), FTransform(ImpactHit.Normal.Rotation(), ImpactHit.Location), ImpactHit.Component.Get(), NULL, NZOwner->Controller);
                    }
                }
                LastImpactEffectLocation = TargetLoc;
                LastImpactEffectTime = GetWorld()->TimeSeconds;
            }
        }
        else
        {
            Super::PlayImpactEffects(TargetLoc, FireMode, SpawnLocation, SpawnRotation);
        }
        
        ImpactCount++;
        
        if ((BurstFireMode && ImpactCount >= BurstFireMode->BurstSize * 2) || (!BurstFireMode && ImpactCount > 1))
        {
            ImpactCount = 0;
        }
    }
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

	if (GetNetMode() != NM_DedicatedServer)
    {
        FireCount++;
        
        UNZWeaponStateFiringBurst* BurstFireMode = Cast<UNZWeaponStateFiringBurst>(FiringState[CurrentFireMode]);
        if ((BurstFireMode && FireCount >= BurstFireMode->BurstSize * 2) || (!BurstFireMode && FireCount > 1))
        {
            FireCount = 0;
        }
    }
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

	if (bDualEnforcerMode)
    {
        FirstPLeftMeshOffset = FVector::ZeroVector;
        FirstPLeftMeshRotation = FRotator::ZeroRotator;
        
        switch (GetWeaponHand())
        {
            case HAND_Center:
                //UE_LOG(NZ, Warning, TEXT("HAND_Center is not implemented yet!"));
            case HAND_Right:
                LeftMesh->SetRelativeLocationAndRotation(GetClass()->GetDefaultObject<ANZWeapon_Enforcer>()->LeftMesh->RelativeLocation, GetClass()->GetDefaultObject<ANZWeapon_Enforcer>()->LeftMesh->RelativeRotation);
                break;
            case HAND_Left:
            {
                // Swap
                LeftMesh->SetRelativeLocationAndRotation(GetClass()->GetDefaultObject<ANZWeapon_Enforcer>()->Mesh->RelativeLocation, GetClass()->GetDefaultObject<ANZWeapon_Enforcer>()->Mesh->RelativeRotation);
                Mesh->SetRelativeLocationAndRotation(GetClass()->GetDefaultObject<ANZWeapon_Enforcer>()->LeftMesh->RelativeLocation, GetClass()->GetDefaultObject<ANZWeapon_Enforcer>()->LeftMesh->RelativeRotation);
                break;
            }
            case HAND_Hidden:
            {
                Mesh->SetRelativeLocationAndRotation(FVector(-50.0f, 20.0f, -50.0f), FRotator::ZeroRotator);
                LeftMesh->SetRelativeLocationAndRotation(FVector(-50.0f, -20.0f, -50.0f), FRotator::ZeroRotator);
                break;
            }
        }
    }
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
    ANZCharacter* HitChar = Cast<ANZCharacter>(HitActor);
    return (HitChar && HitChar->GetWeapon() && HitChar->GetWeapon()->bAffectedByStoppingPower)
        ? StoppingPower
        : InstantHitInfo[CurrentFireMode].Momentum;
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
    CurrentFireMode = InFireMode;
    UNZWeaponStateFiringBurst* BurstFireMode = Cast<UNZWeaponStateFiringBurst>(FiringState[CurrentFireMode]);
    if (InFlashLocation.IsZero())
    {
        FireCount = 0;
        ImpactCount = 0;
        if (BurstFireMode != NULL)
        {
            BurstFireMode->CurrentShot = 0;
        }
    }
    else
    {
        PlayFiringEffects();
        
        FireCount++;
        if (BurstFireMode)
        {
            BurstFireMode->CurrentShot++;
        }
        
        if ((BurstFireMode && FireCount >= BurstFireMode->BurstSize * 2) || (!BurstFireMode && FireCount > 1))
        {
            FireCount = 0;
        }
        
        if (BurstFireMode && BurstFireMode->CurrentShot >= BurstFireMode->BurstSize)
        {
            BurstFireMode->CurrentShot = 0;
        }
    }
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
