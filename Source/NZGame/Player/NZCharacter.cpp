// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCharacter.h"
#include "NZAIController.h"
#include "NZInventory.h"
#include "NZWeapon.h"
#include "NZCharacterMovementComponent.h"
#include "NZPlayerState.h"
#include "NZPlayerController.h"
#include "NZWeaponAttachment.h"
#include "NZGameState.h"
#include "NZReplicatedLoadoutInfo.h"
#include "NZGameMode.h"
#include "NZMutator.h"
#include "NZDamageType_Suicide.h"
#include "NZGameplayStatics.h"
#include "NZCharacterContent.h"
#include "NZWorldSettings.h"
#include "NZProjectile.h"
#include "UnrealNetwork.h"
#include "NZGun.h"
#include "NZDualGun.h"
#include "ComponentReregisterContext.h"
#include "AudioDevice.h"
#include "NZWeaponStateFiring.h"
#include "NZDamageType_Fell.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"

// Sets default values
//ANZCharacter::ANZCharacter()

ANZCharacter::ANZCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UNZCharacterMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    static ConstructorHelpers::FObjectFinder<UClass> DefaultCharacterContentRef(TEXT("Blueprint'/Game/Characters/Default/BP_NZCharacterContent_Default.BP_NZCharacterContent_Default_C'"));
    CharacterData = DefaultCharacterContentRef.Object;
    
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);
    
    // Create a CameraComponent
    CharacterCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CharacterCameraComponent->SetupAttachment(GetCapsuleComponent());
    DefaultBaseEyeHeight = 67.5f;
    BaseEyeHeight = DefaultBaseEyeHeight;
    DefaultCrouchedEyeHeight = 30.f;
    CrouchedEyeHeight = DefaultCrouchedEyeHeight;
    CharacterCameraComponent->RelativeLocation = FVector(0, 0, DefaultBaseEyeHeight);
	CharacterCameraComponent->bUsePawnControlRotation = true;
    
    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->SetupAttachment(CharacterCameraComponent);
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;
    FirstPersonMesh->bReceivesDecals = false;
    FirstPersonMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    FirstPersonMesh->PrimaryComponentTick.AddPrerequisite(this, PrimaryActorTick);

    GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //GetMesh()->bEnablePhysicsOnDedicatedServer = true;  // needed for feign death; death ragdoll shouldn't be invoked on server
    GetMesh()->bReceivesDecals = false;
    GetMesh()->bLightAttachmentsAsGroup = true;
    //GetMesh()->SetRelativeScale3D(FVector(1.15f));
    
    NZCharacterMovement = Cast<UNZCharacterMovementComponent>(GetCharacterMovement());
    
    HealthMax = 100;
    //SuperHealthMax = 199;
    DamageScaling = 1.0f;
    bDamageHurtsHealth = true;
    FireRateMultiplier = 1.0f;
    bSpawnProtectionEligible = true;
    MaxSafeFallSpeed = 2400.0f;
    FallingDamageFactor = 100.0f;
    CrushingDamageFactor = 2.0f;
    
    BobTime = 0.f;
    WeaponBobMagnitude = FVector(0.f, 0.8f, 0.4f);
    WeaponJumpBob = FVector(0.f, 0.f, -3.6f);
    WeaponLandBob = FVector(0.f, 0.f, 10.5f);
    WeaponBreathingBobRate = 0.2f;
    WeaponRunningBobRate = 0.8f;
    WeaponJumpBobInterpRate = 6.5f;
    WeaponHorizontalBobInterpRate = 4.3f;
    WeaponLandBobDecayRate = 5.f;
    EyeOffset = FVector(0.f, 0.f, 0.f);
    CrouchEyeOffset = EyeOffset;
    TargetEyeOffset = EyeOffset;
    EyeOffsetInterpRate = FVector(18.f, 9.f, 9.f);
    CrouchEyeOffsetInterpRate = 12.f;
    EyeOffsetJumpBob = 20.f;
    EyeOffsetLandBob = -110.f;
    EyeOffsetLandBobThreshold = 300.f;
    WeaponLandBobThreshold = 100.f;
    FullWeaponLandBobVelZ = 900.f;
    FullEyeOffsetLandBobVelZ = 750.f;
    WeaponDirChangeDeflection = 4.f;
    RagdollBlendOutTime = 0.75f;
    //bApplyWallSlide = false;
    bCanPickupItems = true;
    RagdollGravityScale = 1.f;
    RagdollCollisionBleedThreshold = 2000.f;
    
    MinPainSoundInterval = 0.35f;
    LastPainSoundTime = -100.f;
    
    //SprintAmbientStartSpeed = 1000.f;
    //FallingAmbientStartSpeed = -1300.f;
    LandEffectSpeed = 500.f;
    
    NetCullDistanceSquared = 500000000.f;
    
    //OnActorBeginOverlap.AddDynamic(this, &ANZCharacter::OnOverlapBegin);
    GetMesh()->OnComponentHit.AddDynamic(this, &ANZCharacter::OnRagdollCollision);
    GetMesh()->SetNotifyRigidBodyCollision(true);
    
    MaxSavedPositionAge = 0.3f;
    MaxShotSynchDelay = 0.1f;
    
    MaxStackedArmor = 150;
    MaxDeathLifeSpan = 30.f;
    //MinWaterSoundInterval = 0.8f;
    //LastWaterSoundTime = 0.f;
    //DrowningDamagePerSecond = 2.f;
    //bHeadIsUnderwater = false;
    //LastBreathTime = 0.f;
    //LastDrownTime = 0.f;
    
    //LowHealthAmbientThreshold = 40;
    //MinOverlapToTelefrag = 25.f;
    //bIsTranslocating = false;
    LastTakeHitTime = -10000.f;
    LastTakeHitReplicatedTime = -10000.f;
    
}

// Called when the game starts or when spawned
void ANZCharacter::BeginPlay()
{
    GetMesh()->SetOwnerNoSee(false);    // Compatibility with old content, we're doing this through UpdateHiddenComponents() now
    
    if (GetWorld()->GetNetMode() != NM_DedicatedServer)
    {
        APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
        if (PC != NULL && PC->MyHUD != NULL)
        {
            PC->MyHUD->AddPostRenderedActor(this);
        }
    }
    
    if (Health == 0 && Role == ROLE_Authority)
    {
        Health = HealthMax;
    }
    
    CharacterCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, DefaultBaseEyeHeight), false);
    if (CharacterCameraComponent->RelativeLocation.Size2D() > 0.0f)
    {
        //UE_LOG(NZ, Warning, TEXT("%s: CameraComponent shouldn't have X/Y translation!"), *GetName());
    }

	// todo:

    Super::BeginPlay();
}

// Called every frame
void ANZCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    
    // todo:
    
    if (GetMesh()->MeshComponentUpdateFlag >= EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered && !GetMesh()->bRecentlyRendered && !IsLocallyControlled() && GetCharacterMovement()->MovementMode == MOVE_Walking && !IsDead())
    {
        // TODO: currently using an arbitrary made up interval and scale factor
        float Speed = GetCharacterMovement()->Velocity.Size();
        if (Speed > 0.0f && GetWorld()->TimeSeconds - LastFootstepTime > 0.35f * GetCharacterMovement()->MaxWalkSpeed / Speed)
        {
            PlayFootstep((LastFoot + 1) & 1, true);
        }
    }

	// todo:
    
    // todo: tick ragdoll recovery
    
/*    // Update eyeoffset
    if (GetCharacterMovement()->MovementMode == MOVE_Walking && !MovementBaseUtility::UseRelativeLocation(BasedMovement.MovementBase))
    {
        // Smooth up/down stairs
        if (GetCharacterMovement()->bJustTeleported && (FMath::Abs(OldZ - GetActorLocation().Z) > GetCharacterMovement()->MaxStepHeight))
        {
            EyeOffset.Z = 0.f;
        }
        else
        {
            EyeOffset.Z += (OldZ - GetActorLocation().Z);
        }
        
        // Avoid clipping
        if (CrouchEyeOffset.Z + EyeOffset.Z > GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - BaseEyeHeight - 12.f)
        {
            if (!GetLocalViewer())
            {
                CrouchEyeOffset.Z = 0.f;
                EyeOffset.Z = FMath::Min(EyeOffset.Z, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - BaseEyeHeight);
            }
            else
            {
                // Trace and see if camera will clip
                static FName CameraClipTrace = FName(TEXT("CameraClipTrace"));
                FCollisionQueryParams Params(CameraClipTrace, false, this);
                FHitResult Hit;
                if (GetWorld()->SweepSingleByChannel(Hit, GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight), GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight) + CrouchEyeOffset + GetTransformedEyeOffset(), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(12.f), Params))
                {
                    EyeOffset.Z = Hit.Location.Z - BaseEyeHeight - GetActorLocation().Z - CrouchEyeOffset.Z;
                }
            }
        }
        else
        {
            EyeOffset.Z = FMath::Max(EyeOffset.Z, 12.f - BaseEyeHeight - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - CrouchEyeOffset.Z);
        }
    }
    OldZ = GetActorLocation().Z;
    
    // Clamp transformed offset z contribution
    FRotationMatrix ViewRotMatrix = FRotationMatrix(GetViewRotation());
    FVector XTransform = ViewRotMatrix.GetScaledAxis(EAxis::X) * EyeOffset.X;
    if ((XTransform.Z > 0.f) && (XTransform.Z + EyeOffset.Z + BaseEyeHeight + CrouchEyeOffset.Z > GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 12.f))
    {
        float MaxZ = FMath::Max(0.f, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 12.f - EyeOffset.Z - BaseEyeHeight - CrouchEyeOffset.Z);
        EyeOffset.X *= MaxZ / XTransform.Z;
    }
    
    // Decay offset
    float InterpTimeX = FMath::Min(1.f, EyeOffsetInterpRate.X * DeltaTime);
    float InterpTimeY = FMath::Min(1.f, EyeOffsetInterpRate.Y * DeltaTime);
    float InterpTimeZ = FMath::Min(1.f, EyeOffsetInterpRate.Z * DeltaTime);
    EyeOffset.X = (1.f - InterpTimeX) * EyeOffset.X + InterpTimeX * TargetEyeOffset.X;
    EyeOffset.Y = (1.f - InterpTimeY) * EyeOffset.Y + InterpTimeY * TargetEyeOffset.Y;
    EyeOffset.Z = (1.f - InterpTimeZ) * EyeOffset.Z + InterpTimeZ * TargetEyeOffset.Z;
    float CrouchInterpTime = FMath::Min(1.f, CrouchEyeOffsetInterpRate * DeltaTime);
    CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
    if (EyeOffset.Z > 0.f)
    {
        // Faster decay if positive
        EyeOffset.Z = (1.f - InterpTimeZ) * EyeOffset.Z + InterpTimeZ * TargetEyeOffset.Z;
    }
    EyeOffset.DiagnosticCheckNaN();
    TargetEyeOffset.X *= FMath::Max(0.f, 1.f - FMath::Min(1.f, EyeOffsetDecayRate.X * DeltaTime));
    TargetEyeOffset.Y *= FMath::Max(0.f, 1.f - FMath::Min(1.f, EyeOffsetDecayRate.Y * DeltaTime));
    TargetEyeOffset.Z *= FMath::Max(0.f, 1.f - FMath::Min(1.f, EyeOffsetDecayRate.Z * DeltaTime));
    TargetEyeOffset.DiagnosticCheckNaN();
*/
    
    if (GetWeapon())
    {
        GetWeapon()->UpdateViewBob(DeltaTime);
    }
    else
    {
        GetWeaponBobOffset(DeltaTime, NULL);
    }
    
    // todo: ambient sound
    
    // todo: water
}

// Called to bind functionality to input
void ANZCharacter::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

}

void ANZCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANZCharacter, NZReplicatedMovement, COND_SimulatedOrPhysics);
	DOREPLIFETIME_CONDITION(ANZCharacter, Health, COND_None);

	//DOREPLIFETIME_CONDITION(ANZCharacter, InventoryList, COND_OwnerOnly);
	// replicate for cases where non-owned inventory is replicated (e.g. spectators)
	// UE4 networking doesn't cause endless replication sending unserializable values like UE3 did so this shouldn't be a big deal
	DOREPLIFETIME_CONDITION(ANZCharacter, InventoryList, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, Weapon, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(ANZCharacter, FlashCount, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANZCharacter, FlashLocation, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, FireMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANZCharacter, FlashExtra, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANZCharacter, LastTakeHitInfo, COND_Custom);
	// todo
	DOREPLIFETIME_CONDITION(ANZCharacter, WeaponClass, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANZCharacter, WeaponAttachmentClass, COND_SkipOwner);
	// todo
	DOREPLIFETIME_CONDITION(ANZCharacter, HolsteredWeaponAttachmentClass, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANZCharacter, DamageScaling, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, FireRateMultiplier, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANZCharacter, AmbientSound, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, CharOverlayFlags, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, WeaponOverlayFlags, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, ReplicatedBodyMaterial, COND_None);
	// todo
	DOREPLIFETIME_CONDITION(ANZCharacter, bSpawnProtectionEligible, COND_None);
	DOREPLIFETIME_CONDITION(ANZCharacter, DrivenVehicle, COND_OwnerOnly);
	// todo
	DOREPLIFETIME_CONDITION(ANZCharacter, WalkMovementReductionPct, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANZCharacter, WalkMovementReductionTime, COND_OwnerOnly);
	// todo
	DOREPLIFETIME_CONDITION(ANZCharacter, ArmorAmount, COND_None);
	// todo


}

void ANZCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    if ((GetNetMode() == NM_DedicatedServer) || (GetCachedScalabilityCVars().DetailMode == 0))
    {
        if (GetMesh())
        {
            GetMesh()->bDisableClothSimulation = true;
        }
    }
    
    if (GetNetMode() != NM_DedicatedServer)
    {
        for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
        {
            // FIXME: NULL check is hack for editor reimport bug breaking number of materials
            if (GetMesh()->GetMaterial(i) != NULL)
            {
                BodyMIs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(i));
            }
        }
    }
}

