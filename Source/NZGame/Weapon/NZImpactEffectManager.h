// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZImpactEffectManager.generated.h"

USTRUCT(BlueprintType)
struct FImpactEffectBind
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere)
    UPhysicalMaterial* PhysicalMaterial;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<class ANZImpactEffect> ImpactEffect;
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class NZGAME_API UNZImpactEffectManager : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "ImpactEffect")
    TArray<FImpactEffectBind> ImpactEffectBindList;
    
	UFUNCTION(BlueprintCallable, Category = "ImpactEffect")
    TSubclassOf<ANZImpactEffect> GetImpactEffectFromPhysicalMaterial(UPhysicalMaterial* InPhysicalMaterial);
};
