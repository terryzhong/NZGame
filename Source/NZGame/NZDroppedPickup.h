// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZDroppedPickup.generated.h"

UCLASS()
class NZGAME_API ANZDroppedPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZDroppedPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:
    /** The item that this pickup represents, given to the Pawn that picks us up - SERVER ONLY */
    UPROPERTY(BlueprintReadOnly, Category = Pickup)
    class ANZInventory* Inventory;
    
    /** Same as Inventory->GetClass(), used to replicate visuals to clients */
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = InventoryTypeUpdated, Category = Pickup)
    TSubclassOf<class ANZInventory> InventoryType;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Pickup)
    UMeshComponent* Mesh;
    
    UPROPERTY(BlueprintReadWrite)
    bool bFullyInitialized;
    
    //UPROPERTY(ReplicatedUsing = OnRep_WeaponSkin)
    //UNZWeaponSkin* WeaponSkin;
    
    //UFUNCTION()
    //virtual void OnRep_WeaponSkin();
    
public:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    UCapsuleComponent* Collision;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    class UNZProjectileMovementComponent* Movement;
	
    
    /** 71
     */
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
    
    
    
    
    /** 101
     * Called to re-check overlapping actors, in case one was previously disallowed for some reason that may no longer be true (not fully initialized, obstruction removed, etc)
     */
    UFUNCTION(BlueprintCallable, Category = Pickup)
    virtual void CheckTouching();
};
