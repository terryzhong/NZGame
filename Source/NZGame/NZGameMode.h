// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZBaseGameMode.h"
#include "NZGameMode.generated.h"


namespace MatchState
{
    const FName PlayerIntro = FName(TEXT("PlayerIntro"));
    const FName CountdownToBegin = FName(TEXT("CountdownToBegin"));
    const FName MatchEnteringOvertime = FName(TEXT("MatchEnteringOvertime"));
    const FName MatchIsInOvertime = FName(TEXT("MatchIsInOvertime"));
    const FName MapVoteHappening = FName(TEXT("MapVoteHappening"));
    const FName MatchIntermission = FName(TEXT("MatchIntermission"));
    const FName MatchExitingIntermission = FName(TEXT("MatchExitingIntermission"));
}

/**
 * 
 */
UCLASS()
class NZGAME_API ANZGameMode : public ANZBaseGameMode
{
	GENERATED_BODY()
	
public:
    ANZGameMode();
    
	
    /** 226
     Remove all items from character inventory list, before giving him game mode's default inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool bClearPlayerInventory;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    TArray<TSubclassOf<class ANZInventory> > DefaultInventory;
    
    /** 237
     If true, firing weapons costs ammo */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Game)
    bool bAmmoIsLimited;
    
    /** First mutator; Mutators are a linked list */
    UPROPERTY(BlueprintReadOnly, Category = Mutator)
    class ANZMutator* BaseMutator;
    
    /** 346
     Workaround for call chain from engine, SetPlayerDefaults() could be called while pawn is alive to reset its values but we don't want it to do new spawn stuff like spawning inventory unless it's called from RestartPlayer() */
    UPROPERTY(Transient, BlueprintReadOnly)
    bool bSetPlayerDefaultsNewSpawn;
    
    /** 416 */
    virtual void RestartPlayer(AController* aPlayer);
    
    /** 418 */
    virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
    
    /** 421 */
    virtual void GiveDefaultInventory(APawn* PlayerPawn);
    
    
    /** 606
     * When pawn wants to pickup something, mutators are given a chance to modify it.
     * If this function returns true, bAllowPickup will determine if the object can be picked up.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    bool OverridePickupQuery(APawn* Other, TSubclassOf<ANZInventory> ItemClass, AActor* Pickup, bool& bAllowPickup);
    
    
    /** 709
     Called when the player attempts to restart using AltFire */
    UFUNCTION(BlueprintNativeEvent, Category = Game)
    bool PlayerCanAltRestart(APlayerController* Player);
    
};
