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
	
    
    UPROPERTY(BlueprintReadWrite, Category = HUD)
    float LastPickupTime;
    
    /** Last time player owning this HUD killed someone */
    UPROPERTY(BlueprintReadWrite, Category = HUD)
    float LastKillTime;
    
    /** Last time player owning this HUD picked up a flag */
    UPROPERTY(BlueprintReadWrite, Category = HUD)
    float LastFlagGrabTime;
    
    /** Sound played when player gets a kill */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Message)
    USoundBase* KillSound;
    
    
    /** 181
     Last time we hit an enemy in LOS */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    float LastConfirmedHitTime;
    
    
    
    virtual float GetCrosshairScale();
    virtual FLinearColor GetCrosshairColor(FLinearColor InColor) const;
    
    /** Returns the necessary input mode for the hud this tick */
    UFUNCTION(BlueprintNativeEvent)
    EInputMode::Type GetInputMode() const;
    
};
