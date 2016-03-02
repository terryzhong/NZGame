// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"


DEFINE_LOG_CATEGORY(NZ);

class FNZModule : public FDefaultGameModuleImpl
{
    virtual void StartupModule() override;
};

IMPLEMENT_PRIMARY_GAME_MODULE( FNZModule, NZGame, "NZGame" );

#if WITH_EDITOR

#include "NZDetailsCustomization.h"

static uint32 NZGetNetworkVersion()
{
    return 3008042;
}

/*
static void AddLevelSummaryAssetTags(const UWorld* InWorld, TArray<UObject::FAssetRegistryTag>& OutTags)
{
    // Add level summary data to the asset registry as part of the world
    ANZWorldSettings* Settings = Cast<ANZWorldSettings>(InWorld->GetWorldSettings());
    if (Settings != NULL && Settings->GetLevelSummary() != NULL)
    {
        Settings->GetLevelSummary()->GetAssetRegisterTags(OutTags);
    }
}
*/

void FNZModule::StartupModule()
{
    FDefaultGameModuleImpl::StartupModule();
    
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout("NZWeapon", FOnGetDetailCustomizationInstance::CreateStatic(&FNZDetailsCustomization::MakeInstance));
    PropertyModule.RegisterCustomClassLayout("NZWeaponAttachment", FOnGetDetailCustomizationInstance::CreateStatic(&FNZDetailsCustomization::MakeInstance));
    PropertyModule.NotifyCustomizationModuleChanged();
    
    //FWorldDelegates::GetAssetTags.AddStatic(&AddLevelSummaryAssetTags);
    
    // Set up our handler for network versioning
    FNetworkVersion::GetLocalNetworkVersionOverride.BindStatic(&NZGetNetworkVersion);
}

#else

void FNZModule::StartupModule()
{
    // Set up our handler for network versioning
    FNetworkVersion::GetLocalNetworkVersionOverride.BindStatic(&NZGetNetworkVersion);
}

#endif




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

