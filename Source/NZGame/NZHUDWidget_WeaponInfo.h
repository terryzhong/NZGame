// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZHUDWidget.h"
#include "NZHUDWidget_WeaponInfo.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZHUDWidget_WeaponInfo : public UNZHUDWidget
{
	GENERATED_BODY()
	
public:
    UNZHUDWidget_WeaponInfo();
    
    virtual void Draw_Implementation(float DeltaTime) override;
    virtual bool ShouldDraw_Implementation(bool bShowScores) override;
    virtual void InitializeWidget(ANZHUD* Hud) override;
    
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
    FHUDRenderObject_Texture BackgroundTexture;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
    FHUDRenderObject_Texture WeaponTexture;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
    FHUDRenderObject_Text AmmoText;
    
    UFUNCTION(BlueprintNativeEvent, Category = RenderObject)
    FText GetAmmoText();
        
private:
    int32 Ammo;
    int32 CarriedAmmo;
};
