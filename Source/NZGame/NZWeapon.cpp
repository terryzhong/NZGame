// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon.h"
#include "NZPlayerController.h"
#include "NZCharacterMovementComponent.h"
#include "NZBot.h"
#include "NZGameState.h"
#include "NZCharacter.h"
#include "NZProjectile.h"
#include "NZGameMode.h"
#include "NZPlayerState.h"



void ANZWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

void ANZWeapon::OnRep_AttachmentType()
{
    if (NZOwner)
    {
        GetNZOwner()->UpdateWeaponAttachment();
    }
    else
    {
        AttachmentType = NULL;
    }
}

void ANZWeapon::OnRep_Ammo()
{
    if (NZOwner != NULL && NZOwner->GetPendingWeapon() == NULL && !HasAnyAmmo())
    {
        ANZPlayerController* PC = Cast<ANZPlayerController>(NZOwner->Controller);
        if (PC != NULL)
        {
            //UE_LOG(NZ, Warning, TEXT("********** %s ran out of ammo for %s"), *GetName(), *PC->GetHumanReadableName());
            PC->SwitchToBestWeapon();
        }
        else
        {
            ANZBot* B = Cast<ANZBot>(NZOwner->Controller);
            if (B != NULL)
            {
                B->SwitchToBestWeapon();
            }
        }
    }
}

bool ANZWeapon::HasAnyAmmo()
{
    bool bHadCost = false;
    
    // Only consider zero cost firemodes as having ammo if they all have no cost
    // the assumption here is that for most weapons with an ammo-using firmode,
    // any that don't use ammo are support firemodes that can't function effectively without the other one
    for (int32 i = GetNumFireModes() - 1; i >= 0; i--)
    {
        if (AmmoCost[i] > 0)
        {
            bHadCost = true;
            if (HasAmmo(i))
            {
                return true;
            }
        }
    }
    return !bHadCost;
}

float ANZWeapon::GetImpartedMomentumMag(AActor* HitActor)
{
    ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
    if ((FriendlyMomentumScaling != 1.f) && GameState != NULL && Cast<ANZCharacter>(HitActor) && GameState->OnSameTeam(HitActor, NZOwner))
    {
        return InstantHitInfo[CurrentFireMode].Momentum * FriendlyMomentumScaling;
    }
    
    return InstantHitInfo[CurrentFireMode].Momentum;
}

void ANZWeapon::NetSynchRandomSeed()
{
    ANZPlayerController* OwningPlayer = NZOwner ? Cast<ANZPlayerController>(NZOwner->GetController()) : NULL;
    if (OwningPlayer && NZOwner && NZOwner->NZCharacterMovement)
    {
        FMath::RandInit(10000.f * NZOwner->NZCharacterMovement->GetCurrentSynchTime());
    }
}

void ANZWeapon::AttachToHolster()
{
    // todo:
    check(false);
}

void ANZWeapon::DetachFromHolster()
{
    // todo:
    check(false);
}

void ANZWeapon::DropFrom(const FVector& StartLocation, const FVector& TossVelocity)
{
    // todo:
    check(false);
}

void ANZWeapon::InitializeDroppedPickup(class ANZDroppedPickup* Pickup)
{
    // todo:
    check(false);
}

bool ANZWeapon::ShouldDropOnDeath()
{
    // todo:
    check(false);
    return false;    
}

bool ANZWeapon::ShouldPlay1PVisuals() const
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::PlayPredictedImpactEffects(FVector ImpactLoc)
{
    // todo:
    check(false);
}

void ANZWeapon::PlayDelayedImpactEffects()
{
    // todo:
    check(false);
}

void ANZWeapon::SpawnDelayedFakeProjectile()
{
    // todo:
    check(false);    
}

float ANZWeapon::GetBringUpTime()
{
    // todo:
    check(false);
    return 0.0f;
}

float ANZWeapon::GetPutDownTime()
{
    // todo:
    check(false);
    return 0.0f;    
}

