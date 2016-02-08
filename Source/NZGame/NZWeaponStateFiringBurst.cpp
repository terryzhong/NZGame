// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateFiringBurst.h"



UNZWeaponStateFiringBurst::UNZWeaponStateFiringBurst()
{
    BurstSize = 3;
    BurstInterval = 0.15f;
    SpreadIncrease = 0.06f;
}

void UNZWeaponStateFiringBurst::BeginState(const UNZWeaponState* PrevState)
{
    CurrentShot = 0;
    if (GetOuterANZWeapon()->Spread.IsValidIndex(GetOuterANZWeapon()->GetCurrentFireMode()))
    {
        GetOuterANZWeapon()->Spread[GetOuterANZWeapon()->GetCurrentFireMode()] = 0.f;
    }
    ShotTimeRemaining = -0.001f;
    RefireCheckTimer();
    GetOuterANZWeapon()->OnStartedFiring();
}

void UNZWeaponStateFiringBurst::UpdateTiming()
{
    // unnecessary since we're manually incrementing
}

void UNZWeaponStateFiringBurst::RefireCheckTimer()
{
    uint8 CurrentFireMode = GetOuterANZWeapon()->GetCurrentFireMode();
    if (GetOuterANZWeapon()->GetNZOwner()->GetPendingWeapon() != NULL || !GetOuterANZWeapon()->HasAmmo(CurrentFireMode))
    {
        GetOuterANZWeapon()->GotoActiveState();
    }
    else if ((CurrentShot < BurstSize) || GetOuterANZWeapon()->GetNZOwner()->IsPendingFire(CurrentFireMode))
    {
        if (CurrentShot == BurstSize)
        {
            CurrentShot = 0;
            if (GetOuterANZWeapon()->Spread.IsValidIndex(CurrentFireMode))
            {
                GetOuterANZWeapon()->Spread[CurrentFireMode] = 0.f;
            }
        }
        GetOuterANZWeapon()->OnContinuedFiring();
        FireShot();
        CurrentShot++;
        if (GetOuterANZWeapon()->Spread.IsValidIndex(CurrentFireMode))
        {
            GetOuterANZWeapon()->Spread[CurrentFireMode] += SpreadIncrease;
        }
        IncrementShotTimer();
    }
    else
    {
        GetOuterANZWeapon()->GotoActiveState();
    }
}

void UNZWeaponStateFiringBurst::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    ShotTimeRemaining -= DeltaTime * GetNZOwner()->GetFireRateMultiplier();
    if (ShotTimeRemaining <= 0.0f)
    {
        RefireCheckTimer();
    }
}

void UNZWeaponStateFiringBurst::IncrementShotTimer()
{
    ShotTimeRemaining += (CurrentShot < BurstSize) ? BurstInterval : FMath::Max(0.01f, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()) - (BurstSize - 1) * BurstInterval);
}

void UNZWeaponStateFiringBurst::PutDown()
{
    if ((CurrentShot == BurstSize) &&
        (ShotTimeRemaining < FMath::Max(0.01f, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()) - BurstSize * BurstInterval)))
    {
        GetOuterANZWeapon()->UnEquip();
    }
}
