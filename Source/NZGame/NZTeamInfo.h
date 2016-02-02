// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "NZTeamInfo.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZTeamInfo : public AInfo
{
	GENERATED_BODY()
	
public:
    /** Team ID, set by NZTeamGameMode */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = ReceivedTeamIndex, Category = Team)
    uint8 TeamIndex;
    
    /** 82
     Internal flag used to prevent the wrong TeamInfo from getting hooked up on clients during seamless travel, because it is possible for two sets to be on the client temporarily */
    UPROPERTY(Replicated)
    bool bFromPreviousLevel;
    
    /** 112 */
    virtual uint8 GetTeamNum() const //override
    {
        return TeamIndex;
    }
	
    /** 121 */
    UFUNCTION()
    virtual void ReceivedTeamIndex();
};