bool ANZWeapon::FollowsInList(ANZWeapon* OtherWeapon)
{
    // Return true if this weapon is after OtherWeapon in the weapon list
    if (!OtherWeapon)
    {
        return true;
    }
    // If same group, order by slot, else order by group number
    return (Group == OtherWeapon->Group) ? (GroupSlot > OtherWeapon->GroupSlot) : (Group > OtherWeapon->Group);
}

void ANZWeapon::DeactivateSpawnProtection()
{
    // todo:
    check(false);
}

void ANZWeapon::UpdateViewBob(float DeltaTime)
{
    // todo:
    check(false);
}

void ANZWeapon::PostInitProperties()
{
    Super::PostInitProperties();
    
    // todo:
}

void ANZWeapon::BeginPlay()
{
    Super::BeginPlay();
    
    // todo:
}

UMeshComponent* ANZWeapon::GetPickupMeshTemplate_Implementation(FVector& OverrideScale) const
{
    // todo:
    check(false);
    return NULL;
}

void ANZWeapon::GotoState(class UNZWeaponState* NewState)
{
    // todo:
    check(false);
}

void ANZWeapon::StartFire(uint8 FireModeNum)
{
    // todo:
    check(false);
}

void ANZWeapon::StopFire(uint8 FireModeNum)
{
    // todo:
    check(false);
}

void ANZWeapon::ServerStartFire_Implementation(uint8 FireModeNum, bool bClientFired)
{
    // todo:
    check(false);
}

bool ANZWeapon::ServerStartFire_Validate(uint8 FireModeNum, bool bClientFired)
{
    return true;
}

void ANZWeapon::ServerStartFireOffset_Implementation(uint8 FireModeNum, uint8 ZOffset, bool bClientFired)
{
    // todo:
    check(false);
}

bool ANZWeapon::ServerStartFireOffset_Validate(uint8 FireModeNum, uint8 ZOffset, bool bClientFired)
{
    return true;
}

void ANZWeapon::ServerStopFire_Implementation(uint8 FireModeNum)
{
    // todo:
    check(false);
}

bool ANZWeapon::ServerStopFire_Validate(uint8 FireModeNum)
{
    return true;
}

bool ANZWeapon::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::EndFiringSequence(uint8 FireModeNum)
{
    // todo:
    check(false);
}

bool ANZWeapon::WillSpawnShot(float DeltaTime)
{
    // todo:
    check(false);
    return false;
}

bool ANZWeapon::CanFireAgain()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::OnStartedFiring_Implementation()
{
    // todo:
    check(false);
}

void ANZWeapon::OnStoppedFiring_Implementation()
{
    // todo:
    check(false);
}

bool ANZWeapon::HandleContinuedFiring()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::OnContinuedFiring_Implementation()
{
    // todo:
    check(false);
}

void ANZWeapon::OnMultiPress_Implementation(uint8 OtherFireMode)
{
    // todo:
    check(false);
}

void ANZWeapon::BringUp(float OverflowTime)
{
    // todo:
    check(false);    
}

bool ANZWeapon::PutDown()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::AttachToOwner_Implementation()
{
    // todo:
    check(false);
}

void ANZWeapon::DetachFromOwner_Implementation()
{
    // todo:
    check(false);
}

bool ANZWeapon::IsChargedFireMode(uint8 TestMode) const
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::GivenTo(ANZCharacter* NewOwner, bool bAutoActivate)
{
    // todo:
    check(false);
}

void ANZWeapon::ClientGivenTo_Internal(bool bAutoActivate)
{
    // todo:
    check(false);
}

void ANZWeapon::Removed()
{
    // todo:
    check(false);
}

void ANZWeapon::ClientRemoved_Implementation()
{
    // todo:
    check(false);
}

void ANZWeapon::FireShot()
{
    // todo:
    check(false);
}

