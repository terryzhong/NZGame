// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "NZPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class NZGAME_API ANZPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
    /** Post process settings used when there are no post process volumes */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PostProcess)
    FPostProcessSettings DefaultPPSettings;
    
    /** Post process settings used when we want to be in stylized mode */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PostProcess)
    TArray<FPostProcessSettings> StylizedPPSettings;
    
    FVector LastThirdPersonCameraLoc;
    
    UPROPERTY()
    AActor* LastThirdPersonTarget;
    
    UPROPERTY(Config)
    float ThirdPersonCameraSmoothingSpeed;
    
    UPROPERTY()
    FVector FlagBaseFreeCamOffset;
    
    UPROPERTY()
    FVector EndGameFreeCamOffset;
    
    /** Offset to Z free camera position */
    UPROPERTY()
    float EndGameFreeCamDistance;
    
    UPROPERTY()
    FVector DeathCamOffset;
    
    /** Offset to Z death camera position */
    UPROPERTY()
    float DeathCamDistance;
    
    UPROPERTY(Config)
    float CurrentCamBonus;
    
    UPROPERTY(Config)
    float FlagCamBonus;
    
    UPROPERTY(Config)
    float PowerupBonus;
    
    /** Bonus for having higher score than current focus */
    UPROPERTY(Config)
    float HigherScoreBonus;
    
    UPROPERTY()
    float CurrentActionBonus;
    
    UPROPERTY()
    float CurrentCameraRoll;
    
    UPROPERTY()
    float WallSlideCameraRoll;
    
    /** Sweep to find valid third person camera offset */
    virtual void CheckCameraSweep(FHitResult& OutHit, AActor* TargetActor, const FVector& Start, const FVector& End);
    
    /**
     * Get CameraStyle after state based and gametype based override logic
     * Generally NZ code should always query the current camera style through this method to account for ragdoll, etc
     */
    virtual FName GetCameraStyleWithOverrides() const;
    
    virtual void UpdateCamera(float DeltaTime) override;
    virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
    virtual void ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutPOV) override;
	
    /** Rate player as camera focus for spectating */
    virtual float RatePlayerCamera(class ANZPlayerState* InPS, class ANZCharacter* Character, APlayerState* CurrentCamPS);
    
    virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
};
