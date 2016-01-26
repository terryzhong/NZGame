// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "NZBasePlayerController.h"
#include "NZHUD.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    class ANZPlayerController* NZPlayerController;
    
    UPROPERTY(Transient)
    TArray<class UNZHUDWidget*> HudWidgets;
    
    
public:
    UFUNCTION(BlueprintNativeEvent)
    EInputMode::Type GetInputMode() const;
	
	
    virtual float GetCrosshairScale();
    virtual FLinearColor GetCrosshairColor(FLinearColor InColor) const;
};
