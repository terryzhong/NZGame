// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameViewportClient.h"
#include "NZGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
	
public:
    
    /** 
     * Panini project the given location using the player's view 
     * PaniniParamsMat will be used to grab parameters for the projection if available, otherwise reasonable default values are used
     */
    FVector PaniniProjectLocation(const FSceneView* SceneView, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat = NULL) const;
    
    /** Calls PaniniProjectLocation() with a SceneView representing the player's view (slower, don't use if SceneView is already avaliable) */
    FVector PaniniProjectLocationForPlayer(ULocalPlayer* Player, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat = NULL) const;
	
};
