// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUDWidget.h"



namespace ERenderObjectType
{
	const FName TextureObject = FName(TEXT("HUDRenderObject_Texture"));
	const FName TextObject = FName(TEXT("HUDRenderObject_Text"));
}

void FNZCanvasTextItem::Draw(class FCanvas* InCanvas)
{
    //SCOPE_CYCLE_COUNTER(STAT_Canvas_TextItemTime);
    
    if (InCanvas == NULL || Font == NULL || Text.IsEmpty())
    {
        return;
    }
    
    if (!FontRenderInfo.bClipText && WordWrapper.IsValid())
    {
        // Word warp
        FTextSizingParameters Parms(Font, Scale.X, Scale.Y);
        Parms.DrawXL = (WrapXL > 0.0f) ? WrapXL : (InCanvas->GetRenderTarget()->GetSizeXY().X - Position.X);
        Parms.DrawYL = InCanvas->GetRenderTarget()->GetSizeXY().Y;
        TArray<FWrappedStringElement> Lines;
        
        UCanvas::WrapString(*WordWrapper, Parms, 0, *Text.ToString(), Lines);
        
        FString NewText;
        for (int32 i = 0; i < Lines.Num(); i++)
        {
            if (i > 0)
            {
                NewText += TEXT("\n");
            }
            NewText += Lines[i].Value;
        }
        Text = FText::FromString(NewText);
    }
    
    bool bHasShadow = ShadowOffset.Size() != 0.0f;
    if (FontRenderInfo.bEnableShadow && !bHasShadow)
    {
        EnableShadow(FLinearColor::Black);
        bHasShadow = true;
    }
    if (Font->ImportOptions.bUseDistanceFieldAlpha)
    {
        // Convert blend mode to distance field type
        switch (BlendMode)
        {
            case SE_BLEND_Translucent:
                BlendMode = (bHasShadow || FontRenderInfo.bEnableShadow) ? SE_BLEND_TranslucentDistanceFieldShadowed : SE_BLEND_TranslucentDistanceField;
                break;
            case SE_BLEND_Masked:
                BlendMode = (bHasShadow || FontRenderInfo.bEnableShadow) ? SE_BLEND_MaskedDistanceFieldShadowed : SE_BLEND_MaskedDistanceField;
                break;
        }
        
        // We don't need to use the double draw method because the shader will do it better
        FontRenderInfo.bEnableShadow = true;
        bHasShadow = false;
    }
    
    CharIncrement = ((float)Font->Kerning + HorizSpacingAdjust) * Scale.X;
    DrawnSize.Y = Font->GetMaxCharHeight() * Scale.Y;
    
    FVector2D DrawPos(Position.X, Position.Y);
    
    // If we are centering the string or we want to fix stereoscopic rending issues we need to measure the string
    if ((bCentreX || bCentreY) || (!bDontCorrectStereoscopic))
    {
        FTextSizingParameters Parameters(Font, Scale.X, Scale.Y);
        UCanvas::CanvasStringSize(Parameters, *Text.ToString());
        
        // Calculate the offset if we are centering
        if (bCentreX || bCentreY)
        {
            // Note we drop the fraction after the length divide or we can end up with coords on 1/2 pixel boundaries
            if (bCentreX)
            {
                DrawPos.X = DrawPos.X - (int32)(Parameters.DrawXL / 2);
            }
            if (bCentreY)
            {
                DrawPos.Y = DrawPos.Y - (int32)(Parameters.DrawYL / 2);
            }
        }
        
        // Check if we want to correct the stereo3d issues - if we do, render the correction now
        bool CorrectStereo = !bDontCorrectStereoscopic && GEngine->IsStereoscopic3D();
        if (CorrectStereo)
        {
            FVector2D StereoOutlineBoxSize(2.0f, 2.0f);
            TileItem.MaterialRenderProxy = GEngine->RemoveSurfaceMaterial->GetRenderProxy(false);
            TileItem.Position = DrawPos - StereoOutlineBoxSize;
            FVector2D CorrectionSize = FVector2D(Parameters.DrawXL, Parameters.DrawYL) + StereoOutlineBoxSize + StereoOutlineBoxSize;
            TileItem.Size = CorrectionSize;
            TileItem.bFreezeTime = true;
            TileItem.Draw(InCanvas);
        }
    }
    
    FLinearColor DrawColor;
    BatchedElements = NULL;
    if (bOutlined && !FontRenderInfo.GlowInfo.bEnableGlow)  // Efficient distance field glow takes priority
    {
        DrawColor = OutlineColor;
        DrawColor.A *= InCanvas->AlphaModulate;
        NZDrawStringInternal(InCanvas, DrawPos + FVector2D(-1.0f, -1.0f), DrawColor);
        NZDrawStringInternal(InCanvas, DrawPos + FVector2D(-1.0f,  1.0f), DrawColor);
        NZDrawStringInternal(InCanvas, DrawPos + FVector2D( 1.0f,  1.0f), DrawColor);
        NZDrawStringInternal(InCanvas, DrawPos + FVector2D( 1.0f, -1.0f), DrawColor);
    }
    
    // If we have a shadow - draw it now
    if (bHasShadow)
    {
        DrawColor = ShadowColor;
        // Copy the Alpha from the shadow otherwise if we fade the text the shadow wont fade - which is almost certainly not what we will want
        DrawColor.A = Color.A;
        DrawColor.A *= InCanvas->AlphaModulate;
        NZDrawStringInternal(InCanvas, DrawPos + ShadowOffset, DrawColor);
    }
    DrawColor = Color;
    DrawColor.A *= InCanvas->AlphaModulate;
    
    // TODO: We need to pass the shadow color and direction in the distance field case... (currently engine support is missing)
    NZDrawStringInternal(InCanvas, DrawPos, DrawColor);
}

