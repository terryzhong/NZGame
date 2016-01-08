// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "NZReplicatedLoadoutInfo.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZReplicatedLoadoutInfo : public AInfo
{
	GENERATED_BODY()
	
	// The weapon
	UPROPERTY(Replicated)
    TSubclassOf<class ANZInventory> ItemClass;
    
	// What rounds are this weapon avaliable in
    UPROPERTY(Replicated)
    uint8 RoundMask;
    
    UPROPERTY(Replicated)
    uint32 bDefaultInclude:1;
    
    UPROPERTY(Replicated)
    uint32 bPurchaseOnly:1;
	
	// What is the current cost of this weapon
    UPROPERTY(Replicated)
    float CurrentCost;
};
