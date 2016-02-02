// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameplayStatics.h"




void UNZGameplayStatics::NZPlaySound(UWorld* TheWorld, USoundBase* TheSound, AActor* SourceActor, ESoundReplicationType RepType, bool bStopWhenOwnerDestroyed, const FVector& SoundLoc, class ANZPlayerController* AmpedListener, APawn* Instigator, bool bNotifyAI)
{
    check(false);
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
