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
#include "NZHUDWidget.h"
#include "NZHUD.h"
#include "NZPlayerState.h"
#include "NZWeaponStateActive.h"
#include "NZWeaponStateInactive.h"
#include "NZWeaponStateFiring.h"
#include "NZWeaponStateFiringCharged.h"
#include "NZWeaponStateEquipping.h"
#include "NZWeaponStateUnequipping.h"
#include "NZWeaponStateZooming.h"
#include "NZWeaponAttachment.h"
#include "NZGameViewportClient.h"
#include "NZImpactEffect.h"
#include "UnrealNetwork.h"
#include "NZDamageType.h"
#include "NZPlayerCameraManager.h"
#include "NZImpactEffectManager.h"

ANZWeapon::ANZWeapon(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AmmoCost.Add(1);
    AmmoCost.Add(1);
    
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bAllowTickOnDedicatedServer = true;
    
    bWeaponStay = true;
    bCanThrowWeapon = true;
    
    ClipAmmoCount = 30;
    MaxCarriedAmmo = 60;
    
    Group = -1;
    BringUpTime = 0.37f;
    PutDownTime = 0.3f;
    RefirePutDownTimePercent = 1.0f;
    WeaponBobScaling = 1.f;
    FiringViewKickback = -20.0f;
    bNetDelayedShot = false;
    
    bFPFireFromCenter = true;
    bFPIgnoreInstantHitFireOffset = true;
    FireOffset = FVector(75.0f, 0.0f, 0.0f);
    FriendlyMomentumScaling = 1.f;
    FireEffectInterval = 1;
    FireEffectCount = 0;
    FireZOffset = 0.f;
    FireZOffsetTime = 0.f;
    MaxTracerDist = 5000.f;
    
    InactiveState = ObjectInitializer.CreateDefaultSubobject<UNZWeaponStateInactive>(this, TEXT("StateInactive"));
	ActiveState = ObjectInitializer.CreateDefaultSubobject<UNZWeaponStateActive>(this, TEXT("StateActive"));
	EquippingState = ObjectInitializer.CreateDefaultSubobject<UNZWeaponStateEquipping>(this, TEXT("StateEquipping"));
	UnequippingStateDefault = ObjectInitializer.CreateDefaultSubobject<UNZWeaponStateUnequipping>(this, TEXT("StateUnequipping"));
	UnequippingState = UnequippingStateDefault;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh->SetOnlyOwnerSee(true);
	Mesh->SetupAttachment(RootComponent);
    //Mesh->bCastInsetShadow = true;
	Mesh->bSelfShadowOnly = true;
	Mesh->bReceivesDecals = false;
	Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	FirstPMeshOffset = FVector(0.f);
	FirstPMeshRotation = FRotator(0.f, 0.f, 0.f);

	for (int32 i = 0; i < 2; i++)
	{
		UNZWeaponStateFiring* NewState = ObjectInitializer.CreateDefaultSubobject<UNZWeaponStateFiring, UNZWeaponStateFiring>(this, FName(*FString::Printf(TEXT("FiringState%i"), i)), false);
		if (NewState != NULL)
		{
			FiringState.Add(NewState);
			FireInterval.Add(1.0f);
			Spread.Add(0.0f);
		}
	}

	RotChgSpeed = 3.f;
	ReturnChgSpeed = 3.f;
	MaxYawLag = 4.4f;
	MaxPitchLag = 3.3f;
	FOVOffset = FVector(1.f);
	bProceduralLagRotation = true;
    
	//static ConstructorHelpers::FObjectFinder<UTexture> WeaponTexture(TEXT("Texture2D''"));
	//HUDIcon.Texture = WeaponTexture.Object;

	BaseAISelectRating = 0.55f;
	DisplayName = NSLOCTEXT("PickupMessage", "WeaponPickedUp", "Weapon");
	bShowPowerupTimer = false;
}

void ANZWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(ANZWeapon, Ammo, COND_None);
    DOREPLIFETIME_CONDITION(ANZWeapon, ClipAmmoCount, COND_None);
    DOREPLIFETIME(ANZWeapon, AttachmentType);
    
    DOREPLIFETIME_CONDITION(ANZWeapon, ZoomCount, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(ANZWeapon, ZoomState, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(ANZWeapon, CurrentZoomMode, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(ANZWeapon, ZoomTime, COND_InitialOnly);
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

void ANZWeapon::OnRep_CarriedAmmo()
{

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
    if (NZOwner != NULL)
    {
        NZOwner->SetHolsteredWeaponAttachmentClass(AttachmentType);
        NZOwner->UpdateHolsteredWeaponAttachment();
    }
}

void ANZWeapon::DetachFromHolster()
{
    if (NZOwner != NULL)
    {
        NZOwner->SetHolsteredWeaponAttachmentClass(NULL);
        NZOwner->UpdateHolsteredWeaponAttachment();
    }
}

void ANZWeapon::DropFrom(const FVector& StartLocation, const FVector& TossVelocity)
{
    if (Role == ROLE_Authority)
    {
        if (NZOwner != NULL && bMustBeHolstered)
        {
            DetachFromHolster();
        }
        
        if (!HasAnyAmmo())
        {
            Destroy();
        }
        else
        {
            Super::DropFrom(StartLocation, TossVelocity);
            if (NZOwner == NULL && CurrentState != InactiveState)
            {
                //UE_LOG(NZ, Warning, TEXT("Weapon %s wasn't properly sent to Inactive state after being dropped!"), *GetName());
                GotoState(InactiveState);
            }
        }
    }
}

void ANZWeapon::InitializeDroppedPickup(class ANZDroppedPickup* Pickup)
{
    Super::InitializeDroppedPickup(Pickup);
    
/*    if (Pickup != NULL)
    {
        Pickup->SetWeaponSkin(WeaponSkin);
    }*/
}

bool ANZWeapon::ShouldDropOnDeath()
{
    return (DroppedPickupClass != NULL) && HasAnyAmmo();
}

bool ANZWeapon::ShouldPlay1PVisuals() const
{
    if (GetNetMode() == NM_DedicatedServer)
    {
        return false;
    }
    else
    {
        // Note we can't check Mesh->LastRenderTime here because of the hidden weapon setting
        return NZOwner && NZOwner->GetLocalViewer() && !NZOwner->GetLocalViewer()->IsBehindView();
    }
}

void ANZWeapon::PlayPredictedImpactEffects(FVector ImpactLoc)
{
    if (!NZOwner)
    {
        return;
    }
    
    ANZPlayerController* NZPC = Cast<ANZPlayerController>(NZOwner->Controller);
    float SleepTime = NZPC ? NZPC->GetProjectileSleepTime() : 0.f;
    if (SleepTime > 0.f)
    {
        if (GetWorldTimerManager().IsTimerActive(PlayDelayedImpactEffectsHandle))
        {
            // Play the delayed effect now, since we are about to replace it
            PlayDelayedImpactEffects();
        }
        FVector SpawnLocation;
        FRotator SpawnRotation;
        GetImpactSpawnPosition(ImpactLoc, SpawnLocation, SpawnRotation);
        DelayedHitScan.ImpactLocation = ImpactLoc;
        DelayedHitScan.FireMode = CurrentFireMode;
        DelayedHitScan.SpawnLocation = SpawnLocation;
        DelayedHitScan.SpawnRotation = SpawnRotation;
        GetWorldTimerManager().SetTimer(PlayDelayedImpactEffectsHandle, this, &ANZWeapon::PlayDelayedImpactEffects, SleepTime, false);
    }
    else
    {
        FVector SpawnLocation;
        FRotator SpawnRotation;
        GetImpactSpawnPosition(ImpactLoc, SpawnLocation, SpawnRotation);
        NZOwner->SetFlashLocation(ImpactLoc, CurrentFireMode);
    }
}

void ANZWeapon::PlayDelayedImpactEffects()
{
    if (NZOwner)
    {
        NZOwner->SetFlashLocation(DelayedHitScan.ImpactLocation, DelayedHitScan.FireMode);
    }
}

void ANZWeapon::SpawnDelayedFakeProjectile()
{
    ANZPlayerController* OwningPlayer = NZOwner ? Cast<ANZPlayerController>(NZOwner->GetController()) : NULL;
    if (OwningPlayer)
    {
        FActorSpawnParameters Params;
        Params.Instigator = NZOwner;
        Params.Owner = NZOwner;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        ANZProjectile* NewProjectile = GetWorld()->SpawnActor<ANZProjectile>(DelayedProjectile.ProjectileClass, DelayedProjectile.SpawnLocation, DelayedProjectile.SpawnRotation, Params);
        if (NewProjectile)
        {
            NewProjectile->InitFakeProjectile(OwningPlayer);
            NewProjectile->SetLifeSpan(FMath::Min(NewProjectile->GetLifeSpan(), 0.002f * FMath::Max(0.f, OwningPlayer->MaxPredictionPing + OwningPlayer->PredictionFudgeFactor)));
        }
    }
}

float ANZWeapon::GetBringUpTime()
{
    return BringUpTime;
}

float ANZWeapon::GetPutDownTime()
{
    return PutDownTime;
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
    if (NZOwner)
    {
        NZOwner->DeactivateSpawnProtection();
    }
}

void ANZWeapon::UpdateViewBob(float DeltaTime)
{
    return;
    ANZPlayerController* MyPC = NZOwner ? NZOwner->GetLocalViewer() : NULL;
    if (MyPC != NULL && Mesh != NULL && NZOwner->GetWeapon() == this && ShouldPlay1PVisuals())
    {
        // If weapon is up in first person, view bob with movement
        if (GetWeaponHand() != HAND_Hidden)
        {
            USkeletalMeshComponent* BobbedMesh = (HandsAttachSocket != NAME_None) ? NZOwner->FirstPersonMesh : Mesh;
            if (FirstPMeshOffset.IsZero())
            {
                FirstPMeshOffset = BobbedMesh->GetRelativeTransform().GetLocation();
                FirstPMeshRotation = BobbedMesh->GetRelativeTransform().Rotator();
            }
            FVector ScaledMeshOffset = FirstPMeshOffset;
            const float FOVScaling = (MyPC != NULL) ? ((MyPC->PlayerCameraManager->GetFOVAngle() - 100.f) * 0.05f) : 1.0f;
            if (FOVScaling > 0.f)
            {
                ScaledMeshOffset.X *= (1.f + (FOVOffset.X - 1.f) * FOVScaling);
                ScaledMeshOffset.Y *= (1.f + (FOVOffset.Y - 1.f) * FOVScaling);
                ScaledMeshOffset.Z *= (1.f + (FOVOffset.Z - 1.f) * FOVScaling);
            }
            
            BobbedMesh->SetRelativeLocation(ScaledMeshOffset);
            FVector BobOffset = NZOwner->GetWeaponBobOffset(DeltaTime, this);
            BobbedMesh->SetWorldLocation(BobbedMesh->GetComponentLocation() + BobOffset);
            
            FRotator NewRotation = NZOwner ? NZOwner->GetControlRotation() : FRotator(0.f, 0.f, 0.f);
            FRotator FinalRotation = NewRotation;
            
            // Add some rotation leading
            if (NZOwner && NZOwner->Controller)
            {
                FinalRotation.Yaw = LagWeaponRotation(NewRotation.Yaw, LastRotation.Yaw, DeltaTime, MaxYawLag, 0);
                FinalRotation.Pitch = LagWeaponRotation(NewRotation.Pitch, LastRotation.Pitch, DeltaTime, MaxPitchLag, 1);
                FinalRotation.Roll = NewRotation.Roll;
            }
            LastRotation = NewRotation;
            BobbedMesh->SetRelativeRotation(FinalRotation + FirstPMeshRotation);
        }
        else
        {
            // For first person footsteps
            NZOwner->GetWeaponBobOffset(DeltaTime, this);
        }
    }
}

void ANZWeapon::PostInitProperties()
{
    Super::PostInitProperties();
    
    if (Group == -1)
    {
        Group = DefaultGroup;
    }
    WeaponBarScale = 0.0f;
    
    if (DisplayName.IsEmpty() ||
        (GetClass() != ANZWeapon::StaticClass() && DisplayName.EqualTo(GetClass()->GetSuperClass()->GetDefaultObject<ANZWeapon>()->DisplayName) &&
         GetClass()->GetSuperClass()->GetDefaultObject<ANZWeapon>()->DisplayName.EqualTo(FText::FromName(GetClass()->GetSuperClass()->GetFName()))))
    {
        DisplayName = FText::FromName(GetClass()->GetFName());
    }
}

void ANZWeapon::BeginPlay()
{
    Super::BeginPlay();
    
    Ammo = ClipAmmoCount;
    CarriedAmmo = MaxCarriedAmmo;
    
    if (ImpactEffectManager == NULL && ImpactEffectManagerClass != NULL)
    {
		//ImpactEffectManager = Cast<UNZImpactEffectManager>(StaticConstructObject(ImpactEffectManagerClass));
		ImpactEffectManager = NewObject<UNZImpactEffectManager>(this, ImpactEffectManagerClass);
    }
    
    InstanceMuzzleFlashArray(this, MuzzleFlash);
    
    // Sanity check some settings
    for (int32 i = 0; i < MuzzleFlash.Num(); i++)
    {
        if (MuzzleFlash[i] != NULL)
        {
            if (RootComponent == NULL && MuzzleFlash[i]->IsRegistered())
            {
                MuzzleFlash[i]->DeactivateSystem();
                MuzzleFlash[i]->KillParticlesForced();
                MuzzleFlash[i]->UnregisterComponent();  // SCS components were registered without out permission
                MuzzleFlash[i]->bWasActive = false;
            }
            MuzzleFlash[i]->bAutoActivate = false;
            MuzzleFlash[i]->SecondsBeforeInactive = 0.0f;
            MuzzleFlash[i]->SetOnlyOwnerSee(false);     // We handle this in ANZPlayerController::UpdateHiddenComponents() instead
            MuzzleFlash[i]->bUseAttachParentBound = true;
        }
    }
    
    // Might have already been activated if at startup, see ClientGivenTo_Internal()
    if (CurrentState == NULL)
    {
        GotoState(InactiveState);
    }
    
    checkSlow(CurrentState != NULL);
}

UMeshComponent* ANZWeapon::GetPickupMeshTemplate_Implementation(FVector& OverrideScale) const
{
    if (AttachmentType != NULL)
    {
        OverrideScale = AttachmentType.GetDefaultObject()->PickupScaleOverride;
        return AttachmentType.GetDefaultObject()->Mesh;
    }
    else
    {
        return Super::GetPickupMeshTemplate_Implementation(OverrideScale);
    }
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
    if (!NZOwner->IsFiringDisabled())
    {
        bool bClientFired = BeginFiringSequence(FireModeNum, false);
        if (Role < ROLE_Authority)
        {
            if (NZOwner)
            {
                float ZOffset = uint8(FMath::Clamp(NZOwner->GetPawnViewLocation().Z - NZOwner->GetActorLocation().Z + 127.5f, 0.f, 255.f));
                if (ZOffset != uint8(FMath::Clamp(NZOwner->BaseEyeHeight + 127.5f, 0.f, 255.f)))
                {
                    ServerStartFireOffset(FireModeNum, ZOffset, bClientFired);
                    return;
                }
            }
            ServerStartFire(FireModeNum, bClientFired);
        }
    }
}

void ANZWeapon::StopFire(uint8 FireModeNum)
{
    EndFiringSequence(FireModeNum);
    if (Role < ROLE_Authority)
    {
        ServerStopFire(FireModeNum);
    }
}

void ANZWeapon::ServerStartFire_Implementation(uint8 FireModeNum, bool bClientFired)
{
    if (NZOwner && !NZOwner->IsFiringDisabled())
    {
        FireZOffsetTime = 0.f;
        BeginFiringSequence(FireModeNum, bClientFired);
    }
}

bool ANZWeapon::ServerStartFire_Validate(uint8 FireModeNum, bool bClientFired)
{
    return true;
}

void ANZWeapon::ServerStartFireOffset_Implementation(uint8 FireModeNum, uint8 ZOffset, bool bClientFired)
{
    if (NZOwner && !NZOwner->IsFiringDisabled())
    {
        FireZOffset = ZOffset - 127;
        FireZOffsetTime = GetWorld()->GetTimeSeconds();
        BeginFiringSequence(FireModeNum, bClientFired);
    }
}

bool ANZWeapon::ServerStartFireOffset_Validate(uint8 FireModeNum, uint8 ZOffset, bool bClientFired)
{
    return true;
}

void ANZWeapon::ServerStopFire_Implementation(uint8 FireModeNum)
{
    EndFiringSequence(FireModeNum);
}

bool ANZWeapon::ServerStopFire_Validate(uint8 FireModeNum)
{
    return true;
}

bool ANZWeapon::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
	if (FireModeNum == 0)
	{
		UE_LOG(NZ, Warning, TEXT("terryzhong test!"));
	}

    if (NZOwner)
    {
        NZOwner->SetPendingFire(FireModeNum, true);
        if (FiringState.IsValidIndex(FireModeNum) && CurrentState != EquippingState && CurrentState != UnequippingState)
        {
            FiringState[FireModeNum]->PendingFireStarted();
        }
        bool bResult = CurrentState->BeginFiringSequence(FireModeNum, bClientFired);
        if (CurrentState->IsFiring() && CurrentFireMode != FireModeNum)
        {
            OnMultiPress(FireModeNum);
        }
        return bResult;
    }
    return false;
}

void ANZWeapon::EndFiringSequence(uint8 FireModeNum)
{
    if (NZOwner)
    {
        NZOwner->SetPendingFire(FireModeNum, false);
    }
    if (FiringState.IsValidIndex(FireModeNum) && CurrentState != EquippingState && CurrentState != UnequippingState)
    {
        FiringState[FireModeNum]->PendingFireStopped();
    }
    CurrentState->EndFiringSequence(FireModeNum);
}

bool ANZWeapon::WillSpawnShot(float DeltaTime)
{
    return (CurrentState != NULL) && CanFireAgain() && CurrentState->WillSpawnShot(DeltaTime);
}

bool ANZWeapon::CanFireAgain()
{
    return (GetNZOwner() && (GetNZOwner()->GetPendingWeapon() == NULL) && HasAmmo(GetCurrentFireMode()));
}

void ANZWeapon::OnStartedFiring_Implementation()
{
}

void ANZWeapon::OnStoppedFiring_Implementation()
{
}

bool ANZWeapon::HandleContinuedFiring()
{
    if (!CanFireAgain() || !GetNZOwner()->IsPendingFire(GetCurrentFireMode()))
    {
        GotoActiveState();
        return false;
    }
    
    OnContinuedFiring();
    return true;
}

void ANZWeapon::OnContinuedFiring_Implementation()
{
}

void ANZWeapon::OnMultiPress_Implementation(uint8 OtherFireMode)
{
}

void ANZWeapon::BringUp(float OverflowTime)
{
    AttachToOwner();
    OnBringUp();
    if (CurrentState)
    {
        CurrentState->BringUp(OverflowTime);
    }
}

bool ANZWeapon::PutDown()
{
    if (eventPreventPutDown())
    {
        return false;
    }
    else
    {
        SetZoomState(EZoomState::EZS_NotZoomed);
        CurrentState->PutDown();
        return true;
    }
}

void ANZWeapon::AttachToOwner_Implementation()
{
    if (NZOwner == NULL)
    {
        return;
    }
    
    if (bMustBeHolstered)
    {
        // Detach from holster if becoming held
        DetachFromHolster();
    }
    
    // Attach
    if (Mesh != NULL && Mesh->SkeletalMesh != NULL)
    {
        UpdateWeaponHand();

		// 如果武器设置了手的
		if (NZOwner->FirstPersonMesh != NULL && AnimClassHands != NULL)
		{
			NZOwner->FirstPersonMesh->SetAnimInstanceClass(AnimClassHands);
		}

       //Mesh->AttachTo(NZOwner->FirstPersonMesh, (GetWeaponHand() != HAND_Hidden) ? HandsAttachSocket : NAME_None);
		Mesh->AttachToComponent(NZOwner->FirstPersonMesh, FAttachmentTransformRules::KeepRelativeTransform, (GetWeaponHand() != HAND_Hidden) ? HandsAttachSocket : NAME_None);
		if (ShouldPlay1PVisuals())
        {
            Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
            Mesh->LastRenderTime = GetWorld()->TimeSeconds;
            Mesh->bRecentlyRendered = true;
            if (OverlayMesh != NULL)
            {
                OverlayMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
                OverlayMesh->LastRenderTime = GetWorld()->TimeSeconds;
                OverlayMesh->bRecentlyRendered = true;
            }
            UpdateViewBob(0.0f);
        }
    }
    
    // Register components now
    Super::RegisterAllComponents();
    RegisterAllActorTickFunctions(true, true);
    if (GetNetMode() != NM_DedicatedServer)
    {
        UpdateOverlays();
        SetSkin(NZOwner->GetSkin());
    }
}

void ANZWeapon::DetachFromOwner_Implementation()
{
    StopFiringEffects();
    
    for (int32 i = 0; i < MuzzleFlash.Num(); i++)
    {
        if (MuzzleFlash[i] != NULL)
        {
            UParticleSystem* SavedTemplate = MuzzleFlash[i]->Template;
            MuzzleFlash[i]->DeactivateSystem();
            MuzzleFlash[i]->KillParticlesForced();
            // FIXME: KillParticlesForced() doesn't kill particles immediately for GPU particles, but the below does...
            MuzzleFlash[i]->SetTemplate(NULL);
            MuzzleFlash[i]->SetTemplate(SavedTemplate);
        }
    }
    
    if (Mesh != NULL && Mesh->SkeletalMesh != NULL)
    {
        Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
        //Mesh->DetachFromParent();
		Mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
        if (OverlayMesh != NULL)
        {
            OverlayMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
        }
    }
    
    // Unregister components so they go away
    UnregisterAllComponents();
    
    if (bMustBeHolstered && HasAnyAmmo() && NZOwner && !NZOwner->IsDead() && !IsPendingKillPending())
    {
        AttachToHolster();
    }
}

bool ANZWeapon::IsChargedFireMode(uint8 TestMode) const
{
	return FiringState.IsValidIndex(TestMode) && Cast<UNZWeaponStateFiringCharged>(FiringState[TestMode]) != NULL;
}

void ANZWeapon::GivenTo(ANZCharacter* NewOwner, bool bAutoActivate)
{
	Super::GivenTo(NewOwner, bAutoActivate);

	// If characters has ammo on it, transfer to weapon
	for (int32 i = 0; i < NewOwner->SavedAmmo.Num(); i++)
	{
		if (NewOwner->SavedAmmo[i].Type == GetClass())
		{
			AddAmmo(NewOwner->SavedAmmo[i].Amount);
			NewOwner->SavedAmmo.RemoveAt(i);
			break;
		}
	}
}

void ANZWeapon::ClientGivenTo_Internal(bool bAutoActivate)
{
	if (bMustBeHolstered && NZOwner && HasAnyAmmo())
	{
		AttachToHolster();
	}

	// Make sure we initialized our state; this can be triggered if the weapon is spawned at game startup, since BeginPlay() will be deferred
	if (CurrentState == NULL)
	{
		GotoState(InactiveState);
	}

	Super::ClientGivenTo_Internal(bAutoActivate);

	// Grab our switch priority
	ANZPlayerController* NZPC = Cast<ANZPlayerController>(NZOwner->Controller);
	if (NZPC != NULL)
	{
		AutoSwitchPriority = NZPC->GetWeaponAutoSwitchPriority(GetNameSafe(this), AutoSwitchPriority);
		NZPC->SetWeaponGroup(this);
	}

    // Assign GroupSlot it required
    int32 MaxGroupSlot = 0;
    bool bDuplicateSlot = false;
    for (TInventoryIterator<ANZWeapon> It(NZOwner); It; ++It)
    {
        if (*It != this && It->Group == Group)
        {
            MaxGroupSlot = FMath::Max<int32>(MaxGroupSlot, It->GroupSlot);
            bDuplicateSlot = bDuplicateSlot || (GroupSlot == It->GroupSlot);
        }
    }

    if (bDuplicateSlot)
    {
        GroupSlot = MaxGroupSlot + 1;
    }
    
    if (bAutoActivate && NZPC != NULL)
    {
        NZPC->CheckAutoWeaponSwitch(this);
    }
}

void ANZWeapon::Removed()
{
    GotoState(InactiveState);
    DetachFromOwner();
    if (bMustBeHolstered)
    {
        DetachFromHolster();
    }
    
    Super::Removed();
}

void ANZWeapon::ClientRemoved_Implementation()
{
    GotoState(InactiveState);
    if (Role < ROLE_Authority)
    {
        DetachFromOwner();
        if (bMustBeHolstered)
        {
            DetachFromHolster();
        }
    }
    
    ANZCharacter* OldOwner = NZOwner;
    
    Super::ClientRemoved_Implementation();
    
    if (OldOwner != NULL && (OldOwner->GetWeapon() == this || OldOwner->GetPendingWeapon() == this))
    {
        OldOwner->ClientWeaponLost(this);
    }
}

void ANZWeapon::FireShot()
{
    NZOwner->DeactivateSpawnProtection();
    ConsumeAmmo(CurrentFireMode);
    
    if (!FireShotOverride() && GetNZOwner() != NULL)    // Script event may kill user
    {
        if (ProjClass.IsValidIndex(CurrentFireMode) && ProjClass[CurrentFireMode] != NULL)
        {
            FireProjectile();
        }
        else if (InstantHitInfo.IsValidIndex(CurrentFireMode) && InstantHitInfo[CurrentFireMode].DamageType != NULL)
        {
            FireInstantHit();
        }
        PlayFiringEffects();
    }
    if (GetNZOwner() != NULL)
    {
        GetNZOwner()->InventoryEvent(InventoryEventName::FiredWeapon);
    }
    FireZOffsetTime = 0.f;
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
            UNZGameplayStatics::NZPlaySound(GetWorld(), FireSound[EffectFiringMode], NZOwner, SRT_AllButOwner);
        }
        
        // Reload sound on local shooter
        if ((GetNetMode() != NM_DedicatedServer) && NZOwner && NZOwner->GetLocalViewer() &&
            ReloadSound.IsValidIndex(EffectFiringMode) && ReloadSound[EffectFiringMode] != NULL)
        {
            UNZGameplayStatics::NZPlaySound(GetWorld(), ReloadSound[EffectFiringMode], NZOwner, SRT_None);
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
    if (GetNetMode() != NM_DedicatedServer)
    {
        // Fire effects
        static FName NAME_HitLocation(TEXT("HitLocation"));
        static FName NAME_LocalHitLocation(TEXT("LocalHitLocation"));
        FireEffectCount++;
        if (FireEffect.IsValidIndex(FireMode) && (FireEffect[FireMode] != NULL) && (FireEffectCount >= FireEffectInterval))
        {
            FVector AdjustedSpawnLocation = SpawnLocation;
            // Panini project the location, if necessary
            if (Mesh != NULL)
            {
                for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
                {
                    if (It->PlayerController != NULL && It->PlayerController->GetViewTarget() == NZOwner)
                    {
                        UNZGameViewportClient* NZViewport = Cast<UNZGameViewportClient>(It->ViewportClient);
                        if (NZViewport != NULL)
                        {
                            AdjustedSpawnLocation = NZViewport->PaniniProjectLocationForPlayer(*It, SpawnLocation, Mesh->GetMaterial(0));
                            break;
                        }
                    }
                }
            }
            FireEffectCount = 0;
            UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect[FireMode], AdjustedSpawnLocation, SpawnRotation, true);
            
            // Limit dist to target
            FVector AdjustedTargetLoc = ((TargetLoc - AdjustedSpawnLocation).SizeSquared() > 4000000.f) ? AdjustedSpawnLocation + MaxTracerDist * (TargetLoc - AdjustedSpawnLocation).GetSafeNormal() : TargetLoc;
            PSC->SetVectorParameter(NAME_HitLocation, AdjustedTargetLoc);
            PSC->SetVectorParameter(NAME_LocalHitLocation, PSC->ComponentToWorld.InverseTransformPosition(AdjustedTargetLoc));
            ModifyFireEffect(PSC);
        }
        // Perhaps the muzzle flash also contains hit effect (constant beam, etc) so set the parameter on it instead
        else if (MuzzleFlash.IsValidIndex(FireMode) && MuzzleFlash[FireMode] != NULL)
        {
            MuzzleFlash[FireMode]->SetVectorParameter(NAME_HitLocation, TargetLoc);
            MuzzleFlash[FireMode]->SetVectorParameter(NAME_LocalHitLocation, MuzzleFlash[FireMode]->ComponentToWorld.InverseTransformPositionNoScale(TargetLoc));
        }
        
        // Always spawn effects instigated by local player unless beyond cull distance
        if ((TargetLoc - LastImpactEffectLocation).Size() >= ImpactEffectSkipDistance ||
            GetWorld()->TimeSeconds - LastImpactEffectTime >= MaxImpactEffectSkipTime)
        {
            FHitResult ImpactHit = GetImpactEffectHit(NZOwner, SpawnLocation, TargetLoc);
            if (ImpactHit.Component.IsValid() && !CancelImpactEffect(ImpactHit))
            {
                if (ImpactEffectManager != NULL)
                {
                    TSubclassOf<ANZImpactEffect> ImpactEffect = ImpactEffectManager->GetImpactEffectFromPhysicalMaterial(ImpactHit.PhysMaterial.Get());
                    if (ImpactEffect != NULL)
                    {
                        ImpactEffect.GetDefaultObject()->SpawnEffect(GetWorld(), FTransform(ImpactHit.Normal.Rotation(), ImpactHit.Location), ImpactHit.Component.Get(), NULL, NZOwner->Controller);
                    }
                }
            }
           
            LastImpactEffectLocation = TargetLoc;
            LastImpactEffectTime = GetWorld()->TimeSeconds;
        }
    }
}

FHitResult ANZWeapon::GetImpactEffectHit(APawn* Shooter, const FVector& StartLoc, const FVector& TargetLoc)
{
    // Trace for precise hit location and hit normal
    FHitResult Hit;
    FVector TargetToGun = (StartLoc - TargetLoc).GetSafeNormal();
    FCollisionQueryParams QueryParams(FName(TEXT("ImpactEffect")), true, Shooter);
    QueryParams.bReturnPhysicalMaterial = true;
    if (Shooter->GetWorld()->LineTraceSingleByChannel(Hit, TargetLoc + TargetToGun * 32.0f, TargetLoc - TargetToGun * 32.0f, COLLISION_TRACE_WEAPON, QueryParams))
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
        Ammo = FMath::Clamp<int32>(Ammo + Amount, 0, ClipAmmoCount);
        
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
    
    const FVector StartTrace = InstantFireStartTrace();
    const FVector EndTrace = InstantFireEndTrace(StartTrace);
    const FVector FireDir = (EndTrace - StartTrace).GetSafeNormal();
    
    FHitResult Hit;
    ANZPlayerController* NZPC = Cast<ANZPlayerController>(NZOwner->Controller);
    ANZPlayerState* PS = NZOwner->Controller ? Cast<ANZPlayerState>(NZOwner->Controller->PlayerState) : NULL;
    float PredictionTime = NZPC ? NZPC->GetPredictionTime() : 0.f;
    HitScanTrace(StartTrace, EndTrace, InstantHitInfo[CurrentFireMode].TraceHalfSize, Hit, PredictionTime);
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
        Hit.Actor->TakeDamage(InstantHitInfo[CurrentFireMode].Damage,
                              FNZPointDamageEvent(InstantHitInfo[CurrentFireMode].Damage, Hit, FireDir, InstantHitInfo[CurrentFireMode].DamageType, FireDir * GetImpartedMomentumMag(Hit.Actor.Get())),
                              NZOwner->Controller,
                              this);
    }
    if (OutHit != NULL)
    {
        *OutHit = Hit;
    }
}

