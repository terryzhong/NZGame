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

UENUM(BlueprintType)
namespace EZoomState
{
    enum Type
    {
        EZS_NotZoomed,
        EZS_ZoomingIn,
        EZS_ZoomingOut,
        EZS_Zoomed,
    };
}

USTRUCT(BlueprintType)
struct FZoomInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
    float StartFOV;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
    float EndFOV;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
    float Time;
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
    
	/** Hook to do effects when the weapon is raised */
    UFUNCTION(BlueprintImplementableEvent)
    void OnBringUp();
    
	/** Allows blueprint to prevent the weapon from being switched away from */
    UFUNCTION(BlueprintImplementableEvent)
    bool eventPreventPutDown();
    
	/** Attach the visuals to Owner's first person view */
    UFUNCTION(BlueprintNativeEvent)
    void AttachToOwner();
    
	/** Detach the visuals from the Owner's first person view */
    UFUNCTION(BlueprintNativeEvent)
    void DetachFromOwner();
    
	/** Return number of fire mode */
    virtual uint8 GetNumFireMode() const
    {
        return FMath::Min3(255, FiringState.Num(), FireInterval.Num());
    }
    
	/** Returns if the specified fire mode */
    virtual bool IsChargedFireMode(uint8 TestMode) const;
    
    virtual void GivenTo(ANZCharacter* NewOwner, bool bAutoActivate) override;
    virtual void ClientGivenTo_Internal(bool bAutoActivate) override;
    
    virtual void Removed() override;
    virtual void ClientRemoved_Implementation() override;
    
	/** Fires a shot and consumes ammo */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void FireShot();
    
	/**
	 * Blueprint override for firing
	 * NOTE: do an authority check before spawning projectiles, etc as this function is called on both sides
	 */
    UFUNCTION(BlueprintImplementableEvent)
    bool FireShotOverride();
    
	/**
	 * Plays an anim on the weapon and optionally hands antomatically handles fire rate modifiers by default, overridden if RateOverride is > 0.0
	 */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void PlayWeaponAnim(UAnimMontage* WeaponAnim, UAnimMontage* HandsAnim = NULL, float RateOverride = 0.0f);
    
	/** Returns montage to play on the weapon for the specified firing mode */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual UAnimMontage* GetFiringAnim(uint8 FireMode, bool bOnHands = false) const;

	/** Play firing effects not associated with the shot's results (e.g. muzzle flash but generally NOT emitter to target) */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void PlayFiringEffects();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void StopFiringEffects();

	/** Blueprint hook to modify spawned instance of FireEffect (e.g. tracer or beam) */
	UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
	void ModifyFireEffect(UParticleSystemComponent* Effect);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void GetImpactSpawnPosition(const FVector& TragetLoc, FVector& SpawnLocation, FRotator& SpawnRotation);

	/** It true, don't spawn impact effect. Used for hitscan hits, skips by default for pawn and projectile hits */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool CancelImpactEffect(const FHitResult& ImpactHit);

	virtual void PlayImpactEffects(const FVector& TargetLoc, uint8 FireMode, const FVector& SpawnLocation, const FRotator& SpawnRotation);

    /** Shared code between NZWeapon and NZWeaponAttachment to refine replicated FlashLocation into impact effect transform via trace */
	static FHitResult GetImpactEffectHit(APawn* Shooter, const FVector& StartLoc, const FVector& TargetLoc);

    /** Return start point for weapons fire */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon)
    virtual FVector GetFireStartLoc(uint8 FireMode = 255);
    
    /** Return base fire direction for weapons fire (i.e. direction player's weapon is pointing) */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon)
    virtual FRotator GetBaseFireRotation();
    
    /** 
     * Return adjusted fire rotation after accounting for spread, aim help, and any other secondary factors affecting aim direction
     * (may include randomized components)
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = Weapon)
    FRotator GetAdjustedAim(FVector StartFireLoc);
    
    /** If owned by a human, set ANZPlayerController::LastShotTargetGuess to closest target to player's aim */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = AI)
    void GuessPlayerTarget(const FVector& StartFireLoc, const FVector& FireDir);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Weapon)
    virtual void AddAmmo(int32 Amount);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Weapon)
    virtual void ConsumeAmmo(uint8 FireModeNum);
    
    virtual void FireInstantHit(bool bDealDamage = true, FHitResult* OutHit = NULL);
    
    UFUNCTION(BlueprintCallable, Category = Firing)
    void K2_FireInstantHit(bool bDealDamage, FHitResult& OutHit);
    
    /** Handles rewind functionality for net games with ping prediction */
    virtual void HitScanTrace(const FVector& StartLocation, const FVector& EndTrace, float TraceRadius, FHitResult& Hit, float PredictionTime);
    
    UFUNCTION(BlueprintCallable, Category = Firing)
    virtual ANZProjectile* FireProjectile();
    
    /** Spawn a projection on both server and owning client, and forward predict it by 1/2 ping on server */
    virtual ANZProjectile* SpawnNetPredictedProjectile(TSubclassOf<ANZProjectile> ProjectileClass, FVector SpawnLocation, FRotator SpawnRotation);
    
    /** Returns whether we can meet AmmoCost for the given fire mode */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool HasAmmo(uint8 FireModeNum);
    
    /** Returns whether we have ammo for any fire mode */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool HasAnyAmmo();
    
    /** Get interval between shots, including any fire rate modifiers */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual float GetRefireTime(uint8 FireModeNum);
    
    inline uint8 GetCurrentFireMode()
    {
        return CurrentFireMode;
    }
    
    inline void GotoActiveState()
    {
        GotoState(ActiveState);
    }
    
    virtual void GotoFireMode(uint8 NewFireMode);
    
    UFUNCTION(Blueprintcallable, Category = Weapon)
    bool IsFiring() const;
    
    virtual bool StackPickup_Implementation(ANZInventory* ContainedInv) override;
    
    /** Update any timers and such due to a weapon timing change; for example, a powerup that changes firing speed */
    virtual void UpdateTiming();
    
    virtual void Tick(float DeltaTime) override;
    
    virtual void Destroyed() override;
    
    static void InstanceMuzzleFlashArray(AActor* Weap, TArray<UParticleSystemComponent*>& MFArray);
    
    inline UNZWeaponState* GetCurrentState()
    {
        return CurrentState;
    }
    
    /** 
     * Called on clients only when the local owner got a kill while holding this weapon
     * NOTE: this weapon didn't necessarily cause the kill (previously fired projecctile, etc), if you care check the damagetype
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
    void NotifyKillWhileHolding(TSubclassOf<UDamageType> DmgType);
    
    virtual float GetHeadshotScale() const
    {
        return 0.0f;
    }
    
    /** Return true if needs HUD ammo display widget drawn */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintCosmetic, Category = HUD)
    bool NeedsAmmoDisplay() const;
    
    /** Returns crosshair color taking into account user settings, red flash on hit, etc */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    FLinearColor GetCrosshairColor(class UNZHUDWidget* WeaponHudWidget) const;
    
    /** The player state of the player currently under the crosshair */
    UPROPERTY()
    ANZPlayerState* TargetPlayerState;
    
    /** The time this player was last seen under the crosshair */
    float TargetLastSeenTime;
    
    /** Returns whether we should draw the friendly fire indicator on the crosshair */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual bool ShouldDrawFFIndicator(APlayerController* Viewer, ANZPlayerState*& HitPlayerState) const;
    
    /** Returns desired crosshair scale (affected by recent pickups) */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual float GetCrosshairScale(class ANZHUD* HUD);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = HUD)
    void DrawWeaponCrosshair(UNZHUDWidget* WeaponHudWidget, float RenderDelta);
    
    UFUNCTION()
    void UpdateCrosshairTarget(ANZPlayerState* NewCrosshairTarget, UNZHUDWidget* WeaponHudWidget, float RenderDelta);
    
    /**
     * Default parameters set on overlay particle effect (if any)
     * Up to the effect to care about them
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
    TArray<struct FParticleSysParam> OverlayEffectParams;
    
    /**
     * Helper for shared overlay code between NZWeapon and NZWeaponAttachment
     * NOTE: can called on default object!
     */
    virtual void UpdateOverlaysShared(AActor* WeaponActor, ANZCharacter* InOwner, USkeletalMeshComponent* InMesh, const TArray<struct FParticleSysParam>& InOverlayEffectParams, USkeletalMeshComponent*& InOverlayMesh) const;
    /** Read WeaponOverlayFlags from owner and apply the appropriate overlay material (if any) */
    virtual void UpdateOverlays();
    
    virtual void SetSkin(UMaterialInterface* NewSkin);
    
    UPROPERTY()
    FRotator LastRotation;
    
    UPROPERTY()
    float OldRotDiff[2];
    UPROPERTY()
    float OldLeadMag[2];
    UPROPERTY()
    float OldMaxDiff[2];
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon Rotation)
    float RotChgSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon Rotation)
    float ReturnChgSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon Rotation)
    float MaxYawLag;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon Rotation)
    float MaxPitchLag;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon Rotation)
    bool bProceduralLagRotation;
    
    virtual bool ShouldLagRot();
    
    virtual float LagWeaponRotation(float NewValue, float LastValue, float DeltaTime, float MaxDiff, int32 Index);
    
    virtual void UpdateWeaponHand();
    
    EWeaponHand GetWeaponHand() const;
    
    virtual void UnEquip();
    
    virtual void GotoEquippingState(float OverflowTime);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon)
    virtual bool IsUnEquipping() { return GetCurrentState() == UnequippingState; }
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    float GetDamageRadius(uint8 TestMode) const;
    
    virtual float BotDesireability_Implementation(APawn* Asker, AActor* Pickup, float PathDistance) const;
    virtual float DetourWeight_Implementation(APawn* Asker, AActor* Pickup, float PathDistance) const;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float BaseAISelectRating;
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    float GetAISelectRating();
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    float SuggestAttackStyle();
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    float SuggestDefenseStyle();
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    bool IsPreparingAttack();
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    bool ShouldAIDelayFiring();
    
    UFUNCTION(BlueprintNativeEvent, Category = AI)
    bool CanAttack(AActor* Target, const FVector& TargetLoc, bool bDirectOnly, bool bPreferCurrentMode, UPARAM(ref) uint8& BestFireMode, UPARAM(ref) FVector& OptimalTargetLoc);
    
    inline bool CanAttack(AActor* Target, const FVector& TargetLoc, bool bDirectOnly)
    {
        uint8 UnusedFireMode;
        FVector UnusedOptimalLoc;
        return CanAttack(Target, TargetLoc, bDirectOnly, false, UnusedFireMode, UnusedOptimalLoc);
    }
    
    virtual bool DoAssistedJump()
    {
        return false;
    }
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Mesh)
    TArray<UMeshComponent*> Get1PMeshes() const;
    
