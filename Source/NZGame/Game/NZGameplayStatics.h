// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NZGameplayStatics.generated.h"

UENUM()
enum ESoundReplicationType
{
    SRT_All,    // Replicate to all in audible range
    SRT_AllButOwner,    // Replicate to all but the owner of SourceActor
    SRT_IfSourceNotReplicated,  // Only replicate to clients on which SourceActor does not exist
    SRT_None,   // No replication; local only
    SRT_MAX
};

/**
 * 
 */
UCLASS()
class NZGAME_API UNZGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    /**
     * Plays a sound with optional replication parameters
     * Additionally will check that clients will actually be able to hear the sound (don't repliate if out of sound's audible range)
     * If called on client, always local only
     */
    UFUNCTION(BlueprintCallable, Category = Sound, meta = (HidePin = "TheWorld", DefaultToSelf = "SourceActor", AutoCreateRefTerm = "SoundLoc"))
    static void NZPlaySound(UWorld* TheWorld, USoundBase* TheSound, AActor* SourceActor = NULL, ESoundReplicationType RepType = SRT_All, bool bStopWhenOwnerDestroyed = false, const FVector& SoundLoc = FVector::ZeroVector, class ANZPlayerController* AmpedListener = NULL, APawn* Instigator = NULL, bool bNotifyAI = true);
    
    /** Retrieves gravity; If no location is specified, level default gravity is returned */
    UFUNCTION(BlueprintCallable, Category = World, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", AutoCreateRefTerm = "TestLoc"))
    static float GetGravityZ(UObject* WorldContextObject, const FVector& TestLoc = FVector::ZeroVector);
	
	
    /** Select visible controlled enemy Pawn for which direction from StartLoc is closest to FireDir and within aiming cone/distance constraints */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game | Targeting")
    static APawn* PickBestAimTarget(AController* AskingC, FVector StartLoc, FVector FireDir, float MinAim, float MaxRange, TSubclassOf<APawn> TargetClass = NULL
#if CPP
    , float* BestAim = NULL, float* BestDist = NULL
#endif
    );

	UFUNCTION(BlueprintCallable, Category = "Effects|Components|ParticleSystem", meta = (WorldContext = "WorldContextObject", Keywords = "particle system", UnsafeDuringActorConstruction = "true"))
	static UParticleSystemComponent* PlayScreenEffect(UObject* WorldContextObject, class UParticleSystem* EmitterTemplate, FVector Offset = FVector(300, 0, 0));

};
