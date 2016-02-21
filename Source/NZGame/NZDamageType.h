// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DamageType.h"
#include "NZDamageType.generated.h"

USTRUCT(BlueprintType)
struct FNZPointDamageEvent : public FPointDamageEvent
{
    GENERATED_BODY()
    
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

USTRUCT(BlueprintType)
struct FNZRadialDamageEvent : public FRadialDamageEvent
{
    GENERATED_BODY()
    
    FNZRadialDamageEvent()
        : FRadialDamageEvent()
        , bScaleMomentum(true)
    {}
    
    /** Momentum magnitude */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    float BaseMomentumMag;
    
    /** Whether to scale the momentum to the percentage of damage received (i.e. due to distance) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    bool bScaleMomentum;
    
    /** ID for this class. NOTE this must be unique for all damage events. */
    static const int32 ClassID = 102;
    virtual int32 GetTypeID() const { return FNZRadialDamageEvent::ClassID; }
    virtual bool IsOfType(int32 InID) const { return (FNZRadialDamageEvent::ClassID == InID) || FRadialDamageEvent::IsOfType(InID); }
};

/**
 * 
 */
UCLASS()
class NZGAME_API UNZDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
    UNZDamageType();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    float WalkMovementReductionPct;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    float WalkMovementReductionDuration;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (EditCondition = "bForceZMomentum"), Category = Momentum)
    float ForceZMomentumPct;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    float SelfMomentumBoost;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    uint32 bSelfMomentumBoostOnlyZ : 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    uint32 bForceZMomentum : 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    uint32 bPreventWalkingZMomentum : 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    uint32 bCausesBlood : 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Momentum)
    uint32 bBlockedByArmor : 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    const UCurveLinearColor* BodyDamageColor;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    const UCurveLinearColor* ArmorDamageColor;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    const UCurveLinearColor* SuperHealthDamageColor;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
    bool bBodyDamageColorRimOnly;
    
    
    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
    void ScoreKill(ANZPlayerState* KillerState, ANZPlayerState* VictimState, APawn* KilledPawn) const;
    
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
    void PlayHitEffects(ANZCharacter* HitPawn, bool bPlayedArmorEffect) const;
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic)
    void PlayDeathEffects(ANZCharacter* DyingPawn) const;
    
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Messages)
    FText ConsoleDeathMessage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Messages)
    FText MaleSuicideMessage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Messages)
    FText RemaleSuicideMessage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Messages)
    FText SelfVictimMessage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Messages)
    FText AssociatedWeaponName;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Stats)
    FString StatsName;
    
    
	
	
};
