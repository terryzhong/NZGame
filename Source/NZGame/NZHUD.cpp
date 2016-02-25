// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUD.h"
#include "NZHUDWidget.h"
#include "NZBasePlayerController.h"


void ANZHUD::CacheFonts()
{

}

void ANZHUD::NotifyKill()
{

}

void ANZHUD::PlayKillNotification()
{

}

void ANZHUD::AddSpectatorWidgets()
{

}

void ANZHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ANZHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

TSubclassOf<UNZHUDWidget> ANZHUD::ResolveHudWidgetByName(const TCHAR* ResourceName)
{
	return NULL;
}

UNZHUDWidget* ANZHUD::AddHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass)
{
	return NULL;
}

UNZHUDWidget* ANZHUD::FindHudWidgetByClass(TSubclassOf<UNZHUDWidget> SearchWidgetClass, bool bExactClass)
{
	return NULL;
}

void ANZHUD::PostRender()
{
	// Always sort the PlayerState array at the beginning of each frame
	ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
	if (GS != NULL)
	{
		GS->SortPSArray();
	}
	Super::PostRender();
}

void ANZHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!IsPendingKillPending() || !IsPendingKill())
	{
		// Find center of the Canvas
		const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
		bool bPreMatchScoreBoard = (GS && !GS->HasMatchStarted() && !GS->IsMatchInCountdown());
		bool bScoreboardIsUp = bShowScores || bPreMatchScoreBoard || bForceScores;
		if (!bFontsCached)
		{
			CacheFonts();
		}
		if (PlayerOwner && PlayerOwner->PlayerState && PlayerOwner->PlayerState->bOnlySpectator)
		{
			AddSpectatorWidgets();
		}

		for (int32 WidgetIndex = 0; WidgetIndex < HudWidgets.Num(); WidgetIndex++)
		{
			if (HudWidgets[WidgetIndex] && !HudWidgets[WidgetIndex]->IsHidden() && !HudWidgets[WidgetIndex]->IsPendingKill())
			{
				HudWidgets[WidgetIndex]->PreDraw(RenderDelta, this, Canvas, Center);
				if (HudWidgets[WidgetIndex]->ShouldDraw(bScoreboardIsUp))
				{
					HudWidgets[WidgetIndex]->Draw(RenderDelta);
				}
				HudWidgets[WidgetIndex]->PostDraw(GetWorld()->GetTimeSeconds());
			}
		}

		if (NZPlayerOwner)
		{
			if (bScoreboardIsUp)
			{
				if (!NZPlayerOwner->CurrentlyViewedScorePS)
				{
					NZPlayerOwner->SetViewedScorePS(GetScorerPlayerState(), NZPlayerOwner->CurrentlyViewedStatsTab);
				}
			}
			else
			{
				if (!NZPlayerOwner->IsBehindView() || !NZPlayerOwner->NZPlayerState || !NZPlayerOwner->NZPlayerState->bOnlySpectator)
				{
					DrawDamageIndicators();
				}
				// todo:
				//if (SpectatorSlideOutWidget && NZPlayerOwner->PlayerState && NZPlayerOwner->PlayerState->bOnlySpectator && SpectatorSlideOutWidget->bShowingStats && !NZPlayerOwner->CurrentlyViewedScorePS)
				//{
				//	NZPlayerOwner->CurrentlyViewedStatsTab = 1;
				//	NZPlayerOwner->SetViewedScorePS(GetScorerPlayerState(), NZPlayerOwner->CurrentlyViewedStatsTab);
				//}
				else
				{
					NZPlayerOwner->SetViewedScorePS(NULL, 0);
				}
				if (bDrawMinimap && NZPlayerOwner->PlayerState && NZPlayerOwner->PlayerState->bOnlySpectator)
				{
					const float MapSize = float(Canvas->SizeY) * 0.75f;
					DrawMinimap(FColor(192, 192, 192, 210), MapSize, FVector2D(Canvas->SizeX - MapSize + MapSize * MinimapOffset.X, MapSize * MinimapOffset.Y));
				}
				if (bDrawDamageNumbers)
				{
					DrawDamageNumbers();
				}
			}
		}
	}
}

