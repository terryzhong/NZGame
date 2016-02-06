// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCharacter.h"
#include "NZBot.h"
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


// Sets default values
ANZCharacter::ANZCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UClass> DefaultCharacterContentRef(TEXT("Class'/Game/Characters/Malcolm/Malcolm.Malcolm_C'"));
    CharacterData = DefaultCharacterContentRef.Object;
    
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);
    
    // Create a CameraComponent
    CharacterCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CharacterCameraComponent->AttachParent = GetCapsuleComponent();
    DefaultBaseEyeHeight = 67.5f;
    BaseEyeHeight = DefaultBaseEyeHeight;
    DefaultCrouchedEyeHeight = 30.f;
    CrouchedEyeHeight = DefaultCrouchedEyeHeight;
    CharacterCameraComponent->RelativeLocation = FVector(0, 0, DefaultBaseEyeHeight);
	CharacterCameraComponent->bUsePawnControlRotation = true;
    
    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->AttachParent = CharacterCameraComponent;
    FirstPersonMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;
    FirstPersonMesh->bReceivesDecals = false;
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
    //bDamageHurtsHealth = true;
    FireRateMultiplier = 1.0f;
    bSpawnProtectionEligible = true;
    //MaxSafeFallSpeed = 2400.0f;
    //FallingDamageFactor = 100.0f;
    //CrushingDamageFactor = 2.0f;
    
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
    
    // Adjust MaxSavedPositionAge for bot tracking purposes
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ANZBot* B = Cast<ANZBot>(It->Get());
        if (B != NULL)
        {
            MaxSavedPositionAge = FMath::Max<float>(MaxSavedPositionAge, B->TrackingReactionTime);
        }
    }

    Super::BeginPlay();
}

// Called every frame
void ANZCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ANZCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ANZCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

bool ANZCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
    // We want to allow zero damage (momentum only) hits so never pass 0 to super call
    return bTearOff || Super::ShouldTakeDamage(FMath::Max<float>(1.0f, Damage), DamageEvent, EventInstigator, DamageCauser);
}

float ANZCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    return 0.f;
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
    
}

void ANZCharacter::PlayTakeHitEffects_Implementation()
{
    
}

void ANZCharacter::PlayDamageEffects_Implementation()
{
    
}

bool ANZCharacter::K2_Died(AController* EventInstigator, TSubclassOf<UDamageType> DamageType)
{
    return false;
}

void ANZCharacter::Died(AController* EventInstigator, const FDamageEvent& DamageEvent)
{
    
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

void ANZCharacter::NotifyPendingServerFire()
{
    if (SavedPositions.Num() > 0)
    {
        SavedPositions.Last().bShotSpawned = true;
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
    // todo:
    check(false);
}

void ANZCharacter::FiringExtraUpdated()
{
    // todo:
    check(false);
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
    
}


bool ANZCharacter::IsDead()
{
	return bTearOff || IsPendingKillPending();
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

APlayerCameraManager* ANZCharacter::GetPlayerCameraManager()
{
    ANZPlayerController* PC = GetLocalViewer();
    return PC != NULL ? PC->PlayerCameraManager : NULL;
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
	float WeaponBobGlobalScaling = (MyWeapon ? MyWeapon->WeaponBobScaling : 1.f)/* * (MyPC ? MyPC->WeaponBobGlobalScaling) : 1.f)*/;
	return WeaponBobGlobalScaling * (CurrentWeaponBob.Y + CurrentJumpBob.Y) * Y + WeaponBobGlobalScaling * (CurrentWeaponBob.Z + CurrentJumpBob.Z) * Z + CrouchEyeOffset + GetTransformedEyeOffset();
}

FVector ANZCharacter::GetTransformedEyeOffset() const
{
	return FVector(0.f);
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


