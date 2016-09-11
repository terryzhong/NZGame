// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "NZBasePlayerController.h"
#include "NZCrosshair.h"
#include "UserWidget.h"
#include "NZHUD.generated.h"

const uint32 MAX_DAMAGE_INDICATORS = 3;     // Max number of damage indicators on the screen at any one time
const float DAMAGE_FADE_DURATION = 1.0f;    // How fast a damage indicator fades out


USTRUCT()
struct FDamageHudIndicator
{
    GENERATED_BODY()
    
public:
    UPROPERTY()
    float RotationAngle;
    
    UPROPERTY()
    float DamageAmount;
    
    UPROPERTY()
    float FadeTime;
    
    UPROPERTY()
    bool bFriendlyFire;
    
    FDamageHudIndicator()
        : RotationAngle(0.0f)
        , DamageAmount(0.0f)
        , FadeTime(0.0f)
        , bFriendlyFire(false)
    {
    }
};

USTRUCT()
struct FEnemyDamageNumber 
{
	GENERATED_BODY()

public:
    UPROPERTY()
    APawn* DamagedPawn;
    
    UPROPERTY()
    float DamageTime;
    
    UPROPERTY()
    uint8 DamageAmount;
    
    UPROPERTY()
    FVector WorldPosition;
    
    UPROPERTY()
    float Scale;
    
    FEnemyDamageNumber()
        : DamagedPawn(NULL)
        , DamageTime(0.f)
        , DamageAmount(0)
        , WorldPosition(FVector(0.f))
        , Scale(1.f)
    {
    }
    
    FEnemyDamageNumber(APawn* InPawn, float InTime, uint8 InDamage, FVector InLoc, float InScale)
        : DamagedPawn(InPawn)
        , DamageTime(InTime)
        , DamageAmount(InDamage)
        , WorldPosition(InLoc)
        , Scale(InScale)
    {
    }
};

/**
 * 
 */
