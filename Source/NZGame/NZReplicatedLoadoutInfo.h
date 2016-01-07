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
	
	UPROPERTY(Replicated)
    TSubclassOf<ANInventory> ItemClass;
    
    UPROPERTY(Replicated)
    uint8 RoundMask;
    
    UPROPERTY(Replicated)
    uint32 bDefaultInclude:1;
    
    UPROPERTY(Replicated)
    uint32 bPurchaseOnly:1;
	
    UPROPERTY(Replicated)
    float CurrentCost;
	
};
