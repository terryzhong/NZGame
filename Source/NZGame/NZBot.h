// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "NZBot.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZBot : public AAIController
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, Category = Skill)
    float Skill;
    
    UPROPERTY(BlueprintReadWrite, Category = Skill)
    float TrackingReactionTime;
    
    void SwitchToBestWeapon();
	
	
    /** 541
     * Notification of incoming instant hit shot that is reasonably likely to have targeted this bot used to prepare evasive actions for NEXT shot
     * (kind of late for current shot) if bot is aware enough
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = AI)
    virtual void ReceiveInstantWarning(ANZCharacter* Shooter, const FVector& FireDir);
    
    
};
