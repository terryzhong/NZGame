// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "NZBasePlayerController.generated.h"

UENUM(BlueprintType)
namespace EInputMode
{
    enum Type
    {
        EIM_None,
        EIM_GameOnly,
        EIM_GameAndUI,
        EIM_UIOnly,
    };
}

/**
 * 
 */
UCLASS()
class NZGAME_API ANZBasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

    UPROPERTY()
    class ANZPlayerState* NZPlayerState;
    
    void InitPlayerState();
	
    
#if !USE_SERVER
    // Called every frame
    virtual void Tick( float DeltaSeconds ) override;
    
    void UpdateInputMode();
#endif
    
    UPROPERTY()
    TEnumAsByte<EInputMode::Type> InputMode;
    
    virtual uint8 GetTeamNum() const;
    
    

	UFUNCTION(Client, Reliable)
	virtual void ClientRequireContentItemListBegin(const FString& CloudId);

	UFUNCTION(Client, Reliable)
	virtual void ClientRequireContentItem(const FString& PakFile, const FString& MD5);

	UFUNCTION(Client, Reliable)
	virtual void ClientRequireContentItemListComplete();

};
