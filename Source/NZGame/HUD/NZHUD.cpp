// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZHUD.h"
#include "NZHUDWidget.h"
#include "NZHUDWidgetMessage.h"
#include "NZBasePlayerController.h"
#include "Json.h"
//#include "IHeadMountedDisplay.h"

ANZHUD::ANZHUD()
{
    WidgetOpacity = 1.0f;

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/UI/Crosshair/Crosshair_Default.Crosshair_Default'"));
	DefaultCrosshairTex = CrosshairTexObj.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> TFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Tiny.fntScoreboard_Tiny'"));
    TinyFont = TFont.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> SFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Small.fntScoreboard_Small'"));
    SmallFont = SFont.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> MFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Medium.fntScoreboard_Medium'"));
    MediumFont = MFont.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> LFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Large.fntScoreboard_Large'"));
    LargeFont = LFont.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> HFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Huge.fntScoreboard_Huge'"));
    HugeFont = HFont.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> ScFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Score.fntScoreboard_Score'"));
    ScoreFont = ScFont.Object;
    
    static ConstructorHelpers::FObjectFinder<UFont> CFont(TEXT("Font'/Game/UI/Fonts/fntScoreboard_Clock.fntScoreboard_Clock'"));
    NumberFont = CFont.Object;
    
    
    
    
    LastKillTime = -100.f;
    LastConfirmedHitTime = -100.0f;
    LastPickupTime = -100.0f;
    bFontsCached = false;
    bShowOverlays = true;
    bHaveAddedSpectatorWidgets = false;
    
    //TeamIconUV[0] = FVector2D(257.f, 940.f);
    //TeamIconUV[0] = FVector2D(333.f, 940.f);
    
    // todo:
    
    bCustomWeaponCrosshairs = true;
}