void ANZCharacter::Destroyed()
{
    Super::Destroyed();
    
    DiscardAllInventory();
    
    if (WeaponAttachment != NULL)
    {
        WeaponAttachment->Destroy();
        WeaponAttachment = NULL;
    }
    
    if (HolsteredWeaponAttachment != NULL)
    {
        HolsteredWeaponAttachment->Destroy();
        HolsteredWeaponAttachment = NULL;
    }

    if (GetWorld()->GetNetMode() != NM_DedicatedServer && GEngine->GetWorldContextFromWorld(GetWorld()) != NULL)
    {
        APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
        if (PC != NULL && PC->MyHUD != NULL)
        {
            PC->MyHUD->RemovePostRenderedActor(this);
        }
    }
    
    if (GetCharacterMovement())
    {
        GetWorldTimerManager().ClearAllTimersForObject(GetCharacterMovement());
    }
    GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ANZCharacter::PossessedBy(AController* NewController)
{
	// TODO: shouldn't base class do this? APawn::Unpossessed() still does SetOwner(NULL)...
	SetOwner(NewController);

	Super::PossessedBy(NewController);
	NotifyTeamChanged();
	NewController->ClientSetRotation(GetActorRotation());
	if (NZCharacterMovement)
	{
		NZCharacterMovement->ResetTimers();
	}

	if (Role == ROLE_Authority)
	{
		//SetCosmeticsFromPlayerState();
	}
}

void ANZCharacter::UnPossessed()
{
	StopFiring();
	SetAmbientSound(NULL);
	//SetStatusAmbientSound(NULL);
	SetLocalAmbientSound(NULL);
	Super::UnPossessed();
}

void ANZCharacter::Restart()
{
	Super::Restart();
	ClearJumpInput();

	// make sure equipped weapon state is synchronized
	if (IsLocallyControlled())
	{
		ANZPlayerController* PC = Cast<ANZPlayerController>(Controller);
		if (PC != NULL && PC->IsInState(NAME_Inactive))
		{
			// respawning from dead, switch to best
			PC->SwitchToBestWeapon();
		}
		else if (PendingWeapon != NULL)
		{
			SwitchWeapon(PendingWeapon);
		}
		else if (Weapon != NULL && Weapon->HasAnyAmmo())
		{
			SwitchWeapon(Weapon);
		}
		else
		{
			SwitchToBestWeapon();
		}
	}
}

void ANZCharacter::PawnClientRestart()
{
    if (NZCharacterMovement)
    {
        NZCharacterMovement->ResetTimers();
    }
    
    Super::PawnClientRestart();
}

void ANZCharacter::NZUpdateSimulatedPosition(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewVelocity)
{
    if (NZCharacterMovement)
    {
        NZCharacterMovement->SimulatedVelocity = NewVelocity;
        
        // Always consider Location as changed if we were spawned this tick as in that case our replicated Location was set as part of spawning, before PreNetReceive()
        if ((NewLocation != GetActorLocation()) || (CreationTime == GetWorld()->TimeSeconds))
        {
            FVector FinalLocation = NewLocation;
            if (GetWorld()->EncroachingBlockingGeometry(this, NewLocation, NewRotation))
            {
                bSimGravityDisabled = true;
            }
            else
            {
                bSimGravityDisabled = false;
            }
            
            // Don't use TeleportTo(), that clears our base.
            SetActorLocationAndRotation(FinalLocation, NewRotation, false);
            
            if (GetCharacterMovement())
            {
                GetCharacterMovement()->bJustTeleported = true;
                
                // Forward simulate this character to match estimated current position on server, based on my ping
                ANZPlayerController* PC = Cast<ANZPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
                float PredictionTime = PC ? PC->GetPredictionTime() : 0.f;
                if ((PredictionTime > 0.f) && (PC->GetViewTarget() != this))
                {
                    check(GetCharacterMovement() == NZCharacterMovement);
                    NZCharacterMovement->SimulateMovement(PredictionTime);
                }
            }
        }
        else if (NewRotation != GetActorRotation())
        {
            GetRootComponent()->MoveComponent(FVector::ZeroVector, NewRotation, false);
        }
    }
}

void ANZCharacter::PostNetReceiveLocationAndRotation()
{
    if (Role == ROLE_SimulatedProxy)
    {
        if (!ReplicatedBasedMovement.HasRelativeLocation())
        {
            const FVector OldLocation = GetActorLocation();
            const FQuat OldRotation = GetActorQuat();
            NZUpdateSimulatedPosition(ReplicatedMovement.Location, ReplicatedMovement.Rotation, ReplicatedMovement.LinearVelocity);
            
            INetworkPredictionInterface* PredictionInterface = Cast<INetworkPredictionInterface>(GetMovementComponent());
            if (PredictionInterface)
            {
                PredictionInterface->SmoothCorrection(OldLocation, OldRotation, GetActorLocation(), GetActorQuat());
            }
        }
        else if (NZCharacterMovement)
        {
            NZCharacterMovement->SimulatedVelocity = ReplicatedMovement.LinearVelocity;
        }
    }
}

void ANZCharacter::OnRep_NZReplicatedMovement()
{
    if (Role == ROLE_SimulatedProxy)
    {
        ReplicatedMovement.Location = NZReplicatedMovement.Location;
        ReplicatedMovement.Rotation = NZReplicatedMovement.Rotation;
        RemoteViewPitch = (uint8)(ReplicatedMovement.Rotation.Pitch * 255.f / 360.f);
        ReplicatedMovement.Rotation.Pitch = 0.f;
        ReplicatedMovement.LinearVelocity = NZReplicatedMovement.LinearVelocity;
        ReplicatedMovement.AngularVelocity = FVector(0.f);
        ReplicatedMovement.bSimulatedPhysicSleep = false;
        ReplicatedMovement.bRepPhysics = false;
        
        OnRep_ReplicatedMovement();
        
        if (NZCharacterMovement)
        {
            NZCharacterMovement->SetReplicatedAcceleration(NZReplicatedMovement.Rotation, NZReplicatedMovement.AccelDir);
        }
    }
}

void ANZCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
    if (bReplicateMovement || AttachmentReplication.AttachParent)
    {
        if (GatherNZMovement())
        {
            DOREPLIFETIME_ACTIVE_OVERRIDE(ANZCharacter, NZReplicatedMovement, bReplicateMovement);
            DOREPLIFETIME_ACTIVE_OVERRIDE(AActor, ReplicatedMovement, false);
        }
        else
        {
            DOREPLIFETIME_ACTIVE_OVERRIDE(ANZCharacter, NZReplicatedMovement, false);
            DOREPLIFETIME_ACTIVE_OVERRIDE(AActor, ReplicatedMovement, bReplicateMovement);
        }
    }
    else
    {
        DOREPLIFETIME_ACTIVE_OVERRIDE(ANZCharacter, NZReplicatedMovement, false);
        DOREPLIFETIME_ACTIVE_OVERRIDE(AActor, ReplicatedMovement, false);
    }
    
    const FAnimMontageInstance* RootMotionMontageInstance = GetRootMotionAnimMontageInstance();
    
    if (RootMotionMontageInstance)
    {
        // Is position stored in local space?
        RepRootMotion.bRelativePosition = BasedMovement.HasRelativeLocation();
        RepRootMotion.bRelativeRotation = BasedMovement.HasRelativeRotation();
        RepRootMotion.Location = RepRootMotion.bRelativePosition ? BasedMovement.Location : GetActorLocation();
        RepRootMotion.Rotation = RepRootMotion.bRelativeRotation ? BasedMovement.Rotation : GetActorRotation();
        RepRootMotion.MovementBase = BasedMovement.MovementBase;
        RepRootMotion.MovementBaseBoneName = BasedMovement.BoneName;
        RepRootMotion.AnimMontage = RootMotionMontageInstance->Montage;
        RepRootMotion.Position = RootMotionMontageInstance->GetPosition();
        
        DOREPLIFETIME_ACTIVE_OVERRIDE(ACharacter, RepRootMotion, true);
    }
    else
    {
        RepRootMotion.Clear();
        DOREPLIFETIME_ACTIVE_OVERRIDE(ACharacter, RepRootMotion, false);
    }
    
    ReplicatedMovementMode = GetCharacterMovement()->PackNetworkMovementMode();
    ReplicatedBasedMovement = BasedMovement;
    
    // Optimization: only update and replicate these values if they are actually going to be used.
    if (BasedMovement.HasRelativeLocation())
    {
        // When velocity becomes zero, force replication so the position is updated to match the server (it may have moved due to simulation on the client).
        ReplicatedBasedMovement.bServerHasVelocity = !GetCharacterMovement()->Velocity.IsZero();
        
        // Make sure absolute rotations are updated in case rotation occurred after the base info was saved.
        if (!BasedMovement.HasRelativeRotation())
        {
            ReplicatedBasedMovement.Rotation = GetActorRotation();
        }
    }
    
    DOREPLIFETIME_ACTIVE_OVERRIDE(ANZCharacter, LastTakeHitInfo, GetWorld()->TimeSeconds - LastTakeHitTime < 0.5f);
    //DOREPLIFETIME_ACTIVE_OVERRIDE(ANZCharacter, HeadArmorFlashCount, GetWorld()->TimeSeconds - LastHeadArmorFlashTime < 0.5f);
    //DOREPLIFETIME_ACTIVE_OVERRIDE(ANZCharacter, CosmeticFlashCount, GetWorld()->TimeSeconds - LastCosmeticFlashTime < 0.5f);
    
    DOREPLIFETIME_ACTIVE_OVERRIDE(ACharacter, RemoteViewPitch, false);
    
    LastTakeHitReplicatedTime = GetWorld()->TimeSeconds;
}

bool ANZCharacter::GatherNZMovement()
{
    UPrimitiveComponent* RootPrimComp = Cast<UPrimitiveComponent>(GetRootComponent());
    if (RootPrimComp && RootPrimComp->IsSimulatingPhysics())
    {
        FRigidBodyState RBState;
        RootPrimComp->GetRigidBodyState(RBState);
        ReplicatedMovement.FillFrom(RBState);
    }
    else if (RootComponent != NULL)
    {
        // If we are attached, don't replicate absolute position
        if (RootComponent->GetAttachParent() != NULL)
        {
            // Networking for attachments assumes the RootComponent of the AttachParent actor.
            // If that's not the case, we can't update this, as the client wouldn't be able to resolve the Component and would detach as a result.
            if (AttachmentReplication.AttachParent != NULL)
            {
                AttachmentReplication.LocationOffset = RootComponent->RelativeLocation;
                AttachmentReplication.RotationOffset = RootComponent->RelativeRotation;
            }
        }
        else
        {
            // TODO FIXME: Make sure not replicated to owning client!!!
            NZReplicatedMovement.Location = RootComponent->GetComponentLocation();
            NZReplicatedMovement.Rotation = RootComponent->GetComponentRotation();
            NZReplicatedMovement.Rotation.Pitch = GetControlRotation().Pitch;
            NZReplicatedMovement.LinearVelocity = GetVelocity();
            
            FVector AccelDir = GetCharacterMovement()->GetCurrentAcceleration();
            AccelDir = AccelDir.GetSafeNormal();
            FRotator FacingRot = NZReplicatedMovement.Rotation;
            FacingRot.Pitch = 0.f;
            FVector CurrentDir = FacingRot.Vector();
            float ForwardDot = CurrentDir | AccelDir;
            
            NZReplicatedMovement.AccelDir = 0;
            if (ForwardDot > 0.5f)
            {
                NZReplicatedMovement.AccelDir |= 1;
            }
            else if (ForwardDot < -0.5f)
            {
                NZReplicatedMovement.AccelDir |= 2;
            }
            
            FVector SideDir = (CurrentDir ^ FVector(0.f, 0.f, 1.f)).GetSafeNormal();
            float SideDot = AccelDir | SideDir;
            if (SideDot > 0.5f)
            {
                NZReplicatedMovement.AccelDir |= 4;
            }
            else if (SideDot < -0.5f)
            {
                NZReplicatedMovement.AccelDir |= 8;
            }
            
            return true;
        }
    }
    
    return false;
}

void ANZCharacter::OnRep_ReplicatedMovement()
{
    if ((bTearOff) && (RootComponent == NULL || !RootComponent->IsSimulatingPhysics()))
    {
        bDeferredReplicatedMovement = true;
    }
    else
    {
        if (RootComponent != NULL)
        {
            // We handle this ourselves, do not use base version
            // Why on earth isn't SyncReplicatedPhysicsSimulation() virtual?
            ReplicatedMovement.bRepPhysics = RootComponent->IsSimulatingPhysics();
        }
        
        Super::OnRep_ReplicatedMovement();
    }
}


void ANZCharacter::NZServerMove_Implementation(float TimeStamp, FVector_NetQuantize InAccel, FVector_NetQuantize ClientLoc, uint8 MoveFlags, float ViewYaw, float ViewPitch, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	if (NZCharacterMovement)
	{
		NZCharacterMovement->ProcessServerMove(TimeStamp, InAccel, ClientLoc, MoveFlags, ViewYaw, ViewPitch, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
	}
}

bool ANZCharacter::NZServerMove_Validate(float TimeStamp, FVector_NetQuantize InAccel, FVector_NetQuantize ClientLoc, uint8 MoveFlags, float ViewYaw, float ViewPitch, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	return true;
}

void ANZCharacter::NZServerMoveOld_Implementation(float OldTimeStamp, FVector_NetQuantize OldAccel, float OldYaw, uint8 OldMoveFlags)
{
	if (NZCharacterMovement)
	{
		NZCharacterMovement->ProcessOldServerMove(OldTimeStamp, OldAccel, OldYaw, OldMoveFlags);
	}
}

bool ANZCharacter::NZServerMoveOld_Validate(float OldTimeStamp, FVector_NetQuantize OldAccel, float OldYaw, uint8 OldMoveFlags)
{
	return true;
}

void ANZCharacter::NZServerMoveQuick_Implementation(float TimeStamp, FVector_NetQuantize InAccel, uint8 PendingFlags)
{
	if (NZCharacterMovement)
	{
		NZCharacterMovement->ProcessQuickServerMove(TimeStamp, InAccel, PendingFlags);
	}
}

bool ANZCharacter::NZServerMoveQuick_Validate(float TimeStamp, FVector_NetQuantize InAccel, uint8 PendingFlags)
{
	return true;
}

void ANZCharacter::NZServerMoveSaved_Implementation(float TimeStamp, FVector_NetQuantize InAccel, uint8 PendingFlags, float ViewYaw, float ViewPitch)
{
	if (NZCharacterMovement)
	{
		NZCharacterMovement->ProcessSavedServerMove(TimeStamp, InAccel, PendingFlags, ViewYaw, ViewPitch);
	}
}

bool ANZCharacter::NZServerMoveSaved_Validate(float TimeStamp, FVector_NetQuantize InAccel, uint8 PendingFlags, float ViewYaw, float ViewPitch)
{
	return true;
}

bool ANZCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
    // We want to allow zero damage (momentum only) hits so never pass 0 to super call
    return bTearOff || Super::ShouldTakeDamage(FMath::Max<float>(1.0f, Damage), DamageEvent, EventInstigator, DamageCauser);
}

float ANZCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    if (!ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser))
    {
        return 0.f;
    }
    else if (Damage < 0.0f)
    {
        //UE_LOG(LogNZCharacter, Warning, TEXT("TakeDamage() called with damage %i of type %s... use HealDamage() to add health"), int32(Damage), *GetNameSafe(DamageEvent.DamageTypeClass));
        return 0.0f;
    }
    else
    {
        //UE_LOG(LogNZCharacter, Verbose, TEXT("%s::TakeDamage() %d Class:%s Causer:%s"), *GetName(), int32(Damage), *GetNameSafe(DamageEvent.DamageTypeClass), *GetNameSafe(DamageCauser));
        
        const UDamageType* const DamageTypeCDO = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
        const UNZDamageType* const NZDamageTypeCDO = Cast<UNZDamageType>(DamageTypeCDO);    // Warning: may be NULL
        
        int32 ResultDamage = FMath::TruncToInt(Damage);
        FVector ResultMomentum = NZGetDamageMomentum(DamageEvent, this, EventInstigator);
        bool bRadialDamage = false;
        if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
        {
            bool bScaleMomentum = !DamageEvent.IsOfType(FNZRadialDamageEvent::ClassID) || ((const FNZRadialDamageEvent&)DamageEvent).bScaleMomentum;
            if (Damage == 0.f)
            {
                if (bScaleMomentum)
                {
                    // Use fake 1.0 damage so we can use the damage scaling code to scale momentum
                    ResultMomentum *= InternalTakeRadialDamage(1.0f, (const FRadialDamageEvent&)DamageEvent, EventInstigator, DamageCauser);
                }
            }
            else
            {
                float AdjustedDamage = InternalTakeRadialDamage(Damage, (const FRadialDamageEvent&)DamageEvent, EventInstigator, DamageCauser);
                if (bScaleMomentum)
                {
                    ResultMomentum *= AdjustedDamage / Damage;
                }
                ResultDamage = FMath::TruncToInt(AdjustedDamage);
                bRadialDamage = true;
            }
        }
        
        int32 AppliedDamage = ResultDamage;
        ANZPlayerState* EnemyPS = EventInstigator ? Cast<ANZPlayerState>(EventInstigator->PlayerState) : NULL;
        if (EnemyPS)
        {
            ANZGameState* GS = Cast<ANZGameState>(GetWorld()->GetGameState());
            if (GS && !GS->OnSameTeam(this, EventInstigator))
            {
                EnemyPS->DamageDone += AppliedDamage;
            }
        }
        
        if (!IsDead())
        {
            // We need to pull the hit info out of FDamageEvent because ModifyDamage() goes through blueprints and that doesn't correctly handle polymorphic structs
            FHitResult HitInfo;
            {
                FVector UnusedDir;
                DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, UnusedDir);
            }
            
            // Note that we split the gametype query out so that it's always in a consistent place
            ANZGameMode* Game = GetWorld()->GetAuthGameMode<ANZGameMode>();
            if (Game != NULL)
            {
                Game->ModifyDamage(ResultDamage, ResultMomentum, this, EventInstigator, HitInfo, DamageCauser, DamageEvent.DamageTypeClass);
            }

            if (bRadialDamage)
            {
                ANZProjectile* Proj = Cast<ANZProjectile>(DamageCauser);
                if (Proj)
                {
                    Proj->StatsHitCredit += ResultDamage;
                }

				// todo:
                //else if (Cast<ANZRemoteRedeemer>(DamageCauser))
                //{
                //    Cast<ANZRemoteRedeemer>(DamageCauser)->StatsHitCredit += ResultDamage;
                //}
            }

            AppliedDamage = ResultDamage;
            ANZInventory* HitArmor = NULL;
            ModifyDamageTaken(AppliedDamage, ResultDamage, ResultMomentum, HitArmor, HitInfo, EventInstigator, DamageCauser, DamageEvent.DamageTypeClass);
            if (HitArmor)
            {
                ArmorAmount = GetArmorAmount();
            }
            if (ResultDamage > 0 || !ResultMomentum.IsZero())
            {
                if (EventInstigator != NULL && EventInstigator != Controller)
                {
                    LastHitBy = EventInstigator;
                }
                
                if (ResultDamage > 0)
                {
                    // This is partially copied from AActor::TakeDamage() (just the calls to the various delegates and K2 notifications)
                    
                    float ActualDamage = float(ResultDamage);	// Engine hooks want float
                    // Generic damage notifications sent for any damage
                    ReceiveAnyDamage(ActualDamage, DamageTypeCDO, EventInstigator, DamageCauser);
                    OnTakeAnyDamage.Broadcast(this, ActualDamage, DamageTypeCDO, EventInstigator, DamageCauser);
                    if (EventInstigator != NULL)
                    {
                        EventInstigator->InstigatedAnyDamage(ActualDamage, DamageTypeCDO, this, DamageCauser);
                    }
                    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
                    {
                        // Point damage event, pass off to helper function
                        FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
                        
                        // K2 notification for this actor
                        if (ActualDamage != 0.f)
                        {
							FHitResult HitResult;
                            ReceivePointDamage(ActualDamage, DamageTypeCDO, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.ImpactNormal, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, EventInstigator, DamageCauser, HitResult);
                            OnTakePointDamage.Broadcast(this, ActualDamage, EventInstigator, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, DamageTypeCDO, DamageCauser);
                        }
                    }
                    else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
                    {
                        // Radial damage event, pass off to helper function
                        FRadialDamageEvent* const RadialDamageEvent = (FRadialDamageEvent*)&DamageEvent;
                        
                        // K2 notification for this actor
                        if (ActualDamage != 0.f)
                        {
                            FHitResult const& Hit = (RadialDamageEvent->ComponentHits.Num() > 0) ? RadialDamageEvent->ComponentHits[0] : FHitResult();
                            ReceiveRadialDamage(ActualDamage, DamageTypeCDO, RadialDamageEvent->Origin, Hit, EventInstigator, DamageCauser);
                        }
                    }
                }
            }
            
            if ((Game->bDamageHurtsHealth && bDamageHurtsHealth) || (!Cast<ANZPlayerController>(GetController()) && (!DrivenVehicle || !Cast<ANZPlayerController>(DrivenVehicle->GetController()))))
            {
				//if ((Health - ResultDamage) <= 0)
				//{
				//	Health = HealthMax;
				//}

                Health -= ResultDamage;
                bWasFallingWhenDamaged = (GetCharacterMovement() != NULL && (GetCharacterMovement()->MovementMode == MOVE_Falling));
            }
            //UE_LOG(LogNZCharacter, Verbose, TEXT("%s took %d damage, %d health remaining"), *GetName(), ResultDamage, Health);
            
            // Let the game Score damage if it wants to make sure not to count overkill damage!
            Game->ScoreDamage(ResultDamage + FMath::Min<int32>(Health, 0), Controller, EventInstigator);
            bool bIsSelfDamage = (EventInstigator == Controller && Controller != NULL);
            if (NZDamageTypeCDO != NULL)
            {
                if (NZDamageTypeCDO->bForceZMomentum && GetCharacterMovement()->MovementMode == MOVE_Walking)
                {
                    ResultMomentum.Z = FMath::Max<float>(ResultMomentum.Z, NZDamageTypeCDO->ForceZMomentumPct * ResultMomentum.Size());
                }
                if (bIsSelfDamage)
                {
                    if (NZDamageTypeCDO->bSelfMomentumBoostOnlyZ)
                    {
                        ResultMomentum.Z *= NZDamageTypeCDO->SelfMomentumBoost;
                    }
                    else
                    {
                        ResultMomentum *= NZDamageTypeCDO->SelfMomentumBoost;
                    }
                }
            }
            
            if (IsRagdoll())
            {
                if (GetNetMode() != NM_Standalone)
                {
					// Intentionally always apply to root because that replicates better, and damp to prevent excessive team boost
                    ANZGameState* GS = EventInstigator ? Cast<ANZGameState>(GetWorld()->GetGameState()) : NULL;
                    float PushScaling = (GS && GS->OnSameTeam(this, EventInstigator)) ? 0.5f : 1.f;
                    GetMesh()->AddImpulseAtLocation(PushScaling * ResultMomentum, GetMesh()->GetComponentLocation());
                }
                else
                {
                    FVector HitLocation = GetMesh()->GetComponentLocation();
                    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
                    {
                        HitLocation = ((const FPointDamageEvent&)DamageEvent).HitInfo.Location;
                    }
                    else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
                    {
                        const FRadialDamageEvent& RadialEvent = (const FRadialDamageEvent&)DamageEvent;
                        if (RadialEvent.ComponentHits.Num() > 0)
                        {
                            HitLocation = RadialEvent.ComponentHits[0].Location;
                        }
                    }
                    GetMesh()->AddImpulseAtLocation(ResultMomentum, HitLocation);
                }
            }
            else if (NZCharacterMovement != NULL)
            {
                NZCharacterMovement->AddDampedImpulse(ResultMomentum, bIsSelfDamage);
                if (NZDamageTypeCDO != NULL && NZDamageTypeCDO->WalkMovementReductionDuration > 0.0f)
                {
                    SetWalkMovementReduction(NZDamageTypeCDO->WalkMovementReductionPct, NZDamageTypeCDO->WalkMovementReductionDuration);
                }
            }
			else
			{
				GetCharacterMovement()->AddImpulse(ResultMomentum, false);
			}
            
			NotifyTakeHit(EventInstigator, AppliedDamage, ResultDamage, ResultMomentum, HitArmor, DamageEvent);
			SetLastTakeHitInfo(Damage, ResultDamage, ResultMomentum, HitArmor, DamageEvent);

			if (Health <= 0)
			{
				// todo:
				//ANZPlayerState* KillerPS = EventInstigator ? Cast<ANZPlayerState>(EventInstigator->PlayerState) : NULL;
				//if (KillerPS)
				//{
				//	ANZProjectile* Proj = Cast<ANZProjectile>(DamageCauser);
				//	KillerPS->bAnnounceWeaponReward = Proj && Proj->bPendingSpecialReward;
				//}

				// ¡Ÿ ±∆¡±ŒÕÊº“À¿Õˆ
				if (Cast<APlayerController>(Controller) != NULL)
				{
					Health = 0;
				}
				else
				{
					Died(EventInstigator, DamageEvent);
				}

				// todo:
				//if (KillerPS)
				//{
				//	KillerPS->bAnnounceWeaponReward = false;
				//}
			}
        }
		else if (IsRagdoll())
		{
			FVector HitLocation = GetMesh()->GetComponentLocation();
			if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
			{
				HitLocation = ((const FPointDamageEvent&)DamageEvent).HitInfo.Location;
			}
			else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
			{
				const FRadialDamageEvent& RadialEvent = (const FRadialDamageEvent&)DamageEvent;
				if (RadialEvent.ComponentHits.Num() > 0)
				{
					HitLocation = RadialEvent.ComponentHits[0].Location;
				}
			}
			GetMesh()->AddImpulseAtLocation(ResultMomentum, HitLocation);
			if (GetNetMode() != NM_DedicatedServer)
			{
				Health -= int32(Damage);
				SetLastTakeHitInfo(Damage, Damage, ResultMomentum, NULL, DamageEvent);

				// todo:
				//TSubclassOf<UNZDamageType> NZDmg(*DamageEvent.DamageTypeClass);
				//if (NZDmg != NULL && NZDmg.GetDefaultObject()->ShouldGib(this))
				//{
				//	GibExplosion();
				//}
			}
		}

		return float(ResultDamage);
    }
}

bool ANZCharacter::ModifyDamageTaken_Implementation(UPARAM(ref) int32& AppliedDamage, UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, UPARAM(ref) ANZInventory*& HitArmor, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	// Check for caused modifiers on instigator
	ANZCharacter* InstigatorChar = NULL;
	if (DamageCauser != NULL)
	{
		InstigatorChar = Cast<ANZCharacter>(DamageCauser->Instigator);
	}
	if (InstigatorChar == NULL && EventInstigator != NULL)
	{
		InstigatorChar = Cast<ANZCharacter>(EventInstigator->GetPawn());
	}
	if (InstigatorChar != NULL && !InstigatorChar->IsDead())
	{
		InstigatorChar->ModifyDamageCaused(AppliedDamage, Damage, Momentum, HitInfo, this, EventInstigator, DamageCauser, DamageType);
	}
	// Check inventory
	for (TInventoryIterator<> It(this); It; ++It)
	{
		if (It->bCallDamageEvents)
		{
			It->ModifyDamageTaken(Damage, Momentum, HitArmor, EventInstigator, HitInfo, DamageCauser, DamageType);
		}
	}
	return false;
}

bool ANZCharacter::ModifyDamageCaused_Implementation(UPARAM(ref) int32& AppliedDamage, UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, const FHitResult& HitInfo, AActor* Victim, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
    if (DamageType && !DamageType->GetDefaultObject<UDamageType>()->bCausedByWorld)
    {
        Damage *= DamageScaling;
        AppliedDamage *= DamageScaling;
    }
    return false;
}


FVector ANZCharacter::GetHeadLocation(float PredictionTime)
{
    return FVector(0.f);
}

bool ANZCharacter::IsHeadShot(FVector HitLocation, FVector ShotDirection, float WeaponHeadScaling, ANZCharacter* ShotInstigator, float PredictionTime)
{
    return false;
}

