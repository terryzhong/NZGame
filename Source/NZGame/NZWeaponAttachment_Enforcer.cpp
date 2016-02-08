// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeaponAttachment_Enforcer.h"


ANZWeaponAttachment_Enforcer::ANZWeaponAttachment_Enforcer()
{
    LeftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P_Left"));
    LeftMesh->AttachParent = RootComponent;
    LeftMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
    LeftAttachSocket = FName(TEXT("LeftWeaponPoint"));
    
    BurstSize = 3;
    AlternateCount = 0;
}

void ANZWeaponAttachment_Enforcer::BeginPlay()
{
    Super::BeginPlay();
    
    NZOwner = Cast<ANZCharacter>(Instigator);
    if (NZOwner != NULL)
    {
        AttachToOwner();
    }
    else
    {
        //UE_LOG(NZ, Warning, TEXT("UTWeaponAttachment: Bad Instigator: %s"), *GetNameSafe(Instigator));
    }
}

void ANZWeaponAttachment_Enforcer::AttachToOwnerNative()
{
    LeftMesh->AttachTo(NZOwner->GetMesh(), LeftAttachSocket);
    LeftMesh->SetRelativeLocation(LeftAttachOffset);
    LeftMesh->bRecentlyRendered = NZOwner->GetMesh()->bRecentlyRendered;
    UpdateOverlays();
    SetSkin(NZOwner->GetSkin());
    
    Super::AttachToOwnerNative();
}

void ANZWeaponAttachment_Enforcer::PlayFiringEffects()
{
}

void ANZWeaponAttachment_Enforcer::StopFiringEffects(bool bIgnoreCurrentMode)
{
}

void ANZWeaponAttachment_Enforcer::UpdateOverlays()
{
    if (WeaponType != NULL)
    {
        WeaponType.GetDefaultObject()->UpdateOverlaysShared(this, NZOwner, Mesh, OverlayEffectParams, OverlayMesh);
        WeaponType.GetDefaultObject()->UpdateOverlaysShared(this, NZOwner, LeftMesh, OverlayEffectParams, LeftOverlayMesh);
    }
}

void ANZWeaponAttachment_Enforcer::SetSkin(UMaterialInterface* NewSkin)
{
    Super::SetSkin(NewSkin);
    
    if (NewSkin != NULL)
    {
        for (int32 i = 0; i < LeftMesh->GetNumMaterials(); i++)
        {
            LeftMesh->SetMaterial(i, NewSkin);
        }
    }
    else
    {
        for (int32 i = 0; i < LeftMesh->GetNumMaterials(); i++)
        {
            LeftMesh->SetMaterial(i, GetClass()->GetDefaultObject<ANZWeaponAttachment_Enforcer>()->LeftMesh->GetMaterial(i));
        }
    }
}

