// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZInventory.h"
#include "NZWeapon.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class NZGAME_API ANZWeapon : public ANZInventory
{
	GENERATED_BODY()
    
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
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    TArray<int32> AmmoCost;
    
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    //TArray<TSubclassOf<ANZProjectile> > ProjClass;
    
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
    //TArray<class FInstantHitDamageInfo> InstantHitInfo;
    
    
    
    
    
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
