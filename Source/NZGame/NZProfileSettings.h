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

USTRUCT()
struct FStoredWeaponGroupInfo
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString WeaponClassName;
    
    UPROPERTY()
    int32 Group;
    
    FStoredWeaponGroupInfo()
        : WeaponClassName(TEXT(""))
        , Group(0)
    {
    }
    
    FStoredWeaponGroupInfo(FString InWeaponClassName, int32 InGroup)
        : WeaponClassName(InWeaponClassName)
        , Group(InGroup)
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

public:
	void ClearWeaponPriorities();
	void SetWeaponPriority(FString WeaponClassName, float NewPriority);
	float GetWeaponPriority(FString WeaponClassName, float DefaultPriority);

    UPROPERTY()
    uint32 SettingsRevisionNum;
    
    UPROPERTY()
    FString CharacterPath;
    
    
    UPROPERTY()
    TArray<FName> Achievements;

    UPROPERTY()
    TArray<FStoredWeaponGroupInfo> WeaponGroups;
    
    TMap<FString, FStoredWeaponGroupInfo> WeaponGroupLookup;
    

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
