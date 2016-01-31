// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDroppedPickup.h"
#include "NZInventory.h"
#include "NZGameMode.h"


// Sets default values
ANZDroppedPickup::ANZDroppedPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZDroppedPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZDroppedPickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ANZDroppedPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
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
    
}

void ANZDroppedPickup::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
}

void ANZDroppedPickup::PhysicsStopped(const FHitResult& ImpactResult)
{
    // It we landed on a mover, attach to it
    if (ImpactResult.Component != NULL && ImpactResult.Component->Mobility == EComponentMobility::Movable)
    {
        Collision->AttachTo(ImpactResult.Component.Get(), NAME_None, EAttachLocation::KeepWorldPosition);
    }
    // todo:
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
    
}

void ANZDroppedPickup::GiveTo_Implementation(APawn* Target)
{
    
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
            
        }
    }
}

