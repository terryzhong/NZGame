// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameState.h"
#include "NZRemoteRedeemer.h"


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

FName ANZGameState::OverrideCameraStyle(APlayerController* PCOwner, FName CurrentCameraStyle)
{
    if (HasMatchEnded())
    {
        return FName(TEXT("FreeCam"));
    }
    // FIXME: Shouldn't this come from the Pawn?
    else if (Cast<ANZRemoteRedeemer>(PCOwner->GetPawn()) != NULL)
    {
        return FName(TEXT("FirstPerson"));
    }
    else
    {
        return CurrentCameraStyle;
    }
}
