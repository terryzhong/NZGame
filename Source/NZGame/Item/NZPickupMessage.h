// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZLocalMessage.h"
#include "NZPickupMessage.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZPickupMessage : public UNZLocalMessage
{
	GENERATED_BODY()
	
public:
    virtual FText ResolveMessage_Implementation(int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const override;
    virtual void ClientReceive(const FClientReceiveData& ClientData) const override;
    virtual bool UseLargeFont(int32 MessageIndex) const override;
    virtual bool ShouldCountInstances_Implementation(int32 MessageIndex) const override;
};
