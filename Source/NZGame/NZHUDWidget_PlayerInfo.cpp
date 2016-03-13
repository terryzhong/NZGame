// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUDWidget_PlayerInfo.h"


UNZHUDWidget_PlayerInfo::UNZHUDWidget_PlayerInfo()
{
	Health = 0;
}

void UNZHUDWidget_PlayerInfo::Draw_Implementation(float DeltaTime)
{
	ANZCharacter* NZC = Cast<ANZCharacter>(NZHUDOwner->NZPlayerOwner->GetViewTarget());
	if (NZC)
	{
		float HealthPercent = (float)NZC->Health / (float)NZC->HealthMax;
		if (HealthPercent > 0.6)
		{
			HealthTexture.RenderColor = FLinearColor::Green;
		}
		else if (HealthPercent > 0.2)
		{
			HealthTexture.RenderColor = FLinearColor::Yellow;
		}
		else
		{
			HealthTexture.RenderColor = FLinearColor::Red;
		}

		Health = NZC->Health;
	}

	Super::Draw_Implementation(DeltaTime);
}

bool UNZHUDWidget_PlayerInfo::ShouldDraw_Implementation(bool bShowScores)
{
	ANZCharacter* NZC = Cast<ANZCharacter>(NZHUDOwner->NZPlayerOwner->GetViewTarget());
	return Super::ShouldDraw_Implementation(bShowScores) && NZC;
}

void UNZHUDWidget_PlayerInfo::InitializeWidget(ANZHUD* Hud)
{
	Super::InitializeWidget(Hud);

	Health = 0;
	HealthText.GetTextDelegate.BindUObject(this, &UNZHUDWidget_PlayerInfo::GetHealthText_Implementation);
}

FText UNZHUDWidget_PlayerInfo::GetHealthText_Implementation()
{
	return FText::AsNumber(Health);
}
