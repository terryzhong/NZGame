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


float ANZWeapon::GetImpartedMomentumMag(AActor* HitActor)
{
    return 0.f;
}


void ANZWeapon::BringUp(float OverflowTime)
{

}

bool ANZWeapon::PutDown()
{
	return false;
}

bool ANZWeapon::FollowsInList(ANZWeapon* OtherWeapon)
{
    // Return true if this weapon is after OtherWeapon in the weapon list
    if (!OtherWeapon)
    {
        return true;
    }
    // If same group, order by slot, else order by group number
    return (Group == OtherWeapon->Group) ? (GroupSlot > OtherWeapon->GroupSlot) : (Group > OtherWeapon->Group);
}


