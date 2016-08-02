// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZLocalPlayer.h"




bool UNZLocalPlayer::AreMenuOpen()
{
    return false;
}

FString UNZLocalPlayer::GetDefaultURLOption(const TCHAR* Key) const
{
    FURL DefaultURL;
    DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);
    FString Op = DefaultURL.GetOption(Key, TEXT(""));
    FString Result;
    Op.Split(TEXT("="), NULL, &Result);
    return Result;
}

void UNZLocalPlayer::SetDefaultURLOption(const FString& Key, const FString& Value)
{
    FURL DefaultURL;
    DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);
    DefaultURL.AddOption(*FString::Printf(TEXT("%s=%s"), *Key, *Value));
    DefaultURL.SaveURLConfig(TEXT("DefaultPlayer"), *Key, GGameIni);
}


void UNZLocalPlayer::LoadProfileSettings()
{
    
}

void UNZLocalPlayer::SaveProfileSettings()
{
    
}

void UNZLocalPlayer::ClearProfileSettings()
{
    
}

void UNZLocalPlayer::OpenSpectatorWindow()
{
    
}

void UNZLocalPlayer::CloseSpectatorWindow()
{
    
}

