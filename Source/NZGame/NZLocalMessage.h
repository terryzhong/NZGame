// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/LocalMessage.h"
#include "NZLocalMessage.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract, NotPlaceable)
class NZGAME_API UNZLocalMessage : public ULocalMessage
{
	GENERATED_BODY()
	
public:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
    FName MessageArea;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
    FName StyleTag;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Message)
    uint32 bIsStatusAnnouncement : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    uint32 bIsSpecial : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    uint32 bIsUnique : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    uint32 bIsPartiallyUnique : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    uint32 bIsConsoleMessage : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    uint32 bOptionalSpoken : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    uint32 bWantsBotReaction : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    float Lefttime;
    
    UPROPERTY(EditDefaultsOnly, Category = Message)
    float AnnouncementDelay;
    
    virtual float GetAnnouncementDelay(int32 Switch);
    
    virtual bool ShouldPlayAnnouncement(const FClientReceiveData& ClientData) const;
    
    virtual void ClientReceive(const FClientReceiveData& ClientData) const override;
    
    UFUNCTION(BlueprintImplementableEvent)
    void OnClientReceive(APlayerController* LocalPC, int32 Switch, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const;
    
    UFUNCTION(BlueprintNativeEvent)
    FText ResolveMessage(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const;
    
	UFUNCTION(BlueprintNativeEvent)
    float GetAnnouncementSpacing(int32 Switch, const UObject* OptionalOject) const;
    
    UFUNCTION(BlueprintNativeEvent)
    FName GetAnnouncementName(int32 Switch, const UObject* OptionalObject) const;
    
    UFUNCTION(BlueprintNativeEvent)
    USoundBase* GetAnnouncementSound(int32 Switch, const UObject* OptionalObject) const;
    
    virtual bool UseLargeFont(int32 MessageIndex) const;
    
    virtual bool UseMegaFont(int32 MessageIndex) const;
    
    virtual bool IsLocalForAnnouncement(const FClientReceiveData& ClientData, bool bCheckFirstPS, bool bCheckSecondPS) const;
    
    UFUNCTION(BlueprintNativeEvent)
    FLinearColor GetMessageColor(int32 MessageIndex) const;
    
    UFUNCTION(BlueprintNativeEvent)
    float GetScaleInTime(int32 MessageIndex) const;
    
    UFUNCTION(BlueprintNativeEvent)
    float GetScaleInSize(int32 MessageIndex) const;
    
    UFUNCTION(BlueprintNativeEvent)
    bool ShouldCountInstances(int32 MessageIndex) const;
    
    UFUNCTION(BlueprintNativeEvent)
    bool InterruptAnnouncement(int32 Switch, const UObject* OptionalObject, TSubclassOf<UNZLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const;
    
    UFUNCTION(BlueprintNativeEvent)
    bool CancelByAnnouncement(int32 Switch, const UObject* OptionalObject, TSubclassOf<UNZLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const;
    
    UFUNCTION(BlueprintNativeEvent)
    void OnAnnouncementPlayed(int32 Switch, const UObject* OptionalObject) const;
    
    UFUNCTION(BlueprintNativeEvent)
    void PrecacheAnnouncements(class UNZAnnouncer* Announcer) const;
	
    virtual void GetArgs(FFormatNamedArguments& Args, int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const;
    
    virtual FText GetText(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const;
    
    virtual float GetLifeTime(int32 Switch) const;
    
    virtual float GetAnnouncementPriority(int32 Switch) const;
    
    UFUNCTION(BlueprintNativeEvent)
    float Blueprint_GetLifeTime(int32 Switch) const;
    
    virtual bool IsConsoleMessage(int32 Switch) const;
    bool PartiallyDuplicates(int32 Switch1, int32 Switch2, class UObject* OptionalObject1, class UObject* OptionalObject2) const;
};
