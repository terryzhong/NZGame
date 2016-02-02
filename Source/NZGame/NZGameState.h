// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "NZGameState.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZGameState : public AGameState
{
	GENERATED_BODY()
	
    
public:
    /** 38
     Teams, if the game type has them */
    UPROPERTY(BlueprintReadOnly, Category = GameState)
    TArray<ANZTeamInfo*> Teams;
    
	
	
    /** 77
     Amount of time after a player spawns where they are immune to damage from enemies */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = GameState)
    float SpawnProtectionTime;

    
    /** 168
     Determines if a player is on the same team */
    UFUNCTION(BlueprintCallable, Category = GameState)
    virtual bool OnSameTeam(const AActor* Actor1, const AActor* Actor2);
    
    /** 271
     * This is called from the NZPlayerCameraManager to allow the game to force an override to the current player camera to make it easier for
     * presentation to be controlled by the server
     */
    virtual FName OverrideCameraStyle(APlayerController* PCOwner, FName CurrentCameraStyle);
};