void ANZWeapon::PlayWeaponAnim(UAnimMontage* WeaponAnim, UAnimMontage* HandsAnim, float RateOverride)
{
    if (RateOverride <= 0.0f)
    {
        RateOverride = NZOwner ? NZOwner->GetFireRateMultiplier() : 1.f;
    }
    if (NZOwner != NULL)
    {
        if (WeaponAnim != NULL)
        {
            UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
            if (AnimInstance != NULL)
            {
                AnimInstance->Montage_Play(WeaponAnim, RateOverride);
            }
        }
        if (HandsAnim != NULL)
        {
            UAnimInstance* AnimInstance = NZOwner->FirstPersonMesh->GetAnimInstance();
            if (AnimInstance != NULL)
            {
                AnimInstance->Montage_Play(HandsAnim, RateOverride);
            }
        }
    }
}

UAnimMontage* ANZWeapon::GetFiringAnim(uint8 FireMode, bool bOnHands) const
{
    const TArray<UAnimMontage*>& AnimArray = bOnHands ? FireAnimationHands : FireAnimation;
    return (AnimArray.IsValidIndex(CurrentFireMode) ? AnimArray[CurrentFireMode] : NULL);
}

void ANZWeapon::PlayFiringEffects()
{
    if (NZOwner != NULL)
    {
        uint8 EffectFiringMode = (Role == ROLE_Authority || NZOwner->Controller != NULL) ? CurrentFireMode : NZOwner->FireMode;
        
        // Try and play the sound if specified
        if (FireSound.IsValidIndex(EffectFiringMode) && FireSound[EffectFiringMode] != NULL)
        {
            // todo:
        }
        
        // Reload sound on local shooter
        if ((GetNetMode() != NM_DedicatedServer) && NZOwner && NZOwner->GetLocalViewer() &&
            ReloadSound.IsValidIndex(EffectFiringMode) && ReloadSound[EffectFiringMode] != NULL)
        {
            // todo:
        }
        
        if (ShouldPlay1PVisuals() && GetWeaponHand() != HAND_Hidden)
        {
            NZOwner->TargetEyeOffset.X = FiringViewKickback;
            
            // Try and play a firing animation if specified
            PlayWeaponAnim(GetFiringAnim(EffectFiringMode, false), GetFiringAnim(EffectFiringMode, true));
            
            // Muzzle flash
            if (MuzzleFlash.IsValidIndex(EffectFiringMode) && MuzzleFlash[EffectFiringMode] != NULL && MuzzleFlash[EffectFiringMode]->Template != NULL)
            {
                // If we detect a looping particle system, then don't reactivate it
                if (!MuzzleFlash[EffectFiringMode]->bIsActive ||
                    MuzzleFlash[EffectFiringMode]->bSuppressSpawning ||
                    !IsLoopingParticleSystem(MuzzleFlash[EffectFiringMode]->Template))
                {
                    MuzzleFlash[EffectFiringMode]->ActivateSystem();
                }
            }
        }
    }
}

void ANZWeapon::StopFiringEffects()
{
    for (UParticleSystemComponent* MF : MuzzleFlash)
    {
        if (MF != NULL)
        {
            MF->DeactivateSystem();
        }
    }
}

void ANZWeapon::GetImpactSpawnPosition(const FVector& TargetLoc, FVector& SpawnLocation, FRotator& SpawnRotation)
{
    SpawnLocation = (MuzzleFlash.IsValidIndex(CurrentFireMode) && MuzzleFlash[CurrentFireMode] != NULL) ? MuzzleFlash[CurrentFireMode]->GetComponentLocation() : NZOwner->GetActorLocation() + NZOwner->GetControlRotation().RotateVector(FireOffset);
    SpawnRotation = (MuzzleFlash.IsValidIndex(CurrentFireMode) && MuzzleFlash[CurrentFireMode] != NULL) ? MuzzleFlash[CurrentFireMode]->GetComponentRotation() : (TargetLoc - SpawnLocation).Rotation();
}

