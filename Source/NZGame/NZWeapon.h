// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZInventory.h"
#include "NZWeapon.generated.h"

USTRUCT(BlueprintType)
struct FInstantHitDamageInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageInfo)
    int32 Damage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageInfo)
    TSubclassOf<UDamageType> DamageType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageInfo)
    float Momentum;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageInfo)
    float TraceRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageInfo)
    float TraceHalfSize;
    
    FInstantHitDamageInfo()
        : Damage(10)
        , Momentum(0.0f)
        , TraceRange(25000.0f)
        , TraceHalfSize(0.0f)
    {}
};

USTRUCT()
struct FDelayedProjectileInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY()
    TSubclassOf<class ANZProjectile> ProjectileClass;
    
    UPROPERTY()
    FVector SpawnLocation;
    
    UPROPERTY()
    FRotator SpawnRotation;
    
    FDelayedProjectileInfo()
        : ProjectileClass(NULL)
        , SpawnLocation(ForceInit)
        , SpawnRotation(ForceInit)
    {}
};

USTRUCT()
struct FDelayedHitScanInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY()
    FVector ImpactLocation;
    
    UPROPERTY()
    uint8 FireMode;
    
    UPROPERTY()
    FVector SpawnLocation;
    
    UPROPERTY()
    FRotator SpawnRotation;
    
    FDelayedHitScanInfo()
        : ImpactLocation(ForceInit)
        , FireMode(0)
        , SpawnLocation(ForceInit)
        , SpawnRotation(ForceInit)
    {}
};

/**
 * 
 */
UCLASS(Config = Game)
class NZGAME_API ANZWeapon : public ANZInventory
{
	GENERATED_BODY()
    
