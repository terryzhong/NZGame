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
    
    virtual void InitializeDroppedPickup(class ANZDroppedPickup* Pickup);
    
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
    
    /** Trigger delayed hitscan effects, delayed because client ping above max forward prediction limit */
    virtual void PlayDelayedImpactEffects();
    
    FTimerHandle SpawnDelayedFakeProjHandle;
    
    /** Spawn a delayed projectile, delayed because client ping above max forward prediction limit */
    virtual void SpawnDelayedFakeProjectile();
    
    /** Time to bring up the weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    float BringUpTime;
    
    /** Time to put down the weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    float PutDownTime;
    
    /** Scales refire put down time for the weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    float RefirePutDownTimePercent;
    
    /** Earliest time can fire again (failsafe for weapon swapping) */
    UPROPERTY()
    float EarliestFireTime;
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual float GetBringUpTime();
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual float GetPutDownTime();
    
    /** Equip anims */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    UAnimMontage* BringUpAnim;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    UAnimMontage* BringUpAnimHands;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    UAnimMontage* PutDownAnim;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    UAnimMontage* PutDownAnimHands;
    
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

    /** Deactivate any owner spawn protection */
    virtual void DeactivateSpawnProtection();
    
    /**
     * Whether this weapon stays around by default when someone picks it up
     * (i.e. multiple people can pick up from the same spot without waiting for respawn time)
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    bool bWeaponStay;
    
    /** Base offset of first person mesh, cached from offset set up in blueprint */
    UPROPERTY()
    FVector FirstPMeshOffset;
    
    /** Base relative roation of first person mesh, cached from offset set up in blueprint */
    UPROPERTY()
    FRotator FirstPMeshRotation;
    
    /** Scaling for 1st person weapon bob */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float WeaponBobScaling;
    
    /** Scaling for 1st person firing view kickback */
    UPROPERTY(EditAnywhere, BLueprintReadWrite, Category = WeaponBob)
    float FiringViewKickback;
    
    virtual void UpdateViewBob(float DeltaTime);
    
    virtual void PostInitProperties() override;
    virtual void BeginPlay() override;
    virtual void RegisterAllComponents() override
    {
        // Don't register in game by default for perf, we'll manually call Super from AttachToOwner()
        if (GetWorld()->WorldType == EWorldType::Editor || GetWorld()->WorldType == EWorldType::Preview)
        {
            Super::RegisterAllComponents();
        }
        else
        {
            RootComponent = NULL;   // This was set for the editor view, but we don't want it
        }
    }
    
    virtual UMeshComponent* GetPickupMeshTemplate_Implementation(FVector& OverrideScale) const override;
    
    virtual void GotoState(class UNZWeaponState* NewState);
    
    /**
     * Notification of state change (CurrentState is new state)
     * If a state change triggers another state change (i.e. within BeginState()/EndState()) this function will only
     * be called once, when CurrentState is the final state
     */
    virtual void StateChanged() {}
    
    /** Firing entry point */
    virtual void StartFire(uint8 FireModeNum);
    virtual void StopFire(uint8 FireModeNum);
    
    /** Tell server fire button was pressed. bClientFired is true if client actually fired weapon */
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerStartFire(uint8 FireModeNum, bool bClientFired);
    
    /** ServerStartFire, also pass Z offset since it is interpolation */
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerStartFireOffset(uint8 FireModeNum, uint8 ZOffset, bool bClientFired);
    
    /** Just replicated ZOffset for shot fire location */
    UPROPERTY()
    float FireZOffset;
    
    /** When received FireZOffset - Only valid for same time and next frame */
    UPROPERTY()
    float FireZOffsetTime;
    
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerStopFire(uint8 FireModeNum);
    
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired);
    virtual void EndFiringSequence(uint8 FireModeNum);
    
    /** Returns true if weapon will true fire a shot this frame - used for network synchronization */
    virtual bool WillSpawnShot(float DeltaTime);
    
    /** Returns true if weapon can fire again (fire button is pressed, weapon is held, has ammo, etc.) */
    virtual bool CanFireAgain();
    
    /** Hook when the firing state starts; called on both client and server */
    UFUNCTION(BlueprintNativeEvent)
    void OnStartedFiring();
    
    /** Hook for the return to active state (was firing, refire timer expired, trigger released and/or out of ammo) */
    UFUNCTION(BlueprintNativeEvent)
    void OnStoppedFiring();
    
    /** Return true and trigger effects if should continue firing, otherwise sends weapon to its active state */
    virtual bool HandleContinuedFiring();
    
    /** Hook for when the weapon has fired, the refire delay passes, and the user still wants to fire (trigger still down) so the firing loop will repeat */
    UFUNCTION(BlueprintNativeEvent)
    void OnContinuedFiring();
    
    /**
     * Blueprint hook for pressing one fire mode while another is currently firing (e.g. hold alt, press primary)
     * CurrentFireMode == current, OtherFireMode == one just pressed
     */
    UFUNCTION(BlueprintNativeEvent)
    void OnMultiPress(uint8 OtherFireMode);

    /**
     * Activates the weapon as part of a weapon switch
     * (this weapon has already been set to Pawn->Weapon)
     * @param OverflowTime - overflow from timer of previous weapon PutDown() due to tick delta
     */
    virtual void BringUp(float OverflowTime = 0.0f);
    
    /**
     * Puts the weapon away as part of a weapon switch
     * Return false to prevent weapon switch (must keep this weapon equipped)
     */
    virtual bool PutDown();
    
    UFUNCTION(BlueprintImplementableEvent)
    void OnBringUp();
    
    UFUNCTION(BlueprintImplementableEvent)
    bool eventPreventPutDown();
    
    UFUNCTION(BlueprintNativeEvent)
    void AttachToOwner();
    
    UFUNCTION(BlueprintNativeEvent)
    void DetachFromOwner();
    
    virtual uint8 GetNumFireMode() const
    {
        return FMath::Min3(255, FiringState.Num(), FireInterval.Num());
    }
    
    virtual bool IsChargedFireMode(uint8 TestMode) const;
    
    virtual void GivenTo(ANZCharacter* NewOwner, bool bAutoActivate) override;
    virtual void ClientGivenTo_Internal(bool bAutoActivate) override;
    
    virtual void Removed() override;
    virtual void ClientRemoved_Implementation() override;
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void FireShot();
    
    UFUNCTION(BlueprintImplementableEvent)
    bool FireShotOverride();
    
    
    
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool HasAnyAmmo();
    
    
    
    
    
    
    
    
};
