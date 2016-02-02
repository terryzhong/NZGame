// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "NZCharacter.generated.h"





/**
 * Ammo counter
 */
USTRUCT(BlueprintType)
struct FStoredAmmo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ammo)
    TSubclassOf<class ANZWeapon> Type;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Ammo)
    int32 Amount;
};

USTRUCT(BlueprintType)
struct FSavedPosition
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY()
    FVector Position;
    
    UPROPERTY()
    FRotator Rotation;
    
    UPROPERTY()
    FVector Velocity;
    
    UPROPERTY()
    bool bTeleported;
    
    UPROPERTY()
    bool bShotSpawned;
    
    float Time;
    
    float TimeStamp;
};


UCLASS()
class NZGAME_API ANZCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANZCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    
    virtual void PostInitializeComponents() override;
    
    virtual void Destroyed() override;
    

    /** First person arm mesh */
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    class USkeletalMeshComponent* FirstPersonMesh;
    
    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* CharacterCameraComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
    class UNZCharacterMovementComponent* NZCharacterMovement;
    
    UPROPERTY(BlueprintReadOnly)
    TSubclassOf<class ANZCharacterContent> CharacterData;
    
    UPROPERTY()
    float DefaultBaseEyeHeight;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float DefaultCrouchedEyeHeight;
    
    /**
     * Whether spawn protection may potentially be applied (still must meet time since spawn check in NZGameMode)
     * Set to false after firing weapon or any other action that is considered offensive
     */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Pawn)
    bool bSpawnProtectionEligible;

    /** Returns whether spawn protection currently applies for this character (valid on client) */
    UFUNCTION(BlueprintCallable, Category = Damage)
    bool IsSpawnProtected();
    
    
    
    
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
    
    
    



    /** 1596
     Spawn/Destroy/Replace the current weapon attachment to represent the equipped weapon (through WeaponClass) */
    UFUNCTION()
    virtual void UpdateWeaponAttachment();
    
    /** Spawn/Destroy/Replace the current holstered weapon attachment to represent the equipped weapon (through WeaponClass) */
    UFUNCTION()
    virtual void UpdateHolsteredWeaponAttachment();



    
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

    
    /** Firemodes with input currently being held down (pending or actually firing) */
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
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
    TArray<TSubclassOf<ANZInventory> > DefaultCharacterInventory;

    
    
    
    
    UPROPERTY()
    TArray<FStoredAmmo> SavedAmmo;





	/** Handles moving forward/backward */
	virtual void MoveForward(float Value);

	/** Handles strafing movement, left and right */
	virtual void MoveRight(float Value);

	/** Handles up and down when swimming or flying */
	virtual void MoveUp(float Value);


    UFUNCTION(BlueprintPure, Category = PlayerController)
    virtual APlayerCameraManager* GetPlayerCameraManager();




    
    /** 630
     */
    UPROPERTY(BlueprintReadWrite, Replicated, Category = Pawn)
    int32 Health;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Pawn)
    int32 HealthMax;
    
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
     
    
    /** 985
     Plays death effects; use LastTakeHitInfo to do damage-specific death effects */
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
    
    
    
    /** 1271 */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Effects)
    virtual void SetWeaponAttachmentClass(TSubclassOf<class ANZWeaponAttachment> NewWeaponAttachmentClass);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Effects)
    virtual void SetHolsteredWeaponAttachmentClass(TSubclassOf<class ANZWeaponAttachment> NewWeaponAttachmentClass);
    
    
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
    
    /** */
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector CurrentWeaponBob;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    FVector WeaponBobMagnitude;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponBob)
    FVector WeaponJumpBob;
    
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector DesiredJumpBob;
    
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector CurrentJumpBob;
    
    /** Returns offset to add to first person mesh for weapon bob */
    virtual FVector GetWeaponBobOffset(float DeltaTime, ANZWeapon* MyWeapon);
    
    
    UPROPERTY(BlueprintReadWrite, Category = WeaponBob)
    FVector TargetEyeOffset;
    
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
    
    
};

