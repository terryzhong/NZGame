// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZImpactEffect.h"


FImpactEffectNamedParameters::FImpactEffectNamedParameters(float DamageRadius)
{
    static FName NAME_DamageRadiusScalar(TEXT("DamageRadiusScalar"));
    FParticleSysParam* Param = new(ParticleParams) FParticleSysParam;
    Param->Name = NAME_DamageRadiusScalar;
    Param->ParamType = EParticleSysParamType::PSPT_Scalar;
    Param->Scalar = DamageRadius;
    
    static FName NAME_DamageRadiusVector(TEXT("DamageRadiusVector"));
    Param = new(ParticleParams) FParticleSysParam;
    Param->Name = NAME_DamageRadiusVector;
    Param->ParamType = EParticleSysParamType::PSPT_Vector;
    Param->Vector = FVector(DamageRadius, DamageRadius, DamageRadius);
}

// Sets default values
ANZImpactEffect::ANZImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANZImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANZImpactEffect::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool ANZImpactEffect::SpawnEffect_Implementation(UWorld* World, const FTransform& InTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, ESoundReplicationType SoundReplication, const FImpactEffectNamedParameters& EffectParams) const
{
    check(false);
    return false;
}

