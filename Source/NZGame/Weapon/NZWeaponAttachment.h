// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZWeaponAttachment.generated.h"

/**
 * The third person representation of a weapon
 * Not spawned on dedicated servers
 */
UCLASS(Blueprintable, NotPlaceable, Abstract)
class NZGAME_API ANZWeaponAttachment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZWeaponAttachment();

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
    
    /** Third person mesh attach point */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FName HolsterSocket;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FVector HolsterOffset;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FRotator HolsterRotation;
    
    /** If true, don't spawn impact effect. Used for hitscan hits, skips by default for pawn and projectile hits */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool CancelImpactEffect(const FHitResult& ImpactHit);
    
    /** 
     * Particle system for firing effects (instant hit beam and such)
     * Particle will be sourced at FireOffset and a parameter HitLocation will be set for the target, if applicable
     */
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<UParticleSystem*> FireEffect;
    
    /** Manager effect for instant hit endpoint */
    UPROPERTY(EditAnywhere, Category = "Weapon")
    TSubclassOf<class UNZImpactEffectManager> ImpactEffectManagerClass;
    
    UPROPERTY(EditAnywhere, Category = "Weapon")
    UNZImpactEffectManager* ImpactEffectManager;
    
    /** Throttling for impact effects - don't spawn another unless last effect is farther than this away or longer age than MaxImpactEffectSkipTime */
    UPROPERTY(EditAnywhere, Category = Weapon)
    float ImpactEffectSkipDistance;
    
	/** Throttling for impact effects - don't spawn another unless last effect is farther than ImpactEffectSkipDistance away or longer ago than this */
    UPROPERTY(EditAnywhere, Category = Weapon)
    float MaxImpactEffectSkipTime;
    
	/** FlashLocation for last played impact effect */
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    FVector LastImpactEffectLocation;
    
	/** Last time an impact effect was played */
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    float LastImpactEffectTime;
    
	/** If set, get impact effect from weapon class (most weapons use game instant hit impact for 1p and 3p) */
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    bool bCopyWeaponImpactEffect;
    
    /** Optional bullet whip sound when instant hit shots pass close by a local player without hitting */
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    USoundBase* BulletWhip;
    
	/** Maximum distance from fire line player can be and still get the bullet whip sound */
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    float MaxBulletWhipDist;
    
    virtual void BeginPlay() override;
    virtual void RegisterAllComponents() override;
    virtual void Destroyed() override;
    
    UFUNCTION(BlueprintNativeEvent)
    void AttachToOwner();
    
    UFUNCTION(BlueprintNativeEvent)
    void DetachFromOwner();
    
    UFUNCTION(BlueprintNativeEvent)
    void HolsterToOwner();
    
    /** Blueprint hook to modify spawned instance of FireEffect (e.g. tracer or beam) */
    UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
    void ModifyFireEffect(UParticleSystemComponent* Effect);
    
    /**
     * Play firing effects (both muzzle flash and any tracers/beams/impact effects)
     * Use NZOwner's FlashLocation and FireMode to determine firing data
     * Don't play sounds as those are played/replicated from NZWeapon by the server as the Pawn/WeaponAttachment may not be relevant
     */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void PlayFiringEffects();
    
    /**
     * Called when FlashExtra on the owner is changed
     * Some weapons may use this to display intermediate firing states (e.g. charging)
     * or to provide extra data to a normal firing sequence (e.g. rocket type used for rocket launcher fire)
     */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void FiringExtraUpdated();

    /** Stop any looping fire effects */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void StopFiringEffects(bool bIgnoreCurrentMode = false);
    
    virtual void AttachToOwnerNative();
    
    virtual void HolsterToOwnerNative();
    
    /**
     * Default prarmeters set on overlay particle effect (if any)
     * up to the effect to care about them
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
    TArray<struct FParticleSysParam> OverlayEffectParams;
    
    /** Read WeaponOverlayFlags from owner and apply the appropriate overlay material (if any) */
    virtual void UpdateOverlays();
    
    /** Set main skin override for the weapon, NULL to restore to default */
    virtual void SetSkin(UMaterialInterface* NewSkin);
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void PlayBulletWhip();
    
    virtual void MarkComponentsAsPendingKill() override;

    /** Blueprint hook to modify team color materials */
    UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
    void NotifyTeamChanged();
};
