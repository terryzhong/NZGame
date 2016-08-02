// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "NZCharacterMovementComponent.h"
#include "NZCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterDiedSignature, class AController*, Killer, const class UDamageType*, DamageType);

/**
 * Replicated movement data of our RootComponent.
 * More efficient than engine's FRepMovement
 */
USTRUCT()
struct FRepNZMovement
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize LinearVelocity;

	UPROPERTY()
	FVector_NetQuantize Location;

	UPROPERTY()
	uint8 AccelDir;

	UPROPERTY()
	FRotator Rotation;

	FRepNZMovement()
		: LinearVelocity(ForceInit)
		, Location(ForceInit)
		, Rotation(ForceInit)
	{}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		bOutSuccess = true;

		bool bOutSuccessLocal = true;

		// Update location, linear velocity
		Location.NetSerialize(Ar, Map, bOutSuccessLocal);
		bOutSuccess &= bOutSuccessLocal;
		Rotation.SerializeCompressed(Ar);
		LinearVelocity.NetSerialize(Ar, Map, bOutSuccessLocal);
		bOutSuccess &= bOutSuccessLocal;
		Ar.SerializeBits(&AccelDir, 8);

		return true;
	}

	bool operator==(const FRepNZMovement& Other) const
	{
		if (LinearVelocity != Other.LinearVelocity)
		{
			return false;
		}

		if (Location != Other.Location)
		{
			return false;
		}

		if (Rotation != Other.Rotation)
		{
			return false;
		}

		if (AccelDir != Other.AccelDir)
		{
			return false;
		}

		return true;
	}

	bool operator!=(const FRepNZMovement& Other) const
	{
		return !(*this == Other);
	}
};

USTRUCT(BlueprintType)
struct FTakeHitInfo
{
    GENERATED_BODY()
    
    /** The amount of damage */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    int32 Damage;
    
    /** The location of the hit (relative to Pawn center) */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    FVector_NetQuantize RelHitLocation;
    
    /** How much momentum was imparted */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    FVector_NetQuantize Momentum;
    
    /** The damage type we were hit with */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    TSubclassOf<UDamageType> DamageType;
    
    /** Shot direction pitch, manually compressed */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    uint8 ShotDirPitch;
    
    /** Shot direction yaw, manually compressed */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    uint8 ShotDirYaw;
    
    /**
     * Number of near-simultaneous shots of same damage type
     * NOTE: On the server, on-hit functions (and effects, if applicable) will get called for every hit regardless of this value 
     *       Therefore, this value should generally only be considered on clients
     */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    uint8 Count;
    
    /**
     * If damage was partially or completely absorbed by inventory, the item that did so 
     * Used to play different effects
     */
    UPROPERTY(BlueprintReadWrite, Category = TakeHitInfo)
    TSubclassOf<class ANZInventory> HitArmor;
};

/**
 * Ammo counter
 */
USTRUCT(BlueprintType)
struct FStoredAmmo
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ammo)
    TSubclassOf<class ANZWeapon> Type;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ammo)
    int32 Amount;
};

USTRUCT(BlueprintType)
struct FSavedPosition
{
    GENERATED_BODY()

	FSavedPosition()
		: Position(FVector(0.f))
		, Rotation(FRotator(0.f))
		, Velocity(FVector(0.f))
		, bTeleported(false)
		, bShotSpawned(false)
		, Time(0.f)
		, TimeStamp(0.f)
	{}

	FSavedPosition(FVector InPos, FRotator InRot, FVector InVel, bool InTeleported, bool InShotSpawned, float InTime, float InTimeStamp)
		: Position(InPos)
		, Rotation(InRot)
		, Velocity(InVel)
		, bTeleported(InTeleported)
		, bShotSpawned(InShotSpawned)
		, Time(InTime)
		, TimeStamp(InTimeStamp)
	{}
    
	/** Position of player at time Time */
    UPROPERTY()
    FVector Position;
    
	/** Rotation of player at time Time */
    UPROPERTY()
    FRotator Rotation;
    
	/** Keep velocity also for bots to use in realistic reaction time based aiming error model */
    UPROPERTY()
    FVector Velocity;
    
	/** True if teleport occurred getting to current position (so don't interpolate) */
    UPROPERTY()
    bool bTeleported;
    
	/** True if shot was spawned at this position */
    UPROPERTY()
    bool bShotSpawned;
    
	/** Current server world time when this position was updated */
    float Time;
    
	/** Client timestamp associated with this position */
    float TimeStamp;
};

