// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZMobileWeaponButtonView.h"



UNZMobileWeaponButtonView::UNZMobileWeaponButtonView()
{
	static ConstructorHelpers::FObjectFinder<UClass> WeaponButtonClassRef(TEXT("WidgetBlueprint'/Game/UI/BP_MobileHUD_WeaponButton.BP_MobileHUD_WeaponButton_C'"));
	CachedWidgetClass = WeaponButtonClassRef.Object;
}

void UNZMobileWeaponButtonView::Initialize(class UNZMobileGameController* InController)
{
	Super::Initialize(InController);

	check(CachedWidget);
	if (CachedWidget->WidgetTree)
	{
		TArray<UWidget*> Widgets;
		CachedWidget->WidgetTree->GetAllWidgets(Widgets);
		//CachedWeaponButtonBG = CachedWidget->GetWidgetFromName(TEXT("WeaponButtonBG"));
	}
}

void UNZMobileWeaponButtonView::Update()
{
	Super::Update();

	UNZMobilePlayerInput* PlayerInput = Cast<UNZMobilePlayerInput>(GetPlayerInput());
	if (PlayerInput == NULL)
	{
		return;
	}

}
