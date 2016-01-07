// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponAttachment.h"


// Sets default values
ANZWeaponAttachment::ANZWeaponAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZWeaponAttachment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZWeaponAttachment::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