USTRUCT(BlueprintType)
struct FBloodDecalInfo
{
    GENERATED_BODY()
    
public:
    /** Material to use for the decal */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DecalInfo)
    UMaterialInterface* Material;
    
    /** Base scale of decal */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DecalInfo)
    FVector2D BaseScale;
    
    /** Range of random scaling applied (always uniform) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DecalInfo)
    FVector2D ScaleMultRange;
    
    FBloodDecalInfo()
    : Material(NULL)
    , BaseScale(32.0f, 32.0f)
    , ScaleMultRange(0.8f, 1.2f)
    {}
};


UCLASS(Config = Game)
class NZGAME_API ANZCharacter : public ACharacter
{
	GENERATED_BODY()

	friend void UNZCharacterMovementComponent::PerformMovement(float DeltaSeconds);

public:
	// Sets default values for this character's properties
	//ANZCharacter();
	ANZCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    
    virtual void PostInitializeComponents() override;
    
    virtual void Destroyed() override;

	// Networking
	virtual void PawnClientRestart() override;

    /** Used for replication of our RootComponent's position and velocity */
	UPROPERTY(ReplicatedUsing = OnRep_NZReplicatedMovement)
	struct FRepNZMovement NZReplicatedMovement;

    /** TODO FIXME: Temporary different name until engine team makes UpdateSimulatedPosition() virtual */
	virtual void NZUpdateSimulatedPosition(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewVelocity);

	virtual void PostNetReceiveLocationAndRotation();

    /** NZReplicatedMovement struct replication event */
	UFUNCTION()
	virtual void OnRep_NZReplicatedMovement();

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

    /** 
     * NZCharacter version of GatherMovement(), gathers into NZReplicatedMovement. 
     * Return true if using NZReplicatedMovement rather than ReplicatedMovement.
     */
	virtual bool GatherNZMovement();

	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(Unreliable, Server, WithValidation)
	virtual void NZServerMove(float TimeStamp, FVector_NetQuantize InAccel, FVector_NetQuantize ClientLoc, uint8 CompressedMoveFlags, float ViewYaw, float ViewPitch, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);

	UFUNCTION(Unreliable, Server, WithValidation)
	virtual void NZServerMoveOld(float OldTimeStamp, FVector_NetQuantize OldAccel, float OldYaw, uint8 OldMoveFlags);

	UFUNCTION(Unreliable, Server, WithValidation)
	virtual void NZServerMoveQuick(float TimeStamp, FVector_NetQuantize InAccel, uint8 PendingFlags);

	UFUNCTION(Unreliable, Server, WithValidation)
	virtual void NZServerMoveSaved(float TimeStamp, FVector_NetQuantize InAccel, uint8 PendingFlags, float ViewYaw, float ViewPitch);
    
    /** 813 */
    virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;
    
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/**
	* Hook to modify damage taken by this Pawn
	* NOTE: Return value is a workaround for blueprint bugs involving ref parameters and is not used
	*/
	UFUNCTION(BlueprintNativeEvent)
	bool ModifyDamageTaken(UPARAM(ref) int32& AppliedDamage, UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, UPARAM(ref) ANZInventory*& HitArmor, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);

	/** 
	 * Hook to modify damage CAUSED by this Pawn - note that EventInstigator may not be equal to Controller if we're in a vehicle, etc
	 * NOTE: Return value is a workaround for blueprint bugs involving ref parameters and is not used
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool ModifyDamageCaused(UPARAM(ref) int32& AppliedDamage, UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, const FHitResult& HitInfo, AActor* Victim, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);

    /**
     * Returns location of head (origin of headshot zone)
     * Will force a skeleton update if mesh hasn't been rendered (or dedicated server) so the provided position is accurate
     */
    virtual FVector GetHeadLocation(float PredictionTime = 0.f);
    
    /** Checks for a head shot - called by weapons with head shot bonuses */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual bool IsHeadShot(FVector HitLocation, FVector ShotDirection, float WeaponHeadScaling, ANZCharacter* ShotInstigator, float PredictionTime = 0.f);
    
    virtual void NotifyTakeHit(AController* InstigatedBy, int32 AppliedDamage, int32 Damage, FVector Momentum, ANZInventory* HitArmor, const FDamageEvent& DamageEvent);

	/** Set LastTakeHitInfo from a damage event and call PlayTakeHitEffects() */
	virtual void SetLastTakeHitInfo(int32 AttemptedDamage, int32 Damage, const FVector& Momentum, ANZInventory* HitArmor, const FDamageEvent& DamageEvent);
    
    /** 
     * Blood effects (chosen at random when spawning blood)
     * Note that these are intentionally split instead of a NZImpactEffect because the sound, particles, and decals are all handled with separate logic
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effects)
    TArray<UParticleSystem*> BloodEffects;
    
    /** Blood decal materials placed on nearby world geometry */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effects)
    TArray<FBloodDecalInfo> BloodDecals;
    
    /** Trace to nearest world geometry and spawn a blood decal at the hit location (if any) */
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = Effects)
    virtual void SpawnBloodDecal(const FVector& TraceStart, const FVector& TraceDir);
    
    /** Last time ragdolling corpse spawned a blood decal */
    UPROPERTY(BlueprintReadWrite, Category = Effects)
    float LastDeathDecalTime;
    
    UPROPERTY(BlueprintReadWrite, Category = Effects)
    float RagdollCollisionBleedThreshold;

    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
    void PlayTakeHitEffects();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
    void PlayDamageEffects();
    
    
    
    /**
     * Called when we die (generally, by running out of health)
     * SERVER ONLY - do not do visual effects here!
     * Return true if we can die, false if immortal (gametype effect, powerup, mutator, etc)
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Meta = (DisplayName = "Died"), Category = Pawn)
    bool K2_Died(AController* EventInstigator, TSubclassOf<UDamageType> DamageType);
    virtual bool Died(AController* EventInstigator, const FDamageEvent& DamageEvent);
    
    UPROPERTY(BlueprintAssignable)
    FCharacterDiedSignature OnDied;
    
    /** Time between StopRagdoll() call and when physics has been fully blended out of our mesh */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ragdoll)
    float RagdollBlendOutTime;
    
    virtual void StartRagdoll();
    virtual void StopRagdoll();
    
