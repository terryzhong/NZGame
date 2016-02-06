// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateFiring_Enforcer.h"



void UNZWeaponStateFiring_Enforcer::BeginState(const UNZWeaponState* PrevState)
{
	GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiring_Enforcer::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()), true);
    ToggleLoopingEffects(true);
    GetOuterANZWeapon()->OnStartedFiring();
    FireShot();
}

void UNZWeaponStateFiring_Enforcer::EndState()
{
    ToggleLoopingEffects(false);
    GetOuterANZWeapon()->OnStoppedFiring();
    GetOuterANZWeapon()->StopFiringEffects();
    GetOuterANZWeapon()->GetNZOwner()->ClearFiringInfo();
    GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(RefireCheckHandle);
    GetOuterANZWeapon()->GetWorldTimerManager().ClearTimer(PutDownHandle);
}

void UNZWeaponStateFiring_Enforcer::UpdateTiming()
{
    GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiring_Enforcer::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()), true);
}

void UNZWeaponStateFiring_Enforcer::PutDown()
{
    HandleDelayedShot();
    
    float TimeTillPutDown = GetOuterANZWeapon()->GetWorldTimerManager().GetTimerRemaining(RefireCheckHandle);
    if (TimeTillPutDown <= GetOuterANZWeapon()->GetPutDownTime())
    {
        Super::PutDown();
    }
    else
    {
        TimeTillPutDown -= GetOuterANZWeapon()->GetPutDownTime();
        GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(PutDownHandle, this, &UNZWeaponStateFiring_Enforcer::PutDown, TimeTillPutDown, false);
    }
}
