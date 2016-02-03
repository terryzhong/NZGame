// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon_Enforcer.h"

const FName NAME_EnforcerKills(TEXT("EnforcerKills"));
const FName NAME_EnforcerDeaths(TEXT("EnforcerDeaths"));
const FName NAME_EnforcerHits(TEXT("EnforcerHits"));
const FName NAME_EnforcerShots(TEXT("EnforcerShots"));


ANZWeapon_Enforcer::ANZWeapon_Enforcer()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultGroup = 2;
	Ammo = 20;
	MaxAmmo = 40;
	LastFireTime = 0.f;
	SpreadResetIncrease = 1.f;
	SpreadIncrease = 0.03f;
	MaxSpread = 0.12f;
	BringUpTime = 0.28f;
	DualBringUpTime = 0.36f;
	PutDownTime = 0.2f;
	DualPutDownTime = 0.3f;
	StoppingPower = 30000.f;
	BaseAISelectRating = 0.4f;
	FireCount = 0;
	ImpactCount = 0;
	bDualEnforcerMode = false;
	bBecomeDual = false;
	bCanThrowWeapon = false;
	bFireLeftSide = false;
	FOVOffset = FVector(0.7f, 1.f, 1.f);
	MaxTracerDist = 2500.f;

	LeftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left"));
	LeftMesh->SetOnlyOwnerSee(true);
	LeftMesh->AttachParent = RootComponent;
	LeftMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	LeftMesh->bSelfShadowOnly = true;
	LeftMesh->bHiddenInGame = true;
	FirstPLeftMeshOffset = FVector(0.f);

	// todo:

	KillStatsName = NAME_EnforcerKills;
	DeathStatsName = NAME_EnforcerDeaths;
	HitsStatsName = NAME_EnforcerHits;
	ShotsStatsName = NAME_EnforcerShots;

	DisplayName = NSLOCTEXT("NZWeapon_Enforcer", "DisplayName", "Enforcer");
}

void ANZWeapon_Enforcer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

float ANZWeapon_Enforcer::GetBringUpTime()
{
	return 0.f;
}

float ANZWeapon_Enforcer::GetPutDownTime()
{
	return 0.f;
}

void ANZWeapon_Enforcer::UpdateViewBob(float DeltaTime)
{

}

void ANZWeapon_Enforcer::PlayFiringEffects()
{

}

void ANZWeapon_Enforcer::FireInstantHit(bool bDealDamage, FHitResult* OutHit)
{

}

bool ANZWeapon_Enforcer::StackPickup_Implementation(ANZInventory* ContainedInv)
{
	return 0.f;
}

void ANZWeapon_Enforcer::BringUp(float OverflowTime)
{

}

void ANZWeapon_Enforcer::PlayImpactEffects(const FVector& TargetLoc, uint8 FireMode, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{

}

void ANZWeapon_Enforcer::UpdateOverlays()
{

}

void ANZWeapon_Enforcer::SetSkin(UMaterialInterface* NewSkin)
{

}

void ANZWeapon_Enforcer::GotoEquippingState(float OverflowTime)
{

}

void ANZWeapon_Enforcer::FireShot()
{

}

void ANZWeapon_Enforcer::StateChanged()
{

}

void ANZWeapon_Enforcer::UpdateWeaponHand()
{

}

TArray<UMeshComponent*> ANZWeapon_Enforcer::Get1PMeshes_Implementation() const
{
	TArray<UMeshComponent*> Result = Super::Get1PMeshes_Implementation();
	Result.Add(LeftMesh);
	Result.Add(LeftOverlayMesh);
	return Result;
}

void ANZWeapon_Enforcer::BecomeDual()
{

}

float ANZWeapon_Enforcer::GetImpartedMomentumMag(AActor* HitActor)
{
	return 0.f;
}

void ANZWeapon_Enforcer::DetachFromOwner_Implementation()
{

}

void ANZWeapon_Enforcer::AttachToOwner_Implementation()
{

}

void ANZWeapon_Enforcer::DualEquipFinished()
{

}

void ANZWeapon_Enforcer::FiringInfoUpdated_Implementation(uint8 InFireMode, uint8 FlashCount, FVector InFlashLocation)
{

}

void ANZWeapon_Enforcer::AttachLeftMesh()
{

}
