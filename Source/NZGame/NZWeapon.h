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
