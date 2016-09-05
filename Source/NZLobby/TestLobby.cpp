// Fill out your copyright notice in the Description page of Project Settings.

#include "NZLobby.h"
#include "TestLobby.h"


// Sets default values
ATestLobby::ATestLobby()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestLobby::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestLobby::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

