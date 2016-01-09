// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponAttachment.h"
#include "NZCharacter.h"
#include "NZProjectile.h"


// Sets default values
ANZWeaponAttachment::ANZWeaponAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ANZWeaponAttachment::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ANZWeaponAttachment::CancelImpactEffect(const FHitResult& ImpactHit)
{
    return ((!ImpactHit.Actor.IsValid() && !ImpactHit.Component.IsValid()) ||
            Cast<ANZCharacter>(ImpactHit.Actor.Get()) ||
            Cast<ANZProjectile>(ImpactHit.Actor.Get()));
}

void ANZWeaponAttachment::AttachToOwner_Implementation()
{
    AttachToOwnerNative();
}

void ANZWeaponAttachment::AttachToOwnerNative()
{
    
}

void ANZWeaponAttachment::DetachFromOwner_Implementation()
{
    
}