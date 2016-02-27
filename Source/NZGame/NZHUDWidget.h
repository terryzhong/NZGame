// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZHUDWidget.generated.h"



struct FNZCanvasTextItem : public FCanvasTextItem
{
    FNZCanvasTextItem(const FVector2D& InPosition, const FText& InText, class UFont* InFont, const FLinearColor& InColor, const TSharedPtr<FCanvasWordWrapper> InWordWrapper = NULL)
        : FCanvasTextItem(InPosition, InText, InFont, InColor)
        , CharIncrement(0.0f)
        , WrapXL(0.0f)
        , WordWrapper(InWordWrapper)
    {
    }
    
    virtual void Draw(class FCanvas* InCanvas) override;
    
    float CharIncrement;
    
    // Word wrap size, if render info has !bClipText
    float WrapXL;
    
    TSharedPtr<FCanvasWordWrapper> WordWrapper;
    
protected:
    // NZ version appropriately handles distance field fonts by slightly overlapping triangles to give the shadows more space
    void NZDrawStringInternal(class FCanvas* InCanvas, const FVector2D& DrawPos, const FLinearColor& DrawColor);
    
private:
    void DrawStringInternal(class FCanvas* InCanvas, const FVector2D& DrawPos, const FLinearColor& DrawColor)
    {
        NZDrawStringInternal(InCanvas, DrawPos, DrawColor);
    }
};

namespace ERenderObjectType
{
	extern const FName TextureObject;
	extern const FName TextObject;
}

class ANZHUD;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class NZGAME_API UNZHUDWidget : public UObject
{
	GENERATED_BODY()
	
public:
    virtual UWorld* GetWorld() const;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	float DesignedResolution;

	/** This is the position of the widget relative to the screen position and origin */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D Position;

	/** The size of the widget as designed. NOTE: Currently no clipping occurs so it's very possible to blow out beyond the bounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D Size;

	/** The widget's position is relative to this origin. Normalized */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D Origin;

	/**
	 * Holds widget's normalized position relative to the actual display
	 * Useful for quickly snapping the widget to the right or bottom edge and can be used with Negative position
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	FVector2D ScreenPosition;

	/** If true, then this widget will attempt to scale itself by to the designed resolution */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	uint32 bScaleByDesignedResolution : 1;

	/** If true, any scaling will maintain the aspect ratio of the widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	uint32 bMaintainAspectRatio : 1;

	/** The opacity of this widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	float Opacity;

	/** Whether to colorize this widget using base HUD color (like team color) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	uint32 bIgnoreHUDBaseColor : 1;

	/** Will be called as soon as the widget is created and tracked by the hud */
	virtual void InitializeWidget(ANZHUD* Hud);

	/** Hide/show this widget */
	virtual bool IsHidden();

	/** Sets the visibility of this widget */
	UFUNCTION(BlueprintCallable, Category = Widgets)
	virtual void SetHidden(bool bIsHidden);

	/** Return true if this widget should be drawn */
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldDraw(bool bShowScores);

	/** Predraw is called before the main drawing function in order to perform any needed scaling / positioning / etc as well as cache the canvas and owner */
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

	/**  */
	UFUNCTION(BlueprintCallable, Category = Widgets)
	virtual FLinearColor ApplyHUDColor(FLinearColor DrawColor);

	bool bShowBounds;

	virtual FString GetClampedName(ANZPlayerState* PS, UFont* NameFont, float NameScale, float MaxWidth);

protected:
	/** If true, this widget will not be rendered */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	uint32 bHidden : 1;

	/** The last time this widget was rendered */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	float LastRenderTime;

	/** RenderPosition is only valid during the rendering potion and holds the position in screen space at the current resolution */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	FVector2D RenderPosition;

	/** Scaled sizing. NOTE: respects bMaintainAspectRatio */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	FVector2D RenderSize;
	
	/** The scale based on the designed resolution */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	float RenderScale;

	/** The precalculated center of the canvas */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	FVector2D CanvasCenter;

	/** The cached reference to the UCanvas object. ONLY VALID during the render phase */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets Live")
	class UCanvas* Canvas;

	/** The scale needed to maintain aspect ratio */
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

	/** Get any scaling overrides */
	virtual float GetDrawScaleOverride();

private:
	/** This is a sorted list of all RenderObject in this widget */
	UPROPERTY()
	TArray<UStructProperty*> RenderObjectList;

};
