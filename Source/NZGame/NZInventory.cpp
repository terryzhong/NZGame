// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZInventory.h"
#include "NZGameState.h"
#include "NZDroppedPickup.h"
#include "NZProjectileMovementComponent.h"
#include "UnrealNetwork.h"


// Sets default values
ANZInventory::ANZInventory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SetReplicates(true);
    bOnlyRelevantToOwner = true;
    
    RespawnTime = 30.0f;
    
    RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent, USceneComponent>(this, TEXT("DummyRoot"), false);
    PickupMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PickupMesh0"), false);
    if (PickupMesh != NULL)
    {
        PickupMesh->AttachParent = RootComponent;
        PickupMesh->bAutoRegister = false;
    }
    
    DroppedPickupClass = ANZDroppedPickup::StaticClass();
    BasePickupDesireability = 0.5f;
    DisplayName = NSLOCTEXT("PickupMessage", "InventoryPickedUp", "Item");
    InitialFlashTime = 0.3f;
    InitialFlashScale = 5.f;
    InitialFlashColor = FLinearColor::White;
    bShowPowerupTimer = true;
    
    MenuDescription = NSLOCTEXT("NZWeapon", "DefaultDescription", "This space let intentionally blank");
    IconColor = FLinearColor::White;
}

// Called when the game starts or when spawned
void ANZInventory::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZInventory::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ANZInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ANZInventory, NextInventory, COND_None);
}

void ANZInventory::PostInitProperties()
{
	Super::PostInitProperties();

    // Attempt to set defaults for event early outs based on whether the class has implemented them
    // Note that this only works for blueprints, C++ classes need to munually set
    if (Cast<UBlueprintGeneratedClass>(GetClass()) != NULL)
    {
        if (!bCallDamageEvents)
        {
            static FName NAME_ModifyDamageTaken(TEXT("ModifyDamageTaken"));
            static FName NAME_PreventHeadShot(TEXT("PreventHeadShot"));
            UFunction* Func = FindFunction(NAME_ModifyDamageTaken);
            bCallDamageEvents = (Func != NULL && Func->Script.Num() > 0);
            if (!bCallDamageEvents)
            {
                Func = FindFunction(NAME_PreventHeadShot);
                bCallDamageEvents = (Func != NULL && Func->Script.Num() > 0);
            }
        }
        if (!bCallOwnerEvent)
        {
            static FName NAME_OwnerEvent(TEXT("OwnerEvent"));
            UFunction* Func = FindFunction(NAME_OwnerEvent);
            bCallOwnerEvent = (Func != NULL && Func->Script.Num() > 0);
        }
    }
}

void ANZInventory::PreInitializeComponents()
{
    // Get rid of components that are only supposed to be part of the pickup mesh
    // TODO: Would be better to not create in the first place, no reasonable engine hook to filter
    TArray<UActorComponent*> SerializedComponents = BlueprintCreatedComponents;
    SerializedComponents += GetInstanceComponents();
    
    for (int32 i = 0; i < SerializedComponents.Num(); i++)
    {
        USceneComponent* SceneComp = Cast<USceneComponent>(SerializedComponents[i]);
        if (SceneComp != NULL && SceneComp->AttachParent != NULL && SceneComp->AttachParent == PickupMesh && !SceneComp->AttachParent->IsRegistered())
        {
            TArray<USceneComponent*> Children;
            SceneComp->GetChildrenComponents(true, Children);
            for (USceneComponent* Child : Children)
            {
                Child->DestroyComponent();
            }
            SceneComp->DestroyComponent();
        }
    }
    
	Super::PreInitializeComponents();
}

void ANZInventory::GivenTo(ANZCharacter* NewOwner, bool bAutoActivate)
{
	Instigator = NewOwner;
	SetOwner(NewOwner);
	NZOwner = NewOwner;
	PrimaryActorTick.AddPrerequisite(NZOwner, NZOwner->PrimaryActorTick);
	eventGivenTo(NewOwner, bAutoActivate);
	ClientGivenTo(Instigator, bAutoActivate);
}

void ANZInventory::ClientGivenTo_Implementation(APawn* NewInstigator, bool bAutoActivate)
{
    if (NewInstigator != NULL)
    {
        Instigator = NewInstigator;
    }
    FlashTimer = InitialFlashTime;
    
    if (Instigator == NULL || !Cast<ANZCharacter>(Instigator)->IsInInventory(this))
    {
        bPendingClientGivenTo = true;
        bPendingAutoActivate = bAutoActivate;
        GetWorld()->GetTimerManager().SetTimer(CheckPendingClientGivenToHandle, this, &ANZInventory::CheckPendingClientGivenTo, 0.1f, false);
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(CheckPendingClientGivenToHandle);
        bPendingClientGivenTo = false;
        ClientGivenTo_Internal(bAutoActivate);
        eventClientGivenTo(bAutoActivate);
    }
}

void ANZInventory::ClientGivenTo_Internal(bool bAutoActivate)
{
    checkSlow(Instigator != NULL);
    SetOwner(Instigator);
    NZOwner = Cast<ANZCharacter>(Instigator);
    checkSlow(NZOwner != NULL);
    PrimaryActorTick.AddPrerequisite(NZOwner, NZOwner->PrimaryActorTick);
}

void ANZInventory::Removed()
{
    eventRemoved();
    
    if (NZOwner != NULL)
    {
        PrimaryActorTick.RemovePrerequisite(NZOwner, NZOwner->PrimaryActorTick);
    }
    
    ClientRemoved();    // Must be first, since it won't replicate after Owner is Lost
    
    Instigator = NULL;
    SetOwner(NULL);
    NZOwner = NULL;
    NextInventory = NULL;
}

