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
#include "NZWeaponStateFiring.h"
#include "NZHUDWidget.h"
#include "NZHUD.h"
#include "NZWeaponStateEquipping.h"



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
    if (NewState == NULL || !NewState->IsIn(this))
    {
        //UE_LOG(NZ, Warning, TEXT("Attempt to send %s to invalid state %s"), *GetName(), *GetFullNameSafe(NewState));
    }
    else if (ensureMsgf(NZOwner != NULL || NewState == InactiveState, TEXT("Attempt to send %s to state %s while not owned"), *GetName(), *GetNameSafe(NewState)))
    {
        if (CurrentState != NewState)
        {
            UNZWeaponState* PrevState = CurrentState;
            if (CurrentState != NULL)
            {
                CurrentState->EndState();   // NOTE: may trigger another GotoState() call
            }
            if (CurrentState == PrevState)
            {
                CurrentState = NewState;
                CurrentState->BeginState(PrevState);    // NOTE: may trigger another GotoState() call
                StateChanged();
            }
        }
    }
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

void ANZWeapon::K2_FireInstantHit(bool bDealDamage, FHitResult& OutHit)
{
    if (!InstantHitInfo.IsValidIndex(CurrentFireMode))
    {
        FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s::FireInstantHit(): Fire mode %i doesn't have instant hit info"), *GetName(), int32(CurrentFireMode)), ELogVerbosity::Warning);
    }
    else if (GetNZOwner() != NULL)
    {
        FireInstantHit(bDealDamage, &OutHit);
    }
    else
    {
        FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s::FireInstantHit(): Weapon is not owned (owner died while script was running?)"), *GetName()), ELogVerbosity::Warning);
    }
}

void ANZWeapon::HitScanTrace(const FVector& StartLocation, const FVector& EndTrace, float TraceRadius, FHitResult& Hit, float PredictionTime)
{
    ECollisionChannel TraceChannel = COLLISION_TRACE_WEAPONNOCHARACTER;
    FCollisionQueryParams QueryParams(GetClass()->GetFName(), true, NZOwner);
    if (TraceRadius <= 0.0f)
    {
        if (!GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndTrace, TraceChannel, QueryParams))
        {
            Hit.Location = EndTrace;
        }
    }
    else
    {
        if (GetWorld()->SweepSingleByChannel(Hit, StartLocation, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(TraceRadius), QueryParams))
        {
            Hit.Location += (EndTrace - StartLocation).GetSafeNormal() * TraceRadius;
        }
        else
        {
            Hit.Location = EndTrace;
        }
    }
    
    if (!(Hit.Location - StartLocation).IsNearlyZero())
    {
        ANZCharacter* BestTarget = NULL;
        FVector BestPoint(0.f);
        FVector BestCapsulePoint(0.f);
        float BestCollisionRadius = 0.f;
        for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
        {
            ANZCharacter* Target = Cast<ANZCharacter>(*Iterator);
            if (Target && (Target != NZOwner))
            {
                FVector TargetLocation = ((PredictionTime > 0.f) && (Role == ROLE_Authority)) ? Target->GetRewindLocation(PredictionTime) : Target->GetActorLocation();
                
                // now see if trace would hit the capsule
                float CollisionHeight = Target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
                // todo:
                /*
                if (Target->NZCharacterMovement && Target->NZCharacterMovement->bIsFloorSliding)
                {
                    TargetLocation.Z = TargetLocation.Z - CollisionHeight + Target->SlideTargetHeight;
                    CollisionHeight = Target->SlideTargetHeight;
                }
                 */
                float CollisionRadius = Target->GetCapsuleComponent()->GetScaledCapsuleRadius();
                
                bool bHitTarget = false;
                FVector ClosestPoint(0.f);
                FVector ClosestCapsulePoint = TargetLocation;
                if (CollisionRadius >= CollisionHeight)
                {
                    ClosestPoint = FMath::ClosestPointOnSegment(TargetLocation, StartLocation, Hit.Location);
                    bHitTarget = ((ClosestPoint - TargetLocation).SizeSquared() < FMath::Square(CollisionHeight + TraceRadius));
                }
                else
                {
                    FVector CapsuleSegment = FVector(0.f, 0.f, CollisionHeight - CollisionRadius);
                    FMath::SegmentDistToSegmentSafe(StartLocation, Hit.Location, TargetLocation - CapsuleSegment, TargetLocation + CapsuleSegment, ClosestPoint, ClosestCapsulePoint);
                    bHitTarget = ((ClosestPoint - ClosestCapsulePoint).SizeSquared() < FMath::Square(CollisionRadius + TraceRadius));
                }
                
                if (bHitTarget && (!BestTarget || ((ClosestPoint - StartLocation).SizeSquared() < (BestPoint - StartLocation).SizeSquared())))
                {
                    BestTarget = Target;
                    BestPoint = ClosestPoint;
                    BestCapsulePoint = ClosestCapsulePoint;
                    BestCollisionRadius = CollisionRadius;
                }
            }
        }
        
        if (BestTarget)
        {
            // We found a player to hit, so update hit result
            
            // First find proper hit location on surface of capsule
            float ClosestDistSq = (BestPoint - BestCapsulePoint).SizeSquared();
            float BackDist = FMath::Sqrt(FMath::Max(0.f, BestCollisionRadius * BestCollisionRadius - ClosestDistSq));
            
            Hit.Location = BestPoint + BackDist * (StartLocation - EndTrace).GetSafeNormal();
            Hit.Normal = (Hit.Location - BestCapsulePoint).GetSafeNormal();
            Hit.ImpactNormal = Hit.Normal;
            Hit.Actor = BestTarget;
            Hit.bBlockingHit = true;
            Hit.Component = BestTarget->GetCapsuleComponent();
            Hit.ImpactPoint = BestPoint;
            Hit.Time = (BestPoint - StartLocation).Size() / (EndTrace - StartLocation).Size();
        }
    }
}

