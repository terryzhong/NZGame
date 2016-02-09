// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDroppedPickup.h"
#include "NZInventory.h"
#include "NZGameMode.h"
#include "NZProjectileMovementComponent.h"
#include "UnrealNetwork.h"
#include "NZRecastNavMesh.h"
#include "NZWorldSettings.h"
#include "NZPickupMessage.h"


// Sets default values
ANZDroppedPickup::ANZDroppedPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    InitialLifeSpan = 15.0f;
    
    Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Collision->SetCollisionProfileName(FName(TEXT("Pickup")));
    Collision->InitCapsuleSize(64.0f, 30.0f);
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ANZDroppedPickup::OnOverlapBegin);
    RootComponent = Collision;
    
    Movement = CreateDefaultSubobject<UNZProjectileMovementComponent>(TEXT("Movement"));
    Movement->HitZStopSimulatingThreshold = 0.7f;
    Movement->UpdatedComponent = Collision;
    Movement->OnProjectileStop.AddDynamic(this, &ANZDroppedPickup::PhysicsStopped);

    SetReplicates(true);
    bReplicateMovement = true;
    NetUpdateFrequency = 1.0f;
}

// Called when the game starts or when spawned
void ANZDroppedPickup::BeginPlay()
{
	Super::BeginPlay();
	
    if (!IsPendingKillPending())
    {
        // Don't allow Instigator to touch until a little time has passed so a live player throwing an item doesn't immediately pick it back up again
        GetWorld()->GetTimerManager().SetTimer(EnableInstigatorTouchHandle, this, &ANZDroppedPickup::EnableInstigatorTouch, 1.0f, false);
    }
}

void ANZDroppedPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // todo:
/*    ANZRecastNavMesh* NavData = GetNZNavData(GetWorld());
    if (NavData != NULL)
    {
        NavData->RemoveFromNavigation(false);
    }
    if (Inventory != NULL && !Inventory->IsPendingKillPending())
    {
        Inventory->Destroy();
    }
    GetWorldTimerManager().ClearAllTimersForObject(this);*/
}

void ANZDroppedPickup::PostNetReceiveVelocity(const FVector& NewVelocity)
{
    Movement->Velocity = NewVelocity;
}

void ANZDroppedPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION(ANZDroppedPickup, InventoryType, COND_None);
    //DOREPLIFETIME_CONDITION(ANZDroppedPickup, WeaponSkin, COND_None);
}

USoundBase* ANZDroppedPickup::GetPickupSound_Implementation() const
{
    if (Inventory != NULL)
    {
        return Inventory->PickupSound;
    }
    else if (InventoryType != NULL)
    {
        return InventoryType.GetDefaultObject()->PickupSound;
    }
    else
    {
        return NULL;
    }
}

void ANZDroppedPickup::SetInventory(ANZInventory* NewInventory)
{
    Inventory = NewInventory;
    InventoryType = (NewInventory != NULL) ? NewInventory->GetClass() : NULL;
    InventoryTypeUpdated();
    
    bFullyInitialized = true;
    CheckTouching();
}

void ANZDroppedPickup::InventoryTypeUpdated_Implementation()
{
    //ANZPickupInventory::CreatePickupMesh(this, Mesh, InventoryType, 0.0f, FRotator::ZeroRotator, false);
}

void ANZDroppedPickup::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bFullyInitialized && (OtherActor != Instigator || !GetWorld()->GetTimerManager().IsTimerActive(EnableInstigatorTouchHandle)))
    {
        APawn* P = Cast<APawn>(OtherActor);
        if (P != NULL && !P->bTearOff && !GetWorld()->LineTraceTestByChannel(P->GetActorLocation(), GetActorLocation(), ECC_Pawn, FCollisionQueryParams(), WorldResponseParams))
        {
            ProcessTouch(P);
        }
    }
}

void ANZDroppedPickup::PhysicsStopped(const FHitResult& ImpactResult)
{
    // It we landed on a mover, attach to it
    if (ImpactResult.Component != NULL && ImpactResult.Component->Mobility == EComponentMobility::Movable)
    {
        Collision->AttachTo(ImpactResult.Component.Get(), NAME_None, EAttachLocation::KeepWorldPosition);
    }
    ANZRecastNavMesh* NavData = GetNZNavData(GetWorld());
    if (NavData != NULL)
    {
        NavData->AddToNavigation(this);
    }
}

bool ANZDroppedPickup::AllowPickupBy_Implementation(APawn* Other, bool bDefaultAllowPickup)
{
    ANZCharacter* NZC = Cast<ANZCharacter>(Other);
    bDefaultAllowPickup = bDefaultAllowPickup && NZC != NULL && NZC->bCanPickupItems && !NZC->IsRagdoll();
    bool bAllowPickup = bDefaultAllowPickup;
    ANZGameMode* NZGameMode = GetWorld()->GetAuthGameMode<ANZGameMode>();
    return (NZGameMode == NULL || !NZGameMode->OverridePickupQuery(Other, InventoryType, this, bAllowPickup)) ? bDefaultAllowPickup : bAllowPickup;
}

