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
    
    
	
	
};
