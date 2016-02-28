// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZRealGun.h"




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
            if (Character->Velocity.Size() > 0)
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
        else if (Character->Velocity.Size() > 200)
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
        
        if (ChangeDirectionFlag1)
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
    DeltaPitchParam = DeltaPitchParam + Chacacter->PitchParam;
    
    bDelayOneFrameForCamera = true;
}