    friend class UNZWeaponState;
    friend class UNZWeaponStateInactive;
    friend class UNZWeaponStateActive;
    friend class UNZWeaponStateEquipping;
    friend class UNZWeaponStateUnequipping;
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, ReplicatedUsing = OnRep_AttachmentType, Category = Weapon)
    TSubclassOf<class ANZWeaponAttachment> AttachmentType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, ReplicatedUsing = OnRep_Ammo, Category = Weapon)
    int32 Ammo;
    
    UFUNCTION()
    virtual void OnRep_AttachmentType();
    
    UFUNCTION()
    virtual void OnRep_Ammo();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Weapon)
    int32 MaxAmmo;
    
    /** Ammo cost for one shot of each fire mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<int32> AmmoCost;
    
    /** Projectile class for fire mode (if applicable) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<TSubclassOf<class ANZProjectile> > ProjClass;
    
    /** Instant hit data for fire mode (if applicable) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<FInstantHitDamageInfo> InstantHitInfo;
    
    /** Firing state for mode, contains core firing sequence and directs to appropriate global firing functions */
    UPROPERTY(Instanced, EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Weapon, NoClear)
    TArray<class UNZWeaponStateFiring*> FiringState;
    
    /** True for melee weapons affected by "stopping power" (momentum added for weapons that don't normally impart much momentum */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    bool bAffectedByStoppingPower;
    
    /** Custom momentum scaling for friendly hitscanned pawns */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    float FriendlyMomentumScaling;
    
    virtual float GetImpartedMomentumMag(AActor* HitActor);
    
    virtual void Serialize(FArchive& Ar) override
    {
        float SavedSwitchPriority = AutoSwitchPriority;
        Super::Serialize(Ar);
        AutoSwitchPriority = SavedSwitchPriority;
    }
    
    /** Synchronize random seed on server and firing client so projectiles stay synchronized */
    virtual void NetSynchRandomSeed();
    
    /** Socket to attach weapon to hands; If None, then the hands are hidden */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FName HandsAttachSocket;
    
    /** Time between shots, trigger checks, etc */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (ClampMin = "0.1"))
    TArray<float> FireInterval;
    
    /** Firing spread (random angle added to shots) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<float> Spread;
    
    /** Sound to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<USoundBase*> FireSound;
    
    /** Sound to play on shooter when weapon is fired. This sound starts at the same time as the FireSound */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<USoundBase*> ReloadSound;
    
    /** Looping (ambient) sound to set on owner while firing */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<USoundBase*> FireLoopingSound;
    
    /** AnimMontage to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<UAnimMontage*> FireAnimation;
    
    /** AnimMontage to play on hands each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<UAnimMontage*> FireAnimationHands;
    
    /** Particle component for muzzle flash */
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<UParticleSystemComponent*> MuzzleFlash;
    
    /** Saved transform of MuzzleFlash components used for UpdateWeaponHand() to know original values from the blueprint */
    UPROPERTY(Transient)
    TArray<FTransform> MuzzleFlashDefaultTransforms;
    
    /** Saved sockets of MuzzleFlash components used for UpdateWeaponHand() to know original values from the blueprint */
    UPROPERTY(Transient)
    TArray<FName> MuzzleFlashSocketNames;
    
    /**
     * Particle system for firing effects (instant hit beam and such)
     * Particle will be sourced at FireOffset and a parameter HitLocation will be set for the target, if applicable
     */
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<UParticleSystem*> FireEffect;
    
    /** Max distance to stretch weapon tracer */
    UPROPERTY(EditAnywhere, Category = Weapon)
    float MaxTracerDist;
    
    /** Fire effect happens once every FireEffectInterval shots */
    UPROPERTY(EditAnywhere, Category = Weapon)
    int32 FireEffectInterval;
    
    /** Shots since last fire effect */
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    int32 FireEffectCount;
    
    UPROPERTY(EditAnywhere, Category = Weapon)
    TArray<TSubclassOf<class ANZImpactEffect> > ImpactEffect;
    
    UPROPERTY(EditAnywhere, Category = Weapon)
    float ImpactEffectSkipDistance;
    
    UPROPERTY(EditAnywhere, Category = Weapon)
    float MaxImpactEffectSkipTime;
    
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    FVector LastImpactEffectLocation;
    
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    float LastImpactEffectTime;
    
    UPROPERTY(BlueprintReadWrite, Category = Weapon)
    TArray<UMaterialInterface*> SavedMeshMaterials;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    bool bMustBeHolstered;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Weapon)
    bool bCanThrowWeapon;
    
    UPROPERTY(EditDefaultsOnly, Category = UI)
    bool bHideInMenus;
    
    UPROPERTY(EditDefaultsOnly, Category = UI)
    bool bHideInCrosshairMenu;
    
    UPROPERTY(EditDefaultsOnly, Category = Weapon)
    FVector FOVOffset;
    
    //UPROPERTY()
    //UNZWeaponSkin* WeaponSkin;
    
    UFUNCTION()
    virtual void AttachToHolster();
    
    UFUNCTION()
    virtual void DetachFromHolster();
    
    virtual void DropFrom(const FVector& StartLocation, const FVector& TossVelocity) override;
    
    virtual bool InitializeDroppedPickup(class ANZDroppedPickup* Pickup);
    
    /** Return true if this weapon should be dropped if held on player death */
    virtual bool ShouldDropOnDeath();
    
    /** First person mesh */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
    USkeletalMeshComponent* Mesh;
    
    USkeletalMeshComponent* GetMesh() const { return Mesh; }
    
    /**
     * Causes weapons fire to originate from the center of the player's view when in first person mode (and human controlled)
     * In other cases the fire start point defaults to the weapon's world position
     */
    UPROPERTY(EditAnywhere, Category = Weapon)
    bool bFPFireFromCenter;
    
    /** If set ignore FireOffset for instant hit fire modes when in first person mode */
    UPROPERTY(EditAnywhere, Category = Weapon)
    bool bFPIgnoreInstantHitFireOffset;
    
    /**
     * Firing offset from weapon for weapons fire.
     * If bFPFireFromCenter is true and it's a player in first person mode, this is from the camera center
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    FVector FireOffset;
    
    /** If true (on server), use the last bSpawnedShot saved position as starting point for this shot to synch with client firing position */
    UPROPERTY()
    bool bNetDelayedShot;
    
    /** Indicates this weapon is most useful in melee range (used by AI) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    bool bMeleeWeapon;
    
    /** 
     * Indicates AI should prioritize accuracy over evasion 
     * (low skill bots will stop moving, higher skill bots prioritize strafing and avoid actions that move enemy across view)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    bool bPrioritizeAccuracy;
    
    /** Indicates AI should target for splash damage (e.g. shoot at feet or nearby walls) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    bool bRecommendSplashDamage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    bool bRecommendSuppressiveFire;
    
    /** Indicates this is a sniping weapon (for AI, will prioritize headshots and long range targeting) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    bool bSniping;
    
    /** Delayed projectile information */
    UPROPERTY()
    FDelayedProjectileInfo DelayedProjectile;
    
    /** Delayed hitscan information */
    UPROPERTY()
    FDelayedHitScanInfo DelayedHitScan;
    
    /**
     *
     */
    UFUNCTION(BlueprintCallable, Category = Effects)
    bool ShouldPlay1PVisuals() const;
    
    /**
     * Play impact effects client-side for predicted hitscan shot - decides whether to delay because of high client ping
     */
    virtual void PlayPredictedImpactEffects(FVector ImpactLoc);
    
    FTimerHandle PlayDelayedImpactEffectsHandle;
    
    virtual void PlayDelayedImpactEffects();
    
    
    
    
    
    
    
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool HasAnyAmmo();
    
    
    

    virtual void BringUp(float OverflowTime = 0.0f);
    
    virtual bool PutDown();
    

    
    
    /**
     * Weapon group - NextWeapon() picks the next highest group, PrevWeapon() the next lowest, etc
     * Generally, the corresponding number key is bound to access the weapons in that group
     */
    UPROPERTY(Config, Transient, BlueprintReadOnly, Category = Selection)
    int32 Group;
    
    /** Group this weapon was assigned to in past UTs when each weapon was in its own slot */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Selection)
    int32 DefaultGroup;
    
    /**
     * If the player acquires more than one weapon in a group, we assign a unique GroupSlot to keep a consistent order
     * This value is only set on clients
     */
    UPROPERTY(Transient, BlueprintReadOnly, Category = Selection)
    int32 GroupSlot;
    
    /** Return true if weapon follows OtherWeapon in the weapon list (used for nextweapon/previousweapon) */
    virtual bool FollowsInList(ANZWeapon* OtherWeapon);
    
    /**
     * User set priority for auto switching and switch to best weapon functionality
     * This value only has meaning on clients
     */
    UPROPERTY(EditAnywhere, Config, Transient, BlueprintReadOnly, Category = Selection)
    float AutoSwitchPriority;
    
    
};
