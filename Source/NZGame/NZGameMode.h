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
    
	
	
    
    UFUNCTION(BlueprintNativeEvent, Category = Game)
    bool PlayerCanAltRestart(APlayerController* Player);
    
    /** If true, firing weapons costs ammo */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Game)
    bool bAmmoIsLimited;
    
};
