// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZHUDWidget.h"
#include "NZHUDWidgetMessage.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZHUDWidgetMessage : public UNZHUDWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	FName ManagedMessageArea;
	
	

	virtual void ReceiveLocalMessage(TSubclassOf<class UNZLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject = NULL);
};
