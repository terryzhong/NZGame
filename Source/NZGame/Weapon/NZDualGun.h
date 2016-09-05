// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZGun.h"
#include "NZDualGun.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZDualGun : public ANZGun
{
	GENERATED_BODY()

	friend class UNZDualGunStateFiring;
	friend class UNZDualGunStateChangeClip;
	friend class UNZDualGunStateLeftHandChangeClip;

public:
	ANZDualGun(const FObjectInitializer& ObjectInitializer);

	/** Change clip */
	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
	UNZWeaponState* LeftHandChangeClipState;

	/* Left hand weapon ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite/*, Replicated, ReplicatedUsing = OnRep_LeftHandAmmo*/)
	int32 LeftHandAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite/*, Replicated, ReplicatedUsing = OnRep_LeftHandCarriedAmmo*/)
	int32 LeftHandCarriedAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,/*Replicated, ReplicatedUsing = OnRep_LeftHandCarriedAmmo,*/ Category = "DualGun")
	int32 LeftHandClipAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,/*Replicated, ReplicatedUsing = OnRep_LeftHandCarriedAmmo,*/ Category = "DualGun")
	int32 LeftHandMaxCarriedAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DualGun")
	USkeletalMeshComponent* LeftHandMesh;

	UPROPERTY()
	FVector FirstPLeftHandMeshOffset;

	UPROPERTY()
	FRotator FirstPLeftHandMeshRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DualGun")
	UAnimMontage* LeftHandChangeClipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DualGun")
	UAnimMontage* LeftHandChangeClipAnimHands;

	UFUNCTION()
	virtual void OnRep_LeftHandAmmo();

	UFUNCTION()
	virtual void OnRep_LeftHandCarriedAmmo();

	virtual void BeginPlay() override;

	virtual FVector InstantFireStartTrace() override;
	virtual FVector InstantFireEndTrace(FVector StartTrace) override;

	virtual void AttachToOwner_Implementation() override;
	virtual void DetachFromOwner_Implementation() override;

	virtual void ProcessHeadMountedDisplay(FVector HeadPosition, FRotator HeadRotation, FVector RightHandPosition, FRotator RightHandRotation, FVector LeftHandPosition, FRotator LeftHandRotation);
	
	/** clip changing related functions */
	virtual bool HasAmmo(uint8 FireModeNum);
	virtual void ConsumeAmmo(uint8 Amount);

	virtual void ChangeClip();
	virtual void LeftHandChangeClip();

	virtual void ChangeClipFinished();
	virtual void LeftHandChangeClipFinished();

	virtual void PlayChangeClipAnim();
	virtual void LeftHandPlayChangeClipAnim();

	virtual bool IsPlayingChangeClipAnim();
	virtual bool LeftHandIsPlayingChangeClipAnim();
	
	/** Firing entry point */
	virtual void PlayFiringEffects();
	virtual void PlayWeaponFiringAnim(uint8 FireModeNum);

protected:
	virtual void GotoChangeClipState();
	virtual void LeftHandGotoChangeClipState();

	UPROPERTY()
	USkeletalMeshComponent* LeftOverlayMesh;
};
