// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZWeaponAttachment.generated.h"

UCLASS()
class NZGAME_API ANZWeaponAttachment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZWeaponAttachment();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
    /** Weapon class that resulted in this attachment; set at spawn time so no need to be reflexive here, just set AttachmentType in NZWeapon */
    UPROPERTY(BlueprintReadOnly, Category = Weapon)
    TSubclassOf<class ANZWeapon> WeaponType;
    
    /** Precast of Instigator to NZCharacter */
    UPROPERTY(BlueprintReadOnly, Category = Weapon)
    class ANZCharacter* NZOwner;
    
    /** Overlay mesh for overlay effects */
    UPROPERTY()
    USkeletalMeshComponent* OverlayMesh;
    
public:
    /** Tells the animation system what movement blends to use */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    int32 WeaponStance;
    
    /** Particle component for muzzle flash */
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<UParticleSystemComponent*> MuzzleFlash;
    
    /** Third person mesh */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
    USkeletalMeshComponent* Mesh;
    
    /** Third person mesh attach point */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FName AttachSocket;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FVector AttachOffset;
    
    /** The attachment mesh is also used for the pickup; Set this override to non-zero to override the scale factor when used that way */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    FVector PickupScaleOverride;
    
    /** If true, don't spawn impact effect. Used for hitscan hits, skips by default for pawn and projectile hits */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool CancelImpactEffect(const FHitResult& ImpactHit);
    
    /** 
     * Particle system for firing effects (instant hit beam and such)
     * Particle will be sourced at FireOffset and a parameter HitLocation will be set for the target, if applicable
     */
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<UParticleSystem*> FireEffect;
    
    /** Optional effect for instant hit endpoint */
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<TSubclassOf<class ANZImpactEffect> > ImpactEffects;
    
    /**  */
    UPROPERTY(EditAnywhere, Category = Weapon)
    float ImpactEffectSkipDistance;
    
    UPROPERTY(EditAnywhere, Category = Weapon)
    float MaxImpactEffectSkipTime;
    
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    FVector LastImpactEffectLocation;
    
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    float LastImpactEffectTime;
    
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    bool bCopyWeaponImpactEffects;
    
    
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    USoundBase* BulletWhip;
    
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    float MaxBulletWhipDist;
    
    
    
    
    UFUNCTION(BlueprintNativeEvent)
    void AttachToOwner();
    virtual void AttachToOwnerNative();
    
    UFUNCTION(BlueprintNativeEvent)
    void DetachFromOwner();
    
    
    
	
};