UCLASS(Config = Game)
class NZGAME_API ANZHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    ANZHUD();
    
    /** Holds the NZPlayerController that owns this hud. NOTE: This is only valid during the render phase */
    class ANZPlayerController* NZPlayerOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Widgets")
	TArray<TSubclassOf<class UUserWidget> > HubUserWidgetClasses;

	TArray<class UUserWidget*> HubUserWidgets;





    
    /** Holds the list of all hud widgets that are currently active */
    UPROPERTY(Transient)
    TArray<class UNZHUDWidget*> HudWidgets;
    
    /** Holds a list of HudWidgets that manage the messaging system */
	TMap<FName, class UNZHUDWidgetMessage*> HudMessageWidgets;

    /** Holds a list of hud widgets to load. These are hardcoded widgets that can't be changed via the ini and are stored as strings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TArray<FString> HudWidgetClasses;

    /** The tiny font to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* TinyFont;

    /** The small font to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* SmallFont;

    /** The med font to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* MediumFont;

    /** The large font to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* LargeFont;

    /** The extreme font to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* HugeFont;
    
    /** The score font to use */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    class UFont* ScoreFont;

    /** The font that only contains numbers */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UFont* NumberFont;

    /** Set when HUD fonts have been cached */
	UPROPERTY(BlueprintReadOnly, Category = HUD)
	bool bFontsCached;

    /** Cache fonts this HUD will use */
	virtual void CacheFonts();

    /** The global opacity for HUD widgets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	float WidgetOpacity;

    /** Cached reference to the spectator message widget */
	UPROPERTY(BlueprintReadOnly, Category = HUD)
	class UNZHUDWidget_Spectator* SpectatorMessageWidget;


    /** Damage values caused by viewed player recently */
	UPROPERTY(BlueprintReadWrite, Category = HUD)
	TArray<FEnemyDamageNumber> DamageNumbers;

	UPROPERTY()
	bool bDrawDamageNumbers;

    /** Draw in screen space damage recently applied by this player to other characters */
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

    /** Queue a kill notification */
	virtual void NotifyKill();

    /** Play kill notification sound and icon */
	virtual void PlayKillNotification();

    /** Crosshair asset pointer */
	UTexture2D* DefaultCrosshairTex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scoreboard)
	UTexture2D* HUDAtlas;

    /** Last time we hit an enemy in LOS */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    float LastConfirmedHitTime;
    
    /** Active damage indicators. NOTE: if FadeTime == 0 then it's not in use */
    UPROPERTY()
    TArray<struct FDamageHudIndicator> DamageIndicators;
    
    /** 
     * This is a list of hud widgets that are defined in DefaultGame.ini to be loaded.
     * NOTE: you can use embedded JSON to set their position. See BuildHudWidget().
     */
    UPROPERTY(Config = Game)
    TArray<FString> RequiredHudWidgetClasses;
    
    /**
     * This is a list of hud widgets that are defined in DefaultGame.ini to be loaded for spectators.
     * NOTE: you can use embedded JSON to set their position. See BuildHudWidget().
     */
    UPROPERTY(Config = Game)
    TArray<FString> SpectatorHudWidgetClasses;
    
    UPROPERTY()
    bool bHaveAddedSpectatorWidgets;
    
	virtual void AddSpectatorWidgets();

    /** Add any of the blueprint based hud widgets */
	virtual void BeginPlay();

	virtual void PostInitializeComponents();

    /** Loads the subclass of a hudwidget using just the resource name */
	TSubclassOf<UNZHUDWidget> ResolveHudWidgetByName(const TCHAR* ResourceName);

    /** Creates and adds a hud widget */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = HUD)
	virtual UNZHUDWidget* AddHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass);

	UNZHUDWidget* FindHudWidgetByClass(TSubclassOf<UNZHUDWidget> SearchWidgetClass, bool bExactClass = false);

    /** We override PostRender so that we can cache bunch of vars that need caching */
	virtual void PostRender();

    /** Primary draw call for the HUD */
	virtual void DrawHUD() override;

    /** Receive a localized message from somewhere to be displayed */
	virtual void ReceiveLocalMessage(TSubclassOf<class UNZLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject = NULL);
    
	UFUNCTION(BlueprintCallable, Category = HUD)
	virtual void ToggleScoreboard(bool bShow);

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	uint32 bShowScores : 1;

	UPROPERTY(BlueprintReadOnly, Category = HUD)
	uint32 bDrawMinimap : 1;

    /** Icon for player on the minimap (rotated BG that indicates direction) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear)
	UTexture2D* PlayerMinimapTexture;

    /** Drawn over selected player on the minimap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear)
	UTexture2D* SelectedPlayerTexture;

	UPROPERTY(BlueprintReadWrite, Category = HUD)
	uint32 bForceScores : 1;

    /** Creates the scoreboard */
	virtual void CreateScoreboard(TSubclassOf<class UNZScoreboard> NewScoreboardClass);

    virtual void PawnDamaged(FVector HitLocation, int32 DamageAmount, bool bFriendlyFire);
	virtual void DrawDamageIndicators();

    /** Called when PlayerOwner caused damage to HitPawn */
    virtual void CausedDamage(APawn* HitPawn, int32 Damage);
    
    virtual class UFont* GetFontFromSizeIndex(int32 FontSizeIndex) const;
    
    /** Returns the base color for this hud. All HUD widgets will start with this */
    virtual FLinearColor GetBaseHUDColor();
    
	/** Get player state for which to display scoring info */
	virtual ANZPlayerState* GetScorerPlayerState();
    
    virtual void NotifyMatchStateChange();
    
    FTimerHandle MatchSummaryHandle;
    
    virtual void OpenMatchSummary();
    
public:
	UPROPERTY(globalconfig)
	float HUDWidgetOpacity;
    
	UPROPERTY(globalconfig)
	float HUDWidgetBorderOpacity;

	UPROPERTY(globalconfig)
	float HUDWidgetSlateOpacity;

	UPROPERTY(globalconfig)
	float HUDWidgetWeaponBarInactiveOpacity;

	UPROPERTY(globalconfig)
	float HUDWidgetWeaponBarScaleOverride;

	UPROPERTY(globalconfig)
	float HUDWidgetWeaponBarInactiveIconOpacity;

	UPROPERTY(globalconfig)
	float HUDWidgetWeaponBarEmptyOpacity;

	UPROPERTY(globalconfig)
	float HUDWidgetScaleOverride;

	UPROPERTY(globalconfig)
	bool bUseWeaponColors;
    
    
    /** Accessor for CachedTeamColor */
    FLinearColor GetWidgetTeamColor();


	int32 NumActualPlayers;

	UPROPERTY()
	TArray<ANZPlayerState*> Leaderboard;



