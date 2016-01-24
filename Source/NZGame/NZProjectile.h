// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZProjectile.generated.h"

UCLASS()
class NZGAME_API ANZProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    
    /** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Projectile)
    USphereComponent* CollisionComp;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
    class UProjectileMovementComponent* ProjectileMovement;
    
    
};
