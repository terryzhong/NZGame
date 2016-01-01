// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "NZGameCharacter.generated.h"

UCLASS()
class NZGAME_API ANZGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANZGameCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
    /** 第一人称手臂模型 */
    UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
    class USkeletalMeshComponent* FirstPersonMesh;
    
    /** 第一人称摄像机 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    class UCameraComponent* CharacterCameraComponent;
    
    UPROPERTY(BlueprintReadOnly)
    TSubclassOf<class ANZCharacterContent> CharacterData;
    
    UPROPERTY()
    float DefaultBaseEyeHeight;
        
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
    float DefaultCrouchedEyeHeight;
        
    UPROPERTY(BlueprintReadOnly, Replicated, Category=Pawn)
    bool bSpawnProtectionEligible;
    
};
