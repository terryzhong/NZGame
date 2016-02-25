// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUDWidget.h"



UWorld* UNZHUDWidget::GetWorld() const
{
    return NULL;
}

void UNZHUDWidget::InitializeWidget(ANZHUD* Hud)
{
    
}

bool UNZHUDWidget::IsHidden()
{
    return false;
}

void UNZHUDWidget::SetHidden(bool bIsHidden)
{
    
}

bool UNZHUDWidget::ShouldDraw_Implementation(bool bShowScores)
{
    return false;
}

void UNZHUDWidget::PreDraw(float DeltaTime, ANZHUD* InNZHUDOwner, UCanvas* InCanvas, FVector2D InCanvasCenter)
{
    
}

void UNZHUDWidget::Draw_Implementation(float DeltaTime)
{
    
}

void UNZHUDWidget::PostDraw(float RenderedTime)
{
    
}

UCanvas* UNZHUDWidget::GetCanvas()
{
    return NULL;
}

FVector2D UNZHUDWidget::GetRenderPosition()
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::GetRenderSize()
{
    return FVector2D(0, 0);
}

float UNZHUDWidget::GetRenderScale()
{
    return 0.f;
}

FLinearColor UNZHUDWidget::ApplyHUDColor(FLinearColor DrawColor)
{
    return FLinearColor();
}

FString UNZHUDWidget::GetClampedName(ANZPlayerState* PS, UFont* NameFont, float NameScale, float MaxWidth)
{
    return FString();
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, FLinearColor OutlineColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, FVector2D ShadowDirection, FLinearColor ShadowColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, FVector2D ShadowDirection, FLinearColor ShadowColor, FLinearColor OutlineColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, const FFontRenderInfo& RenderInfo, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, bool bDrawShadow, FVector2D ShadowDirection, FLinearColor ShadowColor, bool bDrawOutline, FLinearColor OutlineColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment, const FFontRenderInfo& RenderInfo)
{
    return FVector2D(0, 0);
}

void UNZHUDWidget::DrawTexture(UTexture* Texture, float X, float Y, float Width, float Height, float MaterialU, float MaterialV, float MaterialUL, float MaterialVL, float DrawOpacity, FLinearColor DrawColor, FVector2D RenderOffset, float Rotation, FVector2D RotPivot)
{
    
}

void UNZHUDWidget::DrawMaterial(UMaterialInterface* Material, float X, float Y, float Width, float Height, float MaterialU, float MaterialV, float MaterialUWidth, float MaterialVHeight, float DrawOpacity, FLinearColor DrawColor, FVector2D RenderOffset, float Rotation, FVector2D RotPivot)
{
    
}

void UNZHUDWidget::DrawAllRenderObjects(float DeltaTime, FVector2D DrawOffset)
{
    
}

void UNZHUDWidget::RenderObj_Texture(FHUDRenderObject_Texture& TextureObject, FVector2D DrawOffset)
{
    
}

void UNZHUDWidget::RenderObj_TextureAt(FHUDRenderObject_Texture& TextureObject, float X, float Y, float Width, float Height)
{
    
}

FVector2D UNZHUDWidget::RenderObj_Text(FHUDRenderObject_Text& TextObject, FVector2D DrawOffset)
{
    return FVector2D(0, 0);
}

FVector2D UNZHUDWidget::RenderObj_TextAt(FHUDRenderObject_Text& TextObject, float X, float Y)
{
    return FVector2D(0, 0);
}

float UNZHUDWidget::GetDrawScaleOverride()
{
    return 0.f;
}
