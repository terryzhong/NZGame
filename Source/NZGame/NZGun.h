// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeapon.h"
#include "NZGun.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZGun : public ANZWeapon
{
	GENERATED_BODY()

    friend class UNZGunStateChangeClip;
	
public:
    ANZGun();

    /** Change clip */
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    UNZWeaponState* ChangeClipState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
    int32 ClipAmmoCount;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
    float ChangeClipTime;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
    UAnimMontage* ChangeClipAnim;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
    UAnimMontage* ChangeClipAnimHands;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
    float ChangeClipAnimPlayRate;
    
    UFUNCTION(BlueprintCallable, Category = "Gun")
    virtual float GetChangeClipTime()
    {
        return ChangeClipTime;
    }
    
    virtual void ChangeClip();

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
    bool bIsContinousFire;
    
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired);
    virtual void EndFiringSequence(uint8 FireModeNum);
    
    virtual void FireShot();

    virtual bool HandleContinuedFiring();
    
	
};
