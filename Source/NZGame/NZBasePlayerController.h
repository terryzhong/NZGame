// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "NZBasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZBasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
    UPROPERTY()
    class ANZPlayerState* NZPlayerState;
    
    
	
	
	
};
