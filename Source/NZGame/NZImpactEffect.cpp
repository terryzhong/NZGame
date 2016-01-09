// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZImpactEffect.h"


// Sets default values
ANZImpactEffect::ANZImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZImpactEffect::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

