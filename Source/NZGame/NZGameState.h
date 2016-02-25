// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "NZATypes.h"
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

	/** Determines if 2 PlayerState are in score order */
	virtual bool InOrder(class ANZPlayerState* P1, class ANZPlayerState* P2);

	/** Sorts the Player State Array */
	virtual void SortPSArray();

	/** Returns true if the match state is InProgress or later */
	UFUNCTION(BlueprintCallable, Category = GameState)
	virtual bool HasMatchStarted() const;

	UFUNCTION(BlueprintCallable, Category = GameState)
	virtual bool IsMatchInProgress() const;

	UFUNCTION(BlueprintCallable, Category = GameState)
	virtual bool IsMatchInOvertime() const;

	UFUNCTION(BlueprintCallable, Category = GameState)
	virtual bool IsMatchInCountdown() const;

	UFUNCTION(BlueprintCallable, Category = GameState)
	virtual bool IsMatchIntermission() const;
    
    /** 246
     Returns first active overlay material given the passed in flags */
    FOverlayEffect GetFirstOverlay(uint16 Flags, bool bFirstPerson);
    
    /** 271
     * This is called from the NZPlayerCameraManager to allow the game to force an override to the current player camera to make it easier for
     * presentation to be controlled by the server
     */
    virtual FName OverrideCameraStyle(APlayerController* PCOwner, FName CurrentCameraStyle);
    
protected:
    static const uint8 MAX_OVERLAY_MATERIALS = 16;
    
    UPROPERTY(ReplicatedUsing = OnRep_OverlayEffects)
    FOverlayEffect OverlayEffects[MAX_OVERLAY_MATERIALS];
    
    UPROPERTY(ReplicatedUsing = OnRep_OverlayEffects)
    FOverlayEffect OverlayEffects1P[MAX_OVERLAY_MATERIALS];
    
    UFUNCTION()
    virtual void OnRep_OverlayEffects();
};