protected:
    UPROPERTY(BlueprintReadOnly, Category = Ragdoll)
    float RagdollGravityScale;
public:
    inline float GetRagdollGravityScale() const
    {
        return RagdollGravityScale;
    }
    /** Sets gravity for the ragdoll (used by some death effects) */
    UFUNCTION(BlueprintCallable, Category = Ragdoll)
    virtual void SetRagdollGravityScale(float NewScale);

    
    /** First person arm mesh */
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    class USkeletalMeshComponent* FirstPersonMesh;
    
    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* CharacterCameraComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
    class UNZCharacterMovementComponent* NZCharacterMovement;

	/** 1320 */
	UFUNCTION(BlueprintCallable, Category = Team)
	virtual uint8 GetTeamNum() const;


	virtual void NotifyTeamChanged();

	virtual void PlayerChangedTeam();

	virtual void PlayerSuicide();
    
    UPROPERTY(BlueprintReadOnly)
    TSubclassOf<class ANZCharacterContent> CharacterData;
    
    /** Temp replacement for CharacterData->GetDefaultObject() until 4.9 enables that in blueprints */
    UFUNCTION(BlueprintCallable, Category = Character)
    ANZCharacterContent* GetCharacterData() const;
    
    virtual void ApplyCharacterData(TSubclassOf<class ANZCharacterContent> Data);
    
    /** Called when a PC viewing this character switches from behindview to first person or vice versa */
    virtual void BehindViewChange(APlayerController* PC, bool bNowBehindView);
    
    
    /**
     * Whether spawn protection may potentially be applied (still must meet time since spawn check in NZGameMode)
     * Set to false after firing weapon or any other action that is considered offensive
     */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Pawn)
    bool bSpawnProtectionEligible;

    /** Returns whether spawn protection currently applies for this character (valid on client) */
    UFUNCTION(BlueprintCallable, Category = Damage)
    bool IsSpawnProtected();
    
    /** 
     * Set temporarily during client reception of replicated properties because replicated position and switches to ragdoll may be processed out of the desired order
     * When set, OnRep_ReplicatedMovement() will be called after switching to ragdoll
     */
    bool bDeferredReplicatedMovement;
    

    
    
    UPROPERTY()
    UAnimMontage* CurrentEmote;
    
    /** Keep track of emote count so we can clear CurrentEmote */
    UPROPERTY()
    int32 EmoteCount;
    
    /** Stored past position of this player. Used for bot aim error model, and for server side hit resolution */
    UPROPERTY()
    TArray<FSavedPosition> SavedPositions;
    
    /** Maximum interval to hold saved positions for */
    UPROPERTY()
    float MaxSavedPositionAge;
    
    /** 379
     Mark the last saved position as where a shot was spawned so can synch firing to client position */
    virtual void NotifyPendingServerFire();

	/** Called by CharacterMovement after movement */
	virtual void PositionUpdated(bool bShotSpawned);
    
    /** Returns this character's position PredictionTime seconds ago */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual FVector GetRewindLocation(float PredictionTime);
    
    /** Max time server will look back to found client synchronized shot position */
    UPROPERTY(EditAnywhere, Category = Weapon)
    float MaxShotSynchDelay;
    
    /** Returns most recent position with bShotSpawned */
    virtual FVector GetDelayedShotPosition();
    virtual FRotator GetDelayedShotRotation();
    
    /** Return true if there's a recent delayed shot */
    virtual bool DelayedShotFound();
    
    
    /** Limit to armor stacking */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
    int32 MaxStackedArmor;
    
	/** Returns current total armor amount */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	virtual int32 GetArmorAmount();

    
    
    /** Use this to iterator inventory */
    template<typename> friend class TInventoryIterator;
    
protected:
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Pawn)
    class ANZInventory* InventoryList;
