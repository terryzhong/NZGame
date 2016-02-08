// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateFiringBurstEnforcer.h"



UNZWeaponStateFiringBurstEnforcer::UNZWeaponStateFiringBurstEnforcer()
{
    BurstSize = 3;
    BurstInterval = 0.15f;
    SpreadIncrease = 0.06f;
    LastFiredTime = 0.0f;
    
    bFirstVolleyComplete = false;
    bSecondVolleyComplete = false;
}

void UNZWeaponStateFiringBurstEnforcer::BeginState(const UNZWeaponState* PrevState)
{
    if (GetRemainingCooldownTime() == 0.0f || (Cast<ANZWeapon_Enforcer>(GetOuterANZWeapon())->bDualEnforcerMode && !bSecondVolleyComplete))
    {
        if (GetRemainingCooldownTime() == 0.0f)
        {
            bFirstVolleyComplete = false;
            bSecondVolleyComplete = false;
        }
        
        CurrentShot = 0;
        GetOuterANZWeapon()->Spread[GetOuterANZWeapon()->GetCurrentFireMode()] = 0.f;
        RefireCheckTimer();
        GetOuterANZWeapon()->OnStartedFiring();
    }
    else
    {
        // Set the timer to the proper refire time
        GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiringBurstEnforcer::RefireCheckTimer, GetRemainingCooldownTime(), true);
    }
}

void UNZWeaponStateFiringBurstEnforcer::EndState()
{
    GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(RefireCheckHandle);
    GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(PutDownHandle);
    Super::EndState();
}

void UNZWeaponStateFiringBurstEnforcer::IncrementShotTimer()
{
    if ((!Cast<ANZWeapon_Enforcer>(GetOuterANZWeapon())->bDualEnforcerMode && (CurrentShot == BurstSize)) ||
        (Cast<ANZWeapon_Enforcer>(GetOuterANZWeapon())->bDualEnforcerMode && (CurrentShot == BurstSize) && bFirstVolleyComplete))
    {
        GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiringBurstEnforcer::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()) - (BurstSize - 1) * BurstInterval / GetNZOwner()->GetFireRateMultiplier(), true);
    }
    else
    {
        GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiringBurstEnforcer::RefireCheckTimer, BurstInterval / GetNZOwner()->GetFireRateMultiplier(), true);
    }
}

void UNZWeaponStateFiringBurstEnforcer::UpdateTiming()
{
    // TODO: We should really restart the timer at the percentage it currently is, but FTimerManager has no facility to do this
    GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiringBurstEnforcer::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()) - (BurstSize - 1) * BurstInterval / GetNZOwner()->GetFireRateMultiplier(), true);
}

void UNZWeaponStateFiringBurstEnforcer::RefireCheckTimer()
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
            if (!bFirstVolleyComplete)
            {
                bFirstVolleyComplete = true;
            }
            else
            {
                bSecondVolleyComplete = true;
            }
            
            CurrentShot = 0;
            
            if (GetOuterANZWeapon()->Spread.IsValidIndex(CurrentFireMode))
            {
                GetOuterANZWeapon()->Spread[CurrentFireMode] = 0.f;
            }
        }
        
        bool bDualMode = Cast<ANZWeapon_Enforcer>(GetOuterANZWeapon())->bDualEnforcerMode;
        
        if ((bDualMode && !bSecondVolleyComplete) || (!bDualMode && !bFirstVolleyComplete) || GetRemainingCooldownTime() == 0.0f)
        {
            GetOuterANZWeapon()->OnContinuedFiring();
            
            FireShot();
            LastFiredTime = GetOuterANZWeapon()->GetWorld()->GetTimeSeconds();
            if (GetNZOwner() != NULL)
            {
                CurrentShot++;
                if (GetOuterANZWeapon()->Spread.IsValidIndex(CurrentFireMode))
                {
                    GetOuterANZWeapon()->Spread[CurrentFireMode] += SpreadIncrease;
                }
                IncrementShotTimer();
                
                if ((bDualMode && bSecondVolleyComplete) || (!bDualMode && bFirstVolleyComplete))
                {
                    bFirstVolleyComplete = false;
                    bSecondVolleyComplete = false;
                }
            }
        }
        else if (!GetOuterANZWeapon()->GetNZOwner()->IsPendingFire(CurrentFireMode))
        {
            GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiringBurstEnforcer::RefireCheckTimer, GetRemainingCooldownTime(), true);
        }
        else
        {
            GetOuterANZWeapon()->GotoActiveState();
        }
    }
    else
    {
        if (!bFirstVolleyComplete)
        {
            bFirstVolleyComplete = true;
        }
        else
        {
            bSecondVolleyComplete = true;
        }
        GetOuterANZWeapon()->GotoActiveState();
    }
}

void UNZWeaponStateFiringBurstEnforcer::PutDown()
{
    HandleDelayedShot();
    if ((CurrentShot == BurstSize) && GetRemainingCooldownTime() == 0.0f)
    {
        GetOuterANZWeapon()->UnEquip();
    }
}

float UNZWeaponStateFiringBurstEnforcer::GetRemainingCooldownTime()
{
    float CooldownTime = GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()) - (BurstSize - 1) * BurstInterval / GetNZOwner()->GetFireRateMultiplier();
    return FMath::Max<float>(0.0f, (CooldownTime + LastFiredTime) - GetOuterANZWeapon()->GetWorld()->GetTimeSeconds());
}

void UNZWeaponStateFiringBurstEnforcer::ResetTiming()
{
    // Reset the timer if the time remaining on it is greater than the new FireRate
    if (GetOuterANZWeapon()->GetNZOwner()->GetPendingWeapon() == NULL && GetOuterANZWeapon()->HasAmmo(GetOuterANZWeapon()->GetCurrentFireMode()))
    {
        if (GetWorld()->GetTimeSeconds() - LastFiredTime >= BurstInterval / GetNZOwner()->GetFireRateMultiplier())
        {
            RefireCheckTimer();
        }
    }
}