public:
	virtual void BuildHudWidget(FString NewWidgetString);

	virtual bool HasHudWidget(TSubclassOf<UNZHUDWidget> NewWidgetClass);

protected:
    /** Helper function to take a JSON object and try to convert it to the FVector2D */
	virtual FVector2D JSon2FVector2D(const TSharedPtr<FJsonObject> Vector2DObject, FVector2D Default);

    /** Last time CalcStanding() was run */
	float CalcStandingTime;

public:
    /** Calculates the currently viewed player's standing. NOTE: Happens once per frame */
	void CalcStanding();

    /** Takes seconds and converts in to a string */
	FText ConvertTime(FText Prefix, FText Suffix, int32 Seconds, bool bForceHours = false, bool bForceMinutes = true, bool bForceTwoDigits = true) const;

    /** Creates a suffix string based on a value (st, nd, rd, th) */
	FText GetPlaceSuffix(int32 Value);

	void DrawString(FText Text, float X, float Y, ETextHorzPos::Type HorzAlignment, ETextVertPos::Type VertAlignment, UFont* Font, FLinearColor Color, float Scale = 1.0, bool bOutline = false);
	void DrawNumber(int32 Number, float X, float Y, FLinearColor Color, float GlowOpacity, float Scale, int32 MinDigits = 0, bool bRightAlign = false);
    
    virtual float GetCrosshairScale();
    virtual FLinearColor GetCrosshairColor(FLinearColor InColor) const;

private:
    UTexture2D* DamageIndicatorTexture;

public:
    /** Returns the necessary input mode for the hud this tick */
    UFUNCTION(BlueprintNativeEvent)
    EInputMode::Type GetInputMode() const;
    
    /** The list of crosshair information for each weapon */
    UPROPERTY(globalconfig)
    TArray<FCrosshairInfo> CrosshairInfos;

    /** If true, crosshairs can be unique per weapon */
	UPROPERTY(globalconfig)
	bool bCustomWeaponCrosshairs;
    
    /** Gets the crosshair for the weapon. Creates a new one if necessary */
	UFUNCTION(BlueprintCallable, Category = Crosshair)
	class UNZCrosshair* GetCrosshair(ANZWeapon* Weapon);

	FCrosshairInfo* GetCrosshairInfo(ANZWeapon* Weapon);

	UPROPERTY()
	TArray<class UNZCrosshair*> LoadedCrosshairs;

    /**
     * Called by PlayerController (locally) when clicking mouse while crosshair is selected
     * Return true to override default behavior
     */
	virtual bool OverrideMouseClick(FKey Key, EInputEvent EventType)
	{
		return false;
	}

    /** Render target for the minimap */
	UPROPERTY()
	UCanvasRenderTarget2D* MinimapTexture;

    /** Transformation matrix from world locations to minimap locatons */
	FMatrix MinimapTransform;

    /** Offset when displaying minimap snug to an edge */
	FVector2D MinimapOffset;

    /** Map transform for rendering on screen (used to convert clicks to map locations) */
	FMatrix MapToScreen;

    /** Draw the static pre-rendered portions of the minimap to the MinimapTexture */
	UFUNCTION()
	virtual void UpdateMinimapTexture(UCanvas* C, int32 Width, int32 Height);

	virtual void CreateMinimapTexture();

    /** 
     * Draws minimap; creates and updates the minimap render-to-texture if it hasn't been already
     * Sets MapToScreen so subclasses can easily override and use WorldToMapToScreen() to place icons
     */
	UFUNCTION(BlueprintCallable, Category = HUD)
	virtual void DrawMinimap(const FColor& DrawColor, float MapSize, FVector2D DrawPos);

    /** Draw a minimap icon that is a included in a large texture */
	UFUNCTION(BlueprintCallable, Category = HUD)
	virtual void DrawMinimapIcon(UTexture2D* Texture, FVector2D Pos, FVector2D DrawSize, FVector2D UV, FVector2D UVL, FLinearColor DrawColor, bool bDropShadow);

	virtual void DrawMinimapSpectatorIcons();

protected:
    /** Calculates MinimapTransform from the given level bounding box */
	virtual void CalcMinimapTransform(const FBox& LevelBox, int32 MapWidth, int32 MapHeight);

	FVector2D WorldToMapToScreen(const FVector& InPos) const
	{
		return FVector2D(MapToScreen.TransformPosition(MinimapTransform.TransformPosition(InPos)));
	}
};
