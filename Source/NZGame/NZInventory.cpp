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

void ANZInventory::GivenTo(ANZCharacter* NewOwner, bool bAutoActivate)
{
}

void ANZInventory::Removed()
{
}

void ANZInventory::DropFrom(const FVector& StartLocation, const FVector& TossVelocity)
{
}

void ANZInventory::Destroyed()
{
}