ANZProjectile* ANZWeapon::FireProjectile()
{
    //UE_LOG(LogNZWeapon, Verbose, TEXT("%s::FireProjectile()"), *GetName());

    if (GetNZOwner() == NULL)
    {
        //UE_LOG(LogNZWeapon, Warning, TEXT("%s::FireProjectile(): Weapon is not owned (owner died during firing sequence)"), *GetName());
        return NULL;
    }
    
    checkSlow(ProjClass.IsValidIndex(CurrentFireMode) && ProjClass[CurrentFireMode] != NULL);
    if (Role == ROLE_Authority)
    {
        NZOwner->IncrementFlashCount(CurrentFireMode);
        ANZPlayerState* PS = NZOwner->Controller ? Cast<ANZPlayerState>(NZOwner->Controller->PlayerState) : NULL;
        if (PS && (ShotsStatsName != NAME_None))
        {
            PS->ModifyStatsValue(ShotsStatsName, 1);
        }
    }
    // Spawn the projectile at the muzzle
    const FVector SpawnLocation = GetFireStartLoc();
    const FRotator SpawnRotation = GetAdjustedAim(SpawnLocation);
    return SpawnNetPredictedProjectile(ProjClass[CurrentFireMode], SpawnLocation, SpawnRotation);
}

ANZProjectile* ANZWeapon::SpawnNetPredictedProjectile(TSubclassOf<ANZProjectile> ProjectileClass, FVector SpawnLocation, FRotator SpawnRotation)
{
    ANZPlayerController* OwningPlayer = NZOwner ? Cast<ANZPlayerController>(NZOwner->GetController()) : NULL;
    float CatchupTickDelta = ((GetNetMode() != NM_Standalone) && OwningPlayer) ? OwningPlayer->GetPredictionTime() : 0.f;
    
    if ((CatchupTickDelta > 0.f) && (Role != ROLE_Authority))
    {
        float SleepTime = OwningPlayer->GetProjectileSleepTime();
        if (SleepTime > 0.f)
        {
            // Lag is so high need to delay spawn
            if (!GetWorldTimerManager().IsTimerActive(SpawnDelayedFakeProjHandle))
            {
                DelayedProjectile.ProjectileClass = ProjectileClass;
                DelayedProjectile.SpawnLocation = SpawnLocation;
                DelayedProjectile.SpawnRotation = SpawnRotation;
                GetWorldTimerManager().SetTimer(SpawnDelayedFakeProjHandle, this, &ANZWeapon::SpawnDelayedFakeProjectile, SleepTime, false);
            }
            return NULL;
        }
    }
    
    FActorSpawnParameters Params;
    Params.Instigator = NZOwner;
    Params.Owner = NZOwner;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ANZProjectile* NewProjectile = ((Role == ROLE_Authority) || (CatchupTickDelta > 0.f)) ? GetWorld()->SpawnActor<ANZProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, Params) : NULL;
    if (NewProjectile)
    {
        if (NZOwner)
        {
            NZOwner->LastFiredProjectile = NewProjectile;
        }
        if (Role == ROLE_Authority)
        {
            NewProjectile->HitsStatsName = HitsStatsName;
            if ((CatchupTickDelta > 0.f) && NewProjectile->ProjectileMovement)
            {
                // Server ticks projectile to match with when client actually fired
                // TODO: account for CustomTimeDilation?
                if (NewProjectile->PrimaryActorTick.IsTickFunctionEnabled())
                {
                    NewProjectile->TickActor(CatchupTickDelta * NewProjectile->CustomTimeDilation, LEVELTICK_All, NewProjectile->PrimaryActorTick);
                }
                NewProjectile->ProjectileMovement->TickComponent(CatchupTickDelta * NewProjectile->CustomTimeDilation, LEVELTICK_All, NULL);
                NewProjectile->SetForwardTicked(true);
                if (NewProjectile->GetLifeSpan() > 0.f)
                {
                    NewProjectile->SetLifeSpan(FMath::Max(0.001f, NewProjectile->GetLifeSpan() - CatchupTickDelta));
                }
            }
            else
            {
                NewProjectile->SetForwardTicked(false);
            }
        }
        else
        {
            NewProjectile->InitFakeProjectile(OwningPlayer);
            NewProjectile->SetLifeSpan(FMath::Min(NewProjectile->GetLifeSpan(), 2.f * FMath::Max(0.f, CatchupTickDelta)));
        }
    }
    
    return NewProjectile;
}

