// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameCharacter.h"


// Sets default values
ANZGameCharacter::ANZGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UClass> DefaultCharacterContentRef(TEXT(""));
    CharacterData = DefaultCharacterContentRef.Object;
    
    // 设置胶囊体大小
    GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);
    
    // 创建摄像机组件
    CharacterCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    CharacterCameraComponent->AttachParent = GetCapsuleComponent();
    DefaultBaseEyeHeight = 67.5f;
    BaseEyeHeight = DefaultBaseEyeHeight;
    DefaultCrouchedEyeHeight = 30.f;
    CrouchedEyeHeight = DefaultCrouchedEyeHeight;
    CharacterCameraComponent->RelativeLocation = FVector(0, 0, DefaultBaseEyeHeight);
    
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->AttachParent = CharacterCameraComponent;
    FirstPersonMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;
    FirstPersonMesh->bReceivesDecals = false;
    FirstPersonMesh->PrimaryComponentTick.AddPrerequisite(this, PrimaryActorTick);
    
}

// Called when the game starts or when spawned
void ANZGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZGameCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ANZGameCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

