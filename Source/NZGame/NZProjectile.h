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
    
    
    /** 121 
     Set by the firing weapon */
    UPROPERTY()
    FName HitsStatsName;
    
    /** Whether the projectile can impact its Instigator (player who fired it) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    bool bCanHitInstigator;
    
    /** 219
     Perform any custom initialization for this projectile as fake client side projectile */
    virtual void InitFakeProjectile(class ANZPlayerController* OwningPlayer);
    
    /** 228
     Called if server isn't ticking this projectile to make up for network latency */
    virtual void SetForwardTicked(bool bWasForwardTicked) {}
};
