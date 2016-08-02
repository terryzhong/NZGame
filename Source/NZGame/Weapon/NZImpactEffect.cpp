// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZImpactEffect.h"
#include "NZGameUserSettings.h"



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
	if (InstigatedBy != NULL && InstigatedBy->IsLocalPlayerController())
	{
		UParticleSystemComponent* PSC = Cast<UParticleSystemComponent>(NewComp);
		if (PSC)
		{
			PSC->SetLODLevel(0);
		}
	}
}

void ANZImpactEffect::CreateEffectComponents(UWorld* World, const FTransform& BaseTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, const FImpactEffectNamedParameters& EffectParams, USceneComponent* CurrentAttachment, FName TemplateName, const TArray<USceneComponent*>& NativeCompList, const TArray<USCS_Node*>& BPNodes) const
{
    ANZWorldSettings* WS = Cast<ANZWorldSettings>(World->GetWorldSettings());
    for (int32 i = 0; i < NativeCompList.Num(); i++)
    {
        if (NativeCompList[i]->GetAttachParent() == CurrentAttachment && ShouldCreateComponent(NativeCompList[i], NativeCompList[i]->GetFName(), BaseTransform, HitComp, SpawnedBy, InstigatedBy))
        {
            USceneComponent* NewComp = NewObject<USceneComponent>(World, NativeCompList[i]->GetClass(), NAME_None, RF_NoFlags, NativeCompList[i]);
            NewComp->SetupAttachment(NULL);
			NewComp->AttachChildren.Empty();
            UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(NewComp);
            if (Prim != NULL)
            {
                Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            if (CurrentAttachment != NULL)
            {
                //NewComp->AttachTo(CurrentAttachment, NewComp->AttachSocketName);
				NewComp->AttachToComponent(CurrentAttachment, FAttachmentTransformRules::KeepRelativeTransform, NewComp->GetAttachSocketName());
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
	for (int32 i = 0; i < BPNodes.Num(); i++)
	{
		USceneComponent* ComponentTemplate = Cast<USceneComponent>(BPNodes[i]->GetActualComponentTemplate(Cast<UBlueprintGeneratedClass>(GetClass())));
		if (ComponentTemplate != NULL && BPNodes[i]->ParentComponentOrVariableName == TemplateName && ShouldCreateComponent(ComponentTemplate, TemplateName, BaseTransform, HitComp, SpawnedBy, InstigatedBy))
		{
			USceneComponent* NewComp = NewObject<USceneComponent>(World, ComponentTemplate->GetClass(), NAME_None, RF_NoFlags, ComponentTemplate);
			NewComp->SetupAttachment(NULL);
			NewComp->AttachChildren.Empty();
			UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(NewComp);
			if (Prim != NULL)
			{
				Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			if (CurrentAttachment != NULL)
			{
				//NewComp->AttachTo(CurrentAttachment, BPNodes[i]->AttachToName);
				NewComp->AttachToComponent(CurrentAttachment, FAttachmentTransformRules::KeepRelativeTransform, BPNodes[i]->AttachToName);
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
			CreateEffectComponents(World, BaseTransform, HitComp, SpawnedBy, InstigatedBy, EffectParams, NewComp, BPNodes[i]->VariableName, NativeCompList, BPNodes);
		}
	}
}

bool ANZImpactEffect::ShouldCreateComponent_Implementation(const USceneComponent* TestComp, FName CompTemplateName, const FTransform& BaseTransform, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy) const
{
	if (HitComp != NULL && TestComp->IsA(UDecalComponent::StaticClass()) && (!HitComp->bReceivesDecals || !HitComp->ShouldRender()) && !HitComp->IsA(UBrushComponent::StaticClass()))
	{
		// Special case to attach to blocking volumes to project on world geometry behind it
		return false;
	}
	else
	{
		return true;
	}
}

void ANZImpactEffect::ComponentCreated_Implementation(USceneComponent* NewComp, UPrimitiveComponent* HitComp, AActor* SpawnedBy, AController* InstigatedBy, FImpactEffectNamedParameters EffectParams) const
{
	UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(NewComp);
	if (Prim != NULL)
	{
		if (WorldTimeParam != NAME_None)
		{
			for (int32 i = Prim->GetNumMaterials() - 1; i >= 0; i--)
			{
				if (Prim->GetMaterial(i) != NULL)
				{
					UMaterialInstanceDynamic* MID = Prim->CreateAndSetMaterialInstanceDynamic(i);
					MID->SetScalarParameterValue(WorldTimeParam, Prim->GetWorld()->TimeSeconds);
				}
			}
		}
		if (Prim->IsA(UPrimitiveComponent::StaticClass()))
		{
			((UParticleSystemComponent*)Prim)->bAutoDestroy = true;
			((UParticleSystemComponent*)Prim)->SecondsBeforeInactive = 0.0f;
			((UParticleSystemComponent*)Prim)->InstanceParameters += EffectParams.ParticleParams;

			UNZGameUserSettings* UserSettings = Cast<UNZGameUserSettings>(GEngine->GetGameUserSettings());
			Scalability::FQualityLevels QualitySettings = UserSettings->ScalabilityQuality;
			if ((QualitySettings.EffectsQuality < 2) && !Cast<APlayerController>(InstigatedBy))
			{
				for (int32 Index = 0; Index < ((UParticleSystemComponent*)Prim)->EmitterInstances.Num(); Index++)
				{
					if (((UParticleSystemComponent*)Prim)->EmitterInstances[Index])
					{
						((UParticleSystemComponent*)Prim)->EmitterInstances[Index]->LightDataOffset = 0;
					}
				}
			}
		}
		else if (Prim->IsA(UAudioComponent::StaticClass()))
		{
			((UAudioComponent*)Prim)->bAutoDestroy = true;
		}
	}
	else
	{
		UDecalComponent* Decal = Cast<UDecalComponent>(NewComp);
		if (Decal != NULL)
		{
			if (bRandomizeDecalRoll)
			{
				Decal->RelativeRotation.Roll += 360.0f * FMath::FRand();
			}
			if (HitComp != NULL && HitComp->Mobility == EComponentMobility::Movable)
			{
				Decal->bAbsoluteScale = true;
				//Decal->AttachTo(HitComp, NAME_None, EAttachLocation::KeepWorldPosition);
				Decal->AttachToComponent(HitComp, FAttachmentTransformRules::KeepWorldTransform);
			}
			Decal->UpdateComponentToWorld();
		}
		else
		{
			ULightComponent* Light = Cast<ULightComponent>(NewComp);
			if (Light && Light->CastShadows)
			{
				UNZGameUserSettings* UserSettings = Cast<UNZGameUserSettings>(GEngine->GetGameUserSettings());
				Scalability::FQualityLevels QualitySettings = UserSettings->ScalabilityQuality;
				if (QualitySettings.EffectsQuality < 3)
				{
					Light->SetCastShadows(false);
					Light->bAffectTranslucentLighting = false;
				}
			}
		}
	}
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
	if (WorldContextObject == NULL)
	{
		//UE_LOG(NZ, Warning, TEXT("SpawnEffect(): No world context"));
	}
	else if (Effect == NULL)
	{
		//UE_LOG(NZ, Warning, TEXT("SpawnEffect(): No effect specified"));
	}
	else
	{
		Effect.GetDefaultObject()->SpawnEffect(WorldContextObject->GetWorld(), InTransform, HitComp, SpawnedBy, InstigatedBy, SoundReplication);
	}
}

void ANZImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// We allow placing these so artists cam more easily test their effects, but they should never be spawned this way in a normal game situation
	if (GetWorld() != NULL && GetWorld()->WorldType == EWorldType::Game)
	{
		if (GAreScreenMessagesEnabled)
		{
			GEngine->AddOnScreenDebugMessage((uint64)-1, 3.0f, FColor(255, 0, 0), *FString::Printf(TEXT("NZImpactEffects should not be spawned! Use the SpawnEffect function instead. (%s)"), *GetName()));
		}
		//UE_LOG(NZ, Warning, TEXT("UTImpactEffects should not be spawned! Use the SpawnEffect function instead. (%s)"), *GetName());
		Destroy();
	}
}

