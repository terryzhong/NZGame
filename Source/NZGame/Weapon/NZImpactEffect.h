// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZGameplayStatics.h"
#include "NZImpactEffect.generated.h"


USTRUCT(BlueprintType)
struct FImpactEffectNamedParameters
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FParticleSysParam> ParticleParams;
    
    FImpactEffectNamedParameters()
    {
    }
    
    FImpactEffectNamedParameters(float DamageRadius);
};

/**
 * Encapsulates all of the components of an impact or explosion effect (particles, sound, decals, etc) contains functionality to LOD by distance and settings
 * This class is an Actor primarily for the editability features and should not be directly spawned
 */
UCLASS(Blueprintable, Abstract)
class NZGAME_API ANZImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZImpactEffect();

	/** If > 0, effect is not played if beyond this distance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    float CullDistance;
    
    /** If > 0, effect always played if within this distance, even if behind all viewers, wall in the way, etc */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    float AlwaysSpawnDistance;
    
    /** If set, check that spawn location is visible to at least one local player before spawning */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bCheckInView;
    
    /** If set, always spawn when caused by local human player */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bForceForLocalPlayer;
    
    /** If set, attach to hit component (if any) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bAttachToHitComponent;
    
    /** If set, best LOD for local player */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bNoLODForLocalPlayer;
    
    /** If set, apply random roll to any decals */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bRandomizeDecalRoll;
    
    /** Scales how long the decal for this effect lasts */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    float DecalLifeScaling;
    
    /** One shot audio played with the effect */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    USoundBase* Audio;
    
    /** If given a value, materials in created components will have this parameter set to the world time they were created */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    FName WorldTimeParam;
    
    /** Force no LOD for effects for local player, called if bNoLODForLocalPlayer is true */
    virtual void SetNoLocalPlayerLOD(UWorld* World, USceneComponent* NewComp, AController* InstigatedBy) const;
    
    void CreateEffectComponents(UWorld* World, const FTransform& BaseTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, const FImpactEffectNamedParameters& EffectParams, USceneComponent* CurrentAttachment, FName TemplateName, const TArray<USceneComponent*>& NativeCompList, const TArray<USCS_Node*>& BPNodes) const;
    
    /** 
     * After deciding to spawn the effect as a whole, this event can be used to filter out individual components
     * Note that the component passed in is a template and shouldn't be modified
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = Effects)
    bool ShouldCreateComponent(const USceneComponent* TestComp, FName CompTemplateName, const FTransform& BaseTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy) const;
    
    /** Called on each created component for further modification */
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = Effects)
    void ComponentCreated(USceneComponent* NewComp, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, FImpactEffectNamedParameters EffectParams) const;
    
    /**
     * Spawns the effect's components as appropriate for the viewing distance, system settings, etc
     * Some aspects may be replicated (particularly audio), so call even on dedicated server and let the effect sort it out
     */
    UFUNCTION(BlueprintNativeEvent, Category = Effects)
    bool SpawnEffect(UWorld* World, const FTransform& InTransform, UPrimitiveComponent* HitComp = NULL, AActor* SpawnedBy = NULL, AController* InstigatedBy = NULL, ESoundReplicationType SoundReplication = SRT_IfSourceNotReplicated, const FImpactEffectNamedParameters& EffectParams = FImpactEffectNamedParameters()) const;
    
    UFUNCTION(BlueprintCallable, Meta = (DefaultToSelf = "WorldContextObject", DisplayName = "SpawnEffect", AutoCreateRefTerm = "EffectParams"), Category = Effects)
    static void CallSpawnEffect(UObject* WorldContextObject, TSubclassOf<ANZImpactEffect> Effect, const FTransform& InTransform, UPrimitiveComponent* HitComp = NULL, AActor* SpawnedBy = NULL, AController* InstigatedBy = NULL, ESoundReplicationType SoundReplication = SRT_IfSourceNotReplicated, const
        FImpactEffectNamedParameters& EffectParams
#if CPP
        = FImpactEffectNamedParameters()
#endif
    );
    
    virtual void PostInitializeComponents() override;
};
