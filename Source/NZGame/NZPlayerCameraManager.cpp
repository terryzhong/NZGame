// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerCameraManager.h"




FName ANZPlayerCameraManager::GetCameraStyleWithOverrides() const
{
    static const FName NAME_FreeCam = FName(TEXT("FreeCam"));
    static const FName NAME_FirstPerson = FName(TEXT("FirstPerson"));
    static const FName NAME_Default = FName(TEXT("Default"));
    
    AActor* ViewTarget = GetViewTarget();
    ACameraActor* CameraActor = Cast<ACameraActor>(ViewTarget);
    if (CameraActor)
    {
        return NAME_Default;
    }
    
    ANZCharacter* NZCharacter = Cast<ANZCharacter>(ViewTarget);
    if (NZCharacter == NULL)
    {
        return ((ViewTarget == PCOwner->GetPawn()) || (ViewTarget == PCOwner->GetSpectatorPawn())) ? NAME_FirstPerson : NAME_FreeCam;
    }
    else if (NZCharacter->IsDead() || NZCharacter->IsRagdoll() || NZCharacter->EmoteCount > 0)
    {
        // Force third person if target is dead, ragdoll or emoting
        return NAME_FreeCam;
    }
    
    ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
    return (GameState != NULL) ? GameState->OverrideCameraStyle(PCOwner, CameraStyle) : CameraStyle;
}

