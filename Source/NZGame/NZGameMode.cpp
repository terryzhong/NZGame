// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameMode.h"
#include "NZHUD.h"
#include "NZGameState.h"
#include "NZPlayerState.h"
#include "NZPlayerController.h"



ANZGameMode::ANZGameMode()
{
    HUDClass = ANZHUD::StaticClass();
    
    GameStateClass = ANZGameState::StaticClass();
    PlayerStateClass = ANZPlayerState::StaticClass();
    
    
    PlayerControllerClass = ANZPlayerController::StaticClass();
    
}

bool ANZGameMode::PlayerCanAltRestart_Implementation(APlayerController* Player)
{
    return PlayerCanRestart(Player);
}
