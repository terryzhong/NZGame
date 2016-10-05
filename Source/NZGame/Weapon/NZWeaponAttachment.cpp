// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponAttachment.h"
#include "NZCharacter.h"
#include "NZProjectile.h"
#include "NZWorldSettings.h"


// Sets default values
ANZWeaponAttachment::ANZWeaponAttachment()
{
    RootComponent = CreateDefaultSubobject<USceneComponent, USceneComponent>(TEXT("DummyRoot"), false);
    
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    Mesh->bLightAttachmentsAsGroup = true;
    Mesh->bReceivesDecals = false;
    Mesh->bUseAttachParentBound = true;
    
    AttachSocket = FName(TEXT("WeaponPoint"));
    HolsterSocket = FName(TEXT("spine_02"));
    HolsterOffset = FVector(0.f, 0.f, 0.f);
    HolsterRotation = FRotator(0.f, 0.f, 0.f);
    PickupScaleOverride = FVector(2.0f, 2.0f, 2.0f);
    WeaponStance = 0;
    
    bCopyWeaponImpactEffect = true;
    
    MaxBulletWhipDist = 200.0f;
}

bool ANZWeaponAttachment::CancelImpactEffect(const FHitResult& ImpactHit)
{
    return ((!ImpactHit.Actor.IsValid() && !ImpactHit.Component.IsValid()) ||
            Cast<ANZCharacter>(ImpactHit.Actor.Get()) ||
            Cast<ANZProjectile>(ImpactHit.Actor.Get()));
}

void ANZWeaponAttachment::BeginPlay()
{
    Super::BeginPlay();
    
    ANZWeapon::InstanceMuzzleFlashArray(this, MuzzleFlash);
    
    NZOwner = Cast<ANZCharacter>(Instigator);
    if (NZOwner != NULL)
    {
        WeaponType = NZOwner->GetWeaponClass();
        if (WeaponType != NULL && bCopyWeaponImpactEffect && ImpactEffectManager == NULL)
        {
            ImpactEffectManagerClass = WeaponType.GetDefaultObject()->ImpactEffectManagerClass;
            if (ImpactEffectManagerClass != NULL)
            {
                //ImpactEffectManager = Cast<UNZImpactEffectManager>(StaticConstructObject(ImpactEffectManagerClass));
				ImpactEffectManager = NewObject<UNZImpactEffectManager>(this, ImpactEffectManagerClass);
			}
            
            ImpactEffectSkipDistance = WeaponType.GetDefaultObject()->ImpactEffectSkipDistance;
            MaxImpactEffectSkipTime = WeaponType.GetDefaultObject()->MaxImpactEffectSkipTime;
        }
    }
    else
    {
        //UE_LOG(NZ, Warning, TEXT("UTWeaponAttachment: Bad Instigator: %s"), *GetNameSafe(Instigator));
    }
}

void ANZWeaponAttachment::RegisterAllComponents()
{
    for (int32 i = 0; i < MuzzleFlash.Num(); i++)
    {
        if (MuzzleFlash[i] != NULL)
        {
            MuzzleFlash[i]->bAutoActivate = false;
            MuzzleFlash[i]->SecondsBeforeInactive = 0.0f;
            MuzzleFlash[i]->SetOwnerNoSee(false);
            MuzzleFlash[i]->bUseAttachParentBound = true;
        }
    }
    Super::RegisterAllComponents();
}

void ANZWeaponAttachment::Destroyed()
{
    DetachFromOwner();
    GetWorldTimerManager().ClearAllTimersForObject(this);
    Super::Destroyed();
}

void ANZWeaponAttachment::AttachToOwner_Implementation()
{
    AttachToOwnerNative();
}

void ANZWeaponAttachment::DetachFromOwner_Implementation()
{
    if (Mesh != NULL)
    {
        //Mesh->DetachFromParent();
		Mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    }
}

