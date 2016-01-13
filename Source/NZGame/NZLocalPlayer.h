// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LocalPlayer.h"
#include "NZLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
	
public:
    
    virtual bool AreMenuOpen();
	
    
    
    virtual void LoadProfileSettings();
    UFUNCTION()
    virtual void SaveProfileSettings();
    virtual void ClearProfileSettings();
    
    virtual class UNZProfileSettings* GetProfileSettings() { return CurrentProfileSetttings; }
    
    //virtual void SetNickname(FString NewName);
	
protected:
    
    UPROPERTY()
    class UNZProfileSettings* CurrentProfileSetttings;
    
    
    
};
