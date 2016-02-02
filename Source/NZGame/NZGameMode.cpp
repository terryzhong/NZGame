// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameMode.h"
#include "NZHUD.h"
#include "NZGameState.h"
#include "NZPlayerState.h"
#include "NZPlayerController.h"
#include "NZMutator.h"
#include "NZCharacter.h"



ANZGameMode::ANZGameMode()
{
    HUDClass = ANZHUD::StaticClass();
    
    GameStateClass = ANZGameState::StaticClass();
    PlayerStateClass = ANZPlayerState::StaticClass();
    
    
    PlayerControllerClass = ANZPlayerController::StaticClass();
    
}

void ANZGameMode::RestartPlayer(AController* aPlayer)
{
    Super::RestartPlayer(aPlayer);
}


void ANZGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
    Super::SetPlayerDefaults(PlayerPawn);
    
    if (BaseMutator != NULL)
    {
        BaseMutator->ModifyPlayer(PlayerPawn, bSetPlayerDefaultsNewSpawn);
    }
    
    if (bSetPlayerDefaultsNewSpawn)
    {
        GiveDefaultInventory(PlayerPawn);
    }
}

void ANZGameMode::GiveDefaultInventory(APawn* PlayerPawn)
{
    ANZCharacter* NZCharacter = Cast<ANZCharacter>(PlayerPawn);
    if (NZCharacter != NULL)
    {
        if (bClearPlayerInventory)
        {
            NZCharacter->DefaultCharacterInventory.Empty();
        }
        NZCharacter->AddDefaultInventory(DefaultInventory);
    }
}


bool ANZGameMode::PlayerCanAltRestart_Implementation(APlayerController* Player)
{
    return PlayerCanRestart(Player);
}

bool ANZGameMode::OverridePickupQuery_Implementation(APawn* Other, TSubclassOf<ANZInventory> ItemClass, AActor* Pickup, bool& bAllowPickup)
{
    return true;
}
