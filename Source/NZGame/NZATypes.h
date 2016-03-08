// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZATypes.generated.h"



namespace InventoryEventName
{
    const FName Landed = FName(TEXT("Landed"));
    const FName LandedWater = FName(TEXT("LandedWater"));
    const FName FiredWeapon = FName(TEXT("FiredWeapon"));
    const FName Jump = FName(TEXT("Jump"));
    const FName MultiJump = FName(TEXT("MultiJump"));
    const FName Dodge = FName(TEXT("Dodge"));
}

UENUM()
namespace ETextHorzPos
{
	enum Type
	{
		Left,
		Center,
		Right,
		MAX,
	};
}

UENUM()
namespace ETextVertPos
{
	enum Type
	{
		Top,
		Center,
		Bottom,
		MAX,
	};
}

/**
 *
 */
USTRUCT()
struct FTextureUVs
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureUVs")
    float U;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureUVs")
    float V;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureUVs")
    float UL;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureUVs")
    float VL;
    
    FTextureUVs()
    : U(0.0f)
    , V(0.0f)
    , UL(0.0f)
    , VL(0.0f)
    {};
    
    FTextureUVs(float inU, float inV, float inUL, float inVL)
    {
        U = inU; V = inV; UL = inUL;  VL = inVL;
    }
};

USTRUCT(BlueprintType)
struct FHUDRenderObject 
{
	GENERATED_BODY()

public:
	/** Set to true to make this renderobject hidden */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	bool bHidden;

	/** The depth priority, Higher means rendered later */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	float RenderPriority;

	/** Where (in unscaled pixels) should this HUDObject be displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D Position;

	/** How big (in unscaled pixels) is this HUDObject. NOTE: the HUD object will be scaled to fit the size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D Size;

	/** The text color to display this in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FLinearColor RenderColor;

	/** An override for the opacity of this object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	float RenderOpacity;

	FHUDRenderObject()
	{
		RenderPriority = 0.0f;
		RenderColor = FLinearColor::White;
		RenderOpacity = 1.0f;
	}

	virtual float GetWidth() { return Size.X; }
	virtual float GetHeight() { return Size.Y; }
};

USTRUCT(BlueprintType)
struct FHUDRenderObject_Texture : public FHUDRenderObject 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	UTexture* Atlas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FTextureUVs UVs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	bool bUseTeamColors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	TArray<FLinearColor> TeamColorOverrides;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	bool bIsBorderElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	bool bIsSlateElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D RenderOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	float Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D RotPivot;

	FHUDRenderObject_Texture() : FHUDRenderObject()
	{
		Atlas = NULL;
		bUseTeamColors = false;
		bIsBorderElement = false;
		Rotation = 0.0f;
	}

	virtual float GetWidth()
	{
		return (Size.X <= 0) ? UVs.UL : Size.X;
	}

	virtual float GetHeight()
	{
		return (Size.Y <= 0) ? UVs.VL : Size.Y;
	}
};

/**
 * This is a simple delegate that returns an FTEXT value for rendering things in HUD render widgets
 */
DECLARE_DELEGATE_RetVal(FText, FNZGetTextDelegate)

USTRUCT(BlueprintType)
struct FHUDRenderObject_Text : public FHUDRenderObject 
{
	GENERATED_BODY()

public:
	FNZGetTextDelegate GetTextDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	UFont* Font;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	float TextScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	bool bDrawShadow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D ShadowDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FLinearColor ShadowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	bool bDrawOutline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FLinearColor OutlineColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	TEnumAsByte<ETextHorzPos::Type> HorzPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	TEnumAsByte<ETextVertPos::Type> VertPosition;

	FHUDRenderObject_Text() : FHUDRenderObject()
	{
		Font = NULL;
		TextScale = 1.0f;
		bDrawShadow = false;
		ShadowColor = FLinearColor::White;
		bDrawOutline = false;
		OutlineColor = FLinearColor::Black;
		HorzPosition = ETextHorzPos::Left;
		VertPosition = ETextVertPos::Top;
	}

public:
	FVector2D GetSize()
	{
		if (Font)
		{
			FText TextToRender = Text;
			if (GetTextDelegate.IsBound())
			{
				TextToRender = GetTextDelegate.Execute();
			}

			int32 Width = 0;
			int32 Height = 0;
			Font->GetStringHeightAndWidth(TextToRender.ToString(), Height, Width);
			return FVector2D(Width * TextScale, Height * TextScale);
		}
		return FVector2D(0, 0);
	}
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FOverlayEffect
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* Material;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UParticleSystem* Particles;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ParticleAttachPoint;
    
    explicit FOverlayEffect(UMaterialInterface* InMaterial = NULL, UParticleSystem* InParticles = NULL, FName InParticleAttachPoint = NAME_None)
    : Material(InMaterial)
    , Particles(InParticles)
    , ParticleAttachPoint(InParticleAttachPoint)
    {}
    
    inline bool operator==(const FOverlayEffect& B) const
    {
        return Material == B.Material && Particles == B.Particles;
    }
    inline bool IsValid() const
    {
        return Material != NULL || Particles != NULL;
    }
    inline FString ToString() const
    {
        return FString::Printf(TEXT("(Material=%s, Particles=%s)"), *GetFullNameSafe(Material), *GetFullNameSafe(Particles));
    }
};

USTRUCT(BlueprintType)
struct FCrosshairInfo 
{
	GENERATED_BODY()

public:
	FCrosshairInfo()
	{
		// Global is used to describe the crosshair that is used when bCustomWeaponCrosshairs == false
		WeaponClassName = TEXT("Global");
		CrosshairClassName = TEXT("");
		Color = FLinearColor::White;
		Scale = 1.0f;
	}

	UPROPERTY(EditAnywhere, GlobalConfig, Category = CrosshairInfo)
	FString CrosshairClassName;

	UPROPERTY(EditAnywhere, GlobalConfig, Category = CrosshairInfo)
	FString WeaponClassName;

	UPROPERTY(EditAnywhere, GlobalConfig, Category = CrosshairInfo)
	float Scale;

	UPROPERTY(EditAnywhere, GlobalConfig, Category = CrosshairInfo)
	FLinearColor Color;

	bool operator==(const FCrosshairInfo& Other) const
	{
		return WeaponClassName == Other.WeaponClassName;
	}
};



/**
 *
 */
UCLASS()
class NZGAME_API ANZATypes : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZATypes();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
