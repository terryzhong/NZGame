// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZWeapon.h"
#include "NZWeapon_Enforcer.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZWeapon_Enforcer : public ANZWeapon
{
	GENERATED_BODY()
	
public:
	ANZWeapon_Enforcer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<class ANZWeaponAttachment> SingleWieldAttachmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<class ANZWeaponAttachment> DualWieldAttachmentType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	USkeletalMeshComponent* LeftMesh;

protected:
	UPROPERTY()
	USkeletalMeshComponent* LeftOverlayMesh;
public:

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
	UNZWeaponStateEquipping* EnforcerEquippingState;

	UPROPERTY(Instanced, BlueprintReadOnly, Category = States)
	UNZWeaponState* EnforcerUnequippingState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enforcer)
	float SpreadIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enforcer)
	float SpreadResetIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enforcer)
	float MaxSpread;

	UPROPERTY(BlueprintReadWrite, Category = Enforcer)
	float LastFireTime;

	UPROPERTY(BlueprintReadWrite, Category = Enforcer)
	float StoppingPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UAnimMontage* LeftBringUpAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (ClampMin = "0.1"))
	TArray<float> FireIntervalDualWield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<float> SpreadDualWield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float DualBringUpTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float DualPutDownTime;

	virtual float GetBringUpTime() override;
	virtual float GetPutDownTime() override;

	UPROPERTY()
	FVector FirstPLeftMeshOffset;

	UPROPERTY()
	FRotator FirstPLeftMeshRotation;

	UPROPERTY()
	int32 FireCount;

	UPROPERTY()
	bool bFireLeftSide;

	UPROPERTY()
	int32 ImpactCount;

	UPROPERTY()
	bool bDualEnforcerMode;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = BecomeDual, Category = Weapon)
	bool bBecomeDual;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TArray<UParticleSystemComponent*> LeftMuzzleFlash;

	virtual void UpdateViewBob(float DeltaTime) override;

	virtual void PlayFiringEffects() override;
	virtual void FireInstantHit(bool bDealDamage, FHitResult* OutHit) override;
	virtual bool StackPickup_Implementation(ANZInventory* ContainedInv) override;
	virtual void BringUp(float OverflowTime) override;
	virtual void PlayImpactEffects(const FVector& TargetLoc, uint8 FireMode, const FVector& SpawnLocation, const FRotator& SpawnRotation) override;
	virtual void UpdateOverlays() override;
	virtual void SetSkin(UMaterialInterface* NewSkin) override;
	virtual void GotoEquippingState(float OverflowTime) override;
	virtual void FireShot() override;
	virtual void StateChanged() override;
	virtual void UpdateWeaponHand() override;
	virtual TArray<UMeshComponent*> Get1PMeshes_Implementation() const;
	virtual float GetImpartedMomentumMag(AActor* HitActor) override;
	virtual void DetachFromOwner_Implementation() override;
	virtual void AttachToOwner_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void BecomeDual();

	virtual void DualEquipFinished();

	virtual void FiringInfoUpdated_Implementation(uint8 InFireMode, uint8 FlashCount, FVector InFlashLocation) override;

protected:
	virtual void AttachLeftMesh();
	
};