bool ANZWeapon::HasAmmo(uint8 FireModeNum)
{
    return (AmmoCost.IsValidIndex(FireModeNum) && Ammo >= AmmoCost[FireModeNum]);
}

bool ANZWeapon::HasAnyAmmo()
{
    bool bHadCost = false;
    
    // Only consider zero cost firemodes as having ammo if they all have no cost
    // The assumption here is that for most weapons with an ammo-using firemode,
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

float ANZWeapon::GetRefireTime(uint8 FireModeNum)
{
    if (FireInterval.IsValidIndex(FireModeNum))
    {
        float Result = FireInterval[FireModeNum];
        if (NZOwner != NULL)
        {
            Result /= NZOwner->GetFireRateMultiplier();
        }
        return FMath::Max<float>(0.01f, Result);
    }
    else
    {
        //UE_LOG(NZ, Warning, TEXT("Invalid firing mode %i in %s::GetRefireTime()"), int32(FireModeNum), *GetName());
        return 0.1f;
    }
}

void ANZWeapon::GotoFireMode(uint8 NewFireMode)
{
    if (FiringState.IsValidIndex(NewFireMode))
    {
        CurrentFireMode = NewFireMode;
        GotoState(FiringState[NewFireMode]);
    }
}

bool ANZWeapon::IsFiring() const
{
    return CurrentState->IsFiring();
}

bool ANZWeapon::StackPickup_Implementation(ANZInventory* ContainedInv)
{
    AddAmmo(ContainedInv != NULL ? Cast<ANZWeapon>(ContainedInv)->Ammo : GetClass()->GetDefaultObject<ANZWeapon>()->Ammo);
    return true;
}

void ANZWeapon::UpdateTiming()
{
    CurrentState->UpdateTiming();
}

void ANZWeapon::Tick(float DeltaTime)
{
    // Try to gracefully detect being active with no owner, which should never happen because we should always end up going through Removed() and goint to the inactive state
    if (CurrentState != InactiveState && (NZOwner == NULL || NZOwner->IsPendingKillPending()) && CurrentState != NULL)
    {
        //UE_LOG(NZ, Warning, TEXT("%s lost Owner while active (state %s"), *GetName(), *GetNameSafe(CurrentState));
        GotoState(InactiveState);
    }
    
    Super::Tick(DeltaTime);
    
    // Note that this relies on us making BeginPlay() always called before first tick; see UNZGameEngine::LoadMap()
    if (CurrentState != InactiveState)
    {
        CurrentState->Tick(DeltaTime);
    }
    
    TickZoom(DeltaTime);
}

void ANZWeapon::Destroyed()
{
    Super::Destroyed();
    
    // This makes sure timers, etc go away
    GotoState(InactiveState);
}

void ANZWeapon::InstanceMuzzleFlashArray(AActor* Weap, TArray<UParticleSystemComponent*>& MFArray)
{
    TArray<const UBlueprintGeneratedClass*> ParentBPClassStack;
    UBlueprintGeneratedClass::GetGeneratedClassesHierarchy(Weap->GetClass(), ParentBPClassStack);
    for (int32 i = ParentBPClassStack.Num() - 1; i >= 0; i--)
    {
        const UBlueprintGeneratedClass* CurrentBPGClass = ParentBPClassStack[i];
        if (CurrentBPGClass->SimpleConstructionScript)
        {
            TArray<USCS_Node*> ConstructionNodes = CurrentBPGClass->SimpleConstructionScript->GetAllNodes();
            for (int32 j = 0; j < ConstructionNodes.Num(); j++)
            {
                for (int32 k = 0; k < MFArray.Num(); k++)
                {
                    if (Cast<UParticleSystemComponent>(ConstructionNodes[j]->ComponentTemplate) == MFArray[k])
                    {
                        MFArray[k] = Cast<UParticleSystemComponent>((UObject*)FindObjectWithOuter(Weap, ConstructionNodes[j]->ComponentTemplate->GetClass(), ConstructionNodes[j]->VariableName));
                    }
                }
            }
        }
    }
}

FLinearColor ANZWeapon::GetCrosshairColor(class UNZHUDWidget* WeaponHudWidget) const
{
    FLinearColor CrosshairColor = FLinearColor::White;
    return WeaponHudWidget->NZHUDOwner->GetCrosshairColor(CrosshairColor);
}

bool ANZWeapon::ShouldDrawFFIndicator(APlayerController* Viewer, ANZPlayerState*& HitPlayerState) const
{
    bool bDrawFriendlyIndicator = false;
    HitPlayerState = NULL;
    ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
    if (GameState != NULL)
    {
        FVector CameraLoc;
        FRotator CameraRot;
        Viewer->GetPlayerViewPoint(CameraLoc, CameraRot);
        FHitResult Hit;
        GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, CameraLoc + CameraRot.Vector() * 50000.0f, COLLISION_TRACE_WEAPON, FCollisionQueryParams(FName(TEXT("CrosshairFriendIndicator")), true, NZOwner));
        if (Hit.Actor != NULL)
        {
            ANZCharacter* Character = Cast<ANZCharacter>(Hit.Actor.Get());
            if (Character != NULL)
            {
                bDrawFriendlyIndicator = GameState->OnSameTeam(Hit.Actor.Get(), NZOwner);
                
                if (/*!Char->IsFeigningDeath()*/true)
                {
                    if (Character->PlayerState != NULL)
                    {
                        HitPlayerState = Cast<ANZPlayerState>(Character->PlayerState);
                    }
                    else if (Character->DrivenVehicle != NULL && Character->DrivenVehicle->PlayerState != NULL)
                    {
                        HitPlayerState = Cast<ANZPlayerState>(Character->DrivenVehicle->PlayerState);
                    }
                }
            }
        }
    }
    return bDrawFriendlyIndicator;
}

