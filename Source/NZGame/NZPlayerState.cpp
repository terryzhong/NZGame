// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerState.h"



void ANZPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ANZPlayerState::SetCharacter(const FString& CharacterPath)
{
    
}

void ANZPlayerState::NotifyTeamChanged_Implementation()
{

}

float ANZPlayerState::GetStatsValue(FName StatsName) const
{
    return StatsData.FindRef(StatsName);
}

void ANZPlayerState::SetStatsValue(FName StatsName, float NewValue)
{
    LastScoreStatsUpdateTime = GetWorld()->GetTimeSeconds();
    StatsData.Add(StatsName, NewValue);
}

void ANZPlayerState::ModifyStatsValue(FName StatsName, float Change)
{
    LastScoreStatsUpdateTime = GetWorld()->GetTimeSeconds();
    float CurrentValue = StatsData.FindRef(StatsName);
    StatsData.Add(StatsName, CurrentValue + Change);
}

