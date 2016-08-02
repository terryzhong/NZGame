// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZProjectile.h"
#include "NZPlayerController.h"


// Sets default values
ANZProjectile::ANZProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ANZProjectile::InitFakeProjectile(class ANZPlayerController* OwningPlayer)
{
    bFakeClientProjectile = true;
    if (OwningPlayer)
    {
        OwningPlayer->FakeProjectiles.Add(this);
    }
}

