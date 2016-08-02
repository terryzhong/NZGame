// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGun.h"
#include "NZGunStateActive.h"
#include "NZGunStateChangeClip.h"
#include "NZGunComponent_ViewKick.h"


ANZGun::ANZGun(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UNZGunStateActive>(TEXT("StateActive")))
{
    ChangeClipState = ObjectInitializer.CreateDefaultSubobject<UNZGunStateChangeClip>(this, TEXT("StateChangeClip"));
    
    ViewKickComponent = ObjectInitializer.CreateDefaultSubobject<UNZGunComponent_ViewKick>(this, TEXT("ViewKickComponent"));

	ChangeClipTime = 3.f;
	ChangeClipAnimPlayRate = 1.f;
}

void ANZGun::GotoActiveState()
{
    if (ViewKickComponent != NULL)
    {
        ViewKickComponent->Reset();
    }
    
    Super::GotoActiveState();
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
    if (Ammo + CarriedAmmo > ClipAmmoCount)
    {
        CarriedAmmo -= ClipAmmoCount - Ammo;
        Ammo = ClipAmmoCount;
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

void ANZGun::PlayChangeClipAnim()
{
	float RealChangeClipTime = GetChangeClipTime();

	if (NZOwner != NULL && RealChangeClipTime > 0)
	{
		UAnimMontage* WeaponAnim = ChangeClipAnim;
		if (WeaponAnim != NULL)
		{
			UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(WeaponAnim, WeaponAnim->SequenceLength / WeaponAnim->RateScale / RealChangeClipTime);
			}
		}

		UAnimMontage* HandsAnim = ChangeClipAnimHands;
		if (HandsAnim != NULL)
		{
			UAnimInstance* AnimInstance = NZOwner->FirstPersonMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(HandsAnim, HandsAnim->SequenceLength / HandsAnim->RateScale / RealChangeClipTime);
			}
		}
	}
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

FVector ANZGun::InstantFireStartTrace()
{
	// VR设备下，射线检测的方向以枪口朝向为准
	if (NZOwner && NZOwner->Controller && NZOwner->Controller->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		FVector OutLocation;
		FRotator OutRotation;
		Mesh->GetSocketWorldLocationAndRotation(TEXT("MuzzleFlash"), OutLocation, OutRotation);
		return OutLocation;
	}
	else
	{
		return Super::InstantFireStartTrace();
	}
}

FVector ANZGun::InstantFireEndTrace(FVector StartTrace)
{
	// VR设备下，射线检测的方向以枪口朝向为准
	if (NZOwner && NZOwner->Controller && NZOwner->Controller->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		FVector OutLocation;
		FRotator OutRotation;
		Mesh->GetSocketWorldLocationAndRotation(TEXT("MuzzleFlash"), OutLocation, OutRotation);
		return StartTrace + OutRotation.Vector() * InstantHitInfo[CurrentFireMode].TraceRange;
	}
	else
	{
		const FVector FireDir = ModifyForwardDirection(GetAdjustedAim(StartTrace));
		return StartTrace + FireDir * InstantHitInfo[CurrentFireMode].TraceRange;
	}
}

void ANZGun::HitScanTrace(const FVector& StartLocation, const FVector& EndTrace, float TraceRadius, FHitResult& Hit, float PredictionTime)
{
    Super::HitScanTrace(StartLocation, EndTrace, TraceRadius, Hit, PredictionTime);
    
/*    ECollisionChannel TraceChannel = ECC_Pawn;//COLLISION_TRACE_WEAPONNOCHARACTER;
    FCollisionQueryParams QueryParams(GetClass()->GetFName(), true, NZOwner);
    QueryParams.bReturnPhysicalMaterial = true;
    if (TraceRadius <= 0.0f)
    {
        if (!GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndTrace, TraceChannel, QueryParams))
        {
            Hit.Location = EndTrace;
        }
    }
    else
    {
        if (GetWorld()->SweepSingleByChannel(Hit, StartLocation, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(TraceRadius), QueryParams))
        {
            Hit.Location += (EndTrace - StartLocation).GetSafeNormal() * TraceRadius;
        }
        else
        {
            Hit.Location = EndTrace;
        }
    }
    
    ACharacter* HitCharacter = Cast<ACharacter>(Hit.Actor.Get());
    if (HitCharacter != NULL)
    {
        TArray<FHitResult> TempHits;
        if (GetWorld()->ComponentSweepMulti(TempHits, HitCharacter->GetMesh(), StartLocation, EndTrace, FQuat::Identity, FComponentQueryParams()))
        {
            Hit = TempHits[0];
        }
    }*/
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
	// VR设备下，射线检测的方向以枪口朝向为准
	if (NZOwner && NZOwner->Controller && NZOwner->Controller->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		return;
	}
		
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

