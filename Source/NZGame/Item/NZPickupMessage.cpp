// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPickupMessage.h"



FText UNZPickupMessage::ResolveMessage_Implementation(int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
{
    return FText();
}

void UNZPickupMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
}

bool UNZPickupMessage::UseLargeFont(int32 MessageIndex) const
{
    return false;
}

bool UNZPickupMessage::ShouldCountInstances_Implementation(int32 MessageIndex) const
{
    return false;
}