void ANZCharacter::NotifyTakeHit(AController* InstigatedBy, int32 AppliedDamage, int32 Damage, FVector Momentum, ANZInventory* HitArmor, const FDamageEvent& DamageEvent)
{
    if (Role == ROLE_Authority)
    {
        ANZPlayerController* InstigatedByPC = Cast<ANZPlayerController>(InstigatedBy);
        if (InstigatedByPC != NULL)
        {
            InstigatedByPC->NotifyCausedHit(this, FMath::Clamp(AppliedDamage, 0, 255));
        }
        else
        {
            ANZAIController* InstigatedByAI = Cast<ANZAIController>(InstigatedBy);
            if (InstigatedByAI != NULL)
            {
				InstigatedByAI->NotifyCausedHit(this, Damage);
            }
        }
        
        // We do the sound here instead of via PlayTakeHitEffects() so it uses RPCs instead of variable raplication which is higher priority
        // (at small bandwidth cost)
        if (GetWorld()->TimeSeconds - LastPainSoundTime >= MinPainSoundInterval)
        {
            const UDamageType* const DamageTypeCDO = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
            const UNZDamageType* const NZDamageTypeCDO = Cast<UNZDamageType>(DamageTypeCDO);    // Warning: may be NULL
            if (HitArmor != NULL && HitArmor->ReceivedDamageSound != NULL && ((NZDamageTypeCDO == NULL) || NZDamageTypeCDO->bBlockedByArmor))
            {
                UNZGameplayStatics::NZPlaySound(GetWorld(), HitArmor->ReceivedDamageSound, this, SRT_All, false, FVector::ZeroVector, InstigatedByPC, NULL, false);
            }
            else if ((NZDamageTypeCDO == NULL) || NZDamageTypeCDO->bCausesBlood)
            {
                UNZGameplayStatics::NZPlaySound(GetWorld(), CharacterData.GetDefaultObject()->PainSound, this, SRT_All, false, FVector::ZeroVector, InstigatedByPC, NULL, false);
            }
            LastPainSoundTime = GetWorld()->TimeSeconds;
        }
        
        ANZPlayerController* PC = Cast<ANZPlayerController>(Controller);
        if (PC != NULL)
        {
            // Pass some damage even if armor absorbed all of it, so client will get visual hit indicator
            PC->NotifyTakeHit(InstigatedBy, HitArmor ? FMath::Max(Damage, 1) : Damage, Momentum, DamageEvent);
        }
        else
        {
			ANZAIController* AI = Cast<ANZAIController>(Controller);
            if (AI != NULL)
            {
                AI->NotifyTakeHit(InstigatedBy, Damage, Momentum, DamageEvent);
            }
        }
    }
}

void ANZCharacter::SetLastTakeHitInfo(int32 AttemptedDamage, int32 Damage, const FVector& Momentum, ANZInventory* HitArmor, const FDamageEvent& DamageEvent)
{
    // If we haven't replicated a previous hit yet (generally, multi hit within same frame), stack with it
    bool bStackHit = (LastTakeHitTime > LastTakeHitReplicatedTime && DamageEvent.DamageTypeClass == LastTakeHitInfo.DamageType);

    LastTakeHitInfo.Damage = Damage;
    LastTakeHitInfo.DamageType = DamageEvent.DamageTypeClass;
    if (!bStackHit || LastTakeHitInfo.HitArmor == NULL)
    {
        LastTakeHitInfo.HitArmor = ((HitArmor != NULL) && HitArmor->ShouldDisplayHitEffect(AttemptedDamage, Damage, Health, ArmorAmount)) ? HitArmor->GetClass() : NULL;    // The inventory object is bOnlyRelevantToOwner and wouldn't work on other clients
    }
    if ((LastTakeHitInfo.HitArmor == NULL) && (HitArmor == NULL) && (Health > 0) && (Damage == AttemptedDamage) && (Health + Damage > HealthMax) && ((Damage > 90) || (Health > 90)))
    {
        // todo:
        //LastTakeHitInfo.HitArmor = ANZTimedPowerup::StaticClass();
    }
    LastTakeHitInfo.Momentum = Momentum;
    
    FVector NewRelHitLocation(FVector::ZeroVector);
    FVector ShotDir(FVector::ZeroVector);
    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        NewRelHitLocation = ((FPointDamageEvent*)&DamageEvent)->HitInfo.Location - GetActorLocation();
        ShotDir = ((FPointDamageEvent*)&DamageEvent)->ShotDirection;
    }
    else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID) && ((FRadialDamageEvent*)&DamageEvent)->ComponentHits.Num() > 0)
    {
        NewRelHitLocation = ((FRadialDamageEvent*)&DamageEvent)->ComponentHits[0].Location - GetActorLocation();
        ShotDir = (((FRadialDamageEvent*)&DamageEvent)->ComponentHits[0].ImpactPoint - ((FRadialDamageEvent*)&DamageEvent)->Origin).GetSafeNormal();
    }

    // Make sure there's a difference from the last time so replication happens
    if ((NewRelHitLocation - LastTakeHitInfo.RelHitLocation).IsNearlyZero(1.0f))
    {
        NewRelHitLocation.Z += 1.0f;
    }
    LastTakeHitInfo.RelHitLocation = NewRelHitLocation;
    
    // Set shot rotation
    // This differs from momentum in cases of e.g. damage types that kick upwards
    FRotator ShotRot = ShotDir.Rotation();
    LastTakeHitInfo.ShotDirPitch = FRotator::CompressAxisToByte(ShotRot.Pitch);
    LastTakeHitInfo.ShotDirYaw = FRotator::CompressAxisToByte(ShotRot.Yaw);
    
    if (bStackHit)
    {
        LastTakeHitInfo.Count++;
    }
    else
    {
        LastTakeHitInfo.Count = 1;
    }
    
    LastTakeHitTime = GetWorld()->TimeSeconds;
    
    PlayTakeHitEffects();
}

void ANZCharacter::SpawnBloodDecal(const FVector& TraceStart, const FVector& TraceDir)
{
#if !UE_SERVER
    ANZWorldSettings* Settings = Cast<ANZWorldSettings>(GetWorldSettings());
    if (Settings != NULL)
    {
        if (BloodDecals.Num() > 0)
        {
            const FBloodDecalInfo& DecalInfo = BloodDecals[FMath::RandHelper(BloodDecals.Num())];
            if (DecalInfo.Material != NULL)
            {
                static FName NAME_BloodDecal(TEXT("BloodDecal"));
                FHitResult Hit;
                if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceStart + TraceDir * (GetCapsuleComponent()->GetUnscaledCapsuleRadius() + 200.0f), ECC_Visibility, FCollisionQueryParams(NAME_BloodDecal, false, this)) && Hit.Component->bReceivesDecals)
                {
                    UDecalComponent* Decal = NewObject<UDecalComponent>(GetWorld(), UDecalComponent::StaticClass());
                    if (Hit.Component.Get() != NULL && Hit.Component->Mobility == EComponentMobility::Movable)
                    {
                        Decal->SetAbsolute(false, false, true);
                        //Decal->AttachTo(Hit.Component.Get());
						Decal->AttachToComponent(Hit.Component.Get(), FAttachmentTransformRules::KeepRelativeTransform);
                    }
                    else
                    {
                        Decal->SetAbsolute(true, true, true);
                    }
                    FVector2D DecalScale = DecalInfo.BaseScale * FMath::FRandRange(DecalInfo.ScaleMultRange.X, DecalInfo.ScaleMultRange.Y);
                    Decal->DecalSize = FVector(1.0f, DecalScale.X, DecalScale.Y);
                    Decal->SetWorldLocation(Hit.Location);
                    Decal->SetWorldRotation((-Hit.Normal).Rotation() + FRotator(0.0f, 0.0f, 360.0f * FMath::FRand()));
                    Decal->SetDecalMaterial(DecalInfo.Material);
                    Decal->RegisterComponentWithWorld(GetWorld());
                    Settings->AddImpactEffect(Decal);
                }
            }
        }
    }
#endif
}

void ANZCharacter::PlayTakeHitEffects_Implementation()
{
    if (GetNetMode() != NM_DedicatedServer)
    {
        // Send hit notify for spectators
        for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
        {
            ANZPlayerController* PC = Cast<ANZPlayerController>(It->PlayerController);
            if (PC != NULL && PC->GetViewTarget() == this && PC->GetPawn() != this)
            {
                PC->ClientNotifyTakeHit(false, FMath::Clamp(LastTakeHitInfo.Damage, 0, 255), LastTakeHitInfo.RelHitLocation);
            }
        }
        
        // Never play armor effect if dead, prefer blood
        bool bPlayedArmorEffect = (LastTakeHitInfo.HitArmor != NULL && !bTearOff) ? LastTakeHitInfo.HitArmor.GetDefaultObject()->HandleArmorEffects(this) : false;
        TSubclassOf<UNZDamageType> NZDmg(*LastTakeHitInfo.DamageType);
        if (NZDmg != NULL)
        {
            NZDmg.GetDefaultObject()->PlayHitEffects(this, bPlayedArmorEffect);
            if (!NZDmg.GetDefaultObject()->bCausedByWorld)
            {
                LastTakeHitTime = GetWorld()->TimeSeconds;
            }
        }
        
        // Check blood effects
        if (!bPlayedArmorEffect && LastTakeHitInfo.Damage > 0 && (NZDmg == NULL || NZDmg.GetDefaultObject()->bCausesBlood))
        {
            bool bRecentlyRendered = GetWorld()->TimeSeconds - GetLastRenderTime() < 1.0f;
            // TODO: gore setting check
            if (bRecentlyRendered && BloodEffects.Num() > 0)
            {
                UParticleSystem* Blood = BloodEffects[FMath::RandHelper(BloodEffects.Num())];
                if (Blood != NULL)
                {
                    // We want the PSC 'attached' to ourselves for 1P/3P visibility yet using an absolute transform, so the GameplayStatics function don't get the job done
                    UParticleSystemComponent* PSC = NewObject<UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass());
                    PSC->bAutoDestroy = true;
                    PSC->SecondsBeforeInactive = 0.0f;
                    PSC->bAutoActivate = false;
                    PSC->SetTemplate(Blood);
                    PSC->bOverrideLODMethod = false;
                    PSC->RegisterComponentWithWorld(GetWorld());
                    //PSC->AttachTo(GetMesh());
					PSC->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
                    PSC->SetAbsolute(true, true, true);
                    PSC->SetWorldLocationAndRotation(LastTakeHitInfo.RelHitLocation + GetActorLocation(), LastTakeHitInfo.RelHitLocation.Rotation());
                    PSC->SetRelativeScale3D(bPlayedArmorEffect ? FVector(0.7f) : FVector(1.f));
                    PSC->ActivateSystem(true);
                }
            }
            
            // Spawn decal
            bool bSpawnDecal = bRecentlyRendered;
            if (!bSpawnDecal)
            {
                // Spawn blood decals for player locally viewed even in first person mode
                for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
                {
                    if (It->PlayerController != NULL && It->PlayerController->GetViewTarget() == this)
                    {
                        bSpawnDecal = true;
                        break;
                    }
                }
            }
            
            if (bSpawnDecal)
            {
                SpawnBloodDecal(LastTakeHitInfo.RelHitLocation + GetActorLocation(), FRotator(FRotator::DecompressAxisFromByte(LastTakeHitInfo.ShotDirPitch), FRotator::DecompressAxisFromByte(LastTakeHitInfo.ShotDirYaw), 0.0f).Vector());
            }
        }
    }
}

void ANZCharacter::PlayDamageEffects_Implementation()
{
    
}

bool ANZCharacter::K2_Died(AController* EventInstigator, TSubclassOf<UDamageType> DamageType)
{
    return Died(EventInstigator, FNZPointDamageEvent(Health + 1, FHitResult(), FVector(0.0f, 0.0f, -1.0f), DamageType));
}

bool ANZCharacter::Died(AController* EventInstigator, const FDamageEvent& DamageEvent)
{
    if (Role < ROLE_Authority || IsDead())
    {
        // Can't kill pawns on client
        // Can't kill pawns that are already dead
        return false;
    }
    else
    {
        // If this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
        if (DamageEvent.DamageTypeClass != NULL && DamageEvent.DamageTypeClass.GetDefaultObject()->bCausedByWorld && (EventInstigator == NULL || EventInstigator == Controller) && LastHitBy != NULL)
        {
            EventInstigator = LastHitBy;
        }
        
        FHitResult HitInfo;
        {
            FVector UnusedDir;
            DamageEvent.GetBestHitInfo(this, NULL, HitInfo, UnusedDir);
        }
        
        ANZGameMode* Game = GetWorld()->GetAuthGameMode<ANZGameMode>();
        if (Game != NULL && Game->PreventDeath(this, EventInstigator, DamageEvent.DamageTypeClass, HitInfo))
        {
            Health = FMath::Max<int32>(Health, 1);
            return false;
        }
        else
        {
            bTearOff = true;
            Health = FMath::Min<int32>(Health, 0);
            
            AController* ControllerKilled = Controller;
            if (ControllerKilled == NULL)
            {
                ControllerKilled = Cast<AController>(GetOwner());
                if (ControllerKilled == NULL)
                {
                    if (DrivenVehicle != NULL)
                    {
                        ControllerKilled = DrivenVehicle->Controller;
                    }
                }
            }

            GetWorld()->GetAuthGameMode<ANZGameMode>()->Killed(EventInstigator, ControllerKilled, this, DamageEvent.DamageTypeClass);
            
            // todo:
            //ANZPlayerState* PS = Cast<ANZPlayerState>(PlayerState);
            //if (PS != NULL && PS->CarriedObject != NULL)
            //{
            //    PS->CarriedObject->Drop(EventInstigator);
            //}
            
            if (ControllerKilled)
            {
                ControllerKilled->PawnPendingDestroy(this);
            }
            
            OnDied.Broadcast(EventInstigator, DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass.GetDefaultObject() : NULL);
            
			// Set no collision
			SetActorEnableCollision(false);

			PlayDying();
            
            // todo:
            
            return true;
        }
    }
}

void ANZCharacter::StartRagdoll()
{
    NZCharacterMovement->UnCrouch(true);
    if (RootComponent == GetMesh() && GetMesh()->IsSimulatingPhysics())
    {
        // UnCrouch() caused death
        return;
    }

    // todo: TacCom
    
    SetActorEnableCollision(true);
    StopFiring();
    DisallowWeaponFiring(true);
    bInRagdollRecovery = false;
    
    if (!GetMesh()->ShouldTickPose())
    {
        GetMesh()->TickAnimation(0.f, false);
        GetMesh()->RefreshBoneTransforms();
        GetMesh()->UpdateComponentToWorld();
    }
    GetCharacterMovement()->ApplyAccumulatedForces(0.f);
    GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetAllBodiesNotifyRigidBodyCollision(true);
    GetMesh()->UpdateKinematicBonesToAnim(GetMesh()->GetComponentSpaceTransforms(), ETeleportType::TeleportPhysics, true);
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->RefreshBoneTransforms();
    GetMesh()->SetAllBodiesPhysicsBlendWeight(1.f);
    //GetMesh()->DetachFromParent(true);
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    RootComponent = GetMesh();
    GetMesh()->bGenerateOverlapEvents = true;
    GetMesh()->bShouldUpdatePhysicsVolume = true;
    GetMesh()->RegisterClothTick(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //GetCapsuleComponent()->DetachFromParent(false);
	GetCapsuleComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    //GetCapsuleComponent()->AttachTo(GetMesh(), NAME_None, EAttachLocation::KeepWorldPosition);
	GetCapsuleComponent()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform);

    if (bDeferredReplicatedMovement)
    {
        OnRep_ReplicatedMovement();
        // OnRep_ReplicatedMovement() will only apply to the root body but in this case we want to apply to all bodies
        if (GetMesh()->GetBodyInstance())
        {
            GetMesh()->SetAllPhysicsLinearVelocity(GetMesh()->GetBodyInstance()->GetUnrealWorldVelocity());
        }
        else
        {
            //UE_LOG(LogNZCharacter, Warning, TEXT("NZCharacter does not have a body instance!"));
        }
        bDeferredReplicatedMovement = false;
    }
    else
    {
        GetMesh()->SetAllPhysicsLinearVelocity(GetMovementComponent()->Velocity, false);
    }
    
    GetCharacterMovement()->StopActiveMovement();
    GetCharacterMovement()->Velocity = FVector::ZeroVector;
    //bApplyWallSide = false;
    
    // Set up the custom physics override, if necessary
    SetRagdollGravityScale(RagdollGravityScale);
}

