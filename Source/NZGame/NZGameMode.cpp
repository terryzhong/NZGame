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
	PlayerPawnObject = FStringAssetReference(TEXT("/Game/Characters/Default/BP_NZCharacter_Default.BP_NZCharacter_Default_C"));

    HUDClass = ANZHUD::StaticClass();
    
    GameStateClass = ANZGameState::StaticClass();
    PlayerStateClass = ANZPlayerState::StaticClass();
    
    PlayerControllerClass = ANZPlayerController::StaticClass();
    
    bDelayedStart = true;
}

void ANZGameMode::InitGameState()
{
    Super::InitGameState();
    
    NZGameState = Cast<ANZGameState>(GameState);
    if (NZGameState != NULL)
    {
        // todo:
    }
}

bool ANZGameMode::IsEnemy(class AController* First, class AController* Second)
{
    return First && Second && !NZGameState->OnSameTeam(First, Second);
}

void ANZGameMode::Killed(class AController* Killer, class AController* KilledPlayer, class APawn* KilledPawn, TSubclassOf<UDamageType> DamageType)
{
    if ((GetMatchState() != MatchState::MatchEnteringOvertime) && (GetMatchState() != MatchState::WaitingPostMatch) && (GetMatchState() != MatchState::MapVoteHappening))
    {
        ANZPlayerState* const KillerPlayerState = Killer ? Cast<ANZPlayerState>(Killer->PlayerState) : NULL;
        ANZPlayerState* const KilledPlayerState = KilledPlayer ? Cast<ANZPlayerState>(KilledPlayer->PlayerState) : NULL;
        
        if (KilledPlayerState != NULL)
        {
            bool const bEnemyKill = IsEnemy(Killer, KilledPlayer);
            KilledPlayerState->LastKillerPlayerState = KillerPlayerState;
            KilledPlayerState->IncrementDeaths(DamageType, KillerPlayerState);
            TSubclassOf<UNZDamageType> NZDamage(*DamageType);
            if (NZDamage && bEnemyKill)
            {
                NZDamage.GetDefaultObject()->ScoreKill(KillerPlayerState, KilledPlayerState, KilledPawn);
            }
            
            
        }
    }
}


void ANZGameMode::ScoreDamage_Implementation(int32 DamageAmount, AController* Victim, AController* Attacker)
{
    
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

bool ANZGameMode::ModifyDamage_Implementation(UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, APawn* Injured, AController* InstigatedBy, const FHitResult& HitInfo, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	ANZCharacter* InjuredChar = Cast<ANZCharacter>(Injured);
	if (InjuredChar != NULL && InjuredChar->bSpawnProtectionEligible && InstigatedBy != NULL && InstigatedBy != Injured->Controller && GetWorld()->TimeSeconds - Injured->CreationTime < NZGameState->SpawnProtectionTime)
	{
		Damage = 0;
	}

	if (BaseMutator != NULL)
	{
		BaseMutator->ModifyDamage(Damage, Momentum, Injured, InstigatedBy, HitInfo, DamageCauser, DamageType);
	}

	return true;
}

bool ANZGameMode::PreventDeath_Implementation(APawn* KilledPawn, AController* Killer, TSubclassOf<UDamageType> DamageType, const FHitResult& HitInfo)
{
    return (BaseMutator != NULL && BaseMutator->PreventDeath(KilledPawn, Killer, DamageType, HitInfo));
}




bool ANZGameMode::OverridePickupQuery_Implementation(APawn* Other, TSubclassOf<ANZInventory> ItemClass, AActor* Pickup, bool& bAllowPickup)
{
    return true;
}

bool ANZGameMode::PlayerCanAltRestart_Implementation(APlayerController* Player)
{
    return PlayerCanRestart(Player);
}

void ANZGameMode::NotifySpeedHack(ACharacter* Character)
{
    ANZPlayerController* PC = Character ? Cast<ANZPlayerController>(Character->GetController()) : NULL;
    if (PC)
    {
        PC->ClientReceiveLocalizedMessage(GameMessageClass, 15);
    }
}

