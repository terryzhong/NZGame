// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "NZGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

	Scalability::FQualityLevels ScalabilityQuality;
	
	
};
