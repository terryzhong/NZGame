// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerState.h"
#include "NZPlayerController.h"



void ANZPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ANZPlayerState::SetCharacter(const FString& CharacterPath)
{
    // todo:
	check(false);
}

void ANZPlayerState::ServerSetCharacter_Implementation(const FString& CharacterPath)
{
	// todo:
	check(false);
}

bool ANZPlayerState::ServerSetCharacter_Validate(const FString& CharacterPath)
{
	return true;
}

void ANZPlayerState::UpdatePing(float InPing)
{

}

void ANZPlayerState::CalculatePing(float NewPing)
{
	if (NewPing < 0.f)
	{
		// Caused by timestamp wrap around
		return;
	}

	float OldPing = ExactPing;
	Super::UpdatePing(NewPing);

	ANZPlayerController* PC = Cast<ANZPlayerController>(GetOwner());
	if (PC)
	{
		PC->LastPingCalcTime = GetWorld()->GetTimeSeconds();
		if (ExactPing != OldPing)
		{
			PC->ServerUpdatePing(ExactPing);
		}
	}
}


void ANZPlayerState::NotifyTeamChanged_Implementation()
{
	// todo:
	check(false);
}

void ANZPlayerState::OnRep_Deaths()
{
	// todo:
	check(false);
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


float ANZPlayerState::GetAvailableCurrency()
{
    return AvailableCurrency;
}

void ANZPlayerState::AdjustCurrency(float Adjustment)
{
    AvailableCurrency += Adjustment;
    if (AvailableCurrency < 0.0)
    {
        AvailableCurrency = 0.0f;
    }
}

