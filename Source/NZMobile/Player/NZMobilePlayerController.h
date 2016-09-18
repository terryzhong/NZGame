// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/NZPlayerController.h"
#include "NZMobilePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API ANZMobilePlayerController : public ANZPlayerController
{
	GENERATED_BODY()
	
public:
	ANZMobilePlayerController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void InitInputSystem() override;
	virtual void SetupInputComponent() override;
	
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);

protected:
	class UNZMobilePlayerInput* MobilePlayerInput;
};
