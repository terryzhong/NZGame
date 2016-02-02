// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DamageType.h"
#include "NZDamageType.generated.h"

USTRUCT(BlueprintType)
struct FNZPointDamageEvent : public FPointDamageEvent
{
    GENERATED_USTRUCT_BODY()
    
    FNZPointDamageEvent()
        : FPointDamageEvent()
    {}
    
    FNZPointDamageEvent(float InDamage, const FHitResult& InHitInfo, const FVector& InShotDirection, TSubclassOf<UDamageType> InDamageTypeClass, const FVector& InMomentum = FVector::ZeroVector)
        : FPointDamageEvent(InDamage, InHitInfo, InShotDirection, InDamageTypeClass)
        , Momentum(InMomentum)
    {}
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    FVector Momentum;
    
    /** ID for this class. NOTE this must be unique for all damage events */
    static const int32 ClassID = 101;
    virtual int32 GetTypeID() const { return FNZPointDamageEvent::ClassID; }
    virtual bool IsOfType(int32 InID) const { return (FNZPointDamageEvent::ClassID == InID) || FPointDamageEvent::IsOfType(InID); }
};

/**
 * 
 */
UCLASS()
class NZGAME_API UNZDamageType : public UDamageType
{
	GENERATED_BODY()
	
	
	
	
};
