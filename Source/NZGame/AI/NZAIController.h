// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "NZAIController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, NotPlaceable, Abstract)
class NZGAME_API ANZAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void NotifyTakeHit(AController* InstigatedBy, int32 Damage, FVector Momentum, const FDamageEvent& DamageEvent);

	UFUNCTION(BlueprintNativeEvent)
	void NotifyCausedHit(APawn* HitPawn, int32 Damage);

};
