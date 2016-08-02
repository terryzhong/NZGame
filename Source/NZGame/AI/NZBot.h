// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZAIController.h"
#include "NZBot.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZBot : public ANZAIController
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, Category = Skill)
    float Skill;
    
    UPROPERTY(BlueprintReadWrite, Category = Skill)
    float TrackingReactionTime;
    
    /** 293 
     Multiplier of a sound's base radius at which a bot can hear it (i.e. [0-1], > 1.0 is cheated superhuman hearing) */
    UPROPERTY(BlueprintReadWrite, Category = Skill)
    float HearingRadiusMult;
	
	
    /** 541
     * Notification of incoming instant hit shot that is reasonably likely to have targeted this bot used to prepare evasive actions for NEXT shot
     * (kind of late for current shot) if bot is aware enough
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = AI)
    virtual void ReceiveInstantWarning(ANZCharacter* Shooter, const FVector& FireDir);
    
    void SwitchToBestWeapon();
    
    virtual void SeePawn(APawn* Other);
    virtual void HearSound(APawn* Other, const FVector& SoundLoc, float SoundRadius);
    
    virtual void NotifyTakeHit_Implementation(AController* InstigatedBy, int32 Damage, FVector Momentum, const FDamageEvent& DamageEvent);
    virtual void NotifyCausedHit_Implementation(APawn* HitPawn, int32 Damage);
    
    
    
};
