// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameMessage.h"



FText UNZGameMessage::GetText(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
    switch (Switch)
    {
        case 0:
            return GetDefault<UNZGameMessage>(GetClass())->GameBeginsMessage;
            break;
            
        case 9:
            return GetDefault<UNZGameMessage>(GetClass())->YouAreOnRed;
            break;
           
        case 10:
            return GetDefault<UNZGameMessage>(GetClass())->YouAreOnBlue;
            break;
            
        default:
            return FText::GetEmpty();
    }
}
