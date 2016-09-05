// Fill out your copyright notice in the Description page of Project Settings.

#include "NZPVPGame.h"
#include "TestPVPGame.h"


// Sets default values
ATestPVPGame::ATestPVPGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestPVPGame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestPVPGame::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

