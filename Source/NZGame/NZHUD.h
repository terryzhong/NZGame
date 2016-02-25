// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "NZBasePlayerController.h"
#include "NZHUD.generated.h"


USTRUCT()
struct FEnemyDamageNumber 
{
	GENERATED_BODY()

public:

};

/**
 * 
 */
UCLASS(Config = Game)
class NZGAME_API ANZHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    class ANZPlayerController* NZPlayerOwner;
    
    UPROPERTY(Transient)
    TArray<class UNZHUDWidget*> HudWidgets;
    
	TMap<FName, class UNZHUDWidgetMessage*> HudMessageWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TArray<FString> HudWidgetClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* TinyFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* SmallFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* MediumFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* LargeFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* HugeFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* NumberFont;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	bool bFontsCached;

	virtual void CacheFonts();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	float WidgetOpacity;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	class UNZHUDWidget_Spectator* SpectatorMessageWidget;


	UPROPERTY(BlueprintReadWrite, Category = HUD)
	TArray<FEnemyDamageNumber> DamageNumbers;

	UPROPERTY()
	bool bDrawDamageNumbers;

	virtual void DrawDamageNumbers();

    
    UPROPERTY(BlueprintReadWrite, Category = HUD)
    float LastPickupTime;
    
    /** Last time player owning this HUD killed someone */
    UPROPERTY(BlueprintReadWrite, Category = HUD)
    float LastKillTime;
    
    /** Last time player owning this HUD picked up a flag */
    UPROPERTY(BlueprintReadWrite, Category = HUD)
    float LastFlagGrabTime;
    
    /** Sound played when player gets a kill */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Message)
    USoundBase* KillSound;
    
	FTimerHandle PlayKillHandle;

	virtual void NotifyKill();

	virtual void PlayKillNotification();

	UTexture2D* DefaultCrosshairTex;

    /** 181
     Last time we hit an enemy in LOS */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    float LastConfirmedHitTime;
    
	virtual void AddSpectatorWidgets();

	virtual void BeginPlay();

	virtual void PostInitializeComponents();

	TSubclassOf<UNZHUDWidget> ResolveHudWidgetByName(const TCHAR* ResourceName);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = HUD)
	virtual UNZHUDWidget* AddHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass);

	UNZHUDWidget* FindHudWidgetByClass(TSubclassOf<UNZHUDWidget> SearchWidgetClass, bool bExactClass = false);

	virtual void PostRender();

	virtual void DrawHUD() override;

	virtual void ReceiveLocalMessage(TSubclassOf<class UNZLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject = NULL);
    
	UFUNCTION(BlueprintCallable, Category = HUD)
	virtual void ToggleScoreboard(bool bShow);

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	uint32 bShowScores : 1;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	uint32 bDrawMinimap : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear)
	UTexture2D* PlayerMinimapTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear)
	UTexture2D* SelectedPlayerTexture;

	UPROPERTY(BlueprintReadWrite, Category = HUD)
	uint32 bForceScores : 1;

	//virtual void CreateScoreboard(TSubclassOf<class UNZScoreboard> NewScoreboardClass);

	virtual void DrawDamageIndicators();

	/** Get player state for which to display scoring info */
	virtual ANZPlayerState* GetScorerPlayerState();

public:
	virtual void BuildHudWidget(FString NewWidgetString);

	virtual bool HasHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass);

protected:
	virtual FVector2D JSon2FVector2D(const TSharedPtr<FJsonObject> Vector2DObject, FVector2D Default);

	float CalcStandingTime;

public:
	void CalcStanding();

	FText ConvertTime(FText Prefix, FText Suffix, int32 Seconds, bool bForceHours = false, bool bForceMinutes = true, bool bForceTwoDigits = true) const;

	FText GetPlaceSuffix(int32 Value);

	void DrawString(FText Text, float X, float Y, ETextHorzPos::Type HorzAlignment, ETextVertPos::Type VertAlignment, UFont* Font, FLinearColor Color, float Scale = 1.0, bool bOutline = false);
	void DrawNumber(int32 Number, float X, float Y, FLinearColor Color, float GlowOpacity, float Scale, int32 MinDigits = 0, bool bRightAlign = false);

public:

    virtual float GetCrosshairScale();
    virtual FLinearColor GetCrosshairColor(FLinearColor InColor) const;
    
    /** Returns the necessary input mode for the hud this tick */
    UFUNCTION(BlueprintNativeEvent)
    EInputMode::Type GetInputMode() const;

	UPROPERTY(globalconfig)
	bool bCustomWeaponCrosshairs;
    
	UFUNCTION(BlueprintCallable, Category = Crosshair)
	class UNZCrosshair* GetCrosshair(ANZWeapon* Weapon);

	FCrosshairInfo* GetCrosshairInfo(ANZWeapon* Weapon);

	UPROPERTY()
	TArray<class UNZCrosshair*> LoadedCrosshairs;

	virtual bool OverrideMouseClick(FKey Key, EInputEvent EventType)
	{
		return false;
	}

	UPROPERTY()
	UCanvasRenderTarget2D* MinimapTexture;

	FMatrix MinimapTransform;

	FVector2D MinimapOffset;

	FMatrix MapToScreen;

	UFUNCTION()
	virtual void UpdateMinimapTexture(UCanvas* C, int32 Width, int32 Height);

	virtual void CreateMinimapTexture();

	UFUNCTION(BlueprintCallable, Category = HUD)
	virtual void DrawMinimap(const FColor& DrawColor, float MapSize, FVector2D DrawPos);

	UFUNCTION(BlueprintCallable, Category = HUD)
	virtual void DrawMinimapIcon(UTexture2D* Texture, FVector2D Pos, FVector2D DrawSize, FVector2D UV, FVector2D UVL, FLinearColor DrawColor, bool bDropShadow);

	virtual void DrawMinimapSpectatorIcons();

protected:
	virtual void CalcMinimapTransform(const FBox& LevelBox, int32 MapWidth, int32 MapHeight);

	FVector2D WorldToMapToScreen(const FVector& InPos) const
	{
		return FVector2D(MapToScreen.TransformPosition(MinimapTransform.TransformPosition(InPos)));
	}
};
