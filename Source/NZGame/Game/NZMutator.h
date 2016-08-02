// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "NZMutator.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZMutator : public AInfo
{
	GENERATED_BODY()
	
public:
    /** Next in linked list of mutators */
    UPROPERTY(BlueprintReadOnly, Category = Mutator)
    ANZMutator* NextMutator;
    
    /** 91
     Called when a player pawn is spawned and to reset its parameters (like movement speed and such) */
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void ModifyPlayer(APawn* Other, bool bIsNewSpawn);
	
    
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void ModifyInventory(ANZInventory* Inv, ANZCharacter* NewOwner);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddDefaultInventory(TSubclassOf<ANZInventory> InventoryClass);

	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
	bool ModifyDamage(UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, APawn* Injured, AController* InstigatedBy, const FHitResult& HitInfo, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);

    /** Return true to prevent the passed in pawn from dying (i.e. from Died()) */
    UFUNCTION(BlueprintNativeEvent)
    bool PreventDeath(APawn* KilledPawn, AController* Killer, TSubclassOf<UDamageType> DamageType, const FHitResult& HitInfo);

};
