// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "NZBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZBaseGameMode : public AGameMode
{
	GENERATED_BODY()
    
protected:
    FTimerHandle TimerHandle_DefaultTime;
    
    uint32 LobbyInstanceID;
    
    void CreateServerID();
	
public:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable, Category = Came)
    FText DisplayName;
    
    virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
    virtual APlayerController* Login(class UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void GenericPlayerInitialization(AController* C);
    
    virtual void PostInitProperties();
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    
    UPROPERTY(GlobalConfig)
    FString ServerInstanceID;
    
    FGuid ServerInstanceGUID;
    
    virtual bool IsGameInstanceServer() { return LobbyInstanceID > 0; }
    virtual bool IsLobbyServer() { return false; }
    
    
	
	
};
