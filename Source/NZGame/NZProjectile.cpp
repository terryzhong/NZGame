// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZProjectile.h"


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

