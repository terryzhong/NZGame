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
    bCheckInView = true;
    bForceForLocalPlayer = true;
    bRandomizeDecalRoll = true;
    AlwaysSpawnDistance = 500.0f;
    CullDistance = 20000.0f;
    DecalLifeScaling = 1.f;
    bCanBeDamaged = false;
}

void ANZImpactEffect::SetNoLocalPlayerLOD(UWorld* World, USceneComponent* NewComp, AController* InstigatedBy) const
{
}

void ANZImpactEffect::CreateEffectComponents(UWorld* World, const FTransform& BaseTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, const FImpactEffectNamedParameters& EffectParams, USceneComponent* CurrentAttachment, FName TemplateName, const TArray<USceneComponent*>& NativeCompList, const TArray<USCS_Node*>& BPNodes) const
{
    ANZWorldSettings* WS = Cast<ANZWorldSettings>(World->GetWorldSettings());
    for (int32 i = 0; i < NativeCompList.Num(); i++)
    {
        if (NativeCompList[i]->AttachParent == CurrentAttachment && ShouldCreateComponent(NativeCompList[i], NativeCompList[i]->GetFName(), BaseTransform, HitComp, SpawnedBy, InstigatedBy))
        {
            USceneComponent* NewComp = NewObject<USceneComponent>(World, NativeCompList[i]->GetClass(), NAME_None, RF_NoFlags, NativeCompList[i]);
            NewComp->AttachParent = NULL;
            NewComp->AttachChildren.Empty();
            UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(NewComp);
            if (Prim != NULL)
            {
                Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            if (CurrentAttachment != NULL)
            {
                NewComp->AttachTo(CurrentAttachment, NewComp->AttachSocketName);
            }
            if (CurrentAttachment == NULL || CurrentAttachment == HitComp)
            {
                NewComp->SetWorldTransform(FTransform(NewComp->RelativeRotation, NewComp->RelativeLocation, NewComp->RelativeScale3D) * BaseTransform);
            }
            NewComp->RegisterComponentWithWorld(World);
            if (bNoLODForLocalPlayer)
            {
                SetNoLocalPlayerLOD(World, NewComp, InstigatedBy);
            }
            ComponentCreated(NewComp, HitComp, SpawnedBy, InstigatedBy, EffectParams);
            if (WS != NULL)
            {
                WS->AddImpactEffect(NewComp, DecalLifeScaling);
            }
            // Recurse
            CreateEffectComponents(World, BaseTransform, HitComp, SpawnedBy, InstigatedBy, EffectParams, NewComp, NativeCompList[i]->GetFName(), NativeCompList, BPNodes);
        }
    }

}

bool ANZImpactEffect::ShouldCreateComponent_Implementation(const USceneComponent* TestComp, FName CompTemplateName, const FTransform& BaseTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy) const
{
    check(false);
    return false;
}

void ANZImpactEffect::ComponentCreated_Implementation(USceneComponent* NewComp, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, FImpactEffectNamedParameters EffectParams) const
{
    
}

bool ANZImpactEffect::SpawnEffect_Implementation(UWorld* World, const FTransform& InTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, ESoundReplicationType SoundReplication, const FImpactEffectNamedParameters& EffectParams) const
{
    if (World == NULL)
    {
        return false;
    }
    else
    {
        UNZGameplayStatics::NZPlaySound(World, Audio, SpawnedBy, SoundReplication, false, InTransform.GetLocation());
        
        if (World->GetNetMode() == NM_DedicatedServer)
        {
            return false;
        }
        else
        {
            bool bSpawn = true;
            if (bCheckInView)
            {
                ANZWorldSettings* WS = Cast<ANZWorldSettings>(World->GetWorldSettings());
                bSpawn = (WS == NULL || WS->EffectIsRelevant(SpawnedBy, InTransform.GetLocation(), SpawnedBy != NULL, bForceForLocalPlayer && InstigatedBy != NULL && InstigatedBy->IsLocalPlayerController(), CullDistance, AlwaysSpawnDistance, false));
            }
            if (bSpawn)
            {
                // Create components
                TArray<USceneComponent*> NativeCompList;
                GetComponents<USceneComponent>(NativeCompList);
                TArray<USCS_Node*> ConstructionNodes;
                {
                    TArray<const UBlueprintGeneratedClass*> ParentBPClassStack;
                    UBlueprintGeneratedClass::GetGeneratedClassesHierarchy(GetClass(), ParentBPClassStack);
                    for (int32 i = ParentBPClassStack.Num() - 1; i >= 0; i--)
                    {
                        const UBlueprintGeneratedClass* CurrentBPGClass = ParentBPClassStack[i];
                        if (CurrentBPGClass->SimpleConstructionScript)
                        {
                            ConstructionNodes += CurrentBPGClass->SimpleConstructionScript->GetAllNodes();
                        }
                    }
                }
                CreateEffectComponents(World, InTransform, HitComp, SpawnedBy, InstigatedBy, EffectParams, bAttachToHitComponent ? HitComp : NULL, NAME_None, NativeCompList, ConstructionNodes);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

void ANZImpactEffect::CallSpawnEffect(UObject* WorldContextObject, TSubclassOf<ANZImpactEffect> Effect, const FTransform& InTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, ESoundReplicationType SoundReplication, const FImpactEffectNamedParameters& EffectParams)
{
    
}

void ANZImpactEffect::PostInitializeComponents()
{
    
}

