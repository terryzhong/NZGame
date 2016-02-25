// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZHUDWidget.generated.h"



/**
 * 
 */
UCLASS()
class NZGAME_API UNZHUDWidget : public UObject
{
	GENERATED_BODY()
	
public:
    virtual UWorld* GetWorld() const;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	float DesignedResolution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D ScreenPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	uint32 bScaleByDesignedResolution : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	uint32 bMaintainAspectRatio : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	float Opacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	uint32 bIgnoreHUDBaseColor : 1;

	virtual void InitializeWidget(ANZHUD* Hud);

	virtual bool IsHidden();

	UFUNCTION(BlueprintCallable, Category = Widgets)
	virtual void SetHidden(bool bIsHidden);

	UFUNCTION(BlueprintNativeEvent)
	bool ShouldDraw(bool bShowScores);

	virtual void PreDraw(float DeltaTime, ANZHUD* InNZHUDOwner, UCanvas* InCanvas, FVector2D InCanvasCenter);

	UFUNCTION(BlueprintNativeEvent)
	void Draw(float DeltaTime);

	virtual void PostDraw(float RenderedTime);
    
    /** The NZHUD that owns this widget */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZHUD* NZHUDOwner;
	
    /** Cached reference to the player controller that "owns" this Widget. Only valid during the render phase */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZPlayerController* NZPlayerOwner;
    
    /** Cached reference to NZCharacter (if any) that "owns" this widget. Only valid during the render phase */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZCharacter* NZCharacterOwner;
    
    /** Cached reference to the NZGameState for the current match. Only valid during the render phase */
    UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
    class ANZGameState* NZGameState;

	virtual UCanvas* GetCanvas();
	virtual FVector2D GetRenderPosition();
	virtual FVector2D GetRenderSize();
	virtual float GetRenderScale();

	UFUNCTION(BlueprintCallable, Category = Widgets)
	virtual FLinearColor ApplyHUDColor(FLinearColor DrawColor);

	bool bShowBounds;

	virtual FString GetClampedName(ANZPlayerState* PS, UFont* NameFont, float NameScale, float MaxWidth);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	uint32 bHidden : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	float LastRenderTime;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	FVector2D RenderPosition;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	FVector2D RenderSize;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	float RenderScale;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	FVector2D CanvasCenter;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	class UCanvas* Canvas;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	float AspectScale;

	TSharedPtr<FCanvasWordWrapper> WordWrapper;

public:
	FVector2D DrawText(FText Text, float X, float Y, UFont* Font, float TextScale = 1.0, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, ETextHorzPos::Type TextHorzAlignment = ETextHorzPos::Left, ETextVertPos::Type TextVertAlignment = ETextVertPos::Top);

	FVector2D DrawText(FText Text, float X, float Y, UFont* Font, FLinearColor OutlineColor, float TextScale = 1.0, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, ETextHorzPos::Type TextHorzAlignment = ETextHorzPos::Left, ETextVertPos::Type TextVertAlignment = ETextVertPos::Top);

	FVector2D DrawText(FText Text, float X, float Y, UFont* Font, FVector2D ShadowDirection = FVector2D(0, 0), FLinearColor ShadowColor = FLinearColor::Black, float TextScale = 1.0, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, ETextHorzPos::Type TextHorzAlignment = ETextHorzPos::Left, ETextVertPos::Type TextVertAlignment = ETextVertPos::Top);

	FVector2D DrawText(FText Text, float X, float Y, UFont* Font, FVector2D ShadowDirection = FVector2D(0, 0), FLinearColor ShadowColor = FLinearColor::Black, FLinearColor OutlineColor = FLinearColor::Black, float TextScale = 1.0, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, ETextHorzPos::Type TextHorzAlignment = ETextHorzPos::Left, ETextVertPos::Type TextVertAlignment = ETextVertPos::Top);

	FVector2D DrawText(FText Text, float X, float Y, UFont* Font, const FFontRenderInfo& RenderInfo = FFontRenderInfo(), float TextScale = 1.0, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, ETextHorzPos::Type TextHorzAlignment = ETextHorzPos::Left, ETextVertPos::Type TextVertAlignment = ETextVertPos::Top);

	UFUNCTION(BlueprintCallable, Category = "Widgets", meta = (AutoCreateRefTerm = "RenderInfo"))
	virtual FVector2D DrawText(FText Text, float X, float Y, UFont* Font, bool bDrawShadow = false, FVector2D ShadowDirection = FVector2D(0, 0), FLinearColor ShadowColor = FLinearColor::Black, bool bDrawOutline = false, FLinearColor OutlineColor = FLinearColor::Black, float TextScale = 1.0, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, ETextHorzPos::Type TextHorzAlignment = ETextHorzPos::Left, ETextVertPos::Type TextVertAlignment = ETextVertPos::Top
		, const FFontRenderInfo& RenderInfo
#if CPP
		= FFontRenderInfo()
#endif
		);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	virtual void DrawTexture(UTexture* Texture, float X, float Y, float Width, float Height, float MaterialU = 0.0f, float MaterialV = 0.0f, float MaterialUL = 1.0f, float MaterialVL = 1.0f, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, FVector2D RenderOffset = FVector2D(0.0f, 0.0f), float Rotation = 0, FVector2D RotPivot = FVector2D(0.5f, 0.5f));

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	virtual void DrawMaterial(UMaterialInterface* Material, float X, float Y, float Width, float Height, float MaterialU, float MaterialV, float MaterialUWidth, float MaterialVHeight, float DrawOpacity = 1.0f, FLinearColor DrawColor = FLinearColor::White, FVector2D RenderOffset = FVector2D(0.0f, 0.0f), float Rotation = 0, FVector2D RotPivot = FVector2D(0.5f, 0.5f));

protected:
	UFUNCTION(BlueprintCallable, Category = "Render Objects")
	virtual void DrawAllRenderObjects(float DeltaTime, FVector2D DrawOffset);

	UFUNCTION(BlueprintCallable, Category = "Rander Objects")
	virtual void RenderObj_Texture(FHUDRenderObject_Texture& TextureObject, FVector2D DrawOffset = FVector2D(0, 0));

	UFUNCTION(BlueprintCallable, Category = "Render Objects")
	virtual void RenderObj_TextureAt(FHUDRenderObject_Texture& TextureObject, float X, float Y, float Width, float Height);

	UFUNCTION(BlueprintCallable, Category = "Render Objects")
	virtual FVector2D RenderObj_Text(FHUDRenderObject_Text& TextObject, FVector2D DrawOffset = FVector2D(0, 0));

	UFUNCTION(BlueprintCallable, Category = "Render Objects")
	virtual FVector2D RenderObj_TextAt(FHUDRenderObject_Text& TextObject, float X, float Y);

	virtual float GetDrawScaleOverride();

private:
	UPROPERTY()
	TArray<UStructProperty*> RenderObjectList;

};
