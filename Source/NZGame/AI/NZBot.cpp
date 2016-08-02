// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZBot.h"



void ANZBot::SwitchToBestWeapon()
{

}

void ANZBot::ReceiveInstantWarning(ANZCharacter* Shooter, const FVector& FireDir)
{

}

void ANZBot::SeePawn(APawn* Other)
{
    
}

void ANZBot::HearSound(APawn* Other, const FVector& SoundLoc, float SoundRadius)
{
    
}

void ANZBot::NotifyTakeHit_Implementation(AController* InstigatedBy, int32 Damage, FVector Momentum, const FDamageEvent& DamageEvent)
{
	Super::NotifyTakeHit_Implementation(InstigatedBy, Damage, Momentum, DamageEvent);
}

void ANZBot::NotifyCausedHit_Implementation(APawn* HitPawn, int32 Damage)
{
	Super::NotifyCausedHit_Implementation(HitPawn, Damage);
}
