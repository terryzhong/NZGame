// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateFiring.h"



UNZWeaponStateFiring::UNZWeaponStateFiring()
{
    PendingFireSequence = -1;
    bDelayShot = false;
}

bool UNZWeaponStateFiring::IsFiring() const
{
    // Default is we're firing if we're in this state
    return true;
}

bool UNZWeaponStateFiring::WillSpawnShot(float DeltaTime)
{
    return (GetOuterANZWeapon()->GetNZOwner()->IsPendingFire(GetOuterANZWeapon()->GetCurrentFireMode())) &&
        (GetOuterANZWeapon()->GetWorldTimerManager().GetTimerRemaining(RefireCheckHandle) < DeltaTime);
}

void UNZWeaponStateFiring::FireShot()
{
    GetOuterANZWeapon()->FireShot();
}

void UNZWeaponStateFiring::BeginState(const UNZWeaponState* PrevState)
{
    GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiring::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()), true);
    ToggleLoopingEffects(true);
    PendingFireSequence = -1;
    bDelayShot = false;
    GetOuterANZWeapon()->OnStartedFiring();
    FireShot();
    GetOuterANZWeapon()->bNetDelayedShot = false;
}

void UNZWeaponStateFiring::EndState()
{
    bDelayShot = false;
    ToggleLoopingEffects(false);
    GetOuterANZWeapon()->OnStoppedFiring();
    GetOuterANZWeapon()->StopFiringEffects();
    GetOuterANZWeapon()->GetNZOwner()->ClearFiringInfo();
    GetOuterANZWeapon()->GetWorldTimerManager().ClearAllTimersForObject(this);
}

void UNZWeaponStateFiring::UpdateTiming()
{
    // TODO: We should really restart the timer at the percentage it currently is, but FTimerManager has no facility to do this
    GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiring::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()), true);
}

void UNZWeaponStateFiring::RefireCheckTimer()
{
    // Query bot to consider whether to still fire, switch modes, etc
    // todo:
/*    ANZBot* B = Cast<ANZBot>(GetNZOwner()->Controller);
    if (B != NULL)
    {
        B->CheckWeaponFiring();
    }*/
    
    if (GetNZOwner())
    {
        GetOuterANZWeapon()->bNetDelayedShot = (GetNZOwner()->GetNetMode() == NM_DedicatedServer);
        if (PendingFireSequence >= 0)
        {
            bool bClearPendingFire = !GetNZOwner()->IsPendingFire(PendingFireSequence);
            GetNZOwner()->SetPendingFire(PendingFireSequence, true);
            if (GetOuterANZWeapon()->HandleContinuedFiring())
            {
                FireShot();
            }
            if (bClearPendingFire && GetNZOwner() != NULL)
            {
                GetNZOwner()->SetPendingFire(PendingFireSequence, false);
            }
            PendingFireSequence = -1;
        }
        else if (GetOuterANZWeapon()->HandleContinuedFiring())
        {
            bDelayShot = GetOuterANZWeapon()->bNetDelayedShot && !GetNZOwner()->DelayedShotFound() && Cast<APlayerController>(GetNZOwner()->GetController());
            if (bDelayShot)
            {
                FireShot();
            }
        }
    }
    GetOuterANZWeapon()->bNetDelayedShot = false;
}

void UNZWeaponStateFiring::PutDown()
{
    HandleDelayedShot();
    
    // By default, firing states delay put down until the weapon returns to active via player letting go of the trigger, out of ammo, etc
    // However, allow putdown time to overlap with reload time - start a timer to do an early check
    float TimeTillPutDown = GetOuterANZWeapon()->GetWorldTimerManager().GetTimerRemaining(RefireCheckHandle) * GetOuterANZWeapon()->RefirePutDownTimePercent;
    if (!GetOuterANZWeapon()->HasAnyAmmo())
    {
        GetOuterANZWeapon()->EarliestFireTime = GetWorld()->GetTimeSeconds() + GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode());
        Super::PutDown();
    }
    else if (TimeTillPutDown <= GetOuterANZWeapon()->GetPutDownTime())
    {
        GetOuterANZWeapon()->EarliestFireTime = GetWorld()->GetTimeSeconds() + TimeTillPutDown;
        Super::PutDown();
    }
    else
    {
        TimeTillPutDown -= GetOuterANZWeapon()->GetPutDownTime();
        GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(PutDownHandle, this, &UNZWeaponStateFiring::PutDown, TimeTillPutDown, false);
    }
}

void UNZWeaponStateFiring::Tick(float DeltaTime)
{
    HandleDelayedShot();
}

void UNZWeaponStateFiring::HandleDelayedShot()
{
    if (bDelayShot)
    {
        GetOuterANZWeapon()->bNetDelayedShot = true;
        bDelayShot = false;
        FireShot();
        GetOuterANZWeapon()->bNetDelayedShot = false;
    }
}

bool UNZWeaponStateFiring::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
    // On server, might not be quite done reloading yet when client done, so queue firing
    if (bClientFired)
    {
        PendingFireSequence = FireModeNum;
        GetNZOwner()->NotifyPendingServerFire();
    }
    return false;
}

bool UNZWeaponStateFiring::DrawHUD(class UNZHUDWidget* WeaponHudWidget)
{
    return true;
}

void UNZWeaponStateFiring::ToggleLoopingEffects(bool bNowOn)
{
    if (GetOuterANZWeapon()->FireLoopingSound.IsValidIndex(GetFireMode()) && GetOuterANZWeapon()->FireLoopingSound[GetFireMode()] != NULL)
    {
        GetNZOwner()->SetAmbientSound(GetOuterANZWeapon()->FireLoopingSound[GetFireMode()], !bNowOn);
    }
}