void ANZHUD::CacheFonts()
{
    FText MessageText = NSLOCTEXT("ANZHUD", "FontCacheText", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';:-=+*(),.?!");
    FFontRenderInfo TextRenderInfo;
    TextRenderInfo.bEnableShadow = true;
    float YPos = 0.f;
    Canvas->DrawColor = FLinearColor::White.ToFColor(false);
    Canvas->DrawText(TinyFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    Canvas->DrawText(SmallFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    Canvas->DrawText(MediumFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    Canvas->DrawText(LargeFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    Canvas->DrawText(HugeFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    Canvas->DrawText(ScoreFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    Canvas->DrawText(NumberFont, MessageText, 0.f, YPos, 0.1f, 0.1f, TextRenderInfo);
    bFontsCached = true;
}

void ANZHUD::DrawDamageNumbers()
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
    if (bHaveAddedSpectatorWidgets)
    {
        return;
    }
    bHaveAddedSpectatorWidgets = true;
    
    // Parse the widgets found in the ini
    for (int32 i = 0; i < SpectatorHudWidgetClasses.Num(); i++)
    {
        BuildHudWidget(*SpectatorHudWidgetClasses[i]);
    }
    
    UNZLocalPlayer* NZLP = NZPlayerOwner ? Cast<UNZLocalPlayer>(NZPlayerOwner->Player) : NULL;
    if (NZLP)
    {
        NZLP->OpenSpectatorWindow();
    }
}

void ANZHUD::BeginPlay()
{
	Super::BeginPlay();
    
    // Parse the widgets found in the ini
    for (int32 i = 0; i < RequiredHudWidgetClasses.Num(); i++)
    {
        BuildHudWidget(*RequiredHudWidgetClasses[i]);
    }
    
    // Parse any hard coded widgets
    for (int32 WidgetIndex = 0; WidgetIndex < HudWidgetClasses.Num(); WidgetIndex++)
    {
        BuildHudWidget(HudWidgetClasses[WidgetIndex]);
    }

    DamageIndicators.AddZeroed(MAX_DAMAGE_INDICATORS);
    for (int32 i = 0; i < MAX_DAMAGE_INDICATORS; i++)
    {
        DamageIndicators[i].RotationAngle = 0.0f;
        DamageIndicators[i].DamageAmount = 0.0f;
        DamageIndicators[i].FadeTime = 0.0f;
    }

	// 自定义的UMG控件
	for (int32 WidgetIndex = 0; WidgetIndex < HubUserWidgetClasses.Num(); WidgetIndex++)
	{
		if (HubUserWidgetClasses[WidgetIndex] != NULL)
		{
			UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetGameInstance(), HubUserWidgetClasses[WidgetIndex]);
			HubUserWidgets.Add(UserWidget);
			UserWidget->AddToViewport();
		}
	}
}

void ANZHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    
    NZPlayerOwner = Cast<ANZPlayerController>(GetOwner());
    if (NZPlayerOwner)
    {
        NZPlayerOwner->UpdateCrosshairs(this);
    }
}

TSubclassOf<UNZHUDWidget> ANZHUD::ResolveHudWidgetByName(const TCHAR* ResourceName)
{
	UClass* WidgetClass = LoadClass<UNZHUDWidget>(NULL, ResourceName, NULL, LOAD_NoWarn | LOAD_Quiet, NULL);
	if (WidgetClass != NULL)
	{
		return WidgetClass;
	}
	FString BlueprintResourceName = FString::Printf(TEXT("%s_C"), ResourceName);

	WidgetClass = LoadClass<UNZHUDWidget>(NULL, *BlueprintResourceName, NULL, LOAD_NoWarn | LOAD_Quiet, NULL);
	return WidgetClass;
}

UNZHUDWidget* ANZHUD::AddHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass)
{
	if (NewWidgetClass == NULL)
	{
		return NULL;
	}

	UNZHUDWidget* Widget = NewObject<UNZHUDWidget>(GetTransientPackage(), NewWidgetClass);
	HudWidgets.Add(Widget);

	UNZHUDWidgetMessage* MessageWidget = Cast<UNZHUDWidgetMessage>(Widget);
	if (MessageWidget != NULL)
	{
		HudMessageWidgets.Add(MessageWidget->ManagedMessageArea, MessageWidget);
	}

	// todo:
	//// Cache ref to scoreboard (NOTE: only one!)
	//if (Widget->IsA(UNZScoreboard::StaticClass()))
	//{
	//	MyNZScoreboard = Cast<UNZScoreboard>(Widget);
	//}

	Widget->InitializeWidget(this);
	if (Cast<UNZHUDWidget_Spectator>(Widget))
	{
		SpectatorMessageWidget = Cast<UNZHUDWidget_Spectator>(Widget);
	}
	// todo:
	//if (Cast<UNZHUDWidget_ReplayTimeSlider>(Widget))
	//{
	//	ReplayTimeSliderWidget = Cast<UNZHUDWidget_ReplayTimeSlider>(Widget);
	//}
	//if (Cast<UNZHUDWidget_SpectatorSlideOut>(Widget))
	//{
	//	SpectatorSlideOutWidget = Cast<UNZHUDWidget_SpectatorSlideOut>(Widget);
	//}

	return Widget;
}

UNZHUDWidget* ANZHUD::FindHudWidgetByClass(TSubclassOf<UNZHUDWidget> SearchWidgetClass, bool bExactClass)
{
	for (int32 i = 0; i < HudWidgets.Num(); i++)
	{
		if (bExactClass ? HudWidgets[i]->GetClass() == SearchWidgetClass : HudWidgets[i]->IsA(SearchWidgetClass))
		{
			return HudWidgets[i];
		}
	}
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
	//// VR设备下，不绘制HUD
	//if (NZPlayerOwner && NZPlayerOwner->IsLocalPlayerController() && GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	//{
	//	return;
	//}

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
            // If we aren't hidden then set the canvas and render
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
				//else
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
	UNZHUDWidgetMessage* DestinationWidget = (HudMessageWidgets.FindRef(MessageClass->GetDefaultObject<UNZLocalMessage>()->MessageArea));
	if (DestinationWidget != NULL)
	{
		DestinationWidget->ReceiveLocalMessage(MessageClass, RelatedPlayerState_1, RelatedPlayerState_2, MessageIndex, LocalMessageText, OptionalObject);
	}
	else
	{
		//UE_LOG(NZ, Verbose, TEXT("No Message Widget to Display Text"));
	}
}

void ANZHUD::ToggleScoreboard(bool bShow)
{

}

void ANZHUD::CreateScoreboard(TSubclassOf<class UNZScoreboard> NewScoreboardClass)
{
    
}

void ANZHUD::PawnDamaged(FVector HitLocation, int32 DamageAmount, bool bFriendlyFire)
{
    
}

void ANZHUD::DrawDamageIndicators()
{
    
}

void ANZHUD::CausedDamage(APawn* HitPawn, int32 Damage)
{
    
}

UFont* ANZHUD::GetFontFromSizeIndex(int32 FontSizeIndex) const
{
    switch (FontSizeIndex)
    {
        case 0: return TinyFont;
        case 1: return SmallFont;
        case 2: return MediumFont;
        case 3: return LargeFont;
    }
    
    return MediumFont;
}

FLinearColor ANZHUD::GetBaseHUDColor()
{
    return FLinearColor();
}

ANZPlayerState* ANZHUD::GetScorerPlayerState()
{
    ANZPlayerState* PS = NZPlayerOwner->NZPlayerState;
    if (PS && !PS->bOnlySpectator)
    {
        // View your own score unless you are a spectator
        return PS;
    }
    
    APawn* PawnOwner = (NZPlayerOwner->GetPawn() != NULL) ? NZPlayerOwner->GetPawn() : Cast<APawn>(NZPlayerOwner->GetViewTarget());
    if (PawnOwner != NULL && Cast<ANZPlayerState>(PawnOwner->PlayerState) != NULL)
    {
        PS = (ANZPlayerState*)PawnOwner->PlayerState;
    }
    
    return NZPlayerOwner->LastSpectatedPlayerState ? NZPlayerOwner->LastSpectatedPlayerState : PS;
}

void ANZHUD::NotifyMatchStateChange()
{
    
}

void ANZHUD::OpenMatchSummary()
{
    
}

FLinearColor ANZHUD::GetWidgetTeamColor()
{
    // Add code to cache and return the team color if it's a team game
    ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
    if (GS == NULL || (GS->bTeamGame && NZPlayerOwner && NZPlayerOwner->GetViewTarget()))
    {
        APawn* HUDPawn = Cast<APawn>(NZPlayerOwner->GetViewTarget());
        ANZPlayerState* PS = HUDPawn ? Cast<ANZPlayerState>(HUDPawn->PlayerState) : NULL;
        if (PS != NULL)
        {
            return (PS->GetTeamNum() == 0) ? FLinearColor(0.15, 0.0, 0.0, 1.0) : FLinearColor(0.025, 0.025, 0.1, 1.0);
        }
    }
    
    return FLinearColor::Black;
}


void ANZHUD::BuildHudWidget(FString NewWidgetString)
{
    if (NewWidgetString.Trim().Left(1) == TEXT("{"))
    {
        // It's a json command so we have to break it apart
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(NewWidgetString);
        TSharedPtr<FJsonObject> JSONObject;
        if (FJsonSerializer::Deserialize(Reader, JSONObject) && JSONObject.IsValid())
        {
            // We have a valid JSON object
            const TSharedPtr<FJsonValue>* ClassName = JSONObject->Values.Find(TEXT("Classname"));
            if (ClassName->IsValid() && (*ClassName)->Type == EJson::String)
            {
                TSubclassOf<UNZHUDWidget> NewWidgetClass = ResolveHudWidgetByName(*(*ClassName)->AsString());
                if (NewWidgetClass != NULL)
                {
                    UNZHUDWidget* NewWidget = AddHudWidget(NewWidgetClass);
                    
                    // Now look for position overrides
                    const TSharedPtr<FJsonValue>* PositionVal = JSONObject->Values.Find(TEXT("Position"));
                    if (PositionVal != NULL && (*PositionVal)->Type == EJson::Object)
                    {
                        NewWidget->Position = JSon2FVector2D((*PositionVal)->AsObject(), NewWidget->Position);
                    }
                    
                    const TSharedPtr<FJsonValue>* OriginVal = JSONObject->Values.Find(TEXT("Origin"));
                    if (OriginVal != NULL && (*OriginVal)->Type == EJson::Object)
                    {
                        NewWidget->Origin = JSon2FVector2D((*OriginVal)->AsObject(), NewWidget->Origin);
                    }
                    
                    const TSharedPtr<FJsonValue>* ScreenPositionVal = JSONObject->Values.Find(TEXT("ScreenPosition"));
                    if (ScreenPositionVal != NULL && (*ScreenPositionVal)->Type == EJson::Object)
                    {
                        NewWidget->ScreenPosition = JSon2FVector2D((*ScreenPositionVal)->AsObject(), NewWidget->ScreenPosition);
                    }
                    
                    const TSharedPtr<FJsonValue>* SizeVal = JSONObject->Values.Find(TEXT("Size"));
                    if (SizeVal != NULL && (*SizeVal)->Type == EJson::Object)
                    {
                        NewWidget->Size = JSon2FVector2D((*SizeVal)->AsObject(), NewWidget->Size);
                    }
                }
            }
        }
        else
        {
            UE_LOG(NZ, Log, TEXT("Failed to parse JSON HudWidget entry: %s"), *NewWidgetString);
        }
    }
    else
    {
        TSubclassOf<UNZHUDWidget> NewWidgetClass = ResolveHudWidgetByName(*NewWidgetString);
        if (NewWidgetClass != NULL)
        {
            AddHudWidget(NewWidgetClass);
        }
    }
}

bool ANZHUD::HasHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass)
{
	if ((NewWidgetClass == NULL) || (HudWidgets.Num() == 0))
    {
        return false;
    }
    
    for (int32 i = 0; i < HudWidgets.Num(); i++)
    {
        if (HudWidgets[i] && (HudWidgets[i]->GetClass() == NewWidgetClass))
        {
            return true;
        }
    }
    
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
    FVector2D RenderPos = FVector2D(X, Y);
    
    float XL, YL;
    Canvas->TextSize(Font, Text.ToString(), XL, YL, Scale, Scale);
    
    if (HorzAlignment != ETextHorzPos::Left)
    {
        RenderPos.X -= (HorzAlignment == ETextHorzPos::Right ? XL : XL * 0.5f);
    }
    
    if (VertAlignment != ETextVertPos::Top)
    {
        RenderPos.Y -= (VertAlignment == ETextVertPos::Bottom ? YL : YL * 0.5f);
    }
    
    FCanvasTextItem TextItem(RenderPos, Text, Font, Color);
    
    if (bOutline)
    {
        TextItem.bOutlined = true;
        TextItem.OutlineColor = FLinearColor::Black;
    }
    
    TextItem.Scale = FVector2D(Scale, Scale);
    Canvas->DrawItem(TextItem);
}

void ANZHUD::DrawNumber(int32 Number, float X, float Y, FLinearColor Color, float GlowOpacity, float Scale, int32 MinDigits, bool bRightAlign)
{
    FNumberFormattingOptions Opts;
    Opts.MinimumIntegralDigits = MinDigits;
    DrawString(FText::AsNumber(Number, &Opts), X, Y, bRightAlign ? ETextHorzPos::Right : ETextHorzPos::Left, ETextVertPos::Top, NumberFont, Color, Scale, true);
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
	FCrosshairInfo* CrosshairInfo = GetCrosshairInfo(Weapon);
	if (CrosshairInfo != NULL)
	{
		for (int32 i = 0; i < LoadedCrosshairs.Num(); i++)
		{
			if (LoadedCrosshairs[i]->GetClass()->GetPathName() == CrosshairInfo->CrosshairClassName)
			{
				return LoadedCrosshairs[i];
			}
		}

		// Didn't find it so create a new one
		UClass* TestClass = LoadObject<UClass>(NULL, *CrosshairInfo->CrosshairClassName);
		if (TestClass != NULL && !TestClass->HasAnyClassFlags(CLASS_Abstract) && TestClass->IsChildOf(UNZCrosshair::StaticClass()))
		{
			UNZCrosshair* NewCrosshair = NewObject<UNZCrosshair>(this, TestClass);
			LoadedCrosshairs.Add(NewCrosshair);
		}
	}
	return NULL;
}

FCrosshairInfo* ANZHUD::GetCrosshairInfo(ANZWeapon* Weapon)
{
	FString WeaponClass = (!bCustomWeaponCrosshairs || Weapon == NULL) ? TEXT("Global") : Weapon->GetClass()->GetPathName();

	FCrosshairInfo* FoundInfo = CrosshairInfos.FindByPredicate([WeaponClass](const FCrosshairInfo& Info) { return Info.WeaponClassName == WeaponClass;  });
	if (FoundInfo != NULL)
	{
		return FoundInfo;
	}

	// Make a Global one if we couldn't find one
	if (WeaponClass == TEXT("Global"))
	{
		return &CrosshairInfos[CrosshairInfos.Add(FCrosshairInfo())];
	}
	else
	{
		// Create a new crosshair for this weapon based off the global one
		FCrosshairInfo NewInfo;
		FCrosshairInfo* GlobalCrosshair = CrosshairInfos.FindByPredicate([](const FCrosshairInfo& Info) { return Info.WeaponClassName == TEXT("Global"); });
		if (GlobalCrosshair != NULL)
		{
			NewInfo = *GlobalCrosshair;
		}
		NewInfo.WeaponClassName = WeaponClass;
		return &CrosshairInfos[CrosshairInfos.Add(NewInfo)];
	}
}

void ANZHUD::UpdateMinimapTexture(UCanvas* C, int32 Width, int32 Height)
{
/*    FBox LevelBox(0);
    ANZRecastNavMesh* NavMesh = GetNZNavData(GetWorld());
    if (NavMesh != NULL)
    {
        TMap<const UNZPathNode*, FNavMeshTriangleList> TriangleMap;
        NavMesh->GetNodeTriangleMap(TriangleMap);
        // Calculate a bounding box for the level
        for (TMap<const UNZPathNode*, FNavMeshTriangleList>::TConstIterator It(TriangleMap); It; ++It)
        {
            const FNavMeshTriangleList& TriList = It.Value();
            for (const FVector& Vert : TriList.Verts)
            {
                LevelBox += Vert;
            }
        }
        if (LevelBox.IsValid)
        {
            LevelBox = LevelBox.ExpandBy(LevelBox.GetSize() * 0.01f);
            CalcMinimapTransform(LevelBox, Width, Height);
            for (TMap<const UNZPathNode*, FNavMeshTriangleList>::TConstIterator It(TriangleMap); It; ++It)
            {
                const FNavMeshTriangleList& TriList = It.Value();
                
                for (const FNavMeshTriangleList::FTriangle& Tri : TriList.Triangles)
                {
                    bool bInWater = false;
                    FVector Verts[3] = { TriList.Verts[Tri.Indices[0]], TriList.Verts[Tri.Indices[1]], TriList.Verts[Tri.Indices[2]] };
                    for (int32 i = 0; i < ARRAY_COUNT(Verts); i++)
                    {
                        UNZPathNode* Node = NavMesh->FindNearestNode(Verts[i], NavMesh->GetHumanPathSize().GetExtent());
                        if (Node != NULL && Node->PhysicsVolume != NULL && Node->PhysicsVolume->bWaterVolume)
                        {
                            bInWater = true;
                            break;
                        }
                        Verts[i] = MinimapTransform.TransformPosition(Verts[i]);
                    }
                    if (!bInWater)
                    {
                        FCanvasTriangleItem Item(FVector2D(Verts[0]), FVector2D(Verts[1]), FVector2D(Verts[2]), C->DefaultTexture->Resource);
                        C->DrawItem(Item);
                    }
                }
            }
        }
    }
    if (!LevelBox.IsValid)
    {
        for (TActorIterator<AActor> It(GetWorld()); It; ++It)
        {
            TArray<UPrimitiveComponent*> Components;
            It->GetComponents(Components);
            for (UPrimitiveComponent* Prim : Components)
            {
                if (Prim->IsCollisionEnabled())
                {
                    LevelBox += Prim->Bounds.GetBox();
                }
            }
        }
        LevelBox = LevelBox.ExpandBy(LevelBox.GetSize() * 0.01f);
        CalcMinimapTransform(LevelBox, Width, Height);
    }*/
}

void ANZHUD::CreateMinimapTexture()
{
    MinimapTexture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(), UCanvasRenderTarget2D::StaticClass(), 1024, 1024);
    MinimapTexture->ClearColor = FLinearColor::Black;
    MinimapTexture->ClearColor.A = 0.f;
    MinimapTexture->OnCanvasRenderTargetUpdate.AddDynamic(this, &ANZHUD::UpdateMinimapTexture);
    MinimapTexture->UpdateResource();
}

void ANZHUD::DrawMinimap(const FColor& DrawColor, float MapSize, FVector2D DrawPos)
{
    if (MinimapTexture == NULL)
    {
        CreateMinimapTexture();
    }

    FVector ScaleFactor(MapSize / MinimapTexture->GetSurfaceWidth(), MapSize / MinimapTexture->GetSurfaceHeight(), 1.f);
    MapToScreen = FTranslationMatrix(FVector(DrawPos, 0.f) / ScaleFactor) * FScaleMatrix(ScaleFactor);
    if (MinimapTexture != NULL)
    {
        Canvas->DrawColor = DrawColor;
        Canvas->DrawTile(MinimapTexture, MapToScreen.GetOrigin().X, MapToScreen.GetOrigin().Y, MapSize, MapSize, 0.f, 0.f, MinimapTexture->GetSurfaceWidth(), MinimapTexture->GetSurfaceHeight());
    }
    
    if (PlayerOwner && PlayerOwner->PlayerState && PlayerOwner->PlayerState->bOnlySpectator)
    {
        DrawMinimapSpectatorIcons();
    }
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