void ANZWeaponAttachment::HolsterToOwner_Implementation()
{
    HolsterToOwnerNative();
}

void ANZWeaponAttachment::PlayFiringEffects()
{
    // Stop any firing effects for other firemodes
    // This is needed because if the user swaps firemodes very quickly replication might not receive a discrete stop and start new
    StopFiringEffects(true);
    
    if (WeaponType && (WeaponType.GetDefaultObject()->BaseAISelectRating > 0.f))
    {
        NZOwner->LastWeaponFireTime = GetWorld()->GetTimeSeconds();
    }
    
    ANZWorldSettings* WS = Cast<ANZWorldSettings>(GetWorld()->GetWorldSettings());
    bool bEffectsRelevant = (WS == NULL || WS->EffectIsRelevant(NZOwner, NZOwner->GetActorLocation(), true, NZOwner->IsLocallyControlled(), 50000.0f, 2000.0f));
    if (!bEffectsRelevant && !NZOwner->FlashLocation.IsZero())
    {
        bEffectsRelevant = WS->EffectIsRelevant(NZOwner, NZOwner->FlashLocation, true, NZOwner->IsLocallyControlled(), 50000.0f, 2000.0f);
        if (!bEffectsRelevant)
        {
            // Do frustum check versus fire line; Can't use simple vis to location because the fire line may be visible while both endpoints are not
            for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
            {
                if (It->PlayerController != NULL)
                {
                    FSceneViewProjectionData Projection;
                    if (It->GetProjectionData(It->ViewportClient->Viewport, eSSP_FULL, Projection))
                    {
                        FConvexVolume Frustum;
                        GetViewFrustumBounds(Frustum, Projection.ComputeViewProjectionMatrix(), false);
                        FPoly TestPoly;
                        TestPoly.Init();
                        TestPoly.InsertVertex(0, NZOwner->GetActorLocation());
                        TestPoly.InsertVertex(1, NZOwner->FlashLocation);
                        TestPoly.InsertVertex(2, (NZOwner->GetActorLocation() + NZOwner->FlashLocation) * 0.5f + FVector(0.0f, 0.0f, 1.0f));
                        if (Frustum.ClipPolygon(TestPoly))
                        {
                            bEffectsRelevant = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    if (!bEffectsRelevant)
    {
        return;
    }
    
    //UE_LOG(NZ, Log, TEXT("Relevant"));
    
    // Update owner mesh and self so effects are spawned in the correct place
    if (!Mesh->ShouldUpdateTransform(false))
    {
        if (NZOwner->GetMesh() != NULL)
        {
            NZOwner->GetMesh()->RefreshBoneTransforms();
            NZOwner->GetMesh()->UpdateComponentToWorld();
        }
        Mesh->RefreshBoneTransforms();
        Mesh->UpdateComponentToWorld();
    }
    
    // Muzzle flash
    if (MuzzleFlash.IsValidIndex(NZOwner->FireMode) && MuzzleFlash[NZOwner->FireMode] != NULL && MuzzleFlash[NZOwner->FireMode]->Template != NULL)
    {
        // If we detect a looping particle system, then don't reactivate it
        if (!MuzzleFlash[NZOwner->FireMode]->bIsActive || MuzzleFlash[NZOwner->FireMode]->bSuppressSpawning || !IsLoopingParticleSystem(MuzzleFlash[NZOwner->FireMode]->Template))
        {
            MuzzleFlash[NZOwner->FireMode]->ActivateSystem();
        }
    }
    
    static FName NAME_HitLocation(TEXT("HitLocation"));
    static FName NAME_LocalHitLocation(TEXT("LocalHitLocation"));
    const FVector SpawnLocation = (MuzzleFlash.IsValidIndex(NZOwner->FireMode) && MuzzleFlash[NZOwner->FireMode] != NULL)
        ? MuzzleFlash[NZOwner->FireMode]->GetComponentLocation()
        : NZOwner->GetActorLocation() + NZOwner->GetActorRotation().RotateVector(FVector(NZOwner->GetSimpleCollisionCylinderExtent().X, 0.0f, 0.0f));
    if (FireEffect.IsValidIndex(NZOwner->FireMode) && FireEffect[NZOwner->FireMode] != NULL)
    {
        UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect[NZOwner->FireMode], SpawnLocation, (NZOwner->FlashLocation - SpawnLocation).Rotation(), true);
        PSC->SetVectorParameter(NAME_HitLocation, NZOwner->FlashLocation);
        PSC->SetVectorParameter(NAME_LocalHitLocation, PSC->ComponentToWorld.InverseTransformPosition(NZOwner->FlashLocation));
        ModifyFireEffect(PSC);
    }
    // Perhaps the muzzle flash also contains hit effect (constant beam, etc) so set the parameter on it instead
    else if (MuzzleFlash.IsValidIndex(NZOwner->FireMode) && MuzzleFlash[NZOwner->FireMode] != NULL)
    {
        MuzzleFlash[NZOwner->FireMode]->SetVectorParameter(NAME_HitLocation, NZOwner->FlashLocation);
        MuzzleFlash[NZOwner->FireMode]->SetVectorParameter(NAME_LocalHitLocation, MuzzleFlash[NZOwner->FireMode]->ComponentToWorld.InverseTransformPosition(NZOwner->FlashLocation));
    }
    
    if (!NZOwner->FlashLocation.IsZero() &&
        ((NZOwner->FlashLocation - LastImpactEffectLocation).Size() >= ImpactEffectSkipDistance ||
         GetWorld()->TimeSeconds - LastImpactEffectTime >= MaxImpactEffectSkipTime))
    {
        FHitResult ImpactHit = ANZWeapon::GetImpactEffectHit(NZOwner, SpawnLocation, NZOwner->FlashLocation);
        if (ImpactHit.Component.IsValid() && !CancelImpactEffect(ImpactHit))
        {
            if (ImpactEffectManager != NULL)
            {
                TSubclassOf<ANZImpactEffect> ImpactEffect = ImpactEffectManager->GetImpactEffectFromPhysicalMaterial(ImpactHit.PhysMaterial.Get());
                if (ImpactEffect != NULL)
                {
                    ImpactEffect.GetDefaultObject()->SpawnEffect(GetWorld(), FTransform(ImpactHit.Normal.Rotation(), ImpactHit.Location), ImpactHit.Component.Get(), NULL, NZOwner->Controller);
                }
            }
        }
        
        LastImpactEffectLocation = NZOwner->FlashLocation;
        LastImpactEffectTime = GetWorld()->TimeSeconds;
    }
    
    PlayBulletWhip();
}

void ANZWeaponAttachment::FiringExtraUpdated()
{
    
}

void ANZWeaponAttachment::StopFiringEffects(bool bIgnoreCurrentMode)
{
    // We need to default to stopping all modes' firing effects as we can't rely on the replicated value to be correct at this point
    for (uint8 i = 0; i < MuzzleFlash.Num(); i++)
    {
        if (MuzzleFlash[i] != NULL && (!bIgnoreCurrentMode || !MuzzleFlash.IsValidIndex(NZOwner->FireMode) || MuzzleFlash[NZOwner->FireMode] == NULL || (i != NZOwner->FireMode && MuzzleFlash[i] != MuzzleFlash[NZOwner->FireMode])))
        {
            MuzzleFlash[i]->DeactivateSystem();
        }
    }
}

void ANZWeaponAttachment::AttachToOwnerNative()
{
	if (Mesh && NZOwner)
	{
		Mesh->AttachToComponent(NZOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);
		Mesh->SetRelativeLocation(AttachOffset);
		Mesh->bRecentlyRendered = NZOwner->GetMesh()->bRecentlyRendered;
		Mesh->LastRenderTime = NZOwner->GetMesh()->LastRenderTime;
		UpdateOverlays();
		SetSkin(NZOwner->GetSkin());
	}
	else
	{
		UE_LOG(NZ, Warning, TEXT("[terryzhong] -- AttachToOwnerNative failed! Mesh = %s, NZOwner = %s"), Mesh ? *Mesh->GetName() : TEXT("NULL"), NZOwner ? *NZOwner->GetName() : TEXT("NULL"));
	}
}

void ANZWeaponAttachment::HolsterToOwnerNative()
{
	if (Mesh && NZOwner)
	{
		Mesh->AttachToComponent(NZOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, HolsterSocket);
		Mesh->SetRelativeLocation(HolsterOffset);
		Mesh->SetRelativeRotation(HolsterRotation);
		Mesh->bRecentlyRendered = NZOwner->GetMesh()->bRecentlyRendered;
		Mesh->LastRenderTime = NZOwner->GetMesh()->LastRenderTime;
		SetSkin(NZOwner->GetSkin());
	}
	else
	{
		UE_LOG(NZ, Warning, TEXT("[terryzhong] -- HolsterToOwnerNative failed! Mesh = %s, NZOwner = %s"), Mesh ? *Mesh->GetName() : TEXT("NULL"), NZOwner ? *NZOwner->GetName() : TEXT("NULL"));
	}
}

void ANZWeaponAttachment::UpdateOverlays()
{
    if (WeaponType != NULL)
    {
        WeaponType.GetDefaultObject()->UpdateOverlaysShared(this, NZOwner, Mesh, OverlayEffectParams, OverlayMesh);
    }
}

void ANZWeaponAttachment::SetSkin(UMaterialInterface* NewSkin)
{
    if (NewSkin != NULL)
    {
        for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
        {
            Mesh->SetMaterial(i, NewSkin);
        }
    }
    else
    {
        for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
        {
            Mesh->SetMaterial(i, GetClass()->GetDefaultObject<ANZWeaponAttachment>()->Mesh->GetMaterial(i));
        }
    }
}

void ANZWeaponAttachment::PlayBulletWhip()
{
    if (BulletWhip != NULL && !NZOwner->FlashLocation.IsZero())
    {
        const FVector BulletSrc = NZOwner->GetActorLocation();
        const FVector Dir = (NZOwner->FlashLocation - BulletSrc).GetSafeNormal();
        for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
        {
            ANZPlayerController* PC = Cast<ANZPlayerController>(It->PlayerController);
            if (PC != NULL && PC->GetViewTarget() != NZOwner)
            {
                FVector ViewLoc;
                FRotator ViewRot;
                PC->GetPlayerViewPoint(ViewLoc, ViewRot);
                const FVector ClosestPt = FMath::ClosestPointOnSegment(ViewLoc, BulletSrc, NZOwner->FlashLocation);
                if (ClosestPt != BulletSrc && ClosestPt != NZOwner->FlashLocation && (ClosestPt - ViewLoc).Size() <= MaxBulletWhipDist)
                {
                    // Trace to make sure missed shot isn't on the other side of a wall
                    FCollisionQueryParams Params(FName(TEXT("BulletWhip")), true, NZOwner);
                    Params.AddIgnoredActor(PC->GetPawn());
                    if (!GetWorld()->LineTraceTestByChannel(ClosestPt, ViewLoc, COLLISION_TRACE_WEAPON, Params))
                    {
                        PC->ClientHearSound(BulletWhip, this, ClosestPt, false, false, false);
                    }
                }
            }
        }
    }
}

void ANZWeaponAttachment::MarkComponentsAsPendingKill()
{
    // Work around engine bug where components being destroyed try to calc bounding box and end up querying MasterPoseComponent in an invalid state
    if (OverlayMesh != NULL)
    {
        OverlayMesh->SetMasterPoseComponent(NULL);
    }
    Super::MarkComponentsAsPendingKill();
}
