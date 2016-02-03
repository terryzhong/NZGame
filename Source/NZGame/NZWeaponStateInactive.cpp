// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateInactive.h"
#include "NZWeaponStateFiring.h"



void UNZWeaponStateInactive::BeginState(const UNZWeaponState* PrevState)
{
    for (int32 i = 0; i < GetOuterANZWeapon()->FiringState.Num(); i++)
    {
        if (GetOuterANZWeapon()->FiringState[i] != NULL)
        {
            GetOuterANZWeapon()->FiringState[i]->WeaponBecameInactive();
        }
    }
    Super::BeginState(PrevState);
}

void UNZWeaponStateInactive::BringUp(float OverflowTime)
{
    GetOuterANZWeapon()->GotoEquippingState(OverflowTime);
}