public:
    /** 
     * Returns first inventory item in the chain
     * NOTE: usually you should use TInventoryIterator
     */
    inline ANZInventory* GetInventory() { return InventoryList; }
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Pawn|Inventory", meta = (DisplayName = "CreateInventory", AdvancedDisplay = "bAutoActivate"))
    virtual ANZInventory* K2_CreateInventory(TSubclassOf<ANZInventory> NewInvClass, bool bAutoActivate = true);
    
    template<typename InvClass>
    inline InvClass* CreateInventory(TSubclassOf<InvClass> NewInvClass, bool bAutoActivate = true)
    {
        InvClass* Result = (InvClass*)K2_CreateInventory(NewInvClass, bAutoActivate);
        checkSlow(Result == NULL || Result->IsA(InvClass::StaticClass()));
        return Result;
    }
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Pawn)
    virtual bool AddInventory(ANZInventory* InvToAdd, bool bAutoActivate);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Pawn)
    virtual void RemoveInventory(ANZInventory* InvToRemove);
    
    /** Find an inventory item of a specified type */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual ANZInventory* K2_FindInventoryType(TSubclassOf<ANZInventory> Type, bool bExactClass = false) const;
    
    template<typename InvClass>
    inline InvClass* FindInventoryType(TSubclassOf<InvClass> Type, bool bExactClass = false) const
    {
        InvClass* Result = (InvClass*)K2_FindInventoryType(Type, bExactClass);
        checkSlow(Result == NULL || Result->IsA(InvClass::StaticClass()));
        return Result;
    }
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Pawn)
    virtual void TossInventory(ANZInventory* InvToToss, FVector ExactVelocity = FVector::ZeroVector);
    
    /** Discards (generally destroys) all inventory items */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void DiscardAllInventory();
    
    /** Call to propagate a named character event (jumping, firing, etc) to all inventory items with bCallOwnerEvent = true */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void InventoryEvent(FName EventName);
    
    bool IsInInventory(const ANZInventory* TestInv) const;
    
    virtual void AddDefaultInventory(TArray<TSubclassOf<ANZInventory>> DefaultInventoryToAdd);
    
    
    
    /** Switches weapons; handles client/server sync, safe to call on either side. 
     Uses classic groups, temporary until we have full weapon switching configurability menu. FIXMESTEVE */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void SwitchWeapon(ANZWeapon* NewWeapon);

	/** Switches weapon locally, must execute independently on both server and client */
	virtual void LocalSwitchWeapon(ANZWeapon* NewWeapon);

	/** RPC to do weapon switch */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerSwitchWeapon(ANZWeapon* NewWeapon);
	UFUNCTION(Client, Reliable)
	virtual void ClientSwitchWeapon(ANZWeapon* NewWeapon);

	/** Utility to redirect to SwitchToBestWeapon() to the character's Controller (human or AI) */
	void SwitchToBestWeapon();
    
protected:
    /** Firemodes with input currently being held down (pending or actually firing) */
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    TArray<uint8> PendingFire;
public:
    inline bool IsPendingFire(uint8 InFireMode) const { return !IsFiringDisabled() && (InFireMode < PendingFire.Num() && PendingFire[InFireMode] != 0); }
    
    /** Set the pending fire flag; generally should be called by whatever weapon processes the firing command, unless it's an explicit single shot */
    inline void SetPendingFire(uint8 InFireMode, bool bNowFiring)
    {
        if (PendingFire.Num() < InFireMode + 1)
        {
            PendingFire.SetNumZeroed(InFireMode + 1);
        }
        PendingFire[InFireMode] = bNowFiring ? 1 : 0;
    }
    
    inline void ClearPendingFire()
    {
        for (int32 i = 0; i < PendingFire.Num(); i++)
        {
            PendingFire[i] = 0;
        }
    }
    