void ANZDroppedPickup::ProcessTouch_Implementation(APawn* TouchedBy)
{
    if (Role == ROLE_Authority && TouchedBy->Controller != NULL && AllowPickupBy(TouchedBy, true))
    {
        PlayTakenEffects(TouchedBy);    // First allows PlayTakenEffects() to work off Inventory instead of InventoryType if it wants
        GiveTo(TouchedBy);
        ANZGameMode* NZGame = GetWorld()->GetAuthGameMode<ANZGameMode>();
        if (NZGame != NULL && NZGame->NumBots > 0)
        {
            float Radius = 0.0f;
            USoundBase* PickupSound = GetPickupSound();
            if (PickupSound)
            {
                Radius = PickupSound->GetMaxAudibleDistance();
                const FAttenuationSettings* Settings = PickupSound->GetAttenuationSettingsToApply();
                if (Settings != NULL)
                {
                    Radius = FMath::Max<float>(Radius, Settings->GetMaxDimension());
                }
            }
            // todo:
/*            for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
            {
                if (It->IsValid())
                {
                    ANZBot* B = Cast<ANZBot>(It->Get());
                    if (B != NULL)
                    {
                        B->NotifyPickup(TouchedBy, this, Radius);
                    }
                }
            }*/
        }
        Destroy();
    }
}

void ANZDroppedPickup::GiveTo_Implementation(APawn* Target)
{
    if (Inventory != NULL && !Inventory->IsPendingKillPending())
    {
        ANZCharacter* C = Cast<ANZCharacter>(Target);
        if (C != NULL)
        {
            ANZInventory* Duplicate = C->FindInventoryType<ANZInventory>(Inventory->GetClass(), true);
            if (Duplicate == NULL || !Duplicate->StackPickup(Inventory))
            {
                C->AddInventory(Inventory, true);
                
/*                ANZWeapon* WeaponInv = Cast<ANZWeapon>(Inventory);
                if (WeaponInv)
                {
                    if (WeaponSkin)
                    {
                        C->SetSkinForWeapon(WeaponSkin);
                    }
                    else
                    {
                        FString WeaponPathName = WeaponInv->GetPathName();
                        
                        bool bFoundSkin = false;
                        // Set character's skin back to what the NZPlayerState has
                        ANZPlayerState* PS = Cast<ANZPLayerState>(C->PlayerState);
                        if (PS)
                        {
                            for (int32 i = 0; i < PS->WeaponSkins.Num(); i++)
                            {
                                if (PS->WeaponSkins[i]->WeaponType == WeaponPathName)
                                {
                                    C->SetSkinForWeapon(PS->WeaponSkin[i]);
                                    bFoundSkin = true;
                                    break;
                                }
                            }
                        }
                        
                        if (!bFoundSkin)
                        {
                            for (int32 i = 0; i < C->WeaponSkins.Num(); i++)
                            {
                                if (C->WeaponSkins[i]->WeaponType == WeaponPathName)
                                {
                                    C->WeaponSkins.RemoveAt(i);
                                    break;
                                }
                            }
                        }
                    }
                }*/
                
                if (Cast<APlayerController>(Target->GetController()) &&
                    (!Cast<ANZWeapon>(Inventory) || !C->GetPendingWeapon() || (C->GetPendingWeapon()->GetClass() != Inventory->GetClass())))
                {
                    Cast<APlayerController>(Target->GetController())->ClientReceiveLocalizedMessage(UNZPickupMessage::StaticClass(), 0, NULL, NULL, Inventory->GetClass());
                }
                Inventory = NULL;
            }
            else
            {
                if (Cast<APlayerController>(Target->GetController()))
                {
                    Cast<APlayerController>(Target->GetController())->ClientReceiveLocalizedMessage(UNZPickupMessage::StaticClass(), 0, NULL, NULL, Inventory->GetClass());
                }
                Inventory->Destroy();
            }
        }
    }
}

void ANZDroppedPickup::PlayTakenEffects_Implementation(APawn* TakenBy)
{
    USoundBase* PickupSound = GetPickupSound();
    if (PickupSound != NULL)
    {
        UNZGameplayStatics::NZPlaySound(GetWorld(), PickupSound, TakenBy, SRT_All, false, GetActorLocation(), NULL, NULL, false);
    }
    if (GetNetMode() != NM_DedicatedServer)
    {
        UParticleSystem* ParticleTemplate = NULL;
        if (Inventory != NULL)
        {
            ParticleTemplate = Inventory->PickupEffect;
        }
        else if (InventoryType != NULL)
        {
            ParticleTemplate = InventoryType.GetDefaultObject()->PickupEffect;
        }
        if (ParticleTemplate != NULL)
        {
            ANZWorldSettings* WS = Cast<ANZWorldSettings>(GetWorld()->GetWorldSettings());
            if (WS == NULL || WS->EffectIsRelevant(this, GetActorLocation(), true, false, 10000.0f, 1000.0f))
            {
                UGameplayStatics::SpawnEmitterAtLocation(this, ParticleTemplate, GetActorLocation(), GetActorRotation());
            }
        }
    }    
}

void ANZDroppedPickup::EnableInstigatorTouch()
{
    if (Instigator != NULL)
    {
        CheckTouching();
    }
}

void ANZDroppedPickup::CheckTouching()
{
    TArray<AActor*> Overlaps;
    GetOverlappingActors(Overlaps, APawn::StaticClass());
    for (AActor* TestActor : Overlaps)
    {
        APawn* P = Cast<APawn>(TestActor);
        if (P != NULL && P->GetMovementComponent() != NULL)
        {
            FHitResult UnusedHitResult;
            OnOverlapBegin(P, Cast<UPrimitiveComponent>(P->GetMovementComponent()->UpdatedComponent), 0, false, UnusedHitResult);
        }
    }
}

void ANZDroppedPickup::Tick(float DeltaTime)
{
    if (!bFullyInitialized)
    {
        bFullyInitialized = true;
        CheckTouching();
    }
    Super::Tick(DeltaTime);
}

float ANZDroppedPickup::BotDesireability_Implementation(APawn* Asker, float PathDistance)
{
    return 0.f;
}

float ANZDroppedPickup::DetourWeight_Implementation(APawn* Asker, float PathDistance)
{
    return 0.f;
}