void ANZCharacter::StopRagdoll()
{
    // Check for falling damage
    if (!IsDead())
    {
        CheckRagdollFallingDamage(FHitResult(NULL, NULL, GetActorLocation(), FVector(0.f, 0.f, 1.f)));
    }
    
    NZCharacterMovement->Velocity = GetMesh()->GetComponentVelocity();
    
    //GetCapsuleComponent()->DetachFromParent(true);
	GetCapsuleComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    FRotator FixedRotation = GetCapsuleComponent()->RelativeRotation;
    FixedRotation.Pitch = FixedRotation.Roll = 0.f;
    if (Controller != NULL)
    {
        // Always recover in the direction the controller is facing since turning is instant
        FixedRotation.Yaw = Controller->GetControlRotation().Yaw;
    }
    GetCapsuleComponent()->SetRelativeRotation(FixedRotation);
    GetCapsuleComponent()->SetRelativeScale3D(GetClass()->GetDefaultObject<ANZCharacter>()->GetCapsuleComponent()->RelativeScale3D);
    if ((Role == ROLE_Authority) || IsLocallyControlled())
    {
        GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCharacterMovement()->CrouchedHalfHeight);
        bIsCrouched = true;
    }
    RootComponent = GetCapsuleComponent();
    
    GetMesh()->MeshComponentUpdateFlag = GetClass()->GetDefaultObject<ANZCharacter>()->GetMesh()->MeshComponentUpdateFlag;
    GetMesh()->bBlendPhysics = false;   // For some reason bBlendPhysics == false is the value that actually blends instead of using only physics
    GetMesh()->bGenerateOverlapEvents = false;
    GetMesh()->bShouldUpdatePhysicsVolume = false;
    GetMesh()->RegisterClothTick(false);
    
    // TODO: make sure cylinder is in valid position (navmesh?)
    FVector AdjustedLoc = GetActorLocation() + FVector(0.f, 0.f, GetCharacterMovement()->CrouchedHalfHeight);
    GetWorld()->FindTeleportSpot(this, AdjustedLoc, GetActorRotation());
    GetCapsuleComponent()->SetWorldLocation(AdjustedLoc);
    if (NZCharacterMovement)
    {
        NZCharacterMovement->NeedsClientAdjustment();
    }
    
    // Terminate constraints on the root bone so we can move it without interference
    for (int32 i = 0; i < GetMesh()->Constraints.Num(); i++)
    {
        if (GetMesh()->Constraints[i] != NULL && (GetMesh()->GetBoneIndex(GetMesh()->Constraints[i]->ConstraintBone1) == 0 || GetMesh()->GetBoneIndex(GetMesh()->Constraints[i]->ConstraintBone2) == 0))
        {
            GetMesh()->Constraints[i]->TermConstraint();
        }
    }
    
    // Move the root bone to where we put the capsule, then disable further physics
    if (GetMesh()->Bodies.Num() > 0)
    {
        FBodyInstance* RootBody = GetMesh()->GetBodyInstance();
        if (RootBody != NULL)
        {
            TArray<FTransform> BodyTransforms;
            for (int32 i = 0; i < GetMesh()->Bodies.Num(); i++)
            {
                BodyTransforms.Add((GetMesh()->Bodies[i] != NULL) ? GetMesh()->Bodies[i]->GetUnrealWorldTransform() : FTransform::Identity);
            }
            
            const USkeletalMeshComponent* DefaultMesh = GetClass()->GetDefaultObject<ANZCharacter>()->GetMesh();
            FTransform RelativeTransform(DefaultMesh->RelativeRotation, DefaultMesh->RelativeLocation, DefaultMesh->RelativeScale3D);
            GetMesh()->SetWorldTransform(RelativeTransform * GetCapsuleComponent()->GetComponentTransform());
            
            RootBody->SetBodyTransform(GetMesh()->GetComponentTransform(), ETeleportType::TeleportPhysics);
            RootBody->PutInstanceToSleep();
            RootBody->SetInstanceSimulatePhysics(false, true);
            RootBody->PhysicsBlendWeight = 1.f; // Second parameter of SetInstanceSimulatePhysics() doesn't actually work at the moment...
            for (int32 i = 0; i < GetMesh()->Bodies.Num(); i++)
            {
                if (GetMesh()->Bodies[i] != NULL && GetMesh()->Bodies[i] != RootBody)
                {
                    GetMesh()->Bodies[i]->SetBodyTransform(BodyTransforms[i], ETeleportType::TeleportPhysics);
                    GetMesh()->Bodies[i]->PutInstanceToSleep();
                }
            }
        }
    }
    
    bInRagdollRecovery = true;
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ANZCharacter::SetRagdollGravityScale(float NewScale)
{
    for (FBodyInstance* Body : GetMesh()->Bodies)
    {
        if (Body != NULL)
        {
            Body->SetEnableGravity(NewScale != 0.f);
        }
    }
    RagdollGravityScale = NewScale;
}



uint8 ANZCharacter::GetTeamNum() const
{
    check(false);
    return 0;
}

void ANZCharacter::NotifyTeamChanged()
{
    
}

void ANZCharacter::PlayerChangedTeam()
{
    
}

void ANZCharacter::PlayerSuicide()
{
    if (Role == ROLE_Authority)
    {
        FHitResult FakeHit(this, NULL, GetActorLocation(), GetActorRotation().Vector());
        FNZPointDamageEvent FakeDamageEvent(0, FakeHit, FVector(0, 0, 0), UNZDamageType_Suicide::StaticClass());
        UNZGameplayStatics::NZPlaySound(GetWorld(), CharacterData.GetDefaultObject()->PainSound, this, SRT_All, false, FVector::ZeroVector, Cast<ANZPlayerController>(Controller), NULL, false);
        Died(NULL, FakeDamageEvent);
    }
}

ANZCharacterContent* ANZCharacter::GetCharacterData() const
{
    return CharacterData.GetDefaultObject();
}

void ANZCharacter::ApplyCharacterData(TSubclassOf<class ANZCharacterContent> CharType)
{
    ANZPlayerState* PS = Cast<ANZPlayerState>(PlayerState);
    if (CharType != NULL)
    {
        CharacterData = CharType;
    }
    const ANZCharacterContent* Data = CharacterData.GetDefaultObject();
    if (Data->Mesh != NULL)
    {
        FComponentReregisterContext ReregisterContext(GetMesh());
        GetMesh()->OverrideMaterials = Data->Mesh->OverrideMaterials;
        if (PS != NULL && PS->Team != NULL)
        {
            GetMesh()->OverrideMaterials.SetNumZeroed(FMath::Min<int32>(Data->Mesh->GetNumMaterials(), Data->TeamMaterials.Num()));
            for (int32 i = GetMesh()->OverrideMaterials.Num() - 1; i >= 0; i--)
            {
                if (Data->TeamMaterials[i] != NULL)
                {
                    GetMesh()->OverrideMaterials[i] = Data->TeamMaterials[i];
                }
            }
        }
        GetMesh()->SkeletalMesh = Data->Mesh->SkeletalMesh;
        BodyMIs.Empty();
        for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
        {
            if (GetMesh()->GetMaterial(i) != NULL)
            {
                BodyMIs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(i));
            }
        }
        GetMesh()->PhysicsAssetOverride = Data->Mesh->PhysicsAssetOverride;
        GetMesh()->RelativeScale3D = GetClass()->GetDefaultObject<ANZCharacter>()->GetMesh()->RelativeScale3D * Data->Mesh->RelativeScale3D;
        if (GetMesh() != GetRootComponent())
        {
            GetMesh()->RelativeLocation = Data->Mesh->RelativeLocation;
            GetMesh()->RelativeRotation = Data->Mesh->RelativeRotation;
        }
        if (OverlayMesh != NULL)
        {
            //OverlayMesh->DetachFromParent();
			OverlayMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
            OverlayMesh->UnregisterComponent();
            OverlayMesh = NULL;
            UpdateCharOverlays();
        }
        UpdateSkin();
    }
}

void ANZCharacter::BehindViewChange(APlayerController* PC, bool bNowBehindView)
{
    if (PC->GetPawn() != this)
    {
        if (!bNowBehindView)
        {
            if (Weapon != NULL && PC->IsLocalPlayerController() && !Weapon->Mesh->IsAttachedTo(CharacterCameraComponent))
            {
                Weapon->AttachToOwner();
            }
        }
        else
        {
            if (Weapon != NULL && (Controller == NULL || !Controller->IsLocalPlayerController()) && Weapon->Mesh->IsAttachedTo(CharacterCameraComponent))
            {
                Weapon->StopFiringEffects();
                Weapon->DetachFromOwner();
            }
        }
    }
    if (bNowBehindView)
    {
        FirstPersonMesh->MeshComponentUpdateFlag = GetClass()->GetDefaultObject<ANZCharacter>()->FirstPersonMesh->MeshComponentUpdateFlag;
    }
    else
    {
        FirstPersonMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
        FirstPersonMesh->LastRenderTime = GetWorld()->TimeSeconds;
        FirstPersonMesh->bRecentlyRendered = true;
    }
}

bool ANZCharacter::IsSpawnProtected()
{
    if (!bSpawnProtectionEligible)
    {
        return false;
    }
    else
    {
        ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
        return (GameState != NULL && GameState->SpawnProtectionTime > 0.0f && GetWorld()->TimeSeconds - CreationTime < GameState->SpawnProtectionTime);
    }
}

void ANZCharacter::DisallowWeaponFiring(bool bDisallowed)
{
    if (bDisallowed != bDisallowWeaponFiring)
    {
        bDisallowWeaponFiring = bDisallowed;
        if (bDisallowed && Weapon != NULL)
        {
            for (int32 i = 0; i < PendingFire.Num(); i++)
            {
                if (PendingFire[i])
                {
                    StopFire(i);
                }
            }
            if (Weapon != NULL) // StopFire() could have killed us
            {
                for (UNZWeaponStateFiring* FiringState : Weapon->FiringState)
                {
                    if (FiringState != NULL)
                    {
                        FiringState->WeaponBecameInactive();
                    }
                }
            }
        }
    }
}

void ANZCharacter::NotifyPendingServerFire()
{
    if (SavedPositions.Num() > 0)
    {
        SavedPositions.Last().bShotSpawned = true;
    }
}

void ANZCharacter::PositionUpdated(bool bShotSpawned)
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	if (GetCharacterMovement())
	{
		new(SavedPositions) FSavedPosition(GetActorLocation(), GetViewRotation(), GetCharacterMovement()->Velocity, GetCharacterMovement()->bJustTeleported, bShotSpawned, WorldTime, (NZCharacterMovement ? NZCharacterMovement->GetCurrentSynchTime() : 0.f));
	}

	// Maintain one position beyond MaxSavedPositionAge for interpolation
	if (SavedPositions.Num() > 1 && SavedPositions[1].Time < WorldTime - MaxSavedPositionAge)
	{
		SavedPositions.RemoveAt(0);
	}
}

FVector ANZCharacter::GetRewindLocation(float PredictionTime)
{
    FVector TargetLocation = GetActorLocation();
    float TargetTime = GetWorld()->GetTimeSeconds() - PredictionTime;
    if (PredictionTime > 0.f)
    {
        for (int32 i = SavedPositions.Num() - 1; i >= 0; i--)
        {
            TargetLocation = SavedPositions[i].Position;
            if (SavedPositions[i].Time < TargetTime)
            {
                if (!SavedPositions[i].bTeleported && (i < SavedPositions.Num() - 1))
                {
                    float Percent = (SavedPositions[i + 1].Time == SavedPositions[i].Time) ? 1.f : (TargetTime - SavedPositions[i].Time) / (SavedPositions[i + 1].Time - SavedPositions[i].Time);
                    TargetLocation = SavedPositions[i].Position + Percent * (SavedPositions[i + 1].Position - SavedPositions[i].Position);
                }
                break;
            }
        }
    }
    return TargetLocation;
}


FVector ANZCharacter::GetDelayedShotPosition()
{
    const float WorldTime = GetWorld()->GetTimeSeconds();
    for (int32 i = SavedPositions.Num() - 1; i >= 0; i--)
    {
        if (SavedPositions[i].bShotSpawned)
        {
            return SavedPositions[i].Position;
        }
        if (WorldTime - SavedPositions[i].Time > MaxShotSynchDelay)
        {
            break;
        }
    }
    return GetActorLocation();
}

FRotator ANZCharacter::GetDelayedShotRotation()
{
    const float WorldTime = GetWorld()->GetTimeSeconds();
    for (int32 i = SavedPositions.Num() - 1; i >= 0; i--)
    {
        if (SavedPositions[i].bShotSpawned)
        {
            return SavedPositions[i].Rotation;
        }
        if (WorldTime - SavedPositions[i].Time > MaxShotSynchDelay)
        {
            break;
        }
    }
    return GetActorRotation();
}

bool ANZCharacter::DelayedShotFound()
{
    const float WorldTime = GetWorld()->GetTimeSeconds();
    for (int32 i = SavedPositions.Num() - 1; i >= 0; i--)
    {
        if (SavedPositions[i].bShotSpawned)
        {
            return true;
        }
        if (WorldTime - SavedPositions[i].Time > MaxShotSynchDelay)
        {
            break;
        }
    }
    return false;
}

int32 ANZCharacter::GetArmorAmount()
{
	int32 TotalArmor = 0;
	// todo:
	//for (TInventoryIterator<ANZArmor> It(this); It; ++It)
	//{
	//	TotalArmor += It->ArmorAmount;
	//}
	return TotalArmor;
}




