// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZProfileSettings.h"



void UNZProfileSettings::ClearWeaponPriorities()
{
}

void UNZProfileSettings::SetWeaponPriority(FString WeaponClassName, float NewPriority)
{
	for (int32 i = 0; i < WeaponPriorities.Num(); i++)
	{
		if (WeaponPriorities[i].WeaponClassName == WeaponClassName)
		{
			if (WeaponPriorities[i].WeaponPriority != NewPriority)
			{
				WeaponPriorities[i].WeaponPriority = NewPriority;
			}
			return;
		}
	}

	WeaponPriorities.Add(FStoredWeaponPriority(WeaponClassName, NewPriority));
}

float UNZProfileSettings::GetWeaponPriority(FString WeaponClassName, float DefaultPriority)
{
	for (int32 i = 0; i < WeaponPriorities.Num(); i++)
	{
		if (WeaponPriorities[i].WeaponClassName == WeaponClassName)
		{
			return WeaponPriorities[i].WeaponPriority;
		}
	}

	return DefaultPriority;
}

void UNZProfileSettings::UpdateCrosshairs(ANZHUD* HUD)
{
    HUD->LoadedCrosshairs.Empty();
    HUD->CrosshairInfos = CrosshairInfos;
    HUD->bCustomWeaponCrosshairs = bCustomWeaponCrosshairs;
    HUD->SaveConfig();
}

