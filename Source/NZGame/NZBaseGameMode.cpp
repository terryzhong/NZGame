// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZBaseGameMode.h"




ANZBaseGameMode::ANZBaseGameMode()
{
    ReplaySpectatorPlayerControllerClass = NULL;
}

void ANZBaseGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
    
/*    ANZGameSession* NZGameSession = Cast<ANZGameSession>(GameSession);
    if (ErrorMessage.IsEmpty() && NZGameSession)
    {
        bool bJoinAsSpectator = FCString::Stricmp(*UGameplayStatics::ParseOption(Options, TEXT("SpectatorOnly")), TEXT("1")) == 0;
        NZGameSession->ValidatePlayer(Address, UniqueId, ErrorMessage, bJoinAsSpectator);
    }*/
    
}

APlayerController* ANZBaseGameMode::Login(class UPlayer* NewPlayer, ENetRole RemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
    return Super::Login(NewPlayer, RemoteRole, Portal, Options, UniqueId, ErrorMessage);
    
}

void ANZBaseGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    
}

void ANZBaseGameMode::GenericPlayerInitialization(AController* C)
{
    Super::GenericPlayerInitialization(C);
    
}

void ANZBaseGameMode::PostInitProperties()
{
    Super::PostInitProperties();
    
}

void ANZBaseGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    if (!PlayerPawnObject.IsNull())
    {
        DefaultPawnClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *PlayerPawnObject.ToStringReference().ToString(), NULL, LOAD_NoWarn));
    }
    
    Super::InitGame(MapName, Options, ErrorMessage);
    
}