bool ANZWeapon::CancelImpactEffect(const FHitResult& ImpactHit)
{
    return (!ImpactHit.Actor.IsValid() && !ImpactHit.Component.IsValid()) || Cast<ANZCharacter>(ImpactHit.Actor.Get()) || Cast<ANZProjectile>(ImpactHit.Actor.Get());
}

void ANZWeapon::PlayImpactEffects(const FVector& TargetLoc, uint8 FireMode, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
    // todo:
    check(false);
}

FHitResult ANZWeapon::GetImpactEffectHit(APawn* Shooter, const FVector& StartLoc, const FVector& TargetLoc)
{
    // Trace for precise hit location and hit normal
    FHitResult Hit;
    FVector TargetToGun = (StartLoc - TargetLoc).GetSafeNormal();
    if (Shooter->GetWorld()->LineTraceSingleByChannel(Hit, TargetLoc + TargetToGun * 32.0f, TargetLoc - TargetToGun * 32.0f, COLLISION_TRACE_WEAPON, FCollisionQueryParams(FName(TEXT("ImpactEffect")), true, Shooter)))
    {
        return Hit;
    }
    else
    {
        return FHitResult(NULL, NULL, TargetLoc, TargetToGun);
    }
}

FVector ANZWeapon::GetFireStartLoc(uint8 FireMode)
{
    // Default to current firemode
    if (FireMode == 255)
    {
        FireMode = CurrentFireMode;
    }
    if (NZOwner == NULL)
    {
        //UE_LOG(NZ, Warning, TEXT("%s::GetFireStartLoc(): No Owner (died while firing?)"), *GetName());
        return FVector::ZeroVector;
    }
    else
    {
        const bool bIsFirstPerson = Cast<APlayerController>(NZOwner->Controller) != NULL;
        
        FVector BaseLoc;
        if (bFPFireFromCenter && bIsFirstPerson)
        {
            BaseLoc = NZOwner->GetPawnViewLocation();
            if (GetWorld()->GetTimeSeconds() - FireZOffsetTime < 0.06f)
            {
                BaseLoc.Z = FireZOffset + NZOwner->GetActorLocation().Z;
            }
        }
        else
        {
            BaseLoc = NZOwner->GetActorLocation();
        }
        
        if (bNetDelayedShot)
        {
            // Adjust for delayed shot to position client shot from
            BaseLoc = BaseLoc + NZOwner->GetDelayedShotPosition() - NZOwner->GetActorLocation();
        }
        
        // Ignore offset for instant hit shots in first person
        if (FireOffset.IsZero() || (bIsFirstPerson && bFPIgnoreInstantHitFireOffset && InstantHitInfo.IsValidIndex(FireMode) && InstantHitInfo[FireMode].DamageType != NULL))
        {
            return BaseLoc;
        }
        else
        {
            FVector AdjustedFireOffset;
            switch (GetWeaponHand())
            {
                case HAND_Right:
                    AdjustedFireOffset = FireOffset;
                    break;
                case HAND_Left:
                    AdjustedFireOffset = FireOffset;
                    AdjustedFireOffset.Y *= -1.0f;
                    break;
                case HAND_Center:
                case HAND_Hidden:
                    AdjustedFireOffset = FVector::ZeroVector;
                    AdjustedFireOffset.X = FireOffset.X;
                    break;
            }
            // MuzzleOffset is in camera space, so transform it to world space before offseting from the character location to find the final muzzle position
            FVector FinalLoc = BaseLoc + GetBaseFireRotation().RotateVector(AdjustedFireOffset);
            // Trace back towards Instigator's collision, then trace from there to desired location, checking for intervening world geometry
            FCollisionShape Collider;
            if (ProjClass.IsValidIndex(CurrentFireMode) &&
                ProjClass[CurrentFireMode] != NULL &&
                ProjClass[CurrentFireMode].GetDefaultObject()->CollisionComp != NULL)
            {
                Collider = FCollisionShape::MakeSphere(ProjClass[CurrentFireMode].GetDefaultObject()->CollisionComp->GetUnscaledSphereRadius());
            }
            else
            {
                Collider = FCollisionShape::MakeSphere(0.0f);
            }
            
            FCollisionQueryParams Params(FName(TEXT("WeaponStartLoc")), false, NZOwner);
            FHitResult Hit;
            if (GetWorld()->SweepSingleByChannel(Hit, BaseLoc, FinalLoc, FQuat::Identity, COLLISION_TRACE_WEAPON, Collider, Params))
            {
                FinalLoc = Hit.Location - (FinalLoc - BaseLoc).GetSafeNormal();
            }
            return FinalLoc;
        }
    }
}

