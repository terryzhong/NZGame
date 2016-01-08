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
