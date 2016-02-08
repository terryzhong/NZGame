// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, NZGame, "NZGame" );



FCollisionResponseParams WorldResponseParams = []()
{
    FCollisionResponseParams Result(ECR_Ignore);
    Result.CollisionResponse.WorldStatic = ECR_Block;
    Result.CollisionResponse.WorldDynamic = ECR_Block;
    return Result;
}();

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleModuleRequired.h"
#include "particles/ParticleLODLevel.h"
bool IsLoopingParticleSystem(const UParticleSystem* PSys)
{
    for (int32 i = 0; i < PSys->Emitters.Num(); i++)
    {
        if (PSys->Emitters[i]->GetLODLevel(0)->RequiredModule->EmitterLoops <= 0 &&
            PSys->Emitters[i]->GetLODLevel(0)->RequiredModule->bEnabled)
        {
            return true;
        }
    }
    return false;
}

APhysicsVolume* FindPhysicsVolume(UWorld* World, const FVector& TestLoc, const FCollisionShape& Shape)
{
    APhysicsVolume* NewVolume = World->GetDefaultPhysicsVolume();
    
    TArray<FOverlapResult> Hits;
    static FName NAME_PhysicsVolumeTrace = FName(TEXT("PhysicsVolumeTrace"));
    FComponentQueryParams Params(NAME_PhysicsVolumeTrace, NULL);
    
    World->OverlapMultiByChannel(Hits, TestLoc, FQuat::Identity, ECC_Pawn, Shape, Params);
    
    for (int32 HitIdx = 0; HitIdx < Hits.Num(); HitIdx++)
    {
        const FOverlapResult& Link = Hits[HitIdx];
        APhysicsVolume* const V = Cast<APhysicsVolume>(Link.GetActor());
        if (V != NULL && V->Priority > NewVolume->Priority && (V->bPhysicsOnContact || V->EncompassesPoint(TestLoc)))
        {
            NewVolume = V;
        }
    }
    
    return NewVolume;
}

float GetAnimLengthForScaling(UAnimMontage* WeaponAnim, UAnimMontage* HandsAnim)
{
    if (HandsAnim != NULL)
    {
        return HandsAnim->SequenceLength / HandsAnim->RateScale;
    }
    else if (WeaponAnim != NULL)
    {
        return WeaponAnim->SequenceLength / WeaponAnim->RateScale;
    }
    else
    {
        return 0.0f;
    }
}

