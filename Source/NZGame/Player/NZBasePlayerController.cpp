// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZBasePlayerController.h"
#include "NZPlayerState.h"
#include "NZLocalPlayer.h"
#include "NZHUD.h"
#include "Engine/Console.h"
#include "NZTeamInfo.h"



void ANZBasePlayerController::InitPlayerState()
{
    Super::InitPlayerState();
    
    NZPlayerState = Cast<ANZPlayerState>(PlayerState);
}

void ANZBasePlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    UpdateInputMode();
}

void ANZBasePlayerController::UpdateInputMode()
{
    EInputMode::Type NewInputMode = EInputMode::EIM_None;
    
    UNZLocalPlayer* LocalPlayer = Cast<UNZLocalPlayer>(Player);
    if (LocalPlayer != NULL)
    {
        // Menus default to UI
        if (LocalPlayer->AreMenuOpen())
        {
            NewInputMode = EInputMode::EIM_UIOnly;
        }
        else if ((PlayerState && PlayerState->bOnlySpectator) ||
                 LocalPlayer->ViewportClient->ViewportConsole->ConsoleState != NAME_None)   // Console has some focus issues with UI only
        {
            NewInputMode = EInputMode::EIM_GameAndUI;
        }
        else
        {
            // Give blueprint a chance to set the input
            ANZHUD* NZHUD = Cast<ANZHUD>(MyHUD);
            if (NZHUD != NULL)
            {
                NewInputMode = NZHUD->GetInputMode();
            }
            
            // Default to game only if no other input mode is wanted
            if (NewInputMode == EInputMode::EIM_None)
            {
                NewInputMode = EInputMode::EIM_GameOnly;
            }
        }
        
        // Apply the new input if it needs to be changed
        if (NewInputMode != InputMode && NewInputMode != EInputMode::EIM_None)
        {
            InputMode = NewInputMode;
            switch (NewInputMode)
            {
                case EInputMode::EIM_GameOnly:
                    //bShowMouseCursor = false;
                    Super::SetInputMode(FInputModeGameOnly());
                    break;
                case EInputMode::EIM_GameAndUI:
                    bShowMouseCursor = true;
                    Super::SetInputMode(FInputModeGameAndUI().SetWidgetToFocus(LocalPlayer->ViewportClient->GetGameViewportWidget()));
                    break;
                case EInputMode::EIM_UIOnly:
                    bShowMouseCursor = true;
                    Super::SetInputMode(FInputModeUIOnly());
                    break;
            }
        }
    }
}

uint8 ANZBasePlayerController::GetTeamNum() const
{
    ANZPlayerState* PS = Cast<ANZPlayerState>(PlayerState);
    return (PS != NULL && PS->Team != NULL) ? PS->Team->TeamIndex : 255;
}

void ANZBasePlayerController::ClientRequireContentItemListBegin_Implementation(const FString& CloudId)
{

}

void ANZBasePlayerController::ClientRequireContentItem_Implementation(const FString& PakFile, const FString& MD5)
{

}

void ANZBasePlayerController::ClientRequireContentItemListComplete_Implementation()
{

}
