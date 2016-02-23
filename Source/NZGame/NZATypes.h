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

/**
 *
 */
USTRUCT()
struct FTextureUVs
{
    GENERATED_BODY()
    
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

/**
 *
 */
USTRUCT(BlueprintType)
struct FOverlayEffect
{
    GENERATED_BODY()
    
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
struct FBloodDecalInfo
{
    GENERATED_BODY()
    
    /** Material to use for the decal */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DecalInfo)
    UMaterialInterface* Material;
    
    /** Base scale of decal */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DecalInfo)
    FVector2D BaseScale;
    
    /** Range of random scaling applied (always uniform) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DecalInfo)
    FVector2D ScaleMultRange;
    
    FBloodDecalInfo()
        : Material(NULL)
        , BaseScale(32.0f, 32.0f)
        , ScaleMultRange(0.8f, 1.2f)
    {}
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
