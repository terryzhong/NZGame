// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponStateFiring_Enforcer.h"



void UNZWeaponStateFiring_Enforcer::BeginState(const UNZWeaponState* PrevState)
{
	GetOuterANZWeapon()->GetWorldTimerManager().SetTimer(RefireCheckHandle, this, &UNZWeaponStateFiring_Enforcer::RefireCheckTimer, GetOuterANZWeapon()->GetRefireTime(GetOuterANZWeapon()->GetCurrentFireMode()), true);
}

void UNZWeaponStateFiring_Enforcer::EndState()
{

}

void UNZWeaponStateFiring_Enforcer::UpdateTiming()
{

}

void UNZWeaponStateFiring_Enforcer::PutDown()
{

}