ANZInventory* ANZCharacter::K2_CreateInventory(TSubclassOf<ANZInventory> NewInvClass, bool bAutoActivate)
{
    ANZInventory* Inv = NULL;
    if (NewInvClass != NULL)
    {
        Inv = GetWorld()->SpawnActor<ANZInventory>(NewInvClass);
        if (Inv != NULL)
        {
            if (!AddInventory(Inv, bAutoActivate))
            {
                Inv->Destroy();
                Inv = NULL;
            }
        }
    }
    
    return Inv;
}

bool ANZCharacter::AddInventory(ANZInventory* InvToAdd, bool bAutoActivate)
{
    if (InvToAdd != NULL && !InvToAdd->IsPendingKillPending())
    {
        if (InvToAdd->GetNZOwner() != NULL && InvToAdd->GetNZOwner() != this && InvToAdd->GetNZOwner()->IsInInventory(InvToAdd))
        {
            //UE_LOG(NZ, Warning, TEXT("AddInventory (%s): Item %s is already in %s's inventory!"), *GetName(), *InvToAdd->GetName(), *InvToAdd->GetNZOwner()->GetName());
        }
        else
        {
            if (InventoryList == NULL)
            {
                InventoryList = InvToAdd;
            }
            else
            {
                ANZInventory* Last = InventoryList;
                for (ANZInventory* Item = InventoryList; Item != NULL; Item = Item->NextInventory)
                {
                    if (Item == InvToAdd)
                    {
                        //UE_LOG(UT, Warning, TEXT("AddInventory: %s already in %s's inventory!"), *InvToAdd->GetName(), *GetName());
                        return false;
                    }
                    Last = Item;
                }
                Last->NextInventory = InvToAdd;
            }
            InvToAdd->GivenTo(this, bAutoActivate);
            
            if (InvToAdd->GetNZOwner() == this)
            {
                ANZGameMode* Game = GetWorld()->GetAuthGameMode<ANZGameMode>();
                if (Game != NULL && Game->BaseMutator != NULL)
                {
                    Game->BaseMutator->ModifyInventory(InvToAdd, this);
                }
            }
            
            return true;
        }
    }
    
    return false;
}

void ANZCharacter::RemoveInventory(ANZInventory* InvToRemove)
{
    if (InvToRemove != NULL && InventoryList != NULL)
    {
        bool bFound = false;
        if (InvToRemove == InventoryList)
        {
            bFound = true;
            InventoryList = InventoryList->NextInventory;
        }
        else
        {
            for (ANZInventory* TestInv = InventoryList; TestInv->NextInventory != NULL; TestInv = TestInv->NextInventory)
            {
                if (TestInv->NextInventory == InvToRemove)
                {
                    bFound = true;
                    TestInv->NextInventory = InvToRemove->NextInventory;
                    break;
                }
            }
        }
        if (!bFound)
        {
            //UE_LOG(NZ, Warning, TEXT("RemoveInventory (%s): Item %s was not in this character's inventory!"), *GetName(), *InvToRemove->GetName());
        }
        else
        {
            if (InvToRemove == PendingWeapon)
            {
                PendingWeapon = NULL;
            }
            else if (InvToRemove == Weapon)
            {
                Weapon = NULL;
                if (PendingWeapon != NULL)
                {
                    WeaponChanged();
                }
                else
                {
                    WeaponClass = NULL;
                    WeaponAttachmentClass = NULL;
                    UpdateWeaponAttachment();
                }
                if (!bTearOff)
                {
                    if (IsLocallyControlled())
                    {
                        SwitchToBestWeapon();
                    }
                }
            }
            InvToRemove->Removed();
        }
    }
}

ANZInventory* ANZCharacter::K2_FindInventoryType(TSubclassOf<ANZInventory> Type, bool bExactClass) const
{
    for (TInventoryIterator<> It(this); It; ++It)
    {
        if (bExactClass ? (It->GetClass() == Type) : It->IsA(Type))
        {
            return *It;
        }
    }
    
    return NULL;
}

void ANZCharacter::TossInventory(ANZInventory* InvToToss, FVector ExtraVelocity)
{
    if (Role == ROLE_Authority)
    {
        if (InvToToss == NULL)
        {
            //UE_LOG(NZ, Warning, TEXT("TossInventory(): InvToToss == NULL"));
        }
        else if (!IsInInventory(InvToToss))
        {
            //UE_LOG(NZ, Warning, TEXT("Attempt to remove %s which is not in %s's inventory!"), *InvToToss->GetName(), *GetName());
        }
        else
        {
            InvToToss->DropFrom(GetActorLocation() + GetActorRotation().Vector() * GetSimpleCollisionCylinderExtent().X,
                                GetVelocity() + GetActorRotation().RotateVector(ExtraVelocity + FVector(300.0f, 0.0f, 150.0f)));
        }
    }
}

void ANZCharacter::DiscardAllInventory()
{
    // If we're dumping inventory on the server, make sure pending fire doesn't get stuck
    ClearPendingFire();
    
    // Manually iterate here so any items in a bad state still get destroyed and aren't left around
    ANZInventory* Inv = InventoryList;
    while (Inv != NULL)
    {
        ANZInventory* NextInv = Inv->GetNext();
        Inv->Destroy();
        Inv = NextInv;
    }
    Weapon = NULL;
    SavedAmmo.Empty();
}

void ANZCharacter::InventoryEvent(FName EventName)
{
	for (TInventoryIterator<> It(this); It; ++It)
	{
/*		if (It->bCallOwnerEvent)
		{
			It->OwnerEvent(EventName);
		}*/
	}
}

bool ANZCharacter::IsInInventory(const ANZInventory* TestInv) const
{
    // We explicitly iterate all inventory items, even those with uninitialized/unreplicated Owner here
    // to avoid weird inconsistencies where the item is in the list but we think it's free to be reassigned
    for (ANZInventory* Inv = InventoryList; Inv != NULL; Inv = Inv->GetNext())
    {
        if (Inv == TestInv)
        {
            return true;
        }
    }
    
    return false;
}

void ANZCharacter::AddDefaultInventory(TArray<TSubclassOf<ANZInventory>> DefaultInventoryToAdd)
{
    // Check to see if this player has an active loadout. If they do, apply it.
    // NOTE: Loadouts are 100% authoratative. So if we apply any type of loadout, then end the AddDefaultInventory call right there.
    // If you are using the loadout system and want to insure a player has some default items, use bDefaultInclude and make sure their cost is 0.
    ANZPlayerState* NZPlayerState = Cast<ANZPlayerState>(PlayerState);
    if (NZPlayerState && NZPlayerState->Loadout.Num() > 0)
    {
        for (int32 i = 0; i < NZPlayerState->Loadout.Num(); i++)
        {
            if (NZPlayerState->GetAvailableCurrency() >= NZPlayerState->Loadout[i]->CurrentCost)
            {
                AddInventory(GetWorld()->SpawnActor<ANZInventory>(NZPlayerState->Loadout[i]->ItemClass, FVector(0.0f), FRotator(0, 0, 0)), true);
                NZPlayerState->AdjustCurrency(NZPlayerState->Loadout[i]->CurrentCost * -1);
            }
        }
        
        return;
    }
    
    // Add the default character inventory
    for (int32 i = 0; i < DefaultCharacterInventory.Num(); i++)
    {
        AddInventory(GetWorld()->SpawnActor<ANZInventory>(DefaultCharacterInventory[i], FVector(0.0f), FRotator(0, 0, 0)), true);
    }
    
    // Add the default inventory passed in from the game
    for (int32 i = 0; i < DefaultInventoryToAdd.Num(); i++)
    {
        AddInventory(GetWorld()->SpawnActor<ANZInventory>(DefaultInventoryToAdd[i], FVector(0.0f), FRotator(0, 0, 0)), true);
    }
}


void ANZCharacter::SwitchWeapon(ANZWeapon* NewWeapon)
{
	if (NewWeapon != NULL && !IsDead())
	{
		if (Role == ROLE_Authority)
		{
			ClientSwitchWeapon(NewWeapon);
		}
		else if (!IsLocallyControlled())
		{
			//UE_LOG(NZ, Warning, TEXT("Illegal SwitchWeapon() call on remote client"));
		}
		else
		{
			LocalSwitchWeapon(NewWeapon);
			ServerSwitchWeapon(NewWeapon);
		}
	}
}

void ANZCharacter::LocalSwitchWeapon(ANZWeapon* NewWeapon)
{
	if (!IsDead())
	{
		// Make sure clients don't try to switch to weapons that haven't been fully replicated/initialized or that have been removed and the client doesn't know yet
		if (NewWeapon != NULL && (NewWeapon->GetNZOwner() == NULL || (Role == ROLE_Authority && !IsInInventory(NewWeapon))))
		{
			ClientSwitchWeapon(Weapon);
		}
		else
		{
			if (Weapon == NULL)
			{
				if (NewWeapon != NULL)
				{
					// Initial equip
					PendingWeapon = NewWeapon;
					WeaponChanged();
				}
			}
			else if (NewWeapon != NULL)
			{
				if (NewWeapon != Weapon)
				{
					if (Weapon->PutDown())
					{
						// Standard weapon switch to some other weapon
						PendingWeapon = NewWeapon;
					}
				}
				else if (PendingWeapon != NULL)
				{
					// Switching back to weapon that was on its way down
					PendingWeapon = NULL;
					Weapon->BringUp();
				}
			}
			else if (Weapon != NULL && PendingWeapon != NULL && PendingWeapon->PutDown())
			{
				// Stopping weapon switch in progress by passing NULL
				PendingWeapon = NULL;
				Weapon->BringUp();
			}
		}
	}
}

void ANZCharacter::ServerSwitchWeapon_Implementation(ANZWeapon* NewWeapon)
{
	if (NewWeapon != NULL)
	{
		LocalSwitchWeapon(NewWeapon);
	}
}

bool ANZCharacter::ServerSwitchWeapon_Validate(ANZWeapon* NewWeapon)
{
	return true;
}

void ANZCharacter::ClientSwitchWeapon_Implementation(ANZWeapon* NewWeapon)
{
	LocalSwitchWeapon(NewWeapon);
	if (Role < ROLE_Authority)
	{
		ServerSwitchWeapon(NewWeapon);
	}
}

void ANZCharacter::WeaponChanged(float OverflowTime)
{
    if (PendingWeapon != NULL && PendingWeapon->GetNZOwner() == this)
    {
        checkSlow(IsInInventory(PendingWeapon));
        Weapon = PendingWeapon;
        PendingWeapon = NULL;
        WeaponClass = Weapon->GetClass();
        WeaponAttachmentClass = Weapon->AttachmentType;
        Weapon->BringUp(OverflowTime);
        //UpdateWeaponSkinPrefFromProfile();
        UpdateWeaponAttachment();
    }
    else if (Weapon != NULL && Weapon->GetNZOwner() == this)
    {
        Weapon->BringUp(OverflowTime);
    }
    else
    {
        Weapon = NULL;
        WeaponClass = NULL;
        WeaponAttachmentClass = NULL;
        UpdateWeaponAttachment();
    }
    
/*    if (GhostComponent->bChostRecording && Weapon != nullptr)
    {
        GhostComponent->GhostSwitchWeapon(Weapon);
    }*/
}

void ANZCharacter::ClientWeaponLost_Implementation(ANZWeapon* LostWeapon)
{
    if (IsLocallyControlled())
    {
        if (Weapon == LostWeapon)
        {
            Weapon = NULL;
            if (!IsDead())
            {
                if (PendingWeapon == NULL)
                {
                    SwitchToBestWeapon();
                }
                if (PendingWeapon != NULL)
                {
                    WeaponChanged();
                }
                else
                {
                    WeaponClass = NULL;
                    WeaponAttachmentClass = NULL;
                    UpdateWeaponAttachment();
                }
            }
        }
        else if (PendingWeapon == LostWeapon)
        {
            PendingWeapon = NULL;
            WeaponChanged();
        }
    }
}

void ANZCharacter::SwitchToBestWeapon()
{
	if (IsLocallyControlled())
	{
		ANZPlayerController* PC = Cast<ANZPlayerController>(Controller);
		if (PC != NULL)
		{
			PC->SwitchToBestWeapon();
		}
	}
}

void ANZCharacter::SetFlashLocation(const FVector& InFlashLoc, uint8 InFireMode)
{
    bLocalFlashLoc = IsLocallyControlled();
    // Make sure two consecutive shots don't set the same FlashLocation as that will prevent replication and thus clients won't see the shot
    FlashLocation = ((FlashLocation - InFlashLoc).SizeSquared() >= 1.0f) ? InFlashLoc : (InFlashLoc + FVector(0.0f, 0.0f, 1.0f));
    // We reserve the zero vector to stop firing, so make sure we aren't setting a value that would replicate that way
    if (FlashLocation.IsNearlyZero(1.0f))
    {
        FlashLocation.Z += 1.1f;
    }
    FireMode = InFireMode;
    FiringInfoUpdated();    
}

void ANZCharacter::IncrementFlashCount(uint8 InFireMode)
{
    FlashCount++;
    // We reserve zero for not firing; make sure we don't set that
    if ((FlashCount & 0xF) == 0)
    {
        FlashCount++;
    }
    FireMode = InFireMode;
    
    // Pack the Firemode in top 4 bits to prevent misfires when alternating projectile shots
    // eg pri shot, FC = 1 -> alt shot, FC = 0 (stop fire) -> FC = 1 (FC is still 1 so no rep)
    FlashCount = (FlashCount & 0x0F) | FireMode << 4;
    FiringInfoUpdated();
}

void ANZCharacter::SetFlashExtra(uint8 NewFlashExtra, uint8 InFireMode)
{
    FlashExtra = NewFlashExtra;
    FireMode = InFireMode;
    FiringExtraUpdated();    
}

void ANZCharacter::ClearFiringInfo()
{
    bLocalFlashLoc = false;
    FlashLocation = FVector::ZeroVector;
    FlashCount = 0;
    FlashExtra = 0;
    FiringInfoUpdated();    
}

