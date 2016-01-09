// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon.h"



void ANZWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

void ANZWeapon::OnRep_AttachmentType()
{
    
}

void ANZWeapon::OnRep_Ammo()
{
    
}

bool ANZWeapon::HasAnyAmmo()
{
    return true;
}


void ANZWeapon::BringUp(float OverflowTime)
{

}

bool ANZWeapon::PutDown()
{
	return false;
}