float ANZWeapon::GetCrosshairScale(class ANZHUD* HUD)
{
    return HUD->GetCrosshairScale();
}

void ANZWeapon::DrawWeaponCrosshair(UNZHUDWidget* WeaponHudWidget, float RenderDelta)
{
    // todo:
    check(false);
}

void ANZWeapon::UpdateCrosshairTarget(ANZPlayerState* NewCrosshairTarget, UNZHUDWidget* WeaponHudWidget, float RenderDelta)
{
    // todo:
    check(false);
}

void ANZWeapon::UpdateOverlaysShared(AActor* WeaponActor, ANZCharacter* InOwner, USkeletalMeshComponent* InMesh, const TArray<struct FParticleSysParam>& InOverlayEffectParams, USkeletalMeshComponent*& InOverlayMesh) const
{
    // todo:
    check(false);
}

void ANZWeapon::UpdateOverlays()
{
    UpdateOverlaysShared(this, GetNZOwner(), Mesh, OverlayEffectParams, OverlayMesh);
}

void ANZWeapon::SetSkin(UMaterialInterface* NewSkin)
{
    // FIXME: Workaround for poor GetNumMaterials() implementation
    int32 NumMats = Mesh->GetNumMaterials();
    if (Mesh->SkeletalMesh != NULL)
    {
        NumMats = FMath::Max<int32>(NumMats, Mesh->SkeletalMesh->Materials.Num());
    }
    
    // Save off existing materials if we haven't yet done so
    while (SavedMeshMaterials.Num() < NumMats)
    {
        SavedMeshMaterials.Add(Mesh->GetMaterial(SavedMeshMaterials.Num()));
    }
    
    if (NewSkin != NULL)
    {
        for (int32 i = 0; i < NumMats; i++)
        {
            Mesh->SetMaterial(i, NewSkin);
        }
    }
    else
    {
        for (int32 i = 0; i < NumMats; i++)
        {
            Mesh->SetMaterial(i, SavedMeshMaterials[i]);
        }
    }
}

