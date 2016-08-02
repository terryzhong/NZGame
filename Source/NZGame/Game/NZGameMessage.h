// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZLocalMessage.h"
#include "NZGameMessage.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZGameMessage : public UNZLocalMessage
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    FText GameBeginsMessage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    FText YouAreOnRed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    FText YouAreOnBlue;
	
    virtual FText GetText(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const override;
};
