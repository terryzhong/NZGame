// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "NZPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
	
public:
    
    /** 
     * Get CameraStyle after state based and gametype based override logic
     * Generally NZ code should always query the current camera style through this method to account for ragdoll, etc
     */
    virtual FName GetCameraStyleWithOverrides() const;
	
    /** 85
     Rate player as camera focus for spectating */
    virtual float RatePlayerCamera(class ANZPlayerState* InPS, class ANZCharacter* Character, APlayerState* CurrentCamPS);
    
    
};
