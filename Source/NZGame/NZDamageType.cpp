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

FVector NZGetDamageMomentum(const FDamageEvent& DamageEvent, const AActor* HitActor, const AController* EventInstigator)
{
	if (DamageEvent.IsOfType(FNZPointDamageEvent::ClassID))
	{
		return ((const FNZPointDamageEvent&)DamageEvent).Momentum;
	}
	else if (DamageEvent.IsOfType(FNZRadialDamageEvent::ClassID))
	{
		const FNZRadialDamageEvent& RadialEvent = (const FNZRadialDamageEvent&)DamageEvent;
		float Magnitude = RadialEvent.BaseMomentumMag;
		// Default to taking the average of all hit components
		if (RadialEvent.ComponentHits.Num() == 0)
		{
			// Don't think this can happen but doesn't hurt to be safe
			return (HitActor->GetActorLocation() - RadialEvent.Origin).GetSafeNormal() * Magnitude;
		}
		// Accommodate origin being same as hit location
		else if (RadialEvent.ComponentHits.Num() == 1 && (RadialEvent.ComponentHits[0].Location - RadialEvent.Origin).IsNearlyZero() && RadialEvent.ComponentHits[0].Component.IsValid())
		{
			if ((RadialEvent.ComponentHits[0].TraceStart - RadialEvent.ComponentHits[0].TraceEnd).IsNearlyZero())
			{
				// 'Fake' hit generated bacause no component trace succeeded even though radius check worked in this case, use direction to component center
				return (RadialEvent.ComponentHits[0].Component->GetComponentLocation() - RadialEvent.Origin).GetSafeNormal() * Magnitude;
			}
			else
			{
				return (RadialEvent.ComponentHits[0].TraceEnd - RadialEvent.ComponentHits[0].TraceStart).GetSafeNormal() * Magnitude;
			}
		}
		else
		{
			FVector Avg(FVector::ZeroVector);
			for (int32 i = 0; i < RadialEvent.ComponentHits.Num(); i++)
			{
				Avg += RadialEvent.ComponentHits[i].Location;
			}

			return (Avg / RadialEvent.ComponentHits.Num() - RadialEvent.Origin).GetSafeNormal() * Magnitude;
		}
	}
	else
	{
		TSubclassOf<UDamageType> DamageType = DamageEvent.DamageTypeClass;
		if (DamageType == NULL)
		{
			DamageType = UNZDamageType::StaticClass();
		}
		FHitResult HitInfo;
		FVector MomentumDir;
		DamageEvent.GetBestHitInfo(HitActor, EventInstigator, HitInfo, MomentumDir);
		return MomentumDir * DamageType.GetDefaultObject()->DamageImpulse;
	}
}



