// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZDamageType.h"



UNZDamageType::UNZDamageType()
{
    SelfMomentumBoost = 1.f;
    DamageImpulse = 50000.0f;
    DestructibleImpulse = 50000.0f;
    bForceZMomentum = true;
    ForceZMomentumPct = 0.4f;
    bCausesBlood = true;
    bBlockedByArmor = true;
}

void UNZDamageType::ScoreKill_Implementation(ANZPlayerState* KillerState, ANZPlayerState* VictimState, APawn* KilledPawn) const
{
    
}

void UNZDamageType::PlayHitEffects_Implementation(ANZCharacter* HitPawn, bool bPlayedArmorEffect) const
{
/*    if (BodyDamageColor != NULL && (HitPawn->LastTakeHitInfo.Damage > 0 || (HitPawn->LastTakeHitInfo.HitArmor != NULL && !bPlayedArmorEffect)))
    {
        if (HitPawn->LastTakeHitInfo.HitArmor == ANZTimedPowerup::StaticClass())
        {
            HitPawn->SetBodyColorFlash(SuperHealthDamageColor, true);
        }
        else if (bBlockedByArmor && HitPawn->LastTakeHitInfo.HitArmor && ArmorDamageColor)
        {
            HitPawn->SetBodyColorFlash(ArmorDamageColor, true);
        }
        else
        {
            HitPawn->SetBodyColorFlash(BodyDamageColor, bBodyDamageColorRimOnly);
        }
    }*/
}

void UNZDamageType::PlayDeathEffects_Implementation(ANZCharacter* DyingPawn) const
{

}