bool ANZWeapon::ShouldLagRot()
{
    return bProceduralLagRotation;
}

float ANZWeapon::LagWeaponRotation(float NewValue, float LastValue, float DeltaTime, float MaxDiff, int32 Index)
{
    // todo:
    check(false);
    return 0.f;
}

void ANZWeapon::UpdateWeaponHand()
{
    if (Mesh != NULL && NZOwner != NULL)
    {
        FirstPMeshOffset = FVector::ZeroVector;
        FirstPMeshRotation = FRotator::ZeroRotator;
        
        if (MuzzleFlashDefaultTransforms.Num() == 0)
        {
            for (UParticleSystemComponent* PSC : MuzzleFlash)
            {
                MuzzleFlashDefaultTransforms.Add((PSC == NULL) ? FTransform::Identity : PSC->GetRelativeTransform());
                MuzzleFlashSocketNames.Add((PSC == NULL) ? NAME_None : PSC->AttachSocketName);
            }
        }
        else
        {
            for (int32 i = 0; i < FMath::Min3<int32>(MuzzleFlash.Num(), MuzzleFlashDefaultTransforms.Num(), MuzzleFlashSocketNames.Num()); i++)
            {
                if (MuzzleFlash[i] != NULL)
                {
                    MuzzleFlash[i]->AttachSocketName = MuzzleFlashSocketNames[i];
                    MuzzleFlash[i]->SetRelativeTransform(MuzzleFlashDefaultTransforms[i]);
                }
            }
        }
        
        Mesh->AttachSocketName = HandsAttachSocket;
        if (HandsAttachSocket == NAME_None)
        {
            NZOwner->FirstPersonMesh->SetRelativeTransform(FTransform::Identity);
        }
        else
        {
            USkeletalMeshComponent* DefaultHands = NZOwner->GetClass()->GetDefaultObject<ANZCharacter>()->FirstPersonMesh;
            NZOwner->FirstPersonMesh->RelativeLocation = DefaultHands->RelativeLocation;
            NZOwner->FirstPersonMesh->RelativeRotation = DefaultHands->RelativeRotation;
            NZOwner->FirstPersonMesh->RelativeScale3D = DefaultHands->RelativeScale3D;
            NZOwner->FirstPersonMesh->UpdateComponentToWorld();
        }
        
        USkeletalMeshComponent* AdjustMesh = (HandsAttachSocket != NAME_None) ? NZOwner->FirstPersonMesh : Mesh;
        USkeletalMeshComponent* AdjustMeshArchetype = Cast<USkeletalMeshComponent>(AdjustMesh->GetArchetype());
        
        switch (GetWeaponHand())
        {
            case HAND_Center:
                // TODO: not implemented, fallthrough
                //UE_LOG(NZ, Warning, TEXT("HAND_Center is not implemented yet!"));
            case HAND_Right:
                AdjustMesh->SetRelativeLocationAndRotation(AdjustMeshArchetype->RelativeLocation, AdjustMeshArchetype->RelativeRotation);
                break;
            case HAND_Left:
            {
                // TODO: should probably mirror, but mirroring breaks sockets at the moment (engine bug)
                AdjustMesh->SetRelativeLocation(AdjustMeshArchetype->RelativeLocation * FVector(1.0f, -1.0f, 1.0f));
                FRotator AdjustedRotation = (FRotationMatrix(AdjustMeshArchetype->RelativeRotation) * FScaleMatrix(FVector(1.0f, 1.0f, -1.0f))).Rotator();
                AdjustMesh->SetRelativeRotation(AdjustedRotation);
                break;
            }
            case HAND_Hidden:
            {
                AdjustMesh->SetRelativeLocationAndRotation(FVector(-50.0f, 0.0f, -50.0f), FRotator::ZeroRotator);
                if (AdjustMesh != Mesh)
                {
                    Mesh->AttachSocketName = NAME_None;
                    Mesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
                }
                for (int32 i = 0; i < MuzzleFlash.Num() && i < MuzzleFlashDefaultTransforms.Num(); i++)
                {
                    if (MuzzleFlash[i] != NULL)
                    {
                        MuzzleFlash[i]->AttachSocketName = NAME_None;
                        MuzzleFlash[i]->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
                    }
                }
                break;
            }
        }
    }
}

EWeaponHand ANZWeapon::GetWeaponHand() const
{
    if (NZOwner == NULL && Role == ROLE_Authority)
    {
        return HAND_Right;
    }
    else
    {
        ANZPlayerController* Viewer = NULL;
        if (NZOwner != NULL)
        {
            if (Role == ROLE_Authority)
            {
                Viewer = Cast<ANZPlayerController>(NZOwner->Controller);
            }
            if (Viewer == NULL)
            {
                Viewer = NZOwner->GetLocalViewer();
                if (Viewer == NULL && NZOwner->Controller != NULL && NZOwner->Controller->IsLocalPlayerController())
                {
                    // This can happen during initial replication; Controller might be set but ViewTarget not
                    Viewer = Cast<ANZPlayerController>(NZOwner->Controller);
                }
            }
        }
        return (Viewer != NULL) ? Viewer->GetWeaponHand() : HAND_Right;
    }
}

void ANZWeapon::UnEquip()
{
    GotoState(UnequippingState);
}

void ANZWeapon::GotoEquippingState(float OverflowTime)
{
    GotoState(EquippingState);
    if (CurrentState == EquippingState)
    {
        EquippingState->StartEquip(OverflowTime);
    }
}

