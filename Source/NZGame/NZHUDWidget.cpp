// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUDWidget.h"



namespace ERenderObjectType
{
	const FName TextureObject = FName(TEXT("HUDRenderObject_Texture"));
	const FName TextObject = FName(TEXT("HUDRenderObject_Text"));
}

UWorld* UNZHUDWidget::GetWorld() const
{
	return NZHUDOwner ? NZHUDOwner->GetWorld() : NULL;
}

void UNZHUDWidget::InitializeWidget(ANZHUD* Hud)
{
	for (TFieldIterator<UStructProperty> PropIt(GetClass()); PropIt; ++PropIt)
    {
		UStructProperty* Prop = NULL;
		float PropRenderPriority = 0.0;

		const FName CPPName = PropIt->Struct->GetFName();
		if (CPPName == ERenderObjectType::TextureObject)
		{
			// Get the object
			Prop = *PropIt;
			FHUDRenderObject_Texture* PtrToProperty = Prop->ContainerPtrToValuePtr<FHUDRenderObject_Texture>(this, 0);
			if (PtrToProperty)
			{
				PropRenderPriority = PtrToProperty->RenderPriority;
			}
		}
		if (CPPName == ERenderObjectType::TextObject)
		{
			// Get the object
			Prop = *PropIt;
			FHUDRenderObject_Text* PtrToProperty = Prop->ContainerPtrToValuePtr<FHUDRenderObject_Text>(this, 0);
			if (PtrToProperty)
			{
				PropRenderPriority = PtrToProperty->RenderPriority;
			}
		}

		if (Prop)
		{
			int32 InsertPosition = -1;
			for (int32 i = 0; i < RenderObjectList.Num(); i++)
			{
				FHUDRenderObject* ObjPtr = RenderObjectList[i]->ContainerPtrToValuePtr<FHUDRenderObject>(this, 0);
				if (ObjPtr)
				{
					if (PropRenderPriority < ObjPtr->RenderPriority)
					{
						InsertPosition = i;
						break;
					}
				}
			}

			if (InsertPosition >= 0)
			{
				RenderObjectList.Insert(Prop, InsertPosition);
			}
			else
			{
				RenderObjectList.Add(Prop);
			}
		}
    }
}

bool UNZHUDWidget::IsHidden()
{
	return bHidden;
}

void UNZHUDWidget::SetHidden(bool bIsHidden)
{
	bHidden = bIsHidden;
}

bool UNZHUDWidget::ShouldDraw_Implementation(bool bShowScores)
{
	return NZGameState && NZGameState->HasMatchStarted() && (NZCharacterOwner && !NZCharacterOwner->IsDead()) && !bShowScores;
}

void UNZHUDWidget::PreDraw(float DeltaTime, ANZHUD* InNZHUDOwner, UCanvas* InCanvas, FVector2D InCanvasCenter)
{
	NZHUDOwner = InNZHUDOwner;
	if (NZHUDOwner != NULL)
	{
		NZPlayerOwner = NZHUDOwner->NZPlayerOwner;
		if (NZPlayerOwner != NULL)
		{
			ANZPlayerState* PS = NZPlayerOwner->NZPlayerState;
			static const FName NAME_FreeCam = FName(TEXT("FreeCam"));
			NZCharacterOwner = (PS && PS->bOnlySpectator && NZPlayerOwner->PlayerCameraManager && (NZPlayerOwner->PlayerCameraManager->CameraStyle == NAME_FreeCam)) 
				? NULL
				: Cast<ANZCharacter>(NZPlayerOwner->GetViewTarget());
		}
		NZGameState = NZHUDOwner->GetWorld()->GetGameState<ANZGameState>();
	}

	Canvas = InCanvas;
	CanvasCenter = InCanvasCenter;
	AspectScale = Size.Y > 0 ? Size.X / Size.Y : 1.0;

	// Figure out the initial position
	RenderPosition.X = Canvas->ClipX * ScreenPosition.X;
	RenderPosition.Y = Canvas->ClipY * ScreenPosition.Y;
	RenderScale = (bScaleByDesignedResolution) ? Canvas->ClipY / DesignedResolution : 1.0f;
	RenderScale *= GetDrawScaleOverride();

	// Apply any scaling
	RenderSize.Y = Size.Y * RenderScale;
	if (Size.X > 0)
	{
		RenderSize.X = (bMaintainAspectRatio ? RenderSize.Y * AspectScale : RenderSize.X * RenderScale);
	}
	RenderPosition.X += (Position.X * RenderScale) - (RenderSize.X * Origin.X);
	RenderPosition.Y += (Position.Y * RenderScale) - (RenderSize.Y * Origin.Y);
}

void UNZHUDWidget::Draw_Implementation(float DeltaTime)
{
	DrawAllRenderObjects(DeltaTime, FVector2D(0, 0));
}

void UNZHUDWidget::PostDraw(float RenderedTime)
{
	if (bShowBounds)
	{
		Canvas->SetDrawColor(255, 255, 0, 255);
		Canvas->K2_DrawBox(RenderPosition, RenderSize, 1.0);
	}

	LastRenderTime = RenderedTime;
	Canvas = NULL;
	NZCharacterOwner = NULL;
}

UCanvas* UNZHUDWidget::GetCanvas()
{
    return Canvas;
}

FVector2D UNZHUDWidget::GetRenderPosition()
{
    return RenderPosition;
}

FVector2D UNZHUDWidget::GetRenderSize()
{
    return RenderSize;
}

float UNZHUDWidget::GetRenderScale()
{
    return RenderScale;
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

void UNZHUDWidget::DrawTexture(UTexture* Texture, float X, float Y, float Width, float Height, float U, float V, float UL, float VL, float DrawOpacity, FLinearColor DrawColor, FVector2D RenderOffset, float Rotation, FVector2D RotPivot)
{
	float ImageAspectScale = Height > 0 ? Width / Height : 0.0f;

	if (Texture && Texture->Resource)
	{
		if (bScaleByDesignedResolution)
		{
			X *= RenderScale;
			Y *= RenderScale;

			Height = Height * RenderScale;
			Width = (bMaintainAspectRatio) ? (Height * ImageAspectScale) : Width * RenderScale;
		}

		FVector2D RenderPos = FVector2D(RenderPosition.X + X - (Width * RenderOffset.X), RenderPosition.Y + Y - (Height * RenderOffset.Y));

		U = U / Texture->Resource->GetSizeX();
		V = V / Texture->Resource->GetSizeY();
		UL = U + (UL / Texture->Resource->GetSizeX());
		VL = V + (VL / Texture->Resource->GetSizeY());

		DrawColor.A = Opacity * DrawOpacity * NZHUDOwner->WidgetOpacity;

		FCanvasTileItem ImageItem(RenderPos, Texture->Resource, FVector2D(Width, Height), FVector2D(U, V), FVector2D(UL, VL), DrawColor);
		ImageItem.Rotation = FRotator(0, Rotation, 0);
		ImageItem.PivotPoint = RotPivot;
		ImageItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		Canvas->DrawItem(ImageItem);
	}
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
	return NZHUDOwner ? NZHUDOwner->HUDWidgetScaleOverride : 1.0;
}