FVector ANZWeapon::InstantFireStartTrace()
{
    return GetFireStartLoc();
}

FVector ANZWeapon::InstantFireEndTrace(FVector StartTrace)
{
    const FVector FireDir = GetAdjustedAim(StartTrace).Vector();
    return StartTrace + FireDir * InstantHitInfo[CurrentFireMode].TraceRange;
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
            if (Target && (Target != NZOwner) && !Target->IsDead())
            {
                FVector TargetLocation = ((PredictionTime > 0.f) && (Role == ROLE_Authority)) ? Target->GetRewindLocation(PredictionTime) : Target->GetActorLocation();
                
                // now see if trace would hit the capsule
                float CollisionHeight = Target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
                // todo:
                //if (Target->NZCharacterMovement && Target->NZCharacterMovement->bIsFloorSliding)
                //{
                //    TargetLocation.Z = TargetLocation.Z - CollisionHeight + Target->SlideTargetHeight;
                //    CollisionHeight = Target->SlideTargetHeight;
                //}

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

void ANZWeapon::GotoActiveState()
{
    GotoState(ActiveState);
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
                        MFArray[k] = Cast<UParticleSystemComponent>((UObject*)FindObjectWithOuter(Weap, ConstructionNodes[j]->ComponentTemplate->GetClass(), ConstructionNodes[j]->GetVariableName()));
                    }
                }
            }
        }
    }
}

