// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZInventory.h"


// Sets default values
ANZInventory::ANZInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

void ANZInventory::PostInitProperties()
{
	Super::PostInitProperties();

}

void ANZInventory::PreInitializeComponents()
{
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
}

void ANZInventory::ClientGivenTo_Internal(bool bAutoActivate)
{
}

void ANZInventory::Removed()
{
}

void ANZInventory::ClientRemoved_Implementation()
{
}

void ANZInventory::CheckPendingClientGivenTo()
{
}

void ANZInventory::OnRep_Instigator()
{
}

void ANZInventory::DropFrom(const FVector& StartLocation, const FVector& TossVelocity)
{
}

void ANZInventory::Destroyed()
{
}

void ANZInventory::InitializeDroppedPickup(class ANZDroppedPickup* Pickup)
{
}

UMeshComponent* ANZInventory::GetPickupMeshTemplate_Implementation(FVector& OverrideScale) const
{
    // todo:
    return NULL;
}

void ANZInventory::AddOverlayMaterials_Implementation(ANZGameState* GameState) const
{

}

bool ANZInventory::StackPickup_Implementation(ANZInventory* ContainedInv)
{
    return false;
}

