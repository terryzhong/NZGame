// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGun.h"
#include "NZGunStateActive.h"
#include "NZGunStateChangeClip.h"
#include "NZGunViewKickComponent.h"


ANZGun::ANZGun(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UNZGunStateActive>(TEXT("StateActive")))
{
    ChangeClipState = ObjectInitializer.CreateDefaultSubobject<UNZGunStateChangeClip>(this, TEXT("StateChangeClip"));
    
    ViewKickComponent = ObjectInitializer.CreateDefaultSubobject<UNZGunViewKickComponent>(this, TEXT("ViewKickComponent"));
}

void ANZGun::GotoChangeClipState()
{
    GotoState(ChangeClipState);
}

void ANZGun::ChangeClip()
{
    GotoChangeClipState();
}

void ANZGun::ChangeClipFinished()
{
    if (Ammo + CarriedAmmo > MaxAmmo)
    {
        CarriedAmmo -= MaxAmmo - Ammo;
        Ammo = MaxAmmo;
    }
    else
    {
        Ammo = Ammo + CarriedAmmo;
        CarriedAmmo = 0;
    }
}

bool ANZGun::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->BeginFiringSequence(FireModeNum, bClientFired);
    }
    
    return Super::BeginFiringSequence(FireModeNum, bClientFired);
}

void ANZGun::EndFiringSequence(uint8 FireModeNum)
{
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->EndFiringSequence(FireModeNum);
    }
    
    bIsContinousFire = false;
    
    Super::EndFiringSequence(FireModeNum);
}

void ANZGun::FireShot()
{
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->FireShot();
    }
    
    Super::FireShot();
}

void ANZGun::FireInstantHit(bool bDealDamage, FHitResult* OutHit)
{
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->KickBackTheView();
    }
    
    Super::FireInstantHit(bDealDamage, OutHit);
}

FVector ANZGun::InstantFireEndTrace(FVector StartTrace)
{
    const FVector FireDir = ModifyForwardDirection(GetAdjustedAim(StartTrace));
    return StartTrace + FireDir * InstantHitInfo[CurrentFireMode].TraceRange;
}

bool ANZGun::HandleContinuedFiring()
{
    bIsContinousFire = true;
    
    return Super::HandleContinuedFiring();
}

FRotator ANZGun::GetAdjustedAim_Implementation(FVector StartFireLoc)
{
    FRotator Result = Super::GetAdjustedAim_Implementation(StartFireLoc);
    
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->ModifyAdjustedAim(Result);
    }
    
    return Result;
}

void ANZGun::WeaponCalcCamera(float DeltaTime, FVector& OutCamLoc, FRotator& OutCamRot)
{
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->CalcCamera(DeltaTime, OutCamLoc, OutCamRot);
    }
}

FVector ANZGun::ModifyForwardDirection(FRotator AimAngle)
{
    if (ViewKickComponent != NULL)
    {
        return ViewKickComponent->ModifyForwardDirection(AimAngle);
    }
    else
    {
        return AimAngle.Vector();
    }
}

