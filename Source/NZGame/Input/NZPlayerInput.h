// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerInput.h"
#include "NZPlayerInput.generated.h"

USTRUCT()
struct FCustomKeyBinding
{
	GENERATED_BODY()

	FCustomKeyBinding()
		: KeyName(FName(TEXT("")))
		, EventType(IE_Pressed)
		, Command(FString(""))
	{
	}

	FCustomKeyBinding(FName InKeyName, TEnumAsByte<EInputEvent> InEventType, FString InCommand)
		: KeyName(InKeyName)
		, EventType(InEventType)
		, Command(InCommand)
	{
	}

	UPROPERTY()
	FName KeyName;

	UPROPERTY()
	TEnumAsByte<EInputEvent> EventType;

	UPROPERTY()
	FString Command;

	UPROPERTY()
	FString FriendlyName;
};

/**
 * 
 */
UCLASS()
class NZGAME_API UNZPlayerInput : public UPlayerInput
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() {}
	virtual void UpdatePlayerInput(float DeltaTime) {}
	virtual void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {}
	virtual void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {}
	virtual void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location) {}
};
