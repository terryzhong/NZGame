// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponViewKickComponent.h"


// Sets default values for this component's properties
UNZWeaponViewKickComponent::UNZWeaponViewKickComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = false;
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	DetailReactYawShot.Add(0.0723);
	DetailReactYawShot.Add(0.072);
	DetailReactYawShot.Add(0.13);
	DetailReactYawShot.Add(0.073);
	DetailReactYawShot.Add(0.20);

	DetailReactPitchShot.Add(0.13);
	DetailReactPitchShot.Add(0.117);
	DetailReactPitchShot.Add(0.22);
	DetailReactPitchShot.Add(0.16);
	DetailReactPitchShot.Add(0.35);
}

void UNZWeaponViewKickComponent::SetPunchAngle(FRotator Angle)
{
	ANZCharacter* Character = Cast<ANZCharacter>(GetOwner() ? GetOwner()->Instigator : NULL);
	if (Character != NULL)
	{
		Character->SetPunchAngle(Angle);
	}
}

FRotator UNZWeaponViewKickComponent::GetPunchAngle()
{
	ANZCharacter* Character = NULL;
	FRotator ZeroRotator = FRotator::ZeroRotator;
	FRotator PunchRotator = FRotator::ZeroRotator;

	Character = Cast<ANZCharacter>(GetOwner() ? GetOwner()->Instigator : NULL);
	if (Character != NULL)
	{
		PunchRotator.Yaw = Character->PitchParam * 65536 / (PI * 2.f);
		PunchRotator.Pitch = -Character->PitchParam * 65536 / (PI * 2.f);
		return PunchRotator;
	}
	else
	{
		return ZeroRotator;
	}
}

void UNZWeaponViewKickComponent::KickBackTheView()
{
	ANZCharacter* Character = NULL;
	float CurrentDetailReactYawShot = 0.f;
	float CurrentDetailReactPitchShot = 0.f;
	TArray<float> CurrentSideReactDirect;
	float FireCountRefer = 0.f;

	Character = Cast<ANZCharacter>(GetOwner() ? GetOwner()->Instigator : NULL);
	if (Character == NULL)
	{
		return;
	}

	check(DetailReactPitchShot.Num() == 5 && DetailReactYawShot.Num() == 5);

	if (Character->GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		CurrentDetailReactPitchShot = DetailReactPitchShot[4];
		CurrentDetailReactYawShot = DetailReactYawShot[4];
	}
	//else if (Character->bSnake && Character->Acceleration.Size() > 0)
	//{
	//    CurrentDetailReactPitchShot = DetailReactPitchShot[3];
	//    CurrentDetailReactYawShot = DetailReactYawShot[3];
	//}
	else if (Character->bIsCrouched)
	{
		if (Character->GetCharacterMovement()->Velocity.Size() > 0)
		{
			CurrentDetailReactPitchShot = DetailReactPitchShot[3];
			CurrentDetailReactYawShot = DetailReactYawShot[3];
		}
		else
		{
			CurrentDetailReactPitchShot = DetailReactPitchShot[1];
			CurrentDetailReactYawShot = DetailReactYawShot[1];
		}
	}
	else if (Character->GetCharacterMovement()->Velocity.Size() > 200)
	{
		CurrentDetailReactPitchShot = DetailReactPitchShot[2];
		CurrentDetailReactYawShot = DetailReactYawShot[2];
	}
	else
	{
		CurrentDetailReactPitchShot = DetailReactPitchShot[0];
		CurrentDetailReactYawShot = DetailReactYawShot[0];
	}

	DeltaPitchParam = CurrentDetailReactPitchShot * (RecordReactParam + 0.1500000059604645) * -1.0f;
	DeltaYawParam = CurrentDetailReactYawShot * (RecordReactParam + 0.1500000059604645) * -1.0f * AdjustDeltaYawDirection;

	for (int i = 1; i < SideReactDirectSectionNum; i++)
	{
		if (FireCount - 1 <= SideReactDirectSection[i])
		{
			for (int j = 0; j < SideReactDirectSectionNum; j++)
			{
				CurrentSideReactDirect[j] = SideReactDirect[i * SideReactDirectOneSectionNum + j];
			}
			break;
		}
	}

	if (SideReactDirect[SideReactDirectOneSectionNum + 1] == 0)
	{
		DeltaYawParam = DeltaYawParam * (2 * int32(FMath::FRand() * 2.f) - 1);
	}
	else
	{
		if (FireCount == 1)
		{
			bChangeDirectionFlag1 = (int32(FMath::FRand() * 100.f) < SideReactDirect[0]);
		}

		if (FireCount == 1)
		{
			ChangeDirectionFlag3 = int32(FMath::FRand() * 100.f);
		}
		else
		{
			for (int i = 1; i < SideReactDirectSectionNum; i++)
			{
				if (FireCount == SideReactDirectSection[i] + 2)
				{
					ChangeDirectionFlag3 = int32(FMath::FRand() * 100.f);
					break;
				}
			}
		}

		if (bChangeDirectionFlag1)
		{
			FireCountRefer = CurrentSideReactDirect[0];
		}
		else
		{
			FireCountRefer = CurrentSideReactDirect[2];
		}

		if (FireCountRefer < ChangeDirectionFlag3)
		{
			if (CurrentSideReactDirect[1] + FireCountRefer < ChangeDirectionFlag3)
			{
				ShotSpreadFlag = 2;
				DeltaYawParam = DeltaYawParam * -1.0f;
				AdjustDeltaYawDirection = 1;
				bReachYawMax = false;
			}
			else
			{
				DeltaYawParam = 0.0f;
				ShotSpreadFlag = 1;
			}
		}
		else
		{
			ShotSpreadFlag = 0;
		}
	}

	if (ShotsFired > GunTurnRedStartShots)
	{
		if (bIsNoTarget == false)
		{
			StartNoTargetTime = float(ShotsFired);
			bIsNoTarget = true;
		}
	}
	else
	{
		bIsNoTarget = false;
		StartNoTargetTime = 0;
	}

	DeltaPitchParam = DeltaPitchParam + Character->PitchParam;
	DeltaYawParam = DeltaYawParam + Character->YawParam;

	bDelayOneFrameForCamera = true;
}