void ANZInventory::ClientRemoved_Implementation()
{
    if (NZOwner != NULL)
    {
        PrimaryActorTick.RemovePrerequisite(NZOwner, NZOwner->PrimaryActorTick);
    }
    eventClientRemoved();
    SetOwner(NULL);
    NZOwner = NULL;
    Instigator = NULL;
    
    // This will come through replication
    if (Role == ROLE_Authority)
    {
        NextInventory = NULL;
    }
}

void ANZInventory::CheckPendingClientGivenTo()
{
    if (bPendingClientGivenTo && Instigator != NULL)
    {
        bPendingClientGivenTo = false;
        ClientGivenTo_Implementation(NULL, bPendingAutoActivate);
    }
}

void ANZInventory::OnRep_Instigator()
{
    Super::OnRep_Instigator();
    
    // This is for inventory replicated to non-owner
    if (!bPendingClientGivenTo && (GetNZOwner() == NULL || Instigator != NULL))
    {
        NZOwner = Cast<ANZCharacter>(Instigator);
    }
    CheckPendingClientGivenTo();
}

void ANZInventory::DropFrom(const FVector& StartLocation, const FVector& TossVelocity)
{
    if (Role == ROLE_Authority)
    {
        APawn* FormerInstigator = Instigator;
        
        if (NZOwner != NULL)
        {
            NZOwner->RemoveInventory(this);
        }
        Instigator = NULL;
        SetOwner(NULL);
        if (DroppedPickupClass != NULL)
        {
            // Pull back spawn location if it is embedded in world geometry
            FVector AdjustedStartLoc = StartLocation;
            UCapsuleComponent* TestCapsule = DroppedPickupClass.GetDefaultObject()->Collision;
            if (TestCapsule != NULL)
            {
                FCollisionQueryParams QueryParams(FName(TEXT("DropPlacement")), false);
                FHitResult Hit;
                if (GetWorld()->SweepSingleByChannel(Hit, StartLocation - FVector(TossVelocity.X, TossVelocity.Y, 0.0f) * 0.25f, StartLocation, FQuat::Identity, TestCapsule->GetCollisionObjectType(), TestCapsule->GetCollisionShape(), QueryParams, TestCapsule->GetCollisionResponseToChannels()) && !Hit.bStartPenetrating)
                {
                    AdjustedStartLoc = Hit.Location;                    
                }
            }
            
            FActorSpawnParameters Params;
            Params.Instigator = FormerInstigator;
            ANZDroppedPickup* Pickup = GetWorld()->SpawnActor<ANZDroppedPickup>(DroppedPickupClass, AdjustedStartLoc, TossVelocity.Rotation(), Params);
            if (Pickup != NULL)
            {
                Pickup->Movement->Velocity = TossVelocity;
                InitializeDroppedPickup(Pickup);
            }
            else
            {
                Destroy();
            }
        }
        else
        {
            Destroy();
        }
    }
}

void ANZInventory::Destroyed()
{
    if (NZOwner != NULL)
    {
        NZOwner->RemoveInventory(this);
    }
    GetWorldTimerManager().ClearAllTimersForObject(this);
    
    Super::Destroyed();
}

void ANZInventory::InitializeDroppedPickup(class ANZDroppedPickup* Pickup)
{
    if (Pickup != NULL)
    {
        Pickup->SetInventory(this);
    }
}

UMeshComponent* ANZInventory::GetPickupMeshTemplate_Implementation(FVector& OverrideScale) const
{
    return PickupMesh;
}

void ANZInventory::AddOverlayMaterials_Implementation(ANZGameState* GameState) const
{

}

bool ANZInventory::StackPickup_Implementation(ANZInventory* ContainedInv)
{
    return false;
}

bool ANZInventory::ModifyDamageTaken_Implementation(UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, UPARAM(ref) ANZInventory*& HitArmor, AController* InstigatedBy, const FHitResult& HitInfo, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
    return false;
}

bool ANZInventory::PlayArmorEffects_Implementation(ANZCharacter* HitPawn) const
{
    return false;
}

bool ANZInventory::HandleArmorEffects(ANZCharacter* HitPawn) const
{
    return PlayArmorEffects(HitPawn);
}

bool ANZInventory::PreventHeadShot_Implementation(FVector HitLocation, FVector ShotDirection, float WeaponHeadScaling, bool bConsumeArmor)
{
    return false;
}

bool ANZInventory::ShouldDisplayHitEffect_Implementation(int32 AttemptedDamage, int32 DamageAmount, int32 FinalHealth, int32 FinalArmor)
{
    return (FinalHealth > 0) && ((FinalHealth + FinalArmor > 90) || (AttemptedDamage > 90));
}

int32 ANZInventory::GetEffectiveHealthModifier_Implementation(bool bOnlyVisible) const
{
    return 0;
}

void ANZInventory::OwnerEvent_Implementation(FName EventName)
{
    
}

void ANZInventory::DrawInventoryHUD_Implementation(class UNZHUDWidget* Widget, FVector2D Pos, FVector2D Size)
{
    
}

float ANZInventory::BotDesireability_Implementation(APawn* Asker, AActor* Pickup, float PathDistance) const
{
    return BasePickupDesireability;
}

float ANZInventory::DetourWeight_Implementation(APawn* Asker, AActor* Pickup, float PathDistance) const
{
    return 0.f;
}

