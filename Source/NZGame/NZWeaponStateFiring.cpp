// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateFiring.h"



void UNZWeaponStateFiring::RefireCheckTimer()
{

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

