// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "NZTeamInterface.h"
#include "NZPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZPlayerState : public APlayerState, public INZTeamInterface
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


	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = Character)
	void ServerSetCharacter(const FString& CharacterPath);
	inline TSubclassOf<class ANZCharacterContent> GetSelectedCharacter() const { return SelectedCharacter; }

	/** Don't do engine style ping updating */
	virtual void UpdatePing(float InPing) override;

	/** Called on client using the roundtrip time for servermove/ack */
	virtual void CalculatePing(float NewPing);
    
    /**
     * ID that can be used to consistently identify this player for spectating commands
     * IDs are reused when players leave and new ones join, but a given player's ID remains stable and unique as long as that player is in the game
     * NOTE: 1-based, zero is unassigned (bOnlySpectator spectators don't get a value)
     */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = PlayerState)
    uint8 SpectatingID;
    
	UPROPERTY(BlueprintReadOnly, Replicated, Category = PlayerState)
	uint8 SpectatingIDTeam;
    
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = NotifyTeamChanged, Category = PlayerState)
    class ANZTeamInfo* Team;
    
    UPROPERTY(BlueprintReadOnly, Replicated, Category = PlayerState)
    uint32 bWaitingPlayer : 1;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = PlayerState)
	uint32 bReadyToPlay : 1;



	UPROPERTY(BlueprintReadWrite, Category = PlayerState)
	float LastKillTime;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = PlayerState)
	int32 Kills;
    
	UPROPERTY(BlueprintReadWrite, Replicated, Category = PlayerState)
	int32 DamageDone;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_Deaths, Category = PlayerState)
	int32 Deaths;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = PlayerState)
	ANZPlayerState* LastKillerPlayerState;
    
    
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
	
	
	UFUNCTION()
	void OnRep_Deaths();

	/** 485
	 * Returns the team number of the team that owns this object
	 */
	UFUNCTION()
	virtual uint8 GetTeamNum() const;

	virtual void SetTeamForSideSwap_Implementation(uint8 NewTeamNum) override {}
    
private:
    /** 633
     Map of additional stats used for scoring display */
    TMap<FName, float> StatsData;
    
public:
    /** Last time StatsData was updated - used when replicating the data */
    UPROPERTY()
    float LastScoreStatsUpdateTime;
    
    /** Accessors for StatsData */
    float GetStatsValue(FName StatsName) const;
    void SetStatsValue(FName StatsName, float NewValue);
    void ModifyStatsValue(FName StatsName, float Change);
    
    
protected:
    UPROPERTY(Replicated)
    float AvailableCurrency;
    
public:
    UPROPERTY(Replicated)
    TArray<class ANZReplicatedLoadoutInfo*> Loadout;
    

    virtual float GetAvailableCurrency();
    
    virtual void AdjustCurrency(float Adjustment);
    
    

};
