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

void ANZWeapon::InitializeDroppedPickup(class ANZDroppedPickup* Pickup)
{
    // todo:
    check(false);
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

void ANZWeapon::SpawnDelayedFakeProjectile()
{
    // todo:
    check(false);    
}

float ANZWeapon::GetBringUpTime()
{
    // todo:
    check(false);
    return 0.0f;
}

float ANZWeapon::GetPutDownTime()
{
    // todo:
    check(false);
    return 0.0f;    
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

void ANZWeapon::DeactivateSpawnProtection()
{
    // todo:
    check(false);
}

void ANZWeapon::UpdateViewBob(float DeltaTime)
{
    // todo:
    check(false);
}

void ANZWeapon::PostInitProperties()
{
    Super::PostInitProperties();
    
    // todo:
}

void ANZWeapon::BeginPlay()
{
    Super::BeginPlay();
    
    // todo:
}

UMeshComponent* ANZWeapon::GetPickupMeshTemplate_Implementation(FVector& OverrideScale) const
{
    // todo:
    check(false);
    return NULL;
}

void ANZWeapon::GotoState(class UNZWeaponState* NewState)
{
    // todo:
    check(false);
}

void ANZWeapon::StartFire(uint8 FireModeNum)
{
    // todo:
    check(false);
}

void ANZWeapon::StopFire(uint8 FireModeNum)
{
    // todo:
    check(false);
}

void ANZWeapon::ServerStartFire_Implementation(uint8 FireModeNum, bool bClientFired)
{
    // todo:
    check(false);
}

bool ANZWeapon::ServerStartFire_Validate(uint8 FireModeNum, bool bClientFired)
{
    return true;
}

void ANZWeapon::ServerStartFireOffset_Implementation(uint8 FireModeNum, uint8 ZOffset, bool bClientFired)
{
    // todo:
    check(false);
}

bool ANZWeapon::ServerStartFireOffset_Validate(uint8 FireModeNum, uint8 ZOffset, bool bClientFired)
{
    return true;
}

void ANZWeapon::ServerStopFire_Implementation(uint8 FireModeNum)
{
    // todo:
    check(false);
}

bool ANZWeapon::ServerStopFire_Validate(uint8 FireModeNum)
{
    return true;
}

bool ANZWeapon::BeginFiringSequence(uint8 FireModeNum, bool bClientFired)
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::EndFiringSequence(uint8 FireModeNum)
{
    // todo:
    check(false);
}

bool ANZWeapon::WillSpawnShot(float DeltaTime)
{
    // todo:
    check(false);
    return false;
}

bool ANZWeapon::CanFireAgain()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::OnStartedFiring()
{
    // todo:
    check(false);
}

void ANZWeapon::OnStoppedFiring()
{
    // todo:
    check(false);
}

bool ANZWeapon::HandleContinuedFiring()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::OnContinuedFiring()
{
    // todo:
    check(false);
}

void ANZWeapon::OnMultiPress(uint8 OtherFireMode)
{
    // todo:
    check(false);
}

void ANZWeapon::BringUp(float OverflowTime)
{
    // todo:
    check(false);    
}

bool ANZWeapon::PutDown()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::OnBringUp()
{
    // todo:
    check(false);
}

bool ANZWeapon::eventPreventPutDown()
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::AttachToOwner()
{
    // todo:
    check(false);
}

void ANZWeapon::DetachFromOwner()
{
    // todo:
    check(false);
}

bool ANZWeapon::IsChargedFireMode(uint8 TestMode) const
{
    // todo:
    check(false);
    return false;
}

void ANZWeapon::GivenTo(ANZCharacter* NewOwner, bool bAutoActivate)
{
    // todo:
    check(false);
}

void ANZWeapon::ClientGivenTo_Internal(bool bAutoActivate)
{
    // todo:
    check(false);
}

void ANZWeapon::Removed()
{
    // todo:
    check(false);
}

void ANZWeapon::ClientRemoved_Implementation()
{
    // todo:
    check(false);
}

void ANZWeapon::FireShot()
{
    // todo:
    check(false);
}

bool ANZWeapon::FireShotOverride()
{
    // todo:
    check(false);
    return false;
}

