// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZHUDWidget : public UObject
{
	GENERATED_BODY()
	
public:
    //virtual UWorld* GetWorld() const;
    
    
    /** The NZHUD that owns this widget */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZHUD* NZHUDOwner;
	
    /** Cached reference to the player controller that "owns" this Widget. Only valid during the render phase */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZPlayerController* NZPlayerOwner;
    
    /** Cached reference to NZCharacter (if any) that "owns" this widget. Only valid during the render phase */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZCharacter* NZCharacterOwner;
    
    /** Cached reference to the NZGameState for the current match. Only valid during the render phase */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZGameState* NZGameState;
};