FRotator ANZWeapon::GetBaseFireRotation()
{
    if (NZOwner == NULL)
    {
        //UE_LOG(NZ, Warning, TEXT("%s::GetBaseFireRotation(): No Owner (died while firing?)"), *GetName());
        return FRotator::ZeroRotator;
    }
    else if (bNetDelayedShot)
    {
        return NZOwner->GetDelayedShotRotation();
    }
    else
    {
        return NZOwner->GetViewRotation();
    }
}

FRotator ANZWeapon::GetAdjustedAim_Implementation(FVector StartFireLoc)
{
    FRotator BaseAim = GetBaseFireRotation();
    GuessPlayerTarget(StartFireLoc, BaseAim.Vector());
    if (Spread.IsValidIndex(CurrentFireMode) && Spread[CurrentFireMode] > 0.0f)
    {
        // Add in any spread
        FRotationMatrix Mat(BaseAim);
        FVector X, Y, Z;
        Mat.GetScaledAxes(X, Y, Z);
        
        NetSynchRandomSeed();
        float RandY = 0.5f * (FMath::FRand() + FMath::FRand() - 1.f);
        float RandZ = FMath::Sqrt(0.25f - FMath::Square(RandY)) * (FMath::FRand() + FMath::FRand() - 1.f);
        return (X + RandY * Spread[CurrentFireMode] * Y + RandZ * Spread[CurrentFireMode] * Z).Rotation();
    }
    else
    {
        return BaseAim;
    }
}

void ANZWeapon::GuessPlayerTarget(const FVector& StartFireLoc, const FVector& FireDir)
{
    if (Role == ROLE_Authority && NZOwner != NULL)
    {
        ANZPlayerController* PC = Cast<ANZPlayerController>(NZOwner->Controller);
        if (PC != NULL)
        {
            float MaxRange = 100000.0f; // todo: calc projectile mode range?
            if (InstantHitInfo.IsValidIndex(CurrentFireMode) && InstantHitInfo[CurrentFireMode].DamageType != NULL)
            {
                MaxRange = InstantHitInfo[CurrentFireMode].TraceRange * 1.2f;   // extra since player may miss intended target due to being out of range
            }
            // todo:
            //PC->LastShotTargetGuess = UNZGameplayStatics::PickBestAimTarget(PC, StartFireLoc, FireDir, 0.9f, MaxRange);
        }
    }
}

void ANZWeapon::AddAmmo(int32 Amount)
{
    if (Role == ROLE_Authority)
    {
        Ammo = FMath::Clamp<int32>(Ammo + Amount, 0, MaxAmmo);
        
        // trigger weapon switch if necessary
        if (NZOwner != NULL && NZOwner->IsLocallyControlled())
        {
            OnRep_Ammo();
        }
    }
}

void ANZWeapon::ConsumeAmmo(uint8 FireModeNum)
{
    if (Role == ROLE_Authority)
    {
        ANZGameMode* GameMode = GetWorld()->GetAuthGameMode<ANZGameMode>();
        if (AmmoCost.IsValidIndex(FireModeNum) && (!GameMode || GameMode->bAmmoIsLimited || (Ammo > 9)))
        {
            AddAmmo(-AmmoCost[FireModeNum]);
        }
        else if (!AmmoCost.IsValidIndex(FireModeNum))
        {
            //UE_LOG(NZ, Warning, TEXT("Invalid fire mode %i in %s::ConsumeAmmo()"), int32(FireModeNum), *GetName());
        }
    }
}