void UNZWeaponViewKickComponent::CalcCamera(float DeltaTime, FVector& OutCamLoc, FRotator& OutCamRot)
{
	FRotator InitPunchAngle = FRotator::ZeroRotator;
	FRotator DeltaPunchAngle2 = FRotator::ZeroRotator;
	ANZCharacter* Character = NULL;
	float CurrentYawAndPitchPitchFactor = 0.f;
	float CurrentYawAndPitchYawFactor = 0.f;
	float CurrentReactParamCoefficient = 0.f;
	float CurrentFullReactYawCoefficient = 0.f;
	float CurrentFullReactPitchCoefficient = 0.f;
	TArray<float> CurrentShotReactYaw;
	TArray<float> CurrentShotReactPitch;
	float YawSign = 0.f;
	float RemainDeltaTime = 0.f;
	float TimeTick = 0.f;

	Character = Cast<ANZCharacter>(GetOwner() ? GetOwner()->Instigator : NULL);
	if (Character == NULL)
	{
		return;
	}

	InitPunchAngle = GetPunchAngle();
	DeltaPunchAngle2 = InitPunchAngle;

	// todo:

	for (int i = 0; i < CameraYawAndPitchSectionNum; i++)
	{
		if (FireCount <= CameraYawAndPitchSection[i])
		{
			for (int j = 0; j < CameraYawAndPitchOneSectionNum; j++)
			{
				CurrentYawAndPitchPitchFactor = CameraYawAndPitch[i * CameraYawAndPitchOneSectionNum + 1];
				CurrentYawAndPitchYawFactor = CameraYawAndPitch[i * CameraYawAndPitchOneSectionNum];
			}
			break;
		}
	}

	Character->LastPitchAngle = CurrentYawAndPitchPitchFactor * InitPunchAngle.Pitch;
	Character->LastYawAngle = CurrentYawAndPitchYawFactor * InitPunchAngle.Yaw;

	DeltaPunchAngle2.Pitch = CurrentYawAndPitchPitchFactor * InitPunchAngle.Pitch;
	DeltaPunchAngle2.Yaw = CurrentYawAndPitchYawFactor * InitPunchAngle.Yaw;

	SetPunchAngle(DeltaPunchAngle2);

	OutCamRot.Pitch += CurrentYawAndPitchPitchFactor * InitPunchAngle.Pitch;
	OutCamRot.Yaw += CurrentYawAndPitchYawFactor * InitPunchAngle.Yaw;
}
