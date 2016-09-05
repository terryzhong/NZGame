// Fill out your copyright notice in the Description page of Project Settings.

#include "NZVR.h"
#include "NZVRCharacter.h"
#include "NZVestComponent.h"
#include "IHeadMountedDisplay.h"
#include "NZWeapon.h"


ANZVRCharacter::ANZVRCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	VestComponent = NULL;
	VestComponentClass = NULL;
#if PLATFORM_WINDOWS
	static ConstructorHelpers::FClassFinder<UNZVestComponent> DefaultVestComponentClassRef(TEXT("/Game/Characters/Default/NZVestComponent_INVAR"));
	if (DefaultVestComponentClassRef.Succeeded())
	{
		VestComponentClass = DefaultVestComponentClassRef.Class;
	}
#endif // PLATFORM_WINDOWS
}

// Called when the game starts or when spawned
void ANZVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (VestComponentClass != NULL)
	{
		VestComponent = NewObject<UNZVestComponent>(this, VestComponentClass, TEXT("VestComponent"));
		if (VestComponent)
		{
			VestComponent->ConnectVest();
		}
	}
}

void ANZVRCharacter::Destroyed()
{
	Super::Destroyed();

	if (VestComponent)
	{
		VestComponent->CloseVest();
	}
}


float ANZVRCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	int32 ResultDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (VestComponent)
	{
		FHitResult HitInfo;
		FVector HitDir;
		DamageEvent.GetBestHitInfo(this, NULL, HitInfo, HitDir);
		VestComponent->OnTakePointDamage(ResultDamage, HitInfo.ImpactPoint, DamageCauser);
	}

	return ResultDamage;
}

void ANZVRCharacter::ProcessHeadMountedDisplay(FVector HeadPosition, FRotator HeadRotation, FVector RightHandPosition, FRotator RightHandRotation, FVector LeftHandPosition, FRotator LeftHandRotation)
{
	if (GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		// ×ÔÓÉÒÆ¶¯
		if (Controller && Controller->StartSpot.IsValid())
		{
			APlayerStart* PS = Cast<APlayerStart>(Controller->StartSpot.Get());
			if (PS != NULL)
			{
				FVector NewLocation = PS->GetActorLocation() + (FRotator(0, 1, 0) * (GetActorRotation().Yaw - HeadRotation.Yaw)).RotateVector(HeadPosition) + PS->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector(0.f, 0.f, -1.75f);
				SetActorLocation(NewLocation);
			}
			else
			{
				FVector NewLocation = PS->GetActorLocation() + (FRotator(0, 1, 0) * (GetActorRotation().Yaw - HeadRotation.Yaw)).RotateVector(HeadPosition);
				SetActorLocation(NewLocation);
			}
		}

		if (Weapon != NULL)
		{
			Weapon->ProcessHeadMountedDisplay(HeadPosition, HeadRotation, RightHandPosition, RightHandRotation, LeftHandPosition, LeftHandRotation);
		}
	}
}