protected:
    UPROPERTY(BlueprintReadOnly, Category = Weapon)
    UNZWeaponState* CurrentState;
    
    UPROPERTY(BlueprintReadOnly, Category = Weapon)
    uint8 CurrentFireMode;
    
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    UNZWeaponState* ActiveState;
    
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    class UNZWeaponStateEquipping* EquippingState;
    
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    UNZWeaponState* UnequippingStateDefault;
    
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    UNZWeaponState* UnequippingState;
    
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    UNZWeaponState* InactiveState;
    
    UPROPERTY()
    USkeletalMeshComponent* OverlayMesh;
    
public:
    float WeaponBarScale;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    FTextureUVs WeaponBarSelectedUVs;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    FTextureUVs WeaponBarInactiveUVs;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
    FName KillStatsName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
    FName AltKillStatsName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
    FName DeathStatsName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
    FName AltDeathStatsName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
    FName ShotsStatsName;
    
    virtual int32 GetWeaponKillStats(ANZPlayerState* PS) const;
    virtual int32 GetWeaponDeathStats(ANZPlayerState* PS) const;
    virtual float GetWeaponShotsStats(ANZPlayerState* PS) const;
    virtual float GetWeaponHitsStats(ANZPlayerState* PS) const;
    
    UFUNCTION(exec)
    void TestWeaponLoc(float X, float Y, float Z);
    UFUNCTION(exec)
    void TestWeaponRot(float Pitch, float Yaw, float Roll = 0.0f);
    UFUNCTION(exec)
    void TestWeaponScale(float X, float Y, float Z);
    
    UFUNCTION(BlueprintImplementableEvent, Category = Weapon)
    void NotifyTeamChanged();
    
    UFUNCTION(BlueprintNativeEvent, Category = Weapon)
    void FiringInfoUpdated(uint8 InFireMode, uint8 FlashCount, FVector InFlashLocation);
    UFUNCTION(BlueprintNativeEvent, Category = Weapon)
    void FiringExtraUpdated(uint8 NewFlashExtra, uint8 InFireMode);
    UFUNCTION(BlueprintNativeEvent, Category = Weapon)
    void FiringEffectsUpdated(uint8 InFireMode, FVector InFlashLocation);
    
    UPROPERTY(Replicated, ReplicatedUsing = OnRep_ZoomCount)
    uint8 ZoomCount;
    UFUNCTION()
    virtual void OnRep_ZoomCount();
    
    UPROPERTY(BlueprintReadOnly, Category = Zoom, Replicated, ReplicatedUsing = OnRep_ZoomState)
    TEnumAsByte<EZoomState::Type> ZoomState;
    UFUNCTION(BlueprintNativeEvent)
    void OnRep_ZoomState();
    
    UPROPERTY(BlueprintReadOnly, Category = Zoom, Replicated)
    uint8 CurrentZoomMode;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Zoom)
    TArray<FZoomInfo> ZoomModes;
    
    UPROPERTY(BlueprintReadOnly, Category = Zoom, Replicated)
    float ZoomTime;
    
    UFUNCTION(BlueprintCallable, Category = Zoom)
    virtual void SetZoomMode(uint8 NewZoomMode);
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerSetZoomMode(uint8 NewZoomMode);
    virtual void LocalSetZoomMode(uint8 NewZoomMode);
    
    UFUNCTION(BlueprintCallable, Category = Zoom)
    virtual void SetZoomState(TEnumAsByte<EZoomState::Type> NewZoomState);
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerSetZoomState(uint8 NewZoomState);
    virtual void LocalSetZoomState(uint8 NewZoomState);
    
    UFUNCTION(BlueprintNativeEvent)
    void OnZoomedIn();
    
    UFUNCTION(BlueprintNativeEvent)
    void OnZoomedOut();
    
    virtual void TickZoom(float DeltaTime);
};