protected:
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    bool bDisallowWeaponFiring;
public:
    /**
     * Allows disabling all weapon firing from this Pawn
     * NOT replicated, must be called on both sides to work properly
     */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void DisallowWeaponFiring(bool bDisallowed);
    
    inline bool IsFiringDisabled() const
    {
        return bDisallowWeaponFiring;
    }
    
    inline ANZWeapon* GetWeapon() const
    {
        return Weapon;
    }
    
    inline TSubclassOf<ANZWeapon> GetWeaponClass() const
    {
        // Debug check to make sure this matches as expected
        checkSlow(GetNetMode() == NM_Client || (Weapon == NULL ? WeaponClass == NULL : ((UObject*)Weapon)->GetClass() == WeaponClass));
        return WeaponClass;
    }
    
    inline ANZWeapon* GetPendingWeapon() const
    {
        return PendingWeapon;
    }
    
    /** Used for spectating fired projectiles */
    UPROPERTY(BlueprintReadOnly, Category = Weapon)
    class ANZProjectile* LastFiredProjectile;
    
    /**
     * Called by weapon being put down when it has finished being unequipped. Transition PendingWeapon to Weapon and bring it up
     * @param OverflowTime - Amount of time past end of timer that previous weapon PutDown() used (due to frame delta) - pass onto BringUp() to keep things in sync
     */
    virtual void WeaponChanged(float OverflowTime = 0.0f);
    
    /** Called when the client's current weapon has been invalidated (removed from inventory, etc) */
    UFUNCTION(Client, Reliable)
    void ClientWeaponLost(ANZWeapon* LostWeapon);
    
    /** Replicated weapon firing info */
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = FiringInfoReplicated, Category = Weapon)
    uint8 FlashCount;
    
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon)
    uint8 FireMode;
    
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = FiringExtraUpdated, Category = Weapon)
    uint8 FlashExtra;
    
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = FiringInfoReplicated, Category = Weapon)
    FVector_NetQuantize FlashLocation;
    
    UPROPERTY(BlueprintReadOnly, Category = Weapon)
    float LastWeaponFireTime;
    
    /** 595
     Set when client is locally simulating FlashLocation so ignore any replicated value */
    bool bLocalFlashLoc;
    
    /** 
     * Set info for one instance of firing and plays firing effects;
     * Assumed to be a valid shot - call ClearFiringInfo() if the weapon has stopped firing
     * If a location is not needed (projectile) call IncrementFlashCount() instead
     */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void SetFlashLocation(const FVector& InFlashLoc, uint8 InFireMode);
    
    /**
     * Set info for one instance of firing and plays firing effects;
     * Assumed to be a valid shot - call ClearFiringInfo() if the weapon has stopped firing
     * If a location is needed (instant hit, beam, etc) call SetFlashLocation() instead
     */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void IncrementFlashCount(uint8 InFireMode);
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void SetFlashExtra(uint8 NewFlashExtra, uint8 InFireMode);
    
    /** Clears firing variables; i.e. because the weapon has stopped firing */
    UFUNCTION(BlueprintCallable, Category = Weapon)
    virtual void ClearFiringInfo();
    
    /** Called when firing variables are updated to trigger/stop effects */
    virtual void FiringInfoUpdated();
    
    /** Called when FlashExtra is updated; routes call to weapon attachment */
    UFUNCTION()
    virtual void FiringExtraUpdated();
    
    /** Repnotify handler for firing variables, generally just calls FiringInfoUpdated() */
    UFUNCTION()
    virtual void FiringInfoReplicated();
    
    
    /** Returns true if any local PlayerController is viewing this Pawn */
    bool IsLocallyViewed()
    {
        for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
        {
            if (It->PlayerController != NULL && It->PlayerController->GetViewTarget() == this)
            {
                return true;
            }
        }
        return false;
    }



    /** 1596
     Spawn/Destroy/Replace the current weapon attachment to represent the equipped weapon (through WeaponClass) */
    UFUNCTION()
    virtual void UpdateWeaponAttachment();
    
    /** Spawn/Destroy/Replace the current holstered weapon attachment to represent the equipped weapon (through WeaponClass) */
    UFUNCTION()
    virtual void UpdateHolsteredWeaponAttachment();

protected:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Movement)
	float WalkMovementReductionPct;

	/** Time remaining until WalkMovementReductionPct is reset to zero */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Movement)
	float WalkMovementReductionTime;
public:
	inline float GetWalkMovementReductionPct()
	{
		return WalkMovementReductionPct;
	}

	/** Set walking movement reduction */
	UFUNCTION(BlueprintCallable, Category = Movement)
	virtual void SetWalkMovementReduction(float InPct, float InDuration);

    
    /** Weapon firing */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void StartFire(uint8 FireModeNum);
    
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void StopFire(uint8 FireModeNum);
    
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void StopFiring();
    
    virtual void PawnStartFire(uint8 FireModeNum = 0) override
    {
        StartFire(FireModeNum);
    }

	UFUNCTION(BlueprintCallable, Category = Pawn)
	virtual void ChangeClip();

	UFUNCTION(BlueprintCallable, Category = Pawn)
	virtual void LeftHandChangeClip();

    /** 1682
     Weapon and attachments */
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    class ANZWeapon* PendingWeapon;
    
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Pawn)
    class ANZWeapon* Weapon;
    
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = UpdateWeaponAttachment, Category = Pawn)
    TSubclassOf<ANZWeapon> WeaponClass;
    
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    class ANZWeaponAttachment* WeaponAttachment;

    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = UpdateWeaponAttachment, Category = Pawn)
    TSubclassOf<ANZWeaponAttachment> WeaponAttachmentClass;
    
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    class ANZWeaponAttachment* HolsteredWeaponAttachment;
    
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = UpdateHolsteredWeaponAttachment, Category = Pawn)
    TSubclassOf<ANZWeaponAttachment> HolsteredWeaponAttachmentClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
    TArray<TSubclassOf<ANZInventory> > DefaultCharacterInventory;

    // Ambient sounds
protected:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = AmbientSoundUpdated, Category = Audio)
    USoundBase* AmbientSound;
    
    UPROPERTY(BlueprintReadOnly, Category = Audio)
    UAudioComponent* AmbientSoundComp;
    
    /** Ambient sound played only on owning client */
    UPROPERTY(BlueprintReadOnly, Category = Audio)
    USoundBase* LocalAmbientSound;
    
    /** Volume of Ambient sound played only on owning client */
    UPROPERTY(BlueprintReadOnly, Category = Audio)
    float LocalAmbientVolume;
    
    UPROPERTY(BlueprintReadOnly, Category = Audio)
    UAudioComponent* LocalAmbientSoundComp;
    
    /** Status ambient sound played only on owning client */
    UPROPERTY(BlueprintReadOnly, Category = Audio)
    USoundBase* StatusAmbientSound;
    
    UPROPERTY(BlueprintReadOnly, Category = Audio)
    UAudioComponent* StatusAmbientSoundComp;
    
