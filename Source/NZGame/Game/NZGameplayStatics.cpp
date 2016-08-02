// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/DemoNetDriver.h"



void UNZGameplayStatics::NZPlaySound(UWorld* TheWorld, USoundBase* TheSound, AActor* SourceActor, ESoundReplicationType RepType, bool bStopWhenOwnerDestroyed, const FVector& SoundLoc, class ANZPlayerController* AmpedListener, APawn* Instigator, bool bNotifyAI)
{
    if (TheSound != NULL && !GExitPurge)
    {
        if (SourceActor == NULL && SoundLoc.IsZero())
        {
            //UE_LOG(NZ, Warning, TEXT("NZPlaySound(): No source (SourceActor == None and SoundLoc not specified)"));
        }
        else if (SourceActor == NULL && TheWorld == NULL)
        {
            //UE_LOG(NZ, Warning, TEXT("NZPlaySound(): Missing SourceActor"));
        }
        else if (TheWorld == NULL && SourceActor->GetWorld() == NULL)
        {
            //UE_LOG(NZ, Warning, TEXT("NZPlaySound(): Source isn't in a world"));
        }
        else
        {
            if (TheWorld == NULL)
            {
                TheWorld = SourceActor->GetWorld();
            }
            if (RepType >= SRT_MAX)
            {
                //UE_LOG(NZ, Warning, TEXT("NZPlaySound(): Unexpected RepType"));
                RepType = SRT_All;
            }
            
            const FVector& SourceLoc = !SoundLoc.IsZero() ? SoundLoc : SourceActor->GetActorLocation();
            
            if (TheWorld->GetNetMode() != NM_Standalone && TheWorld->GetNetDriver() != NULL)
            {
                APlayerController* TopOwner = NULL;
                for (AActor* TestActor = SourceActor; TestActor != NULL && TopOwner == NULL; TestActor = TestActor->GetOwner())
                {
                    TopOwner = Cast<APlayerController>(TestActor);
                }
                
                for (int32 i = 0; i < TheWorld->GetNetDriver()->ClientConnections.Num(); i++)
                {
                    ANZPlayerController* PC = Cast<ANZPlayerController>(TheWorld->GetNetDriver()->ClientConnections[i]->OwningActor);
                    if (PC != NULL)
                    {
                        bool bShouldReplicate;
                        switch (RepType)
                        {
                            case SRT_All:
                                bShouldReplicate = true;
                                break;
                                
                            case SRT_AllButOwner:
                                bShouldReplicate = PC != TopOwner;
                                break;
                                
                            case SRT_IfSourceNotReplicated:
                                bShouldReplicate = SourceActor == NULL || TheWorld->GetNetDriver()->ClientConnections[i]->ActorChannels.Find(SourceActor) == NULL;
                                break;
                                
                            case SRT_None:
                                bShouldReplicate = false;
                                break;
                                
                            default:
                                // should be impossible
                                //UE_LOG(NZ, Warning, TEXT("NZPlaySound(): Unhandled sound replication type %i"), int32(RepType));
                                bShouldReplicate = true;
                                break;
                        }
                        
                        if (bShouldReplicate)
                        {
                            PC->HearSound(TheSound, SourceActor, SourceLoc, bStopWhenOwnerDestroyed, AmpedListener == PC);
                        }
                    }
                }
            }
            
            // Write into demo if there is one
            if (TheWorld->DemoNetDriver != NULL && TheWorld->DemoNetDriver->ServerConnection == NULL && RepType != SRT_None && RepType != SRT_IfSourceNotReplicated)
            {
                // TODO: Engine doesn't set this on record for some reason
                // ANZPlayerController* PC = Cast<ANZPlayerController>(TheWorld->DemoNetDriver->SpectatorController);
                ANZPlayerController* PC = (TheWorld->DemoNetDriver->ClientConnections.Num() > 0) ? Cast<ANZPlayerController>(TheWorld->DemoNetDriver->ClientConnections[0]->PlayerController) : NULL;
                if (PC != NULL)
                {
                    PC->ClientHearSound(TheSound, SourceActor, (SourceActor != NULL && SourceActor->GetActorLocation() == SourceLoc) ? FVector::ZeroVector : SourceLoc, bStopWhenOwnerDestroyed, false, false);
                }
            }
            
            for (FLocalPlayerIterator It(GEngine, TheWorld); It; ++It)
            {
                ANZPlayerController* PC = Cast<ANZPlayerController>(It->PlayerController);
                if (PC != NULL && PC->IsLocalPlayerController())
                {
                    PC->HearSound(TheSound, SourceActor, SourceLoc, bStopWhenOwnerDestroyed, AmpedListener == PC);
                }
            }
            
            if (bNotifyAI)
            {
                if (Instigator == NULL)
                {
                    Instigator = Cast<APawn>(SourceActor);
                }
                if (Instigator != NULL)
                {
                    // NOTE: All sound attenuation treated as a sphere
                    float Radius = TheSound->GetMaxAudibleDistance();
                    const FAttenuationSettings* Settings = TheSound->GetAttenuationSettingsToApply();
                    if (Settings != NULL)
                    {
                        Radius = FMath::Max<float>(Radius, Settings->GetMaxDimension());
                    }
                    for (FConstControllerIterator It = TheWorld->GetControllerIterator(); It; ++It)
                    {
                        if (It->IsValid())
                        {
                            ANZBot* B = Cast<ANZBot>(It->Get());
                            if (B != NULL && B->GetPawn() != NULL && B->GetPawn() != Instigator && (Radius <= 0.0f || Radius > (SourceLoc - B->GetPawn()->GetActorLocation()).Size() * B->HearingRadiusMult))
                            {
                                B->HearSound(Instigator, SourceLoc, Radius);
                            }
                        }
                    }
                }
            }
        }
    }
}

