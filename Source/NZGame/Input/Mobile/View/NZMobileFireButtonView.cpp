// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileFireButtonView.h"


UNZMobileFireButtonView::UNZMobileFireButtonView()
{
	static ConstructorHelpers::FObjectFinder<UClass> FireButtonClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_FireButton.BP_MobileHUD_FireButton_C'"));
	CachedWidgetClass = FireButtonClassRef.Object;
}


void UNZMobileFireButtonView::Initialize(class UNZMobileGameController* InController)
{
	Super::Initialize(InController);

	check(CachedWidget);
	if (CachedWidget->WidgetTree)
	{
		TArray<UWidget*> Widgets;
		CachedWidget->WidgetTree->GetAllWidgets(Widgets);
		CachedFireButtonBG = CachedWidget->GetWidgetFromName(TEXT("FireButtonBG"));
		CachedFireButtonDirection = CachedWidget->GetWidgetFromName(TEXT("FireButtonDirection"));
		CachedFireButtonTopDirectionButton = CachedWidget->GetWidgetFromName(TEXT("FireButtonTopDirectionButton"));
		CachedFireButtonBullet = CachedWidget->GetWidgetFromName(TEXT("FireButtonBullet"));
	}
}

void UNZMobileFireButtonView::Update()
{
	Super::Update();

	UNZMobilePlayerInput* PlayerInput = Cast<UNZMobilePlayerInput>(GetPlayerInput());
	if (PlayerInput == NULL)
	{
		return;
	}

	if (PlayerInput->IsFixed())
	{
		UpdateFixedFire(PlayerInput);
	}
	else
	{
		UpdateFollowFire(PlayerInput);
	}
}

void UNZMobileFireButtonView::UpdateFixedFire(UNZMobilePlayerInput* PlayerInput)
{
	check(PlayerInput);
	CachedFireButtonDirection->SetVisibility(ESlateVisibility::Hidden);

	// Ò¡¸Ë°´Å¥
	FMobileInputData RotationInput = PlayerInput->GetRotationInputData();
	FVector2D Rotation = RotationInput.Location - RotationInput.BeginLocation;
	float Size = Rotation.Size();
	float Radius = CachedFireButtonBG->GetDesiredSize().Size();
	Size = Size < Radius ? Size : Radius;
	Rotation = Rotation.GetSafeNormal() * Size;
	CachedFireButtonTopDirectionButton->SetRenderTranslation(Rotation);
	CachedFireButtonBullet->SetRenderTranslation(Rotation);

	// ·½Ïò
	if (Size <= Radius * 0.5)
	{
		CachedFireButtonDirection->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CachedFireButtonDirection->SetVisibility(ESlateVisibility::Visible);
		float Angle = FQuat::FindBetween(FVector(0.f, -1.f, 0.f).GetSafeNormal(), FVector(Rotation, 0.0f).GetSafeNormal()).Rotator().Yaw;
		CachedFireButtonDirection->SetRenderAngle(Angle);
	}
}

void UNZMobileFireButtonView::UpdateFollowFire(UNZMobilePlayerInput* PlayerInput)
{

}