void ANZWeapon::NotifyKillWhileHolding_Implementation(TSubclassOf<UDamageType> DmgType)
{
    
}

bool ANZWeapon::NeedsAmmoDisplay_Implementation() const
{
    for (int32 i = GetNumFireModes() - 1; i >= 0; i--)
    {
        if (AmmoCost[i] > 0)
        {
            return true;
        }
    }
    return false;
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

void ANZWeapon::DrawWeaponCrosshair_Implementation(UNZHUDWidget* WeaponHudWidget, float RenderDelta)
{
	bool bDrawCrosshair = true;
	for (int32 i = 0; i < FiringState.Num(); i++)
	{
		bDrawCrosshair = FiringState[i]->DrawHUD(WeaponHudWidget) && bDrawCrosshair;
	}

	if (bDrawCrosshair && WeaponHudWidget && WeaponHudWidget->NZHUDOwner)
	{
		UTexture2D* CrosshairTexture = WeaponHudWidget->NZHUDOwner->DefaultCrosshairTex;
		if (CrosshairTexture != NULL)
		{
			float W = CrosshairTexture->GetSurfaceWidth();
			float H = CrosshairTexture->GetSurfaceHeight();
			float CrosshairScale = GetCrosshairScale(WeaponHudWidget->NZHUDOwner);

			// Draw a different indicator if there is a friendly where the camera is pointing
			ANZPlayerState* PS;
			if (ShouldDrawFFIndicator(WeaponHudWidget->NZHUDOwner->PlayerOwner, PS))
			{
				WeaponHudWidget->DrawTexture(WeaponHudWidget->NZHUDOwner->HUDAtlas, 0, 0, W * CrosshairScale, H * CrosshairScale, 407, 940, 72, 72, 1.0, FLinearColor::Green, FVector2D(0.5f, 0.5f));
			}
			else
			{
				UNZCrosshair* Crosshair = WeaponHudWidget->NZHUDOwner->GetCrosshair(this);
				FCrosshairInfo* CrosshairInfo = WeaponHudWidget->NZHUDOwner->GetCrosshairInfo(this);

				if (Crosshair != NULL && CrosshairInfo != NULL)
				{
					Crosshair->DrawCrosshair(WeaponHudWidget->GetCanvas(), this, RenderDelta, GetCrosshairScale(WeaponHudWidget->NZHUDOwner) * CrosshairInfo->Scale, WeaponHudWidget->NZHUDOwner->GetCrosshairColor(CrosshairInfo->Color));
				}
				else
				{
					WeaponHudWidget->DrawTexture(CrosshairTexture, 0, 0, W * CrosshairScale, H * CrosshairScale, 0.0, 0.0, 16, 16, 1.0, GetCrosshairColor(WeaponHudWidget), FVector2D(0.5f, 0.5f));
				}
				UpdateCrosshairTarget(PS, WeaponHudWidget, RenderDelta);
			}
		}
	}
}

void ANZWeapon::UpdateCrosshairTarget(ANZPlayerState* NewCrosshairTarget, UNZHUDWidget* WeaponHudWidget, float RenderDelta)
{
	if (NewCrosshairTarget != NULL)
	{
		TargetPlayerState = NewCrosshairTarget;
		TargetLastSeenTime = GetWorld()->GetTimeSeconds();
	}

	if (TargetPlayerState != NULL)
	{
		float TimeSinceSeen = GetWorld()->GetTimeSeconds() - TargetLastSeenTime;
		static float MAXNAMEDRAWTIME = 0.3f;
		if (TimeSinceSeen < MAXNAMEDRAWTIME)
		{
			static float MAXNAMEFULLALPHA = 0.22f;
			float Alpha = (TimeSinceSeen < MAXNAMEFULLALPHA) ? 1.f : (1.f - ((TimeSinceSeen - MAXNAMEFULLALPHA) / (MAXNAMEDRAWTIME - MAXNAMEFULLALPHA)));

			float H = WeaponHudWidget->NZHUDOwner->DefaultCrosshairTex->GetSurfaceHeight();
			FText PlayerName = FText::FromString(TargetPlayerState->PlayerName);
			WeaponHudWidget->DrawText(PlayerName, 0.f, H * 2.f, WeaponHudWidget->NZHUDOwner->SmallFont, false, FVector2D(0.f, 0.f), FLinearColor::Black, true, FLinearColor::Black, 1.0f, Alpha, FLinearColor::Red, ETextHorzPos::Center);
		}
		else
		{
			TargetPlayerState = NULL;
		}
	}
}

void ANZWeapon::UpdateOverlaysShared(AActor* WeaponActor, ANZCharacter* InOwner, USkeletalMeshComponent* InMesh, const TArray<struct FParticleSysParam>& InOverlayEffectParams, USkeletalMeshComponent*& InOverlayMesh) const
{
    ANZGameState* GameState = WeaponActor ? WeaponActor->GetWorld()->GetGameState<ANZGameState>() : NULL;
    if (GameState != NULL && InOwner != NULL && InMesh != NULL)
    {
        FOverlayEffect TopOverlay = GameState->GetFirstOverlay(InOwner->GetWeaponOverlayFlags(), Cast<ANZWeapon>(WeaponActor) != NULL);
        
    }
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
    // Check if NewValue is clockwise from LastValue
    NewValue = FMath::UnwindDegrees(NewValue);
    LastValue = FMath::UnwindDegrees(LastValue);
    
    float LeadMag = 0.f;
    float RotDiff = NewValue - LastValue;
    if ((RotDiff == 0.f) || (OldRotDiff[Index] == 0.f))
    {
        LeadMag = ShouldLagRot() ? OldLeadMag[Index] : 0.f;
        if ((RotDiff == 0.f) && (OldRotDiff[Index] == 0.f))
        {
            OldMaxDiff[Index] = 0.f;
        }
    }
    else if ((RotDiff > 0.f) == (OldRotDiff[Index] > 0.f))
    {
        if (ShouldLagRot())
        {
            MaxDiff = FMath::Min(1.f, FMath::Abs(RotDiff) / (66.f * DeltaTime)) * MaxDiff;
            if (OldMaxDiff[Index] != 0.f)
            {
                MaxDiff = FMath::Max(OldMaxDiff[Index], MaxDiff);
            }
            
            OldMaxDiff[Index] = MaxDiff;
            LeadMag = (NewValue > LastValue) ? -1.f * MaxDiff : MaxDiff;
        }
        LeadMag = (DeltaTime < 1.f / RotChgSpeed)
            ? LeadMag = (1.f - RotChgSpeed * DeltaTime) * OldLeadMag[Index] + RotChgSpeed * DeltaTime * LeadMag
            : LeadMag = 0.f;
    }
    else
    {
        OldMaxDiff[Index] = 0.f;
        if (DeltaTime < 1.f / ReturnChgSpeed)
        {
            LeadMag = (1.f - ReturnChgSpeed * DeltaTime) * OldLeadMag[Index] + ReturnChgSpeed * DeltaTime * LeadMag;
        }
    }
    OldLeadMag[Index] = LeadMag;
    OldRotDiff[Index] = RotDiff;
    
    return LeadMag;
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
                MuzzleFlashSocketNames.Add((PSC == NULL) ? NAME_None : PSC->GetAttachSocketName());
            }
        }
        else
        {
            for (int32 i = 0; i < FMath::Min3<int32>(MuzzleFlash.Num(), MuzzleFlashDefaultTransforms.Num(), MuzzleFlashSocketNames.Num()); i++)
            {
                if (MuzzleFlash[i] != NULL)
                {
					//MuzzleFlash[i]->AttachSocketName = MuzzleFlashSocketNames[i];
					MuzzleFlash[i]->AttachToComponent(MuzzleFlash[i]->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform, MuzzleFlashSocketNames[i]);
                    MuzzleFlash[i]->SetRelativeTransform(MuzzleFlashDefaultTransforms[i]);
                }
            }
        }
        
		//Mesh->AttachSocketName = HandsAttachSocket;
		Mesh->AttachToComponent(Mesh->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform, HandsAttachSocket);
        if (HandsAttachSocket == NAME_None)
        {
            //NZOwner->FirstPersonMesh->SetRelativeTransform(FTransform::Identity);
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
                    //Mesh->SetupAttachment(NULL, NAME_None);
					Mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
                    Mesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
                }
                for (int32 i = 0; i < MuzzleFlash.Num() && i < MuzzleFlashDefaultTransforms.Num(); i++)
                {
                    if (MuzzleFlash[i] != NULL)
                    {
                        //MuzzleFlash[i]->SetupAttachment(NULL, NAME_None);
						MuzzleFlash[i]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
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

float ANZWeapon::GetDamageRadius_Implementation(uint8 TestMode) const
{
    if (ProjClass.IsValidIndex(TestMode) && ProjClass[TestMode] != NULL)
    {
        return ProjClass[TestMode].GetDefaultObject()->DamageParams.OuterRadius;
    }
    else
    {
        return 0.0f;
    }
}

float ANZWeapon::BotDesireability_Implementation(APawn* Asker, AActor* Pickup, float PathDistance) const
{
    // todo:
    check(false);
    return 0.f;
}

float ANZWeapon::DetourWeight_Implementation(APawn* Asker, AActor* Pickup, float PathDistance) const
{
    // todo:
    check(false);
    return 0.f;
}

float ANZWeapon::GetAISelectRating_Implementation()
{
    return HasAnyAmmo() ? BaseAISelectRating : 0.0f;
}

float ANZWeapon::SuggestAttackStyle_Implementation()
{
    return 0.0f;
}

float ANZWeapon::SuggestDefenseStyle_Implementation()
{
    return 0.0f;
}

bool ANZWeapon::IsPreparingAttack_Implementation()
{
    return false;
}

bool ANZWeapon::ShouldAIDelayFiring_Implementation()
{
    return false;
}

bool ANZWeapon::CanAttack_Implementation(AActor* Target, const FVector& TargetLoc, bool bDirectOnly, bool bPreferCurrentMode, UPARAM(ref) uint8& BestFireMode, UPARAM(ref) FVector& OptimalTargetLoc)
{
    OptimalTargetLoc = TargetLoc;
    bool bVisible = false;
    ANZBot* B = Cast<ANZBot>(NZOwner->Controller);
    if (B != NULL)
    {
        // todo:
    }
    else
    {
        const FVector StartLoc = GetFireStartLoc();
        FCollisionQueryParams Params(FName(TEXT("CanAttack")), false, Instigator);
        Params.AddIgnoredActor(Target);
        bVisible = !GetWorld()->LineTraceTestByChannel(StartLoc, TargetLoc, COLLISION_TRACE_WEAPON, Params);
    }
    if (bVisible)
    {
        // Skip zoom modes by default
        TArray<uint8, TInlineAllocator<4> > ValidAIModes;
        for (uint8 i = 0; i < GetNumFireModes(); i++)
        {
            if (Cast<UNZWeaponStateZooming>(FiringState[i]) == NULL)
            {
                ValidAIModes.Add(i);
            }
        }
        if (!bPreferCurrentMode && ValidAIModes.Num() > 0)
        {
            BestFireMode = ValidAIModes[FMath::RandHelper(ValidAIModes.Num())];
        }
        return true;
    }
    else
    {
        return false;
    }
}

TArray<UMeshComponent*> ANZWeapon::Get1PMeshes_Implementation() const
{
    TArray<UMeshComponent*> Result;
    Result.Add(Mesh);
    Result.Add(OverlayMesh);
    return Result;
}

int32 ANZWeapon::GetWeaponKillStats(ANZPlayerState* PS) const
{
    int32 KillCount = 0;
    if (PS)
    {
        if (KillStatsName != NAME_None)
        {
            KillCount += PS->GetStatsValue(KillStatsName);
        }
        if (AltKillStatsName != NAME_None)
        {
            KillCount += PS->GetStatsValue(AltKillStatsName);
        }
    }
    return KillCount;
}

int32 ANZWeapon::GetWeaponDeathStats(ANZPlayerState* PS) const
{
    int32 DeathCount = 0;
    if (PS)
    {
        if (DeathStatsName != NAME_None)
        {
            DeathCount += PS->GetStatsValue(DeathStatsName);
        }
        if (AltDeathStatsName != NAME_None)
        {
            DeathCount += PS->GetStatsValue(AltDeathStatsName);
        }
    }
    return DeathCount;
}

float ANZWeapon::GetWeaponShotsStats(ANZPlayerState* PS) const
{
    return (PS && (ShotsStatsName != NAME_None)) ? PS->GetStatsValue(ShotsStatsName) : 0.f;
}

float ANZWeapon::GetWeaponHitsStats(ANZPlayerState* PS) const
{
    return (PS && (HitsStatsName != NAME_None)) ? PS->GetStatsValue(HitsStatsName) : 0.f;
}

void ANZWeapon::TestWeaponLoc(float X, float Y, float Z)
{
    Mesh->SetRelativeLocation(FVector(X, Y, Z));
}

void ANZWeapon::TestWeaponRot(float Pitch, float Yaw, float Roll)
{
    Mesh->SetRelativeRotation(FRotator(Pitch, Yaw, Roll));
}

void ANZWeapon::TestWeaponScale(float X, float Y, float Z)
{
    Mesh->SetRelativeScale3D(FVector(X, Y, Z));
}

void ANZWeapon::FiringInfoUpdated_Implementation(uint8 InFireMode, uint8 FlashCount, FVector InFlashLocation)
{
    if (FlashCount > 0 || !InFlashLocation.IsZero())
    {
        CurrentFireMode = InFireMode;
        PlayFiringEffects();
    }
    else
    {
        StopFiringEffects();
    }
}

void ANZWeapon::FiringExtraUpdated_Implementation(uint8 NewFlashExtra, uint8 InFireMode)
{
    
}

void ANZWeapon::FiringEffectsUpdated_Implementation(uint8 InFireMode, FVector InFlashLocation)
{
    FVector SpawnLocation;
    FRotator SpawnRotation;
    GetImpactSpawnPosition(InFlashLocation, SpawnLocation, SpawnRotation);
    PlayImpactEffects(InFlashLocation, InFireMode, SpawnLocation, SpawnRotation);
}

void ANZWeapon::OnRep_ZoomCount()
{
    if (CreationTime != 0.0f && GetWorld()->TimeSeconds - CreationTime > 0.2)
    {
        ZoomTime = 0.0f;
    }
}

void ANZWeapon::OnRep_ZoomState_Implementation()
{
    if (GetNetMode() != NM_DedicatedServer && ZoomState == EZoomState::EZS_NotZoomed && GetNZOwner() && GetNZOwner()->GetPlayerCameraManager())
    {
        GetNZOwner()->GetPlayerCameraManager()->UnlockFOV();
    }
}

void ANZWeapon::SetZoomMode(uint8 NewZoomMode)
{
    // Only locally controlled players set the zoom mode so the server stays in sync
    if (GetNZOwner() && GetNZOwner()->IsLocallyControlled() && CurrentZoomMode != NewZoomMode)
    {
        if (Role < ROLE_Authority)
        {
            ServerSetZoomMode(NewZoomMode);
        }
        LocalSetZoomMode(NewZoomMode);
    }
}

void ANZWeapon::ServerSetZoomMode_Implementation(uint8 NewZoomMode)
{
    LocalSetZoomMode(NewZoomMode);
}

bool ANZWeapon::ServerSetZoomMode_Validate(uint8 NewZoomMode)
{
    return true;
}

void ANZWeapon::LocalSetZoomMode(uint8 NewZoomMode)
{
    if (ZoomModes.IsValidIndex(CurrentZoomMode))
    {
        CurrentZoomMode = NewZoomMode;
    }
    else
    {
        //UE_LOG(LogNZWeapon, Warning, TEXT("%s::LocalSetZoomMode(): Invalid Zoom Mode: %d"), *GetName(), NewZoomMode);
    }
}

void ANZWeapon::SetZoomState(TEnumAsByte<EZoomState::Type> NewZoomState)
{
    // Only locally controlled players set the zoom state so the server stays in sync
    if (GetNZOwner() && GetNZOwner()->IsLocallyControlled() && NewZoomState != ZoomState)
    {
        if (Role < ROLE_Authority)
        {
            ServerSetZoomState(NewZoomState);
        }
        LocalSetZoomState(NewZoomState);
    }
}

void ANZWeapon::ServerSetZoomState_Implementation(uint8 NewZoomState)
{
    LocalSetZoomState(NewZoomState);
}

bool ANZWeapon::ServerSetZoomState_Validate(uint8 NewZoomState)
{
    return true;
}

void ANZWeapon::LocalSetZoomState(uint8 NewZoomState)
{
    if (ZoomModes.IsValidIndex(CurrentZoomMode))
    {
        if (NewZoomState != ZoomState)
        {
            ZoomState = (EZoomState::Type)NewZoomState;
            
            // Need to reset the zoom time since this state might be skipped on spec clients if states switch too fast
            if (ZoomState == EZoomState::EZS_NotZoomed)
            {
                ZoomCount++;
                OnRep_ZoomCount();
            }
            OnRep_ZoomState();
        }
    }
    else
    {
        //UE_LOG(LogNZWeapon, Warning, TEXT("%s::LocalSetZoomState(): Invalid Zoom Mode: %d"), *GetName(), CurrentZoomMode);
    }
}

void ANZWeapon::OnZoomedIn_Implementation()
{
    SetZoomState(EZoomState::EZS_Zoomed);
}

void ANZWeapon::OnZoomedOut_Implementation()
{
    SetZoomState(EZoomState::EZS_NotZoomed);
}

void ANZWeapon::TickZoom(float DeltaTime)
{
    if (GetNZOwner() != NULL && ZoomModes.IsValidIndex(CurrentZoomMode))
    {
        if (ZoomState != EZoomState::EZS_NotZoomed)
        {
            if (ZoomState == EZoomState::EZS_ZoomingIn)
            {
                ZoomTime += DeltaTime;
                
                if (ZoomTime >= ZoomModes[CurrentZoomMode].Time)
                {
                    OnZoomedIn();
                }
            }
            else if (ZoomState == EZoomState::EZS_ZoomingOut)
            {
                ZoomTime -= DeltaTime;
                
                if (ZoomTime <= 0.0f)
                {
                    OnZoomedOut();
                }
            }
            ZoomTime = FMath::Clamp(ZoomTime, 0.0f, ZoomModes[CurrentZoomMode].Time);
            
            // Do the FOV change
            if (GetNetMode() != NM_DedicatedServer)
            {
                float StartFOV = ZoomModes[CurrentZoomMode].StartFOV;
                float EndFOV = ZoomModes[CurrentZoomMode].EndFOV;
                
                // Use the players default FOV if the FOV is zero
                ANZPlayerController* NZPC = Cast<ANZPlayerController>(GetWorld()->GetFirstPlayerController());
                if (NZPC != NULL)
                {
                    if (StartFOV == 0.0f)
                    {
                        StartFOV = NZPC->ConfigDefaultFOV;
                    }
                    if (EndFOV == 0.0f)
                    {
                        EndFOV = NZPC->ConfigDefaultFOV;
                    }
                }
                
                // Calculate the FOV based on the zoom time
                float FOV = 90.0f;
                if (ZoomModes[CurrentZoomMode].Time == 0.0f)
                {
                    FOV = StartFOV;
                }
                else
                {
                    FOV = FMath::Lerp(StartFOV, EndFOV, ZoomTime / ZoomModes[CurrentZoomMode].Time);
                    FOV = FMath::Clamp(FOV, EndFOV, StartFOV);
                }
                
                ANZPlayerCameraManager* Camera = Cast<ANZPlayerCameraManager>(GetNZOwner()->GetPlayerCameraManager());
                if (Camera != NULL && Camera->GetCameraStyleWithOverrides() == FName(TEXT("Default")))
                {
                    Camera->SetFOV(FOV);
                }
            }
        }
    }
}