public:
    /** 
     * Sets replicated ambient (looping) sound on this Pawn
     * Only one ambient sound can be set at a time
     * Pass bClear with a valid NewAmbientSound to remove only if NewAmbientSound == CurrentAmbientSound
     */
    UFUNCTION(BlueprintCallable, Category = Audio)
    virtual void SetAmbientSound(USoundBase* NewAmbientSound, bool bClear = false);
    
    UFUNCTION()
    void AmbientSoundUpdated();
    
    /**
     * Sets local (not replicated) ambient (looping) sound on this Pawn
     * Only one ambient sound can be set at a time
     * Pass bClear with a valid NewAmbientSound to remove only if NewAmbientSound == CurrentAmbientSound
     */
    UFUNCTION(BlueprintCallable, Category = Audio)
    virtual void SetLocalAmbientSound(USoundBase* NewAmbientSound, float SoundVolume = 0.f, bool bClear = false);
    
    UFUNCTION()
    void LocalAmbientSoundUpdated();
    
    
    UPROPERTY()
    TArray<FStoredAmmo> SavedAmmo;



    /**
     * Landing just occurred, play any effects desired. Landing sound will be played server-side elsewhere.
     * Called on server and owning client.
     */
    UFUNCTION(BlueprintNativeEvent)
    void PlayLandedEffect();
    
    /** Landing assist just occurred */
    UFUNCTION(BlueprintImplementableEvent)
    void OnLandingAssist();

	/** Handles moving forward/backward */
	virtual void MoveForward(float Value);

	/** Handles strafing movement, left and right */
	virtual void MoveRight(float Value);

	/** Handles up and down when swimming or flying */
	virtual void MoveUp(float Value);

	virtual void Sprint();
	virtual void UnSprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSprinting;



    UFUNCTION(BlueprintPure, Category = PlayerController)
    virtual APlayerCameraManager* GetPlayerCameraManager();

    /** Min Z speed to spawn LandEffect */
    UPROPERTY(EditAnywhere, Category = Effects)
    float LandEffectSpeed;
    
    /** Particle component for high velocity jump landing */
    UPROPERTY(EditAnywhere, Category = Effects)
    UParticleSystem* LandEffect;
    
    /** Particle component for normal ground footstep */
    UPROPERTY(EditAnywhere, Category = Effects)
    UParticleSystem* GroundFootstepEffect;
    
    /** Particle component for water footstep */
    UPROPERTY(EditAnywhere, Category = Effects)
    UParticleSystem* WaterFootstepEffect;

    /** Plays a footstep effect; called via animation when anims are active (in vis range and not server), otherwise on interval via Tick() */
    UFUNCTION(BlueprintCallable, Category = Effects)
    virtual void PlayFootstep(uint8 FootNum, bool bFirstPerson = false);
    
    /** Play jumping sound/effects; should be called on server and owning client */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Effects)
    void PlayJump(const FVector& JumpLocation, const FVector& JumpDir);

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
    USoundBase* FootstepSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
    USoundBase* OwnFootstepSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
    USoundBase* WaterFootstepSound;
    
    // Falling
    /** Landing at faster than this velocity results in damage (note: use positive number) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling Damage")
    float MaxSafeFallSpeed;
    
    /** Amount of falling damage dealt if the player's fall speed is double MaxSafeFallSpeed (scaled linearly from there) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling Damage")
    float FallingDamageFactor;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CrushingDamageFactor;
    
    /** 
     * Blueprint override for take falling damage.
     * Return true to keep TakeFallingDamage() from causing damage.
     * FallingSpeed is the Z velocity at landing, and Hit describes the impacted surface.
     */
    UFUNCTION(BlueprintImplementableEvent)
    bool HandleFallingDamage(float FallingSpeed, const FHitResult& Hit);
    
    UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual void TakeFallingDamage(const FHitResult& Hit, float FallingSpeed);
    
    virtual void Landed(const FHitResult& Hit) override;

    // Swimming
    
    virtual bool FeetAreInWater() const;
    
    virtual APhysicsVolume* PositionIsInWater(const FVector& Position) const;
    
    /** 630
     */
    UPROPERTY(BlueprintReadWrite, Replicated, Category = Pawn)
    int32 Health;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pawn)
    int32 HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pawn)
	bool bDamageHurtsHealth;

	/** Replicated to spectators, not authoritative */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = Pawn)
	int32 ArmorAmount;

	/** True if this character was falling when last took damage */
	UPROPERTY()
	bool bWasFallingWhenDamaged;
    
    UPROPERTY(BlueprintReadWrite, Replicated, Category = Pawn)
    float DamageScaling;
    
    /** Multiplier to firing speed */
    UPROPERTY(Replicated, ReplicatedUsing = FireRateChanged)
    float FireRateMultiplier;
    
    /** Accessors to FireRateMultiplier */
    UFUNCTION(BlueprintCallable, Category = Pawn)
    float GetFireRateMultiplier();
    
    UFUNCTION(BlueprintCallable, Category = Pawn)
    void SetFireRateMultiplier(float InMult);
    
    UFUNCTION()
    void FireRateChanged();
    
    UPROPERTY(BlueprintReadWrite, Replicated, ReplicatedUsing = PlayTakeHitEffects, Category = Pawn)
    FTakeHitInfo LastTakeHitInfo;
   
    /** Time of last SetLastTakeHitInfo() - authority only */
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    float LastTakeHitTime;
    
    /** Last time LastTakeHitInfo was checked for replication; used to combine multiple hits into one LastTakeHitInfo */
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    float LastTakeHitReplicatedTime;
    
    
    /** 743
     */
    UPROPERTY(ReplicatedUsing = OnRep_DrivenVehicle)
    APawn* DrivenVehicle;
    
    UFUNCTION()
    virtual void OnRep_DrivenVehicle();
    
    virtual void StartDriving(APawn* Vehicle);
    virtual void StopDriving(APawn* Vehicle);
    
    
    
    /** 897
     Time Character died */
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    float TimeOfDeath;
     
    
    /** Plays death effects; use LastTakeHitInfo to do damage-specific death effects */
    virtual void PlayDying();
    
    virtual void TornOff() override
    {
        PlayDying();
    }
    
    virtual bool IsRecentlyDead();
    
    virtual void DeactivateSpawnProtection();
    
    UFUNCTION(BlueprintCallable, Category = Pawn)
    bool IsDead();
    
