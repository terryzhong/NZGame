// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeaponAttachment.h"
#include "NZWeaponAttachment_Enforcer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZWeaponAttachment_Enforcer : public ANZWeaponAttachment
{
	GENERATED_BODY()
	
public:
    ANZWeaponAttachment_Enforcer();
    
    /** Third person left hand mesh */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
    USkeletalMeshComponent* LeftMesh;
    
    UPROPERTY()
    USkeletalMeshComponent* LeftOverlayMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FName LeftAttachSocket;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
    FVector LeftAttachOffset;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
    int32 BurstSize;
    
    UPROPERTY()
    uint32 AlternateCount;
    
    virtual void BeginPlay() override;
    virtual void AttachToOwnerNative() override;
    virtual void PlayFiringEffects() override;
    virtual void StopFiringEffects(bool bIgnoreCurrentMode) override;
    virtual void UpdateOverlays() override;
    virtual void SetSkin(UMaterialInterface* NewSkin) override;
};
