// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZProfileSettings.generated.h"

USTRUCT()
struct FStoredWeaponPriority
{
	GENERATED_BODY()

	UPROPERTY()
	FString WeaponClassName;

	UPROPERTY()
	float WeaponPriority;

	FStoredWeaponPriority()
		: WeaponClassName(TEXT(""))
		, WeaponPriority(0.0)
	{
	}

	FStoredWeaponPriority(FString InWeaponClassName, float InWeaponPriority)
		: WeaponClassName(InWeaponClassName)
		, WeaponPriority(InWeaponPriority)
	{
	}
};

/**
 * 
 */
UCLASS()
class NZGAME_API UNZProfileSettings : public UObject
{
	GENERATED_BODY()

	void ClearWeaponPriorities();
	void SetWeaponPriority(FString WeaponClassName, float NewPriority);
	float GetWeaponPriority(FString WeaponClassName, float DefaultPriority);


protected:
	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	TArray<uint8> RawInputSettings;

	UPROPERTY()
	TArray<struct FInputActionKeyMapping> ActionMappings;

	UPROPERTY()
	TArray<struct FInputAxisKeyMapping> AxisMappings;

	UPROPERTY()
	TArray<struct FInputAxisConfigEntry> AxisConfig;
	
	UPROPERTY()
	TArray<FCustomKeyBinding> CustomBinds;


	UPROPERTY()
	TArray<FStoredWeaponPriority> WeaponPriorities;


};
