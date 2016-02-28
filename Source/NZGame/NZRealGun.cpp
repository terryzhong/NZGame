// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZRealGun.h"



void ANZRealGun::SetPunchAngle(FRotator Angle)
{
	ANZCharacter* Character = Cast<ANZCharacter>(Instigator);
	if (Character != NULL)
	{
		Character->SetPunchAngle(Angle);
	}
}

FRotator ANZRealGun::GetPunchAngle()
{
	ANZCharacter* Character = NULL;
	FRotator ZeroRotator = FRotator::ZeroRotator;
	FRotator PunchRotator = FRotator::ZeroRotator;

	Character = Cast<ANZCharacter>(Instigator);
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

void ANZRealGun::KickBackTheView()
{
    ANZCharacter* Character = NULL;
    float CurrentDetailReactYawShot;
    float CurrentDetailReactPitchShot;
    TArray<float> CurrentSideReactDirect;
    int i, j;
    float FireCountRefer;
    
    Character = Cast<ANZCharacter>(Instigator);
    if (Character == NULL)
    {
        return;
    }
    
    if (Character->GetCharacterMovement()->MovementMode == MOVE_Falling)
    {
        CurrentDetailReactYawShot = DetailReactYawShot[4];
        CurrentDetailReactPitchShot = DetailReactPitchShot[4];
    }
    //else if (Character->bSnake && Character->Acceleration.Size() > 0)
    //{
    //    CurrentDetailReactYawShot = DetailReactYawShot[3];
    //    CurrentDetailReactPitchShot = DetailReactPitchShot[3];
    //}
    else if (Character->bIsCrouched)
    {
        if (Character->GetCharacterMovement()->Velocity.Size() > 0)
        {
            CurrentDetailReactYawShot = DetailReactYawShot[3];
            CurrentDetailReactPitchShot = DetailReactPitchShot[3];
        }
        else
        {
            CurrentDetailReactYawShot = DetailReactYawShot[1];
            CurrentDetailReactPitchShot = DetailReactPitchShot[1];
        }
    }
    else if (Character->GetCharacterMovement()->Velocity.Size() > 200)
    {
        CurrentDetailReactYawShot = DetailReactYawShot[2];
        CurrentDetailReactPitchShot = DetailReactPitchShot[2];
    }
    else
    {
        CurrentDetailReactYawShot = DetailReactYawShot[0];
        CurrentDetailReactPitchShot = DetailReactPitchShot[0];
    }
    
    DeltaYawParam = CurrentDetailReactYawShot * (RecordReactParam + 0.1500000059604645) * -1.0f * AdjustDeltaYawDirection;
    DeltaPitchParam = CurrentDetailReactPitchShot * (RecordReactParam + 0.1500000059604645) * -1.0f;
    
    for (i = 1; i < SideReactDirectSectionNum; i++)
    {
        if (FireCount - 1 <= SideReactDirectSection[i])
        {
            for (j = 0; j < SideReactDirectSectionNum; j++)
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
            for (i = 1; i < SideReactDirectSectionNum; i++)
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
    
    DeltaYawParam = DeltaYawParam + Character->YawParam;
    DeltaPitchParam = DeltaPitchParam + Character->PitchParam;
    
    bDelayOneFrameForCamera = true;
}

void ANZRealGun::WeaponCalcCamera(float DeltaTime, FVector& OutCamLoc, FRotator& OutCamRot)
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

	Character = Cast<ANZCharacter>(Instigator);
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
