// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZCharacterContent.generated.h"

UCLASS()
class NZGAME_API ANZCharacterContent : public AActor
{
	GENERATED_BODY()
    
public:	
	// Sets default values for this actor's properties
	ANZCharacterContent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, AssetRegistrySearchable)
    FText DisplayName;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    bool bIsFemale;
    
    /** Mesh relative */
    inline USkeletalMeshComponent* GetMesh() const { return Mesh; }
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    USkeletalMesh* SkeletonMesh;
    
    /** Sound relative */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Sounds)
    USoundBase* LandingSound;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Sounds)
    USoundBase* JumpSound;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Sounds)
    USoundBase* PainSound;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Sounds)
    USoundBase* WaterEntrySound;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Sounds)
    USoundBase* WaterExitSound;
    
protected:
    UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
    USkeletalMeshComponent* Mesh;
    
    friend class ANZCharacter;
};
