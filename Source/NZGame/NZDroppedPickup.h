// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZDroppedPickup.generated.h"

/**
 * A dropped, partially used inventory item that was previously held by a player
 * Note that this is NOT a subclass NZPickup
 */
UCLASS(NotPlaceable)
class NZGAME_API ANZDroppedPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZDroppedPickup();

protected:
    /** The item that this pickup represents, given to the Pawn that picks us up - SERVER ONLY */
    UPROPERTY(BlueprintReadOnly, Category = Pickup)
    class ANZInventory* Inventory;
    
    /** Same as Inventory->GetClass(), used to replicate visuals to clients */
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = InventoryTypeUpdated, Category = Pickup)
    TSubclassOf<class ANZInventory> InventoryType;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Pickup)
    UMeshComponent* Mesh;
    
    /** Set after all needed properties are set for this pickup to grant its item (e.g. SetInventoryType() called in the case of the default implementation, may vary for subclasses) */
    UPROPERTY(BlueprintReadWrite)
    bool bFullyInitialized;
    
    //UPROPERTY(ReplicatedUsing = OnRep_WeaponSkin)
    //UNZWeaponSkin* WeaponSkin;
    
    //UFUNCTION()
    //virtual void OnRep_WeaponSkin();
    
public:
    //inline void SetWeaponSkin(UNZWeaponSkin* InWeaponSkin)
    //{
    //    WeaponSkin = InWeaponSkin;
    //    OnRepWeaponSkin();
    //}
    
    inline const UMeshComponent* GetMesh() const
    {
        return Mesh;
    }
    
    inline TSubclassOf<ANZInventory> GetInventoryType() const
    {
        return InventoryType;
    }
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    UCapsuleComponent* Collision;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    class UNZProjectileMovementComponent* Movement;
	
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
    //virtual void Reset_Implementation() override;

    UFUNCTION(BlueprintNativeEvent)
    class USoundBase* GetPickupSound() const;
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
    virtual void SetInventory(class ANZInventory* NewInventory);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
    void InventoryTypeUpdated();
    
    UFUNCTION()
    virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    UFUNCTION()
    virtual void PhysicsStopped(const FHitResult& ImpactResult);
    
    UFUNCTION(BlueprintNativeEvent)
    bool AllowPickupBy(APawn* Other, bool bDefaultAllowPickup);
    
    UFUNCTION(BlueprintNativeEvent)
    void ProcessTouch(APawn* TouchedBy);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void GiveTo(APawn* Target);
    
    /** 
     * Plays effects/audio for the pickup being taken
     * NOTE: Only called on server, so you need to make sure anything triggered here can replicate to clients
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category = Pickup)
    void PlayTakenEffects(APawn* TakenBy);
    
    FTimerHandle EnableInstigatorTouchHandle;
    
    /** Defer Instigator picking the item back up */
    UFUNCTION()
    void EnableInstigatorTouch();

    /** Called to re-check overlapping actors, in case one was previously disallowed for some reason that may no longer be true (not fully initialized, obstruction removed, etc) */
    UFUNCTION(BlueprintCallable, Category = Pickup)
    virtual void CheckTouching();
    
    virtual void Tick(float DeltaTime);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AI)
    float BotDesireability(APawn* Asker, float PathDistance);
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AI)
    float DetourWeight(APawn* Asker, float PathDistance);
};