void FNZCanvasTextItem::NZDrawStringInternal(class FCanvas* InCanvas, const FVector2D& DrawPos, const FLinearColor& DrawColor)
{
    if (GetFontCacheType() == EFontCacheType::Runtime)
    {
        BlendMode = SE_BLEND_TranslucentAlphaOnly;
        DrawStringInternal_RuntimeCache(InCanvas, DrawPos, DrawColor);
        return;
    }
    
    DrawnSize = FVector2D(EForceInit::ForceInitToZero);
    FVector2D CurrentPos = FVector2D(EForceInit::ForceInitToZero);
    FHitProxyId HitProxyId = InCanvas->GetHitProxyId();
    FTexture* LastTexture = NULL;
    UTexture2D* Tex = NULL;
    FVector2D InvTextureSize(1.0f, 1.0f);
    const TArray<TCHAR>& Chars = Text.ToString().GetCharArray();
    
    float ExtraXSpace = 0.0f;
    float ExtraYSpace = 0.0f;
    if (Font->ImportOptions.bUseDistanceFieldAlpha)
    {
        if (FontRenderInfo.bEnableShadow || BlendMode == SE_BLEND_MaskedDistanceFieldShadowed || BlendMode == SE_BLEND_TranslucentDistanceFieldShadowed)
        {
            ExtraXSpace = 2.0f;
            ExtraYSpace = 2.0f;
        }
        if (FontRenderInfo.GlowInfo.bEnableGlow)
        {
            float GlowRadius = Font->ImportOptions.Height * 0.5;
            float EdgeRadius = FMath::Min<float>(FMath::Min<float>(FontRenderInfo.GlowInfo.GlowInnerRadius.X, FontRenderInfo.GlowInfo.GlowInnerRadius.Y),
                                                 FMath::Min<float>(FontRenderInfo.GlowInfo.GlowOuterRadius.X, FontRenderInfo.GlowInfo.GlowOuterRadius.Y));
            if (EdgeRadius < 0.5f)
            {
                GlowRadius = FMath::CeilToFloat(GlowRadius * (0.5f - EdgeRadius) * 2.0f);
                ExtraXSpace = FMath::Max<float>(ExtraXSpace, GlowRadius);
                ExtraYSpace = FMath::Max<float>(ExtraYSpace, GlowRadius);
            }
        }
    }
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
    return DrawText(Text, X, Y, Font, false, FVector2D::ZeroVector, FLinearColor::Black, false, FLinearColor::Black, TextScale, DrawOpacity, DrawColor, TextHorzAlignment, TextVertAlignment);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, FLinearColor OutlineColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return DrawText(Text, X, Y, Font, false, FVector2D::ZeroVector, FLinearColor::Black, true, OutlineColor, TextScale, DrawOpacity, DrawColor, TextHorzAlignment, TextVertAlignment);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, FVector2D ShadowDirection, FLinearColor ShadowColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return DrawText(Text, X, Y, Font, true, ShadowDirection, ShadowColor, false, FLinearColor::Black, TextScale, DrawOpacity, DrawColor, TextHorzAlignment, TextVertAlignment);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, FVector2D ShadowDirection, FLinearColor ShadowColor, FLinearColor OutlineColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return DrawText(Text, X, Y, Font, true, ShadowDirection, ShadowColor, true, OutlineColor, TextScale, DrawOpacity, DrawColor, TextHorzAlignment, TextVertAlignment);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, const FFontRenderInfo& RenderInfo, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment)
{
    return DrawText(Text, X, Y, Font, false, FVector2D::ZeroVector, FLinearColor::Black, false, FLinearColor::Black, TextScale, DrawOpacity, DrawColor, TextHorzAlignment, TextVertAlignment, RenderInfo);
}

