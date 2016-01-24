// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "NZGameState.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZGameState : public AGameState
{
	GENERATED_BODY()
	
    
public:
	UFUNCTION(BlueprintCallable, Category = GameState)
    virtual bool OnSameTeam(const AActor* Actor1, const AActor* Actor2);
	
	
};