protected:
    /** Destroys dead character if no longer onscreen */
    UFUNCTION()
    void DeathCleanupTimer();
    
protected:
    /** Set during ragdoll recovery (still blending out physics, playing recover anim, etc) */
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    bool bInRagdollRecovery;
    
public:
    /** Return true if character is in a ragdoll state */
    UFUNCTION(BlueprintCallable, Category = "Pawn | Character")
    virtual bool IsRagdoll() const
    {
        return bInRagdollRecovery || (RootComponent == GetMesh() && GetMesh()->IsSimulatingPhysics());
    }
    
    virtual FVector GetLocationCenterOffset() const;
    
    
    UPROPERTY(BlueprintReadWrite, Category = Sounds)
    float LastPainSoundTime;
    
    UPROPERTY(BlueprintReadWrite, Category = Sounds)
    float MinPainSoundInterval;
    
    
    /** 1271 */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Effects)
    virtual void SetWeaponAttachmentClass(TSubclassOf<class ANZWeaponAttachment> NewWeaponAttachmentClass);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Effects)
    virtual void SetHolsteredWeaponAttachmentClass(TSubclassOf<class ANZWeaponAttachment> NewWeaponAttachmentClass);
    
    /** 1254
     Uses CharOverlayFlags to apply the desired overlay material (if any) to OverlayMesh */
    UFUNCTION()
    virtual void UpdateCharOverlays();
    
    /** 1278
     Uses WeaponOverlayFlags to apply the desired overlay material (if any) to OverlayMesh */
    UFUNCTION()
    virtual void UpdateWeaponOverlays();
    
protected:
    /** Last time PlayFootstep() was called, for timing footsteps when animations are disabled */
    float LastFootstepTime;
    
    /** Last FootNum for PlayFootstep(), for alternating when animations are disabled */
    uint8 LastFoot;
    
    /** Replicated overlays, bits match entries in NZGameState's OverlayMaterials array */
    UPROPERTY(Replicated, ReplicatedUsing = UpdateCharOverlays)
    uint16 CharOverlayFlags;
    UPROPERTY(Replicated, ReplicatedUsing = UpdateWeaponOverlays)
    uint16 WeaponOverlayFlags;
    
    /** Mesh with current active overlay material on it (created dynamically when needed) */
    UPROPERTY(BlueprintReadOnly, Category = Effects)
    USkeletalMeshComponent* OverlayMesh;
    
public:
    inline int16 GetCharOverlayFlags() { return CharOverlayFlags; }
    inline int16 GetWeaponOverlayFlags() { return WeaponOverlayFlags; }
    
    /** 1294
     * Sets full body material override only one at a time
     * Is allowed pass NULL to restore default skin
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Effects)
    virtual void SetSkin(UMaterialInterface* NewSkin);
    
    /** Replicated character material override */
protected:
    UPROPERTY(Replicated, ReplicatedUsing = UpdateSkin)
    UMaterialInterface* ReplicatedBodyMaterial;
public:
    inline UMaterialInterface* GetSkin() { return ReplicatedBodyMaterial; }
    
    /** Apply skin in ReplicatedBodyMaterial or restore to default if it's NULL */
    UFUNCTION()
    virtual void UpdateSkin();

    /** Runtime material instance for setting body material parameters (team color, etc) */
