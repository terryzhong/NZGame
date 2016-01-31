// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameState.h"


void ANZGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

bool ANZGameState::OnSameTeam(const AActor* Actor1, const AActor* Actor2)
{
    // todo:
    check(false);
    return false;
}
