// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NZGameplayStatics.generated.h"

UENUM()
enum ESoundReplicationType
{
    SRT_All,    // Replicate to all in audible range
    SRT_AllButOwner,    // Replicate to all but the owner of SourceActor
    SRT_IfSourceNotReplicated,  // Only replicate to clients on which SourceActor does not exist
    SRT_None,   // No replication; local only
    SRT_MAX
};

/**
 * 
 */
UCLASS()
class NZGAME_API UNZGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
	
	
};
