// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponState.h"
#include "NZWeaponStateEquipping.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class NZGAME_API UNZWeaponStateEquipping : public UNZWeaponState
{
	GENERATED_BODY()
	
public:
    float PartialEquipTime;
    
    float EquipTime;
    
    int32 PendingFireSequence;
    
    virtual void BeginState(const UNZWeaponState* PrevState) override;
    
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired) override;
    
    virtual void StartEquip(float OverflowTime);
    virtual void EndState() override
    {
        GetOuterANZWeapon()->GetWorldTimerManager().ClearAllTimersForObject(this);
    }
    
    virtual void Tick(float DeltaTime)
    {
        if (!GetOuterANZWeapon()->GetWorldTimerManager().IsTimerActive(BringUpFinishedHandle))
        {
            //UE_LOG(NZ, Warning, TEXT("%s in state Equipping with no equip timer!"), *GetOuterAUTWeapon()->GetName());
            BringUpFinished();
        }
    }
    
    FTimerHandle BringUpFinishedHandle;
    
    virtual void BringUpFinished();
    
    virtual void PutDown() override
    {
        
    }
};
