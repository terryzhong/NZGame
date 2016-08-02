// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZHUDWidget.h"
#include "NZHUDWidget_PlayerInfo.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZHUDWidget_PlayerInfo : public UNZHUDWidget
{
	GENERATED_BODY()
	
public:
	UNZHUDWidget_PlayerInfo();

	virtual void Draw_Implementation(float DeltaTime) override;
	virtual bool ShouldDraw_Implementation(bool bShowScores) override;
	virtual void InitializeWidget(ANZHUD* Hud) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
	FHUDRenderObject_Texture BackgroundTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
	FHUDRenderObject_Texture HealthTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
	FHUDRenderObject_Texture TeamTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
	FHUDRenderObject_Texture RoleTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
	FHUDRenderObject_Text HealthText;

	UFUNCTION(BlueprintNativeEvent, Category = RenderObject)
	FText GetHealthText();

private:
	int32 Health;
};
