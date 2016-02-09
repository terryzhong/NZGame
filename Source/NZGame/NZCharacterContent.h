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
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, AssetRegistrySearchable)
    FText DisplayName;
    
    /** Character gender */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    bool bIsFemale;
    
    /**
     * If set this achievement is required for this character to be available
     * (NOTE: Achievements are currently client side only and not validated by server)
     */
    UPROPERTY(EditDefaultsOnly, AssetRegistrySearchable, Meta = (DisplayName = "Required Offline Achievement"))
    FName RequiredAchievement;
    
    /** Mesh relative */
    inline USkeletalMeshComponent* GetMesh() const { return Mesh; }
    
    /** Mesh to swap in when the character is skeletized */
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
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    TArray<UMaterialInterface*> TeamMaterials;
    
    friend class ANZCharacter;
};
