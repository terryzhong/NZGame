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
	PlayerPawnObject = FStringAssetReference(TEXT("/Game/Blueprints/Nicholas.Nicholas_C"));

    //HUDClass = ANZHUD::StaticClass();
    
    GameStateClass = ANZGameState::StaticClass();
    PlayerStateClass = ANZPlayerState::StaticClass();
    
    PlayerControllerClass = ANZPlayerController::StaticClass();
    
}

void ANZGameMode::RestartPlayer(AController* aPlayer)
{
	if ((aPlayer == NULL) || (aPlayer->PlayerState == NULL) || aPlayer->PlayerState->PlayerName.IsEmpty())
	{
		//UE_LOG(NZ, Warning, TEXT("RestartPlayer with a bad player, bad playerstate, or empty player name"));
		return;
	}

	if (!IsMatchInProgress() || aPlayer->PlayerState->bOnlySpectator)
	{
		return;
	}

	{
		TGuardValue<bool> FlagGuard(bSetPlayerDefaultsNewSpawn, true);
		Super::RestartPlayer(aPlayer);

		ANZCharacter* NZC = Cast<ANZCharacter>(aPlayer->GetPawn());
		if (NZC != NULL && NZC->GetClass()->GetDefaultObject<ANZCharacter>()->Health == 0)
		{
			NZC->Health = NZC->HealthMax;
		}
	}

	// todo:

	if (Cast<ANZPlayerController>(aPlayer) != NULL)
	{
		((APlayerController*)aPlayer)->ClientForceGarbageCollection();
		if (!aPlayer->IsLocalController())
		{
			((ANZPlayerController*)aPlayer)->ClientSwitchToBestWeapon();
		}
	}

	// Clear spawn choices
	//Cast<ANZPlayerState>(aPlayer->PlayerState)->RespawnChoiceA = NULL;
	//Cast<ANZPlayerState>(aPlayer->PlayerState)->RespawnChoiceB = NULL;

	// todo:
	//Cast<ANZPlayerState>(aPlayer->PlayerState)->LastKillTime = -100.f;
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
