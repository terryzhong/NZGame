// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
//#include "IHeadMountedDisplay.h"
#include "NZHUDWidget_WeaponCrosshair.h"


UNZHUDWidget_WeaponCrosshair::UNZHUDWidget_WeaponCrosshair()
{
	Size = FVector2D(0.0f, 0.0f);
	ScreenPosition = FVector2D(0.5f, 0.5f);
}

bool UNZHUDWidget_WeaponCrosshair::ShouldDraw_Implementation(bool bShowScores)
{
	//// VR设备不需要绘制准星
	//if (NZPlayerOwner && NZPlayerOwner->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	//{
	//	return false;
	//}
	return Super::ShouldDraw_Implementation(bShowScores);
}

void UNZHUDWidget_WeaponCrosshair::Draw_Implementation(float DeltaTime)
{
	if (NZHUDOwner != NULL && NZHUDOwner->NZPlayerOwner != NULL)
	{
		ANZCharacter* NZCharacter = Cast<ANZCharacter>(NZHUDOwner->NZPlayerOwner->GetViewTarget());
		if (NZCharacter)
		{
			if (NZCharacter->GetWeapon() != NULL)
			{
				NZCharacter->GetWeapon()->DrawWeaponCrosshair(this, DeltaTime);
			}
			else
			{
				// Fall back crosshair
				UTexture2D* CrosshairTexture = NZHUDOwner->DefaultCrosshairTex;
				if (CrosshairTexture != NULL)
				{
					float W = CrosshairTexture->GetSurfaceWidth();
					float H = CrosshairTexture->GetSurfaceHeight();
					float CrosshairScale = NZHUDOwner->GetCrosshairScale();

					DrawTexture(CrosshairTexture, 0, 0, W * CrosshairScale, H * CrosshairScale, 0.0, 0.0, 16, 16, 1.0, NZHUDOwner->GetCrosshairColor(FLinearColor::White), FVector2D(0.5f, 0.5f));
				}
			}
		}

		const float TimeSinceKill = GetWorld()->GetTimeSeconds() - NZHUDOwner->LastKillTime;
		const float SkullDisplayTime = 0.8f;
		if (TimeSinceKill < SkullDisplayTime)
		{
			float Size = 32.f * (1.f + FMath::Min(1.5f * (TimeSinceKill - 0.2f) / SkullDisplayTime, 1.f));
			FLinearColor SkullColor = FLinearColor::White;
			float Opacity = 0.7f - 0.6f * TimeSinceKill / SkullDisplayTime;
			DrawTexture(NZHUDOwner->HUDAtlas, 0, -2.f * Size, Size, Size, 725, 0, 28, 36, Opacity, SkullColor, FVector2D(0.5f, 0.5f));
		}

		// todo:
		//const float TimeSinceGrab = GetWorld()->GetTimeSeconds() - NZHUDOwner->LastFlagGrabTime;
		//const float FlagDisplayTime = 1.f;
		//if (TimeSinceGrab < FlagDisplayTime)
		//{
		//	ANZPlayerState* PS = NZHUDOwner->GetScorerPlayerState();
		//	if (PS && PS->CarriedObject)
		//	{
		//		float FlagPct = FMath::Max(0.f, TimeSinceGrab - 0.5f * FlagDisplayTime) / FlagDisplayTime;
		//		float Size = 64.f * (1.f + 5.f * FlagPct);
		//		FLinearColor FlagColor = FLinearColor::White;
		//		int32 TeamIndex = PS->CarriedObject->GetTeamNum();
		//		ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
		//		if (GS != NULL && GS->Teams.IsValidIndex(TeamIndex) && GS->Teams[TeamIndex] != NULL)
		//		{
		//			FlagColor = GS->Teams[TeamIndex]->TeamColor;
		//		}
		//		float Opacity = 0.8f - 0.8f * FlagPct;
		//		DrawTexture(NZHUDOwner->HUDAtlas, 0.f, -48.f, Size, Size, 843.f, 87.f, 43.f, 41.f, Opacity, FlagColor, FVector2D(0.5f, 0.5f));
		//	}
		//}
	}
}