void ANZCharacter::FiringInfoUpdated()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance != NULL)
    {
        AnimInstance->Montage_Stop(0.2f);
    }
    
    ANZPlayerController* NZPC = GetLocalViewer();
    if ((bLocalFlashLoc || NZPC == NULL || NZPC->GetPredictionTime() == 0.f || !IsLocallyControlled()) && Weapon != NULL && Weapon->ShouldPlay1PVisuals())
    {
        if (!FlashLocation.IsZero())
        {
            uint8 EffectFiringMode = Weapon->GetCurrentFireMode();
            // If non-local first person spectator, also play firing effects from here
            if (Controller == NULL)
            {
                EffectFiringMode = FireMode;
                Weapon->FiringInfoUpdated(FireMode, FlashCount, FlashLocation);
                Weapon->FiringEffectsUpdated(FireMode, FlashLocation);
            }
            else
            {
                FVector SpawnLocation;
                FRotator SpawnRotation;
                Weapon->GetImpactSpawnPosition(FlashLocation, SpawnLocation, SpawnRotation);
                Weapon->PlayImpactEffects(FlashLocation, EffectFiringMode, SpawnLocation, SpawnRotation);
            }
        }
        else if (Controller == NULL)
        {
            Weapon->FiringInfoUpdated(FireMode, FlashCount, FlashLocation);
        }
        if (FlashCount == 0 && FlashLocation.IsZero() && WeaponAttachment != NULL)
        {
            WeaponAttachment->StopFiringEffects();
        }
    }
    else if (WeaponAttachment != NULL)
    {
        if (FlashCount != 0 || !FlashLocation.IsZero())
        {
            if (!IsLocallyControlled() || NZPC == NULL || NZPC->IsBehindView())
            {
                WeaponAttachment->PlayFiringEffects();
            }
        }
        else
        {
            // Always call Stop to avoid effects mismatches where we switched view modes during a firing sequence
            // and some effect ends up being left on forever
            WeaponAttachment->StopFiringEffects();
        }
    }
}

void ANZCharacter::FiringExtraUpdated()
{
    ANZPlayerController* NZPC = Cast<ANZPlayerController>(Controller);
    if (WeaponAttachment != NULL && (!IsLocallyControlled() || NZPC == NULL || NZPC->IsBehindView()))
    {
        WeaponAttachment->FiringExtraUpdated();
    }
    if (Weapon != NULL && NZPC == NULL)
    {
        Weapon->FiringExtraUpdated(FlashExtra, FireMode);
    }
}

void ANZCharacter::FiringInfoReplicated()
{
    if (!bLocalFlashLoc)
    {
        FiringInfoUpdated();
    }
}

void ANZCharacter::UpdateWeaponAttachment()
{
    if (GetNetMode() != NM_DedicatedServer)
    {
        TSubclassOf<ANZWeaponAttachment> NewAttachmentClass = WeaponAttachmentClass;
        if (NewAttachmentClass == NULL)
        {
            NewAttachmentClass = (WeaponClass != NULL) ? WeaponClass.GetDefaultObject()->AttachmentType : NULL;
        }
        if (WeaponAttachment != NULL && (NewAttachmentClass == NULL || (WeaponAttachment != NULL && WeaponAttachment->GetClass() != NewAttachmentClass)))
        {
            WeaponAttachment->Destroy();
            WeaponAttachment = NULL;
        }
        if (WeaponAttachment == NULL && NewAttachmentClass != NULL)
        {
            FActorSpawnParameters Params;
            Params.Instigator = this;
            Params.Owner = this;
            WeaponAttachment = GetWorld()->SpawnActor<ANZWeaponAttachment>(NewAttachmentClass, Params);
            if (WeaponAttachment != NULL)
            {
                WeaponAttachment->AttachToOwner();
            }
        }
        
        //UpdateWeaponSkin();
    }
}

void ANZCharacter::UpdateHolsteredWeaponAttachment()
{
    if (GetNetMode() != NM_DedicatedServer)
    {
        TSubclassOf<ANZWeaponAttachment> NewAttachmentClass = HolsteredWeaponAttachmentClass;
        if (HolsteredWeaponAttachment != NULL &&
            (NewAttachmentClass == NULL || (HolsteredWeaponAttachment != NULL && HolsteredWeaponAttachment->GetClass() != NewAttachmentClass)))
        {
            HolsteredWeaponAttachment->Destroy();
            HolsteredWeaponAttachment = NULL;
        }
        if (HolsteredWeaponAttachment == NULL && NewAttachmentClass != NULL)
        {
            FActorSpawnParameters Params;
            Params.Instigator = this;
            Params.Owner = this;
            HolsteredWeaponAttachment = GetWorld()->SpawnActor<ANZWeaponAttachment>(NewAttachmentClass, Params);
            if (HolsteredWeaponAttachment != NULL)
            {
                HolsteredWeaponAttachment->HolsterToOwner();
            }
        }
    }
}

void ANZCharacter::SetWalkMovementReduction(float InPct, float InDuration)
{
	WalkMovementReductionPct = (InDuration > 0.f) ? InPct : 0.f;
	WalkMovementReductionTime = InDuration;
	if (NZCharacterMovement)
	{
		NZCharacterMovement->NeedsClientAdjustment();
	}
}


void ANZCharacter::StartFire(uint8 FireModeNum)
{
    //UE_LOG(LogNZCharacter, Verbose, TEXT("StartFire %d"), FireModeNum);

    if (!IsLocallyControlled())
    {
        //UE_LOG(LogNZCharacter, Warning, TEXT("StartFire() can only be called on the owning client"));
    }
/*    else if (bFeigningDeath)
    {
        FeignDeath();
    }*/
    else if (Weapon != NULL && EmoteCount == 0)
    {
        Weapon->StartFire(FireModeNum);
    }
    
    // todo:
}

void ANZCharacter::StopFire(uint8 FireModeNum)
{
    if (DrivenVehicle ? !DrivenVehicle->IsLocallyControlled() : !IsLocallyControlled())
    {
        //UE_LOG(LogNZCharacter, Warning, TEXT("StopFire() can only be called on the owning client"));
    }
    else if (Weapon != NULL)
    {
        Weapon->StopFire(FireModeNum);
    }
    else
    {
        SetPendingFire(FireModeNum, false);
    }
    
    // todo:
}

void ANZCharacter::StopFiring()
{
    for (int32 i = 0; i < PendingFire.Num(); i++)
    {
        if (PendingFire[i])
        {
            StopFire(i);
        }
    }
}

void ANZCharacter::ChangeClip()
{
    ANZGun* Gun = Cast<ANZGun>(Weapon);
	if (Gun != NULL)
	{
        Gun->ChangeClip();
	}
}

void ANZCharacter::LeftHandChangeClip()
{
	ANZDualGun* DualGun = Cast<ANZDualGun>(Weapon);
	if (DualGun != NULL)
	{
		DualGun->LeftHandChangeClip();
	}
}

void ANZCharacter::SetAmbientSound(USoundBase* NewAmbientSound, bool bClear)
{
    if (bClear)
    {
        if (NewAmbientSound == AmbientSound)
        {
            AmbientSound = NULL;
        }
    }
    else
    {
        AmbientSound = NewAmbientSound;
    }
    AmbientSoundUpdated();
}

void ANZCharacter::AmbientSoundUpdated()
{
    if (AmbientSound == NULL)
    {
        if (AmbientSoundComp != NULL)
        {
            AmbientSoundComp->Stop();
        }
    }
    else
    {
        if (AmbientSoundComp == NULL)
        {
            AmbientSoundComp = NewObject<UAudioComponent>(this);
            AmbientSoundComp->bAutoDestroy = false;
            AmbientSoundComp->bAutoActivate = false;
            //AmbientSoundComp->AttachTo(RootComponent);
			AmbientSoundComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            AmbientSoundComp->RegisterComponent();
        }
        if (AmbientSoundComp->Sound != AmbientSound)
        {
            // Don't attenuate/spatialize sounds made by a local viewtarget
            AmbientSoundComp->bAllowSpatialization = true;
            
            if (GEngine->GetMainAudioDevice() && !GEngine->GetMainAudioDevice()->IsHRTFEnabledForAll())
            {
                for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
                {
                    if (It->PlayerController != NULL && It->PlayerController->GetViewTarget() == this)
                    {
                        AmbientSoundComp->bAllowSpatialization = false;
                        break;
                    }
                }
            }
            
            AmbientSoundComp->SetSound(AmbientSound);
        }
        if (!AmbientSoundComp->IsPlaying())
        {
            AmbientSoundComp->Play();
        }
    }
}

void ANZCharacter::SetLocalAmbientSound(USoundBase* NewAmbientSound, float SoundVolume, bool bClear)
{
    if (bClear)
    {
        if ((NewAmbientSound != NULL) && (NewAmbientSound == LocalAmbientSound))
        {
            LocalAmbientSound = NULL;
            LocalAmbientSoundUpdated();
        }
    }
    else
    {
        LocalAmbientSound = NewAmbientSound;
        LocalAmbientSoundUpdated();
        if (LocalAmbientSoundComp && LocalAmbientSound)
        {
            LocalAmbientSoundComp->SetVolumeMultiplier(SoundVolume);
        }
    }
}

void ANZCharacter::LocalAmbientSoundUpdated()
{
    if (LocalAmbientSound == NULL)
    {
        if (LocalAmbientSoundComp != NULL)
        {
            LocalAmbientSoundComp->Stop();
        }
    }
    else
    {
        if (LocalAmbientSoundComp == NULL)
        {
            LocalAmbientSoundComp = NewObject<UAudioComponent>(this);
            LocalAmbientSoundComp->bAutoDestroy = false;
            LocalAmbientSoundComp->bAutoActivate = false;
            //LocalAmbientSoundComp->AttachTo(RootComponent);
			LocalAmbientSoundComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            LocalAmbientSoundComp->RegisterComponent();
        }
        if (LocalAmbientSoundComp->Sound != LocalAmbientSound)
        {
            LocalAmbientSoundComp->SetSound(LocalAmbientSound);
        }
        if (!LocalAmbientSoundComp->IsPlaying())
        {
            LocalAmbientSoundComp->Play();
        }
    }
}

void ANZCharacter::PlayLandedEffect_Implementation()
{
    UParticleSystem* EffectToPlay = ((GetNetMode() != NM_DedicatedServer) && (FMath::Abs(GetCharacterMovement()->Velocity.Z)) > LandEffectSpeed) ? LandEffect : NULL;
    ANZWorldSettings* WS = Cast<ANZWorldSettings>(GetWorld()->GetWorldSettings());
    if ((EffectToPlay != NULL) && WS->EffectIsRelevant(this, GetActorLocation(), true, true, 10000.f, 0.f, false))
    {
        FRotator EffectRot = GetCharacterMovement()->CurrentFloor.HitResult.Normal.Rotation();
        EffectRot.Pitch -= 90.f;
        UParticleSystemComponent* LandedPSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EffectToPlay, GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 4.f), EffectRot, true);
        float EffectScale = FMath::Clamp(FMath::Square(GetCharacterMovement()->Velocity.Z) / (2.f * LandEffectSpeed * LandEffectSpeed), 0.5f, 1.f);
        LandedPSC->SetRelativeScale3D(FVector(EffectScale, EffectScale, 1.f));
    }
}


void ANZCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// Find out which way is forward
		const FRotator Rotation = GetControlRotation();
		FRotator YawRotation = (NZCharacterMovement && NZCharacterMovement->Is3DMovementMode()) ? Rotation : FRotator(0, Rotation.Yaw, 0);

		// Add Movement in forward direction
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void ANZCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// Find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Add Movement in right direction
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

void ANZCharacter::MoveUp(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in up direction
		AddMovementInput(FVector(0.f, 0.f, 1.f), Value);
	}
}

void ANZCharacter::Sprint()
{
	if (GetCharacterMovement()->Velocity.Size() >= 20)
	{
		bSprinting = true;
	}
}

void ANZCharacter::UnSprint()
{
    bSprinting = false;
}


APlayerCameraManager* ANZCharacter::GetPlayerCameraManager()
{
    ANZPlayerController* PC = GetLocalViewer();
    return PC != NULL ? PC->PlayerCameraManager : NULL;
}

void ANZCharacter::PlayFootstep(uint8 FootNum, bool bFirstPerson)
{
    if ((GetWorld()->TimeSeconds - LastFootstepTime < 0.1f) || IsDead())
    {
        return;
    }
    
    ANZPlayerController* NZPC = Cast<ANZPlayerController>(Controller);
    if (NZPC && IsLocallyControlled() && !bFirstPerson && !NZPC->IsBehindView())
    {
        return;
    }
    
    UParticleSystem* FootStepEffect = NULL;
    float MaxParticleDist = 1500.f;
    if (FeetAreInWater())
    {
        UNZGameplayStatics::NZPlaySound(GetWorld(), WaterFootstepSound, this, SRT_IfSourceNotReplicated);
        FootStepEffect = WaterFootstepEffect;
        MaxParticleDist = 5000.f;
    }
    else if (GetLocalViewer())
    {
        UNZGameplayStatics::NZPlaySound(GetWorld(), OwnFootstepSound, this, SRT_IfSourceNotReplicated);
        FootStepEffect = GetLocalViewer()->IsBehindView() && (GetVelocity().Size() > 500.f) ? GroundFootstepEffect : NULL;
    }
    else
    {
        UNZGameplayStatics::NZPlaySound(GetWorld(), FootstepSound, this, SRT_IfSourceNotReplicated);
        FootStepEffect = (GetVelocity().Size() > 500.f) ? GroundFootstepEffect : NULL;
    }
    if (FootStepEffect && GetMesh() && (GetWorld()->GetTimeSeconds() - GetMesh()->LastRenderTime < 0.05f) && (GetLocalViewer() || (GetCachedScalabilityCVars().DetailMode != 0)))
    {
        ANZWorldSettings* WS = Cast<ANZWorldSettings>(GetWorld()->GetWorldSettings());
        if (WS->EffectIsRelevant(this, GetActorLocation(), true, true, MaxParticleDist, 0.f, false))
        {
            FVector EffectLocation = GetActorLocation();
            EffectLocation.Z = EffectLocation.Z + 4.f - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FootStepEffect, EffectLocation, GetActorRotation(), true);
        }
    }
    LastFoot = FootNum;
    LastFootstepTime = GetWorld()->TimeSeconds;
}

void ANZCharacter::PlayJump_Implementation(const FVector& JumpLocation, const FVector& JumpDir)
{
    
}

void ANZCharacter::TakeFallingDamage(const FHitResult& Hit, float FallingSpeed)
{
    if (Role == ROLE_Authority && NZCharacterMovement)
    {
        if (FallingSpeed < -1.f * MaxSafeFallSpeed && !HandleFallingDamage(FallingSpeed, Hit))
        {
            if (FeetAreInWater())
            {
                FallingSpeed += 100.f;
            }
            if (FallingSpeed < -1.f * MaxSafeFallSpeed)
            {
                float FallingDamage = -100.f * (FallingSpeed + MaxSafeFallSpeed) / MaxSafeFallSpeed;
                FallingDamage -= NZCharacterMovement->FallingDamageReduction(FallingDamage, Hit);
                if (FallingDamage >= 1.0f)
                {
                    FNZPointDamageEvent DamageEvent(FallingDamage, Hit, GetCharacterMovement()->Velocity.GetSafeNormal(), UNZDamageType_Fell::StaticClass());
                    TakeDamage(DamageEvent.Damage, DamageEvent, Controller, this);
                }
            }
        }
    }
}

