// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGun.h"
#include "NZGunStateChangeClip.h"
#include "NZGunViewKickComponent.h"


ANZGun::ANZGun()
{
    ChangeClipState = CreateDefaultSubobject<UNZGunStateChangeClip>(TEXT("StateChangeClip"));
    
    ViewKickComponent = CreateDefaultSubobject<UNZGunViewKickComponent>(TEXT("ViewKickComponent"));
}

void ANZGun::ChangeClip()
{
    GotoState(ChangeClipState);
}

bool ANZGun::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
    return Super::BeginFiringSequence(FireModeNum, bClientFired);
}

void ANZGun::EndFiringSequence(uint8 FireModeNum)
{
    bIsContinousFire = false;
    
    Super::EndFiringSequence(FireModeNum);
}

void ANZGun::FireShot()
{
    Super::FireShot();
}


bool ANZGun::HandleContinuedFiring()
{
    bIsContinousFire = true;
    
    return Super::HandleContinuedFiring();
}
