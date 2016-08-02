// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCharacterContent.h"


// Sets default values
ANZCharacterContent::ANZCharacterContent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DummyRoot")));  // needed so Mesh has RelativeLocation/RelativeRotation in the editor
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("Mesh")));
    Mesh->SetupAttachment(RootComponent);
    Mesh->AlwaysLoadOnClient = true;
    Mesh->AlwaysLoadOnServer = true;
    Mesh->bCastDynamicShadow = true;
    Mesh->bAffectDynamicIndirectLighting = true;
    Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
    Mesh->SetCollisionProfileName(FName(TEXT("CharacterMesh")));
    Mesh->bGenerateOverlapEvents = false;
    Mesh->SetCanEverAffectNavigation(false);
    Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->bEnablePhysicsOnDedicatedServer = true;   // needed for feign death; death ragdoll shouldn't be invoked on server
    Mesh->bReceivesDecals = false;
    
    DisplayName = NSLOCTEXT("NZ", "UntitledCharacter", "Untitled Character");
}
