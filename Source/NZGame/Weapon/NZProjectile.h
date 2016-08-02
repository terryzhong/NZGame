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

    
    /** 93
     Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Projectile)
    USphereComponent* CollisionComp;
	
    /** Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
    class UProjectileMovementComponent* ProjectileMovement;
    
    /** Additional Z axis speed added to projectile on spawn - NOTE: blueprint changes only work in defaults or construction script as value is applied to velocity on spawn */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
    float TossZ;
    
    /** Percentage of instigator's velocity imparted to this projectile */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
    float InstigatorVelocityPct;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    FRadialDamageParams DamageParams;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    TSubclassOf<UDamageType> MyDamageType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    float Momentum;
    
    /** How much stats hit credit to give when this projectile hits */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    float StatsHitCredit;
    
    /** 121 
     Set by the firing weapon */
    UPROPERTY()
    FName HitsStatsName;
    
    /** 125
     Whether the projectile can impact its Instigator (player who fired it) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    bool bCanHitInstigator;
    
    /** 191
     Fake client side projectile (spawned on owning client) */
    UPROPERTY()
    bool bFakeClientProjectile;
    
    /** Fake projectile on this client providing visuals for me */
    UPROPERTY()
    ANZProjectile* MyFakeProjectile;
    
    /** 219
     Perform any custom initialization for this projectile as fake client side projectile */
    virtual void InitFakeProjectile(class ANZPlayerController* OwningPlayer);
    
    /** 228
     Called if server isn't ticking this projectile to make up for network latency */
    virtual void SetForwardTicked(bool bWasForwardTicked) {}
};
