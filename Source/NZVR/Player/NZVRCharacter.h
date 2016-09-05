// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/NZCharacter.h"
#include "NZVRCharacter.generated.h"

/**
 * 
 */
UCLASS()
class NZVR_API ANZVRCharacter : public ANZCharacter
{
	GENERATED_BODY()
	
public:
	ANZVRCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "HMD")
	virtual void ProcessHeadMountedDisplay(FVector HeadPosition, FRotator HeadRotation, FVector RightHandPosition, FRotator RightHandRotation, FVector LeftHandPosition, FRotator LeftHandRotation);

public:
	/** VestComponent */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vest)
	class UNZVestComponent* VestComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vest)
	TSubclassOf<UNZVestComponent> VestComponentClass;
	
};
