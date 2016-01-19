// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon.h"
#include "NZPlayerController.h"
#include "NZCharacterMovementComponent.h"



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

void ANZWeapon::NetSynchRandomSeed()
{
    ANZPlayerController* OwningPlayer = NZOwner ? Cast<ANZPlayerController>(NZOwner->GetController()) : NULL;
    if (OwningPlayer && NZOwner && NZOwner->NZCharacterMovement)
    {
        FMath::RandInit(10000.f * NZOwner->NZCharacterMovement->GetCurrentSynchTime());
    }
}

void ANZWeapon::AttachToHolster()
{
    // todo:
    check(false);
}

void ANZWeapon::DetachFromHolster()
{
    // todo:
    check(false);
}

void ANZWeapon::DropFrom(const FVector& StartLocation, const FVector& TossVelocity)
{
    // todo:
    check(false);
}

bool ANZWeapon::InitializeDroppedPickup(class ANZDroppedPickup* Pickup)
{
    // todo:
    check(false);
    return false;
}

bool ANZWeapon::ShouldDropOnDeath()
{
    // todo:
    check(false);
    return false;    
}

bool ANZWeapon::ShouldPlay1PVisuals() const
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::PlayPredictedImpactEffects(FVector ImpactLoc)
{
    // todo:
    check(false);
}

void ANZWeapon::PlayDelayedImpactEffects()
{
    // todo:
    check(false);
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