protected:
    UPROPERTY(BlueprintReadOnly, Category = Pawn)
    TArray<UMaterialInstanceDynamic*> BodyMIs;
public:
    inline const TArray<UMaterialInstanceDynamic*>& GetBodyMIs() const { return BodyMIs; }
    
    
    
    // Weapon bob and eye offset
    
    /** Current 1st person weapon deflection due to running bob */
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector CurrentWeaponBob;
    
	/** Max 1st person weapon bob deflection with axes based on player view */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    FVector WeaponBobMagnitude;
    
	/** Z deflection of first person weapon when player jumps */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    FVector WeaponJumpBob;
    
	/** Z deflection of first person weapon when player lands */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	FVector WeaponLandBob;

	/** Desired 1st person weapon deflection due to jumping */
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector DesiredJumpBob;
    
	/** Current jump bob (interpolating to DesiredJumpBob) */
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector CurrentJumpBob;

	/** Time used for weapon bob sinusoids, reset on landing */
	UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
	float BobTime;

	/** Rate of weapon bob when standing still */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	float WeaponBreathingBobRate;

	/** Rate of weapon bob when running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	float WeaponRunningBobRate;

	/** How fast to interpolate to Jump/Land bob offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	float WeaponJumpBobInterpRate;

	/** Max horizontal weapon movement interpolation rate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	float WeaponHorizontalBobInterpRate;

	/** How fast to decay out Land bob offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	float WeaponLandBobDecayRate;

	/** Get Max eye offset land bob deflection at landing velocity Z of FullEyeOffsetLandBobVelZ + EyeOffsetLandBobThreshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
	float WeaponDirChangeDeflection;

	UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
	FVector EyeOffset;

	UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
	FVector CrouchEyeOffset;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float DefaultCrouchedEyeHeight;
    
    
    /** Target eye position offset from base view position */
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector TargetEyeOffset;
    
    /** How fast EyeOffset interpolates to TargetEyeOffset */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    FVector EyeOffsetInterpRate;
    
    /** How fast CrouchEyeOffset interpolates to 0 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float CrouchEyeOffsetInterpRate;
    
    /** How fast TargetEyeOffset decays */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    FVector EyeOffsetDecayRate;
    
    /** Jump target view bob magnitude */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float EyeOffsetJumpBob;
    
    /** Jump Landing target view bob magnitude */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float EyeOffsetLandBob;
    
    /** Jump Landing target view bob velocity threshold */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float EyeOffsetLandBobThreshold;
    
    /** Jump Landing target weapon bob velocity threshold */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float WeaponLandBobThreshold;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float FullWeaponLandBobVelZ;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    float FullEyeOffsetLandBobVelZ;
    
    /** Get max weapon land bob deflection at landing velocity Z of FullWeaponLandBobVelZ + WeaponLandBobThreshold */
    UPROPERTY()
    float DefaultBaseEyeHeight;
    
    /** Maximum amount of time Pawn stays around when dead even if visible (may be cleaned up earlier if not visible) */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Death)
    float MaxDeathLifeSpan;
    

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual float GetWeaponBobScaling();

    /** Returns offset to add to first person mesh for weapon bob */
    virtual FVector GetWeaponBobOffset(float DeltaTime, ANZWeapon* MyWeapon);

	/** Returns eyeoffset transformed into current view */
	virtual FVector GetTransformedEyeOffset() const;

	/** Returns amount of eyeoffset scaling for this character's view */
	virtual float GetEyeOffsetScaling() const;

	virtual FVector GetPawnViewLocation() const override;

	/** View kick relevant */
	float PunchAnglePitch;
	float PunchAngleYaw;
	float PitchParam;
	float YawParam;
	float LastPitchAngle;
	float LastYawAngle;
	
	virtual void SetPunchAngle(FRotator Angle);
	virtual FRotator GetPunchAngle();

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
    
    
    virtual bool TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest = false, bool bNoCheck = false) override;
    
    UFUNCTION()
    virtual void OnOverlapBegin(AActor* OtherActor);
    
    virtual void CheckRagdollFallingDamage(const FHitResult& Hit);

    UFUNCTION()
    virtual void OnRagdollCollision(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    
    
    
    /** 1512
     Whether this pawn can obtain pickup items (NZPickup, NZDroppedPickup) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanPickupItems;
    
    
    
    
    /** 1847
     Local player currently viewing this character */
    UPROPERTY()
    class ANZPlayerController* CurrentViewerPC;
    
    virtual class ANZPlayerController* GetLocalViewer();
    
    /** Previous actor location Z when last updated eye offset */
    UPROPERTY()
    float OldZ;
    
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAiming;


public:
	UFUNCTION(BlueprintCallable, Category = "HMD")
	virtual void ProcessHeadMountedDisplay(FVector HeadPosition, FRotator HeadRotation, FVector RightHandPosition, FRotator RightHandRotation, FVector LeftHandPosition, FRotator LeftHandRotation);

};

