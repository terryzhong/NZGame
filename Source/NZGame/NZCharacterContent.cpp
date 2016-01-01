// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCharacterContent.h"


// Sets default values
ANZCharacterContent::ANZCharacterContent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZCharacterContent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZCharacterContent::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

