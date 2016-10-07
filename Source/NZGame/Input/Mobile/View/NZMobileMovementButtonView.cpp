// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobilePlayerInput.h"
#include "NZMobileMovementButtonView.h"


UNZMobileMovementButtonView::UNZMobileMovementButtonView()
{
    static ConstructorHelpers::FObjectFinder<UClass> MovementButtonClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_MovementButton.BP_MobileHUD_MovementButton_C'"));
    CachedWidgetClass = MovementButtonClassRef.Object;
}

void UNZMobileMovementButtonView::Initialize(class UNZMobileGameController* InController)
{
	Super::Initialize(InController);

	check(CachedWidget);
	if (CachedWidget->WidgetTree)
	{
		TArray<UWidget*> Widgets;
		CachedWidget->WidgetTree->GetAllWidgets(Widgets);
		CachedJoystickBG = CachedWidget->GetWidgetFromName(TEXT("JoystickBG"));
		CachedJoystickDirection = CachedWidget->GetWidgetFromName(TEXT("JoystickDirection"));
		CachedJoystickTopDirectionButton = CachedWidget->GetWidgetFromName(TEXT("JoystickTopDirectionButton"));
		CachedJoystickFrontArrow = CachedWidget->GetWidgetFromName(TEXT("JoystickFrontArrow"));
		CachedJoystickBackArrow = CachedWidget->GetWidgetFromName(TEXT("JoystickBackArrow"));
		CachedJoystickLeftArrow = CachedWidget->GetWidgetFromName(TEXT("JoystickLeftArrow"));
		CachedJoystickRightArrow = CachedWidget->GetWidgetFromName(TEXT("JoystickRightArrow"));
	}
}

void UNZMobileMovementButtonView::Update()
{
	Super::Update();

	UNZMobilePlayerInput* PlayerInput = Cast<UNZMobilePlayerInput>(GetPlayerInput());
	if (PlayerInput == NULL)
	{
		return;
	}

	if (PlayerInput->GetMoveHandleType() == NZMobileMoveHandle_Joystick)
	{
		UpdateJoystick(PlayerInput);
	}
	else if (PlayerInput->GetMoveHandleType() == NZMobileMoveHandle_Classics)
	{
		UpdateClassics(PlayerInput);
	}
}

void UNZMobileMovementButtonView::UpdateJoystick(UNZMobilePlayerInput* PlayerInput)
{
	check(PlayerInput);
	FVector2D Accel = PlayerInput->GetMovementAccel();

	CachedJoystickDirection->SetVisibility(ESlateVisibility::Hidden);
	CachedJoystickFrontArrow->SetVisibility(ESlateVisibility::Hidden);
	CachedJoystickBackArrow->SetVisibility(ESlateVisibility::Hidden);
	CachedJoystickLeftArrow->SetVisibility(ESlateVisibility::Hidden);
	CachedJoystickRightArrow->SetVisibility(ESlateVisibility::Hidden);

	// Arrow
	if (Accel.X == 0 && Accel.Y != 0)
	{
		if (Accel.Y < 0)
		{
			CachedJoystickBackArrow->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CachedJoystickFrontArrow->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (Accel.Y == 0 && Accel.X != 0)
	{
		if (Accel.X < 0)
		{
			CachedJoystickLeftArrow->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CachedJoystickRightArrow->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// Direction button
	FMobileInputData MovementInput = PlayerInput->GetMovementInputData();
	FVector2D Movement = MovementInput.Location - MovementInput.BeginLocation;
	float Size = Movement.Size();
    float Radius = (CachedJoystickBG->GetDesiredSize().X + CachedJoystickBG->GetDesiredSize().Y) * 0.5 * 0.5;
	Size = Size < Radius ? Size : Radius;
	Movement = Movement.GetSafeNormal() * Size;
	CachedJoystickTopDirectionButton->SetRenderTranslation(Movement);

	// Direction
	if (Size <= Radius * 0.3)
	{
		CachedJoystickDirection->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CachedJoystickDirection->SetVisibility(ESlateVisibility::Visible);
		float Angle = FQuat::FindBetween(FVector(0.f, -1.f, 0.f).GetSafeNormal(), FVector(Movement, 0.0f).GetSafeNormal()).Rotator().Yaw;
		CachedJoystickDirection->SetRenderAngle(Angle);
	}

}

void UNZMobileMovementButtonView::UpdateClassics(UNZMobilePlayerInput* PlayerInput)
{

}
