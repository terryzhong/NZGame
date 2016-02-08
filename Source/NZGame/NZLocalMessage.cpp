// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZLocalMessage.h"




float UNZLocalMessage::GetAnnouncementDelay(int32 Switch)
{
    return 0.f;
}

bool UNZLocalMessage::ShouldPlayAnnouncement(const FClientReceiveData& ClientData) const
{
    return false;
}

void UNZLocalMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
    
}

FText UNZLocalMessage::ResolveMessage_Implementation(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
    return FText();
}

float UNZLocalMessage::GetAnnouncementSpacing_Implementation(int32 Switch, const UObject* OptionalOject) const
{
    return 0.f;
}

FName UNZLocalMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject) const
{
    return NAME_None;
}

USoundBase* UNZLocalMessage::GetAnnouncementSound_Implementation(int32 Switch, const UObject* OptionalObject) const
{
    return NULL;
}

bool UNZLocalMessage::UseLargeFont(int32 MessageIndex) const
{
    return false;
}

bool UNZLocalMessage::UseMegaFont(int32 MessageIndex) const
{
    return false;
}

bool UNZLocalMessage::IsLocalForAnnouncement(const FClientReceiveData& ClientData, bool bCheckFirstPS, bool bCheckSecondPS) const
{
    return false;
}

FLinearColor UNZLocalMessage::GetMessageColor_Implementation(int32 MessageIndex) const
{
    return FLinearColor();
}

float UNZLocalMessage::GetScaleInTime_Implementation(int32 MessageIndex) const
{
    return 0.f;
}

float UNZLocalMessage::GetScaleInSize_Implementation(int32 MessageIndex) const
{
    return 0.f;
}

bool UNZLocalMessage::ShouldCountInstances_Implementation(int32 MessageIndex) const
{
    return false;
}

bool UNZLocalMessage::InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UNZLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const
{
    return false;
}

bool UNZLocalMessage::CancelByAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UNZLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const
{
    return false;
}

void UNZLocalMessage::OnAnnouncementPlayed_Implementation(int32 Switch, const UObject* OptionalObject) const
{
    
}

void UNZLocalMessage::PrecacheAnnouncements_Implementation(class UNZAnnouncer* Announcer) const
{
    
}

void UNZLocalMessage::GetArgs(FFormatNamedArguments& Args, int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
    
}

FText UNZLocalMessage::GetText(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
    return FText();
}

float UNZLocalMessage::GetLifeTime(int32 Switch) const
{
    return 0.f;
}

float UNZLocalMessage::GetAnnouncementPriority(int32 Switch) const
{
    return 0.f;
}

float UNZLocalMessage::Blueprint_GetLifeTime_Implementation(int32 Switch) const
{
    return 0.f;
}

bool UNZLocalMessage::IsConsoleMessage(int32 Switch) const
{
    return false;
}

bool UNZLocalMessage::PartiallyDuplicates(int32 Switch1, int32 Switch2, class UObject* OptionalObject1, class UObject* OptionalObject2) const
{
    return false;
}

