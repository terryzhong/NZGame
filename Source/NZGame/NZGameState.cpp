// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameState.h"
#include "NZRemoteRedeemer.h"
#include "NZWeapon.h"


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

FOverlayEffect ANZGameState::GetFirstOverlay(uint16 Flags, bool bFirstPerson)
{
    if (Flags == 0)
    {
        return FOverlayEffect();
    }
    else
    {
        for (int32 i = 0; i < ARRAY_COUNT(OverlayEffects); i++)
        {
            if (Flags & (1 << i))
            {
                return (bFirstPerson && OverlayEffects1P[i].IsValid()) ? OverlayEffects1P[i] : OverlayEffects[i];
            }
        }
        return FOverlayEffect();
    }
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

void ANZGameState::OnRep_OverlayEffects()
{
    
}


