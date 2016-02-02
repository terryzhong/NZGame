// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZTeamInfo.h"
#include "NZGameState.h"


void ANZTeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

void ANZTeamInfo::ReceivedTeamIndex()
{
    if (!bFromPreviousLevel && TeamIndex != 255)
    {
        ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
        if (GameState != NULL)
        {
            if (GameState->Teams.Num() <= TeamIndex)
            {
                GameState->Teams.SetNum(TeamIndex + 1);
            }
            GameState->Teams[TeamIndex] = this;
        }
    }
}
