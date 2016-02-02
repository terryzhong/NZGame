// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCarriedObject.h"
#include "NZTeamInfo.h"


// Sets default values
ANZCarriedObject::ANZCarriedObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZCarriedObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZCarriedObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ANZCarriedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

uint8 ANZCarriedObject::GetTeamNum() const
{
    return (Team != NULL) ? Team->GetTeamNum() : 255;
}


