// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZGameplayStatics.h"
#include "NZImpactEffect.generated.h"


USTRUCT(BlueprintType)
struct FImpactEffectNamedParameters
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FParticleSysParam> ParticleParams;
    
    FImpactEffectNamedParameters()
    {
    }
    
    FImpactEffectNamedParameters(float DamageRadius);
};

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class NZGAME_API ANZImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZImpactEffect();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    float CullDistance;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    float AlwaysSpawnDistance;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bCheckInView;
    
    
    /** One shot audio played with the effect */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    USoundBase* Audio;
    
    
    /** 86
     * Spawns the effect's components as appropriate for the viewing distance, system settings, etc
     * Some aspects may be replicated (particularly audio), so call even on dedicated server and let the effect sort it out
     */
    UFUNCTION(BlueprintNativeEvent, Category = Effects)
    bool SpawnEffect(UWorld* World, const FTransform& InTransform, UPrimitiveComponent* HitComp = NULL, AActor* SpawnedBy = NULL, AController* InstigatedBy = NULL, ESoundReplicationType SoundReplication = SRT_IfSourceNotReplicated, const FImpactEffectNamedParameters& EffectParams = FImpactEffectNamedParameters()) const;
    
    
};