FVector2D UNZHUDWidget::DrawText(FText Text, float X, float Y, UFont* Font, bool bDrawShadow, FVector2D ShadowDirection, FLinearColor ShadowColor, bool bDrawOutline, FLinearColor OutlineColor, float TextScale, float DrawOpacity, FLinearColor DrawColor, ETextHorzPos::Type TextHorzAlignment, ETextVertPos::Type TextVertAlignment, const FFontRenderInfo& RenderInfo)
{
    float XL = 0.0f, YL = 0.0f;
    FVector2D TextSize;
    if (Font && !Text.IsEmpty())
    {
        Canvas->StrLen(Font, Text.ToString(), XL, YL);
        TextSize = FVector2D(XL, YL);
        
        if (bScaleByDesignedResolution)
        {
            X *= RenderScale;
            Y *= RenderScale;
        }
        
        FVector2D RenderPos = FVector2D(RenderPosition.X + X, RenderPosition.Y + Y);
        float TextScaling = bScaleByDesignedResolution ? RenderScale * TextScale : TextScale;
        // Handle justification
        if (TextHorzAlignment != ETextHorzPos::Left || TextVertAlignment != ETextVertPos::Top)
        {
            XL *= TextScaling;
            YL *= TextScaling;
            
            if (TextHorzAlignment != ETextHorzPos::Left)
            {
                RenderPos.X -= TextHorzAlignment == ETextHorzPos::Right ? XL : XL * 0.5f;
            }
            if (TextVertAlignment != ETextVertPos::Top)
            {
                RenderPos.Y -= TextVertAlignment == ETextVertPos::Bottom ? YL : YL * 0.5f;
            }
        }
        
        DrawColor.A = Opacity * DrawOpacity * NZHUDOwner->WidgetOpacity;
        Canvas->DrawColor = DrawColor.ToFColor(false);
        
        if (!WordWrapper.IsValid())
        {
            WordWrapper = MakeShareable(new FCanvasWordWrapper());
        }
        FNZCanvasTextItem TextItem(RenderPos, Text, Font, DrawColor, WordWrapper);
        TextItem.FontRenderInfo = RenderInfo;
        
        if (bDrawOutline)
        {
            TextItem.bOutlined = true;
            TextItem.OutlineColor = OutlineColor;
            TextItem.OutlineColor.A *= DrawOpacity * NZHUDOwner->WidgetOpacity;
        }
        
        if (bDrawShadow)
        {
            ShadowColor.A *= DrawOpacity * NZHUDOwner->WidgetOpacity;
            TextItem.EnableShadow(ShadowColor, ShadowDirection);
        }
        
        TextItem.Scale = FVector2D(TextScaling, TextScaling);
        Canvas->DrawItem(TextItem);
    }
    
    return TextSize;
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
    if (Material)
    {
        if (bScaleByDesignedResolution)
        {
            X *= RenderScale;
            Y *= RenderScale;
            
            Width = (bMaintainAspectRatio) ? ((Height * RenderScale) * AspectScale) : Width * RenderScale;
            Height = Height * RenderScale;
        }
        
        FVector2D RenderPos = FVector2D(RenderPosition.X + X - (Width * RenderOffset.X), RenderPosition.Y + Y - (Height * RenderOffset.Y));
        FCanvasTileItem MaterialItem(RenderPos, Material->GetRenderProxy(0), FVector2D(Width, Height), FVector2D(MaterialU, MaterialV), FVector2D(MaterialU + MaterialUWidth, MaterialV + MaterialVHeight));
        
        DrawColor.A = Opacity * DrawOpacity * NZHUDOwner->WidgetOpacity;
        MaterialItem.SetColor(DrawColor);
        MaterialItem.Rotation = FRotator(0, Rotation, 0);
        MaterialItem.PivotPoint = RotPivot;
        Canvas->DrawColor.A *= DrawOpacity * NZHUDOwner->WidgetOpacity;
        Canvas->DrawItem(MaterialItem);
    }
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