float UNZGameplayStatics::GetGravityZ(UObject* WorldContextObject, const FVector& TestLoc)
{
    UWorld* World = (WorldContextObject != NULL) ? WorldContextObject->GetWorld() : NULL;
    if (World == NULL)
    {
        return GetDefault<UPhysicsSettings>()->DefaultGravityZ;
    }
    else if (TestLoc.IsZero())
    {
        return World->GetDefaultGravityZ();
    }
    else
    {
        APhysicsVolume* Volume = FindPhysicsVolume(World, TestLoc, FCollisionShape::MakeSphere(0.0f));
        return (Volume != NULL) ? Volume->GetGravityZ() : World->GetDefaultGravityZ();
    }
}


APawn* UNZGameplayStatics::PickBestAimTarget(AController* AskingC, FVector StartLoc, FVector FireDir, float MinAim, float MaxRange, TSubclassOf<APawn> TargetClass, float* BestAim, float* BestDist)
{
    check(false);
    return NULL;
}

UParticleSystemComponent* UNZGameplayStatics::PlayScreenEffect(UObject* WorldContextObject, class UParticleSystem* EmitterTemplate, FVector Offset)
{
	ANZPlayerController* PlayerController = Cast<ANZPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	if (PlayerController)
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

		FRotationTranslationMatrix ViewM(ViewRotation, ViewLocation);
		// 先解决本地空间的变换，再变换到视点空间
		FMatrix NewM = FRotationMatrix(FRotator(0, -90, 0)) * FTranslationMatrix(Offset) * ViewM;

		USceneComponent* ComponentToAttach = NULL;
		if (PlayerController->GetNZCharacter() && PlayerController->GetNZCharacter()->CharacterCameraComponent)
		{
			ComponentToAttach = PlayerController->GetNZCharacter()->CharacterCameraComponent;
		}
		else
		{
			ComponentToAttach = PlayerController->PlayerCameraManager->GetViewTarget()->GetRootComponent();
		}

		if (ComponentToAttach)
		{
			UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(EmitterTemplate, ComponentToAttach, NAME_None, NewM.GetOrigin(), NewM.Rotator(), EAttachLocation::KeepWorldPosition);
			if (PSC)
			{
				PSC->SetDepthPriorityGroup(ESceneDepthPriorityGroup::SDPG_Foreground);
			}

			return PSC;
		}
	}

	return NULL;
}