void ANZWeapon::FireInstantHit(bool bDealDamage, FHitResult* OutHit)
{
    //UE_LOG(LogNZWeapon, Verbose, TEXT("%s::FireInstantHit()"), *GetName());

    checkSlow(InstantHitInfo.IsValidIndex(CurrentFireMode));
    
    const FVector SpawnLocation = GetFireStartLoc();
    const FRotator SpawnRotation = GetAdjustedAim(SpawnLocation);
    const FVector FireDir = SpawnRotation.Vector();
    const FVector EndTrace = SpawnLocation + FireDir * InstantHitInfo[CurrentFireMode].TraceRange;
    
    FHitResult Hit;
    ANZPlayerController* NZPC = Cast<ANZPlayerController>(NZOwner->Controller);
    ANZPlayerState* PS = NZOwner->Controller ? Cast<ANZPlayerState>(NZOwner->Controller->PlayerState) : NULL;
    float PredictionTime = NZPC ? NZPC->GetPredictionTime() : 0.f;
    HitScanTrace(SpawnLocation, EndTrace, InstantHitInfo[CurrentFireMode].TraceHalfSize, Hit, PredictionTime);
    if (Role == ROLE_Authority)
    {
        if (PS && (ShotsStatsName != NAME_None))
        {
            PS->ModifyStatsValue(ShotsStatsName, 1);
        }
        NZOwner->SetFlashLocation(Hit.Location, CurrentFireMode);
        
        // todo:
        // warn bot target, if any
/*        if (NZPC != NULL)
        {
            APawn* PawnTarget = Cast<APawn>(Hit.Actor.Get());
            if (PawnTarget != NULL)
            {
                NZPC->LastShotTargetGuess = PawnTarget;
            }
            // If not dealing damage, it's the caller's responsibility to send warnings if desired
            if (bDealDamage && NZPC->LastShotTargetGuess != NULL)
            {
                ANZBot* EnemyBot = Cast<ANZBot>(NZPC->LastShotTargetGuess->Controller);
                if (EnemyBot != NULL)
                {
                    EnemyBot->ReceiveInstantWarning(NZOwner, FireDir);
                }
            }
        }
        else if (bDealDamage)
        {
            ANZBot* B = Cast<ANZBot>(NZOwner->Controller);
            if (B != NULL)
            {
                APawn* PawnTarget = Cast<APawn>(Hit.Actor.Get());
                if (PawnTarget == NULL)
                {
                    PawnTarget = Cast<APawn>(B->GetTarget());
                }
                if (PawnTarget != NULL)
                {
                    ANZBot* EnemyBot = Cast<ANZBot>(PawnTarget->Controller);
                    if (EnemyBot != NULL)
                    {
                        EnemyBot->ReceiveInstantWarning(NZOwner, FireDir);
                    }
                }
            }
        }*/
    }
    else if (PredictionTime > 0.f)
    {
        PlayPredictedImpactEffects(Hit.Location);
    }
    if (Hit.Actor != NULL && Hit.Actor->bCanBeDamaged && bDealDamage)
    {
        if ((Role == ROLE_Authority) && PS && (HitsStatsName != NAME_None))
        {
            PS->ModifyStatsValue(HitsStatsName, 1);
        }
        // todo:
        //Hit.Actor->TakeDamage(InstantHitInfo[CurrentFireMode].Damage, FNZPointDamageEvent(InstantHitInfo[CurrentFireMode].Damage, Hit, FireDir, InstantHitInfo[CurrentFireMode].DamageType, FireDIr * GetImpartedMomentumMag(Hit.Actor.Get())), NZOwner->Controller, this);
    }
    if (OutHit != NULL)
    {
        *OutHit = Hit;
    }
}


