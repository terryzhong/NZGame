// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZRemoteRedeemer.h"


// Sets default values
ANZRemoteRedeemer::ANZRemoteRedeemer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZRemoteRedeemer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZRemoteRedeemer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ANZRemoteRedeemer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

