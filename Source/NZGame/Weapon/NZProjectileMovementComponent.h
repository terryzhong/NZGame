// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/ProjectileMovementComponent.h"
#include "NZProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
    float AccelRate;
    
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Projectile)
    FVector Acceleration;
    
    UPROPERTY(Replicated)
    FVector ReplicatedAcceleration;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, Meta = (EditCondition = "!bBounce"))
    float HitZStopSimulatingThreshold;
	
	
};