void ANZHUD::ReceiveLocalMessage(TSubclassOf<class UNZLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject)
{

}

void ANZHUD::ToggleScoreboard(bool bShow)
{

}

void ANZHUD::DrawDamageIndicators()
{

}

ANZPlayerState* ANZHUD::GetScorerPlayerState()
{
	return NULL;
}

void ANZHUD::BuildHudWidget(FString NewWidgetString)
{

}

bool ANZHUD::HasHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass)
{
	return false;
}

FVector2D ANZHUD::JSon2FVector2D(const TSharedPtr<FJsonObject> Vector2DObject, FVector2D Default)
{
	return FVector2D(0.f, 0.f);
}

void ANZHUD::CalcStanding()
{

}

FText ANZHUD::ConvertTime(FText Prefix, FText Suffix, int32 Seconds, bool bForceHours, bool bForceMinutes, bool bForceTwoDigits) const
{
	return FText::GetEmpty();
}

FText ANZHUD::GetPlaceSuffix(int32 Value)
{
	return FText::GetEmpty();
}

void ANZHUD::DrawString(FText Text, float X, float Y, ETextHorzPos::Type HorzAlignment, ETextVertPos::Type VertAlignment, UFont* Font, FLinearColor Color, float Scale, bool bOutline)
{

}

void ANZHUD::DrawNumber(int32 Number, float X, float Y, FLinearColor Color, float GlowOpacity, float Scale, int32 MinDigits, bool bRightAlign)
{

}

EInputMode::Type ANZHUD::GetInputMode_Implementation() const
{
    return EInputMode::EIM_None;
}

float ANZHUD::GetCrosshairScale()
{
    // Apply pickup scaling
    float PickupScale = 1.f;
    const float WorldTime = GetWorld()->GetTimeSeconds();
    if (LastPickupTime > WorldTime - 0.3f)
    {
        if (LastPickupTime > WorldTime - 0.15f)
        {
            PickupScale = (1.f + 5.f * (WorldTime - LastPickupTime));
        }
        else
        {
            PickupScale = (1.f + 5.f * (LastPickupTime + 0.3f - WorldTime));
        }
    }
    
    if (Canvas != NULL)
    {
        PickupScale = PickupScale * Canvas->ClipX / 1920.f;
    }
    
    return PickupScale;
}

FLinearColor ANZHUD::GetCrosshairColor(FLinearColor CrosshairColor) const
{
    float TimeSinceHit = GetWorld()->TimeSeconds - LastConfirmedHitTime;
    if (TimeSinceHit < 0.4f)
    {
        CrosshairColor = FMath::Lerp<FLinearColor>(FLinearColor::Red, CrosshairColor, FMath::Lerp<float>(0.f, 1.f, FMath::Pow((GetWorld()->TimeSeconds - LastConfirmedHitTime) / 0.4f, 2.0f)));
    }
    return CrosshairColor;
}

UNZCrosshair* ANZHUD::GetCrosshair(ANZWeapon* Weapon)
{
	return NULL;
}

FCrosshairInfo* ANZHUD::GetCrosshairInfo(ANZWeapon* Weapon)
{
	return NULL;
}

void ANZHUD::UpdateMinimapTexture(UCanvas* C, int32 Width, int32 Height)
{

}

void ANZHUD::CreateMinimapTexture()
{

}

void ANZHUD::DrawMinimap(const FColor& DrawColor, float MapSize, FVector2D DrawPos)
{

}

void ANZHUD::DrawMinimapIcon(UTexture2D* Texture, FVector2D Pos, FVector2D DrawSize, FVector2D UV, FVector2D UVL, FLinearColor DrawColor, bool bDropShadow)
{

}

void ANZHUD::DrawMinimapSpectatorIcons()
{

}

void ANZHUD::CalcMinimapTransform(const FBox& LevelBox, int32 MapWidth, int32 MapHeight)
{

}
