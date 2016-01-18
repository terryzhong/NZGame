// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "NZPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = NotifyTeamChanged, Category = PlayerState)
    TSubclassOf<class ANZCharacterContent> SelectedCharacter;
    
    UPROPERTY()
    class ANZCharacter* CachedCharacter;
    
public:
    UFUNCTION(BLueprintCallable, Category = Character)
    virtual void SetCharacter(const FString& CharacterPath);
    
    /**
     * ID that can be used to consistently identify this player for spectating commands
     * IDs are reused when players leave and new ones join, but a given player's ID remains stable and unique as long as that player is in the game
     * NOTE: 1-based, zero is unassigned (bOnlySpectator spectators don't get a value)
     */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = PlayerState)
    uint8 SpectatingID;
    
    
    
    
	UFUNCTION(BlueprintNativeEvent)
	void NotifyTeamChanged();
    
    /**
     * How long until this player can respawn.
     * It's not directly replicated to the clients instead it's set locally via OnDeathsReceived.
     * It will be set to the value of 'GameState.RespawnWaitTime'
     */
    UPROPERTY(BlueprintReadWrite, Category = PlayerState)
    float RespawnTime;
    
    UPROPERTY(BlueprintReadWrite, Category = PlayerState)
    float ForceRespawnTime;
    
    UPROPERTY(replicated)
    bool bChosePrimaryRespawnChoice;
    
    UPROPERTY(replicated)
    class APlayerState* RespawnChoiceA;
    
    UPROPERTY(replicated)
    class APlayerState* RespawnChoiceB;
	
	
public:
    UPROPERTY(Replicated)
    TArray<class ANZReplicatedLoadoutInfo*> Loadout;

};
