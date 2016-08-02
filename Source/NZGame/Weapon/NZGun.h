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
    ANZGun(const FObjectInitializer& ObjectInitializer);

    /** Change clip */
    UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
    UNZWeaponState* ChangeClipState;
	
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
    
	virtual void GotoActiveState();

	virtual void ChangeClip();
    virtual void ChangeClipFinished();

	UFUNCTION(BlueprintCallable, Category = "Gun")
	virtual void PlayChangeClipAnim();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gun")
    virtual bool IsChangingClip() { return GetCurrentState() == ChangeClipState; }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
    bool bIsContinousFire;
    
    /** Override fire interfaces */
    virtual bool BeginFiringSequence(uint8 FireModeNum, bool bClientFired) override;
    virtual void EndFiringSequence(uint8 FireModeNum) override;
    virtual void FireShot() override;
    virtual void FireInstantHit(bool bDealDamage = true, FHitResult* OutHit = NULL) override;
    virtual void HitScanTrace(const FVector& StartLocation, const FVector& EndTrace, float TraceRadius, FHitResult& Hit, float PredictionTime) override;
	virtual FVector InstantFireStartTrace() override;
    virtual FVector InstantFireEndTrace(FVector StartTrace) override;
    virtual bool HandleContinuedFiring() override;
    virtual FRotator GetAdjustedAim_Implementation(FVector StartFireLoc) override;
    virtual void WeaponCalcCamera(float DeltaTime, FVector& OutCamLoc, FRotator& OutCamRot) override;
    
    virtual FVector ModifyForwardDirection(FRotator AimAngle);

    /** Component that kick back the view */
    UPROPERTY(EditAnywhere, Category = "Gun")
    class UNZGunComponent_ViewKick* ViewKickComponent;

protected:
	virtual void GotoChangeClipState();
};