void ANZCharacter::Landed(const FHitResult& Hit)
{
    
}



bool ANZCharacter::FeetAreInWater() const
{
    FVector FootLocation = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
    return (PositionIsInWater(FootLocation) != NULL);
}

APhysicsVolume* ANZCharacter::PositionIsInWater(const FVector& Position) const
{
    // todo:
    return NULL;
}




float ANZCharacter::GetFireRateMultiplier()
{
    return FMath::Max<float>(FireRateMultiplier, 0.1f);
}

void ANZCharacter::SetFireRateMultiplier(float InMult)
{
    FireRateMultiplier = InMult;
    FireRateChanged();
}

void ANZCharacter::FireRateChanged()
{
    if (Weapon != NULL)
    {
        Weapon->UpdateTiming();
    }
}



void ANZCharacter::OnRep_DrivenVehicle()
{
    if (DrivenVehicle)
    {
        StartDriving(DrivenVehicle);
    }    
}

void ANZCharacter::StartDriving(APawn* Vehicle)
{
    DrivenVehicle = Vehicle;
    StopFiring();
    if (GetCharacterMovement() != NULL)
    {
        GetCharacterMovement()->StopActiveMovement();
    }
}

void ANZCharacter::StopDriving(APawn* Vehicle)
{
    if (DrivenVehicle == Vehicle)
    {
        DrivenVehicle = NULL;
    }
}

void ANZCharacter::PlayDying()
{
    TimeOfDeath = GetWorld()->TimeSeconds;
    
    SetAmbientSound(NULL);
    SetLocalAmbientSound(NULL);
    
    SpawnBloodDecal(GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()), FVector(0.f, 0.f, -1.f));
    LastDeathDecalTime = GetWorld()->TimeSeconds;
    
    if (GetNetMode() != NM_DedicatedServer && (GetWorld()->TimeSeconds - GetLastRenderTime() < 3.f || IsLocallyViewed()))
    {
        TSubclassOf<UNZDamageType> NZDmg(*LastTakeHitInfo.DamageType);
        
        {
            //StartRagdoll();
            
            if (NZDmg != NULL)
            {
                NZDmg.GetDefaultObject()->PlayDeathEffects(this);
            }
            
            // StartRagdoll() changes collision properties, which can potentially result in a new overlay -> more damage -> gib explosion -> Destroy()
            // SetTimer() has a dumb assert if the target of the function is already destroyed, so we need to check it ourselves
            if (!IsPendingKillPending())
            {
                FTimerHandle TempHandle;
                GetWorldTimerManager().SetTimer(TempHandle, this, &ANZCharacter::DeathCleanupTimer, 15.f, false);
            }
        }
    }
    else
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        SetLifeSpan(0.25f);
    }
}

bool ANZCharacter::IsRecentlyDead()
{
    return IsDead() && (GetWorld()->GetTimeSeconds() - TimeOfDeath < 1.f);
}

void ANZCharacter::DeactivateSpawnProtection()
{
    bSpawnProtectionEligible = false;
    // TODO: visual effect
}

bool ANZCharacter::IsDead()
{
    return bTearOff || IsPendingKillPending();
}

void ANZCharacter::DeathCleanupTimer()
{
    if (!IsLocallyViewed() && (bHidden || GetWorld()->TimeSeconds - GetLastRenderTime() > 0.5f || GetWorld()->TimeSeconds - TimeOfDeath > MaxDeathLifeSpan))
    {
        Destroy();
    }
    else
    {
        FTimerHandle TempHandle;
        GetWorldTimerManager().SetTimer(TempHandle, this, &ANZCharacter::DeathCleanupTimer, 0.5f, false);
    }
}





FVector ANZCharacter::GetLocationCenterOffset() const
{
    return (!IsRagdoll() || RootComponent != GetMesh()) ? FVector::ZeroVector : (GetMesh()->Bounds.Origin - GetMesh()->GetComponentLocation());
}



void ANZCharacter::SetWeaponAttachmentClass(TSubclassOf<class ANZWeaponAttachment> NewWeaponAttachmentClass)
{
    
}

void ANZCharacter::SetHolsteredWeaponAttachmentClass(TSubclassOf<class ANZWeaponAttachment> NewWeaponAttachmentClass)
{
    
}

void ANZCharacter::UpdateCharOverlays()
{
    
}

void ANZCharacter::UpdateWeaponOverlays()
{
    
}

void ANZCharacter::SetSkin(UMaterialInterface* NewSkin)
{
    
}

void ANZCharacter::UpdateSkin()
{
    
}

float ANZCharacter::GetWeaponBobScaling()
{
	ANZPlayerController* PC = Cast<ANZPlayerController>(GetController());
	return PC ? PC->WeaponBobGlobalScaling : 1.f;
}

FVector ANZCharacter::GetWeaponBobOffset(float DeltaTime, ANZWeapon* MyWeapon)
{
	FRotationMatrix RotMatrix = FRotationMatrix(GetViewRotation());
	FVector X = RotMatrix.GetScaledAxis(EAxis::X);
	FVector Y = RotMatrix.GetScaledAxis(EAxis::Y);
	FVector Z = RotMatrix.GetScaledAxis(EAxis::Z);

	float InterpTime = FMath::Min(1.f, WeaponJumpBobInterpRate * DeltaTime);
	if (!GetCharacterMovement() || GetCharacterMovement()->IsFalling())
	{
		BobTime = 0.f;
		CurrentWeaponBob.Y *= FMath::Max(0.f, 1.f - WeaponLandBobDecayRate * DeltaTime);
		CurrentWeaponBob.Z *= FMath::Max(0.f, 1.f - WeaponLandBobDecayRate * DeltaTime);
	}
	else
	{
		float Speed = GetCharacterMovement()->Velocity.Size();
		float LastBobTime = BobTime;
		float BobFactor = (WeaponBreathingBobRate + WeaponRunningBobRate * Speed / GetCharacterMovement()->MaxWalkSpeed);
		BobTime += DeltaTime * BobFactor;
		DesiredJumpBob *= FMath::Max(0.f, 1.f - WeaponLandBobDecayRate * DeltaTime);
		FVector AccelDir = GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal();
		if ((AccelDir | GetCharacterMovement()->Velocity) < 0.5 * GetCharacterMovement()->MaxWalkSpeed)
		{
			if ((AccelDir | Y) > 0.65f)
			{
				DesiredJumpBob.Y = -1.f * WeaponDirChangeDeflection;
			}
			else if ((AccelDir | Y) < -0.65f)
			{
				DesiredJumpBob.Y = WeaponDirChangeDeflection;
			}
		}
		CurrentWeaponBob.X = 0.f;
		if (NZCharacterMovement && /*NZCharacterMovement->bIsFloorSliding*/false)
		{
		}
		else
		{
			CurrentWeaponBob.Y = WeaponBobMagnitude.Y * BobFactor * FMath::Sin(8.f * BobTime);
			CurrentWeaponBob.Z = WeaponBobMagnitude.Z * BobFactor * FMath::Sin(16.f * BobTime);
		}

		if (GetCharacterMovement()->MovementMode == MOVE_Walking && Speed > 10.0f && !bIsCrouched &&
			(FMath::FloorToInt(0.5f + 8.f * BobTime / PI) != FMath::FloorToInt(0.5f + 8.f * LastBobTime / PI)) &&
			(GetMesh()->MeshComponentUpdateFlag >= EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered) && !GetMesh()->bRecentlyRendered)
		{
			//PlayFootstep((LastFoot + 1£© & 1, true);
		}
	}

	float JumpYInterp = ((DesiredJumpBob.Y == 0.f) || (DesiredJumpBob.Z == 0.f)) ? FMath::Min(1.f, WeaponJumpBobInterpRate * DeltaTime) : FMath::Min(1.f, WeaponHorizontalBobInterpRate * FMath::Abs(DesiredJumpBob.Y) * DeltaTime);
	CurrentJumpBob.X = (1.f - InterpTime) * CurrentJumpBob.X + InterpTime * DesiredJumpBob.X;
	CurrentJumpBob.Y = (1.f - JumpYInterp) * CurrentJumpBob.Y + JumpYInterp * DesiredJumpBob.X;
	CurrentJumpBob.Z = (1.f - InterpTime) * CurrentJumpBob.Z + InterpTime * DesiredJumpBob.Z;

	ANZPlayerController* MyPC = Cast<ANZPlayerController>(GetController());
	float WeaponBobGlobalScaling = (MyWeapon ? MyWeapon->WeaponBobScaling : 1.f) * (MyPC ? MyPC->WeaponBobGlobalScaling : 1.f);
	return WeaponBobGlobalScaling * (CurrentWeaponBob.Y + CurrentJumpBob.Y) * Y + WeaponBobGlobalScaling * (CurrentWeaponBob.Z + CurrentJumpBob.Z) * Z + CrouchEyeOffset + GetTransformedEyeOffset();
}

FVector ANZCharacter::GetTransformedEyeOffset() const
{
	FRotationMatrix ViewRotMatrix = FRotationMatrix(GetViewRotation());
	FVector XTransform = ViewRotMatrix.GetScaledAxis(EAxis::X) * EyeOffset.X;
	if ((XTransform.Z > KINDA_SMALL_NUMBER) && (XTransform.Z + EyeOffset.Z + BaseEyeHeight + CrouchEyeOffset.Z > GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 12.f))
	{
		float MaxZ = FMath::Max(0.f, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - 12.f - EyeOffset.Z - BaseEyeHeight - CrouchEyeOffset.Z);
		XTransform = XTransform * MaxZ / XTransform.Z;
	}
	return GetEyeOffsetScaling() * (XTransform + ViewRotMatrix.GetScaledAxis(EAxis::Y) * EyeOffset.Y) + FVector(0.f, 0.f, EyeOffset.Z);
}

float ANZCharacter::GetEyeOffsetScaling() const
{
	float EyeOffsetGlobalScaling = Cast<ANZPlayerController>(GetController()) ? Cast<ANZPlayerController>(GetController())->EyeOffsetGlobalScaling : 1.f;
	return FMath::Clamp(EyeOffsetGlobalScaling, 0.f, 1.f);
}

FVector ANZCharacter::GetPawnViewLocation() const
{
	return GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight) + CrouchEyeOffset + GetTransformedEyeOffset();
}

void ANZCharacter::SetPunchAngle(FRotator Angle)
{
    if (PunchAnglePitch != Angle.Pitch)
    {
        UE_LOG(NZ, Log,  TEXT("PunchAnglePitch:%d, Angle.Pitch:%d"), int32(PunchAnglePitch * 182.0444), int32(Angle.Pitch * 182.0444));
    }
    
	PunchAnglePitch = Angle.Pitch;
	PunchAngleYaw = Angle.Yaw;
}

FRotator ANZCharacter::GetPunchAngle()
{
	FRotator PunchAngle;
	PunchAngle.Pitch = PunchAnglePitch;
	PunchAngle.Yaw = PunchAngleYaw;
	PunchAngle.Roll = 0;
	return PunchAngle;
}


void ANZCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (bFindCameraComponentWhenViewTarget && CharacterCameraComponent && CharacterCameraComponent->bIsActive)
	{
		float SavedFOV = OutResult.FOV;
		CharacterCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.FOV = SavedFOV;
		OutResult.Location = OutResult.Location + CrouchEyeOffset + GetTransformedEyeOffset();
	}
	else
	{
		GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
	}

	if (Weapon != NULL)
	{
		Weapon->WeaponCalcCamera(DeltaTime, OutResult.Location, OutResult.Rotation);
	}
}

bool ANZCharacter::TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest, bool bNoCheck)
{
    return Super::TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);
}

void ANZCharacter::OnOverlapBegin(AActor* OtherActor)
{

}

void ANZCharacter::CheckRagdollFallingDamage(const FHitResult& Hit)
{
    FVector MeshVelocity = GetMesh()->GetComponentVelocity();
    // Physics numbers don't seem to match up... biasing towards more falling damage over less to minimize exploits
    // Besides, faceplanting ought to hurt more than landing on your feet, right? :)
    MeshVelocity.Z *= 2.0f;
    if (MeshVelocity.Z < -1.f * MaxSafeFallSpeed)
    {
        FVector SavedVelocity = GetCharacterMovement()->Velocity;
        GetCharacterMovement()->Velocity = MeshVelocity;
        TakeFallingDamage(Hit, GetCharacterMovement()->Velocity.Z);
        GetCharacterMovement()->Velocity = SavedVelocity;
        // Clear Z velocity on the mesh so that this collision won't happen again unless there's a new fall
        for (int32 i = 0; i < GetMesh()->Bodies.Num(); i++)
        {
            FVector Vel = GetMesh()->Bodies[i]->GetUnrealWorldVelocity();
            Vel.Z = 0.f;
            GetMesh()->Bodies[i]->SetLinearVelocity(Vel, false);
        }
    }
}

void ANZCharacter::OnRagdollCollision(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (IsDead())
    {
        if (NormalImpulse.SizeSquared() > RagdollCollisionBleedThreshold * RagdollCollisionBleedThreshold)
        {
            // Maybe spawn blood as the ragdoll smacks into things
            if (OtherComp != NULL && OtherActor != this && GetWorld()->TimeSeconds - LastDeathDecalTime > 0.25f && GetWorld()->TimeSeconds - GetLastRenderTime() < 1.f)
            {
                SpawnBloodDecal(GetActorLocation(), -Hit.Normal);
                LastDeathDecalTime = GetWorld()->TimeSeconds;
            }
        }
    }
    // Cause falling damage on Z axis collisions
    else if (!bInRagdollRecovery)
    {
        CheckRagdollFallingDamage(Hit);
    }
}

ANZPlayerController* ANZCharacter::GetLocalViewer()
{
    if (CurrentViewerPC && ((Controller == CurrentViewerPC) || (CurrentViewerPC->GetViewTarget() == this)))
    {
        return CurrentViewerPC;
    }
    CurrentViewerPC = NULL;
    for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
    {
        if (It->PlayerController != NULL && It->PlayerController->GetViewTarget() == this)
        {
            CurrentViewerPC = Cast<ANZPlayerController>(It->PlayerController);
            break;
        }
    }
    return CurrentViewerPC;
}
