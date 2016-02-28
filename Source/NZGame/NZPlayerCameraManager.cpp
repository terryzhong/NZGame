// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerCameraManager.h"




void ANZPlayerCameraManager::CheckCameraSweep(FHitResult& OutHit, AActor* TargetActor, const FVector& Start, const FVector& End)
{
    static const FName NAME_FreeCam = FName(TEXT("FreeCam"));
    FCollisionQueryParams BoxParams(NAME_FreeCam, false, TargetActor);
    
    GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Camera, FCollisionShape::MakeBox(FVector(12.f)), BoxParams);
}

FName ANZPlayerCameraManager::GetCameraStyleWithOverrides() const
{
    static const FName NAME_FreeCam = FName(TEXT("FreeCam"));
    static const FName NAME_FirstPerson = FName(TEXT("FirstPerson"));
    static const FName NAME_Default = FName(TEXT("Default"));
    
    AActor* ViewTarget = GetViewTarget();
    ACameraActor* CameraActor = Cast<ACameraActor>(ViewTarget);
    if (CameraActor)
    {
        return NAME_Default;
    }
    
    ANZCharacter* NZCharacter = Cast<ANZCharacter>(ViewTarget);
    if (NZCharacter == NULL)
    {
        return ((ViewTarget == PCOwner->GetPawn()) || (ViewTarget == PCOwner->GetSpectatorPawn())) ? NAME_FirstPerson : NAME_FreeCam;
    }
    else if (NZCharacter->IsDead() || NZCharacter->IsRagdoll() || NZCharacter->EmoteCount > 0)
    {
        // Force third person if target is dead, ragdoll or emoting
        return NAME_FreeCam;
    }
    
    ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
    return (GameState != NULL) ? GameState->OverrideCameraStyle(PCOwner, CameraStyle) : CameraStyle;
}

void ANZPlayerCameraManager::UpdateCamera(float DeltaTime)
{
    if (GetNetMode() == NM_DedicatedServer)
    {
        CameraStyle = NAME_Default;
        LastThirdPersonCameraLoc = FVector::ZeroVector;
        ViewTarget.CheckViewTarget(PCOwner);
        // Our camera is now viewing there
        FMinimalViewInfo NewPOV;
        NewPOV.FOV = DefaultFOV;
        NewPOV.OrthoWidth = DefaultOrthoWidth;
        NewPOV.bConstrainAspectRatio = false;
        NewPOV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
        NewPOV.PostProcessBlendWeight = 1.0f;
        
        const bool bK2Camera = BlueprintUpdateCamera(ViewTarget.Target, NewPOV.Location, NewPOV.Rotation, NewPOV.FOV);
        if (!bK2Camera)
        {
            ViewTarget.Target->CalcCamera(DeltaTime, NewPOV);
        }
        
        // Cache results
        FillCameraCache(NewPOV);
    }
    else
    {
        Super::UpdateCamera(DeltaTime);
    }
}

void ANZPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    static const FName NAME_FreeCam = FName(TEXT("FreeCam"));
    static const FName NAME_GameOver = FName(TEXT("GameOver"));
    
    FName SavedCameraStyle = CameraStyle;
    CameraStyle = GetCameraStyleWithOverrides();
    
    // Smooth third person camera all the time
    if (OutVT.Target == PCOwner)
    {
        OutVT.POV.FOV = DefaultFOV;
        OutVT.POV.OrthoWidth = DefaultOrthoWidth;
        OutVT.POV.bConstrainAspectRatio = false;
        OutVT.POV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
        OutVT.POV.PostProcessBlendWeight = 1.0f;
        
        ApplyCameraModifiers(DeltaTime, OutVT.POV);
        
        if (OutVT.POV.Location.IsZero())
        {
            OutVT.POV.Location = PCOwner->GetFocalLocation();
        }
        OutVT.POV.Rotation = PCOwner->GetControlRotation();
    }
    else if (CameraStyle == NAME_FreeCam)
    {
        ANZCharacter* NZCharacter = Cast<ANZCharacter>(OutVT.Target);
        OutVT.POV.FOV = DefaultFOV;
        OutVT.POV.OrthoWidth = DefaultOrthoWidth;
        OutVT.POV.bConstrainAspectRatio = false;
        OutVT.POV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
        OutVT.POV.PostProcessBlendWeight = 1.0f;
        
        FVector DesiredLoc = (Cast<AController>(OutVT.Target) && !LastThirdPersonCameraLoc.IsZero()) ? LastThirdPersonCameraLoc : OutVT.Target->GetActorLocation();
        AActor* TargetActor = OutVT.Target;
        if (NZCharacter != NULL && NZCharacter->IsRagdoll() && NZCharacter->GetCapsuleComponent() != NULL)
        {
            // We must use the capsule location here as the ragdoll's root component can be rubbing a wall
            DesiredLoc = NZCharacter->GetCapsuleComponent()->GetComponentLocation();
        }
        else if (Cast<APlayerState>(OutVT.Target))
        {
            DesiredLoc = LastThirdPersonCameraLoc;
        }
        
        // If camera is jumping to a new location and we have LOS then interp
        bool bSnapCamera = ((NZCharacter && NZCharacter->IsDead()) || LastThirdPersonCameraLoc.IsZero() || (TargetActor != LastThirdPersonTarget));
        if (!bSnapCamera && ((DesiredLoc - LastThirdPersonCameraLoc).SizeSquared() > 250000.f))
        {
            FHitResult Result;
            CheckCameraSweep(Result, TargetActor, DesiredLoc, LastThirdPersonCameraLoc);
            bSnapCamera = Result.bBlockingHit;
        }
        FVector Loc = bSnapCamera ? DesiredLoc : FMath::VInterpTo(LastThirdPersonCameraLoc, DesiredLoc, DeltaTime, ThirdPersonCameraSmoothingSpeed);
        
        // todo:
        
        LastThirdPersonCameraLoc = Loc;
        LastThirdPersonTarget = TargetActor;
        
        ANZPlayerController* NZPC = Cast<ANZPlayerController>(PCOwner);
        bool bGameOver = (NZPC != NULL && NZPC->GetStateName() == NAME_GameOver);
        bool bUseDeathCam = !bGameOver && NZCharacter && (NZCharacter->IsDead() || NZCharacter->IsRagdoll());
        
        float CameraDistance = bUseDeathCam ? DeathCamDistance : FreeCamDistance;
        FVector CameraOffset = bUseDeathCam ? DeathCamOffset : FreeCamOffset;
        if (bGameOver)
        {
            CameraDistance = EndGameFreeCamDistance;
            CameraOffset = EndGameFreeCamOffset;
        }
        
        FRotator Rotator = (!NZPC || (NZPC->MouseButtonPressCount > 0)) ? PCOwner->GetControlRotation() : NZPC->GetSpectatingRotation(Loc, DeltaTime);
        if (bUseDeathCam)
        {
            Rotator.Pitch = FRotator::NormalizeAxis(Rotator.Pitch);
            Rotator.Pitch = FMath::Clamp(Rotator.Pitch, -85.f, -5.f);
        }
        if (Cast<ANZProjectile>(TargetActor) && !TargetActor->IsPendingKillPending())
        {
            Rotator = TargetActor->GetVelocity().Rotation();
            CameraDistance = 60.f;
            Loc = DesiredLoc;
        }

        FVector Pos = Loc + FRotationMatrix(Rotator).TransformVector(CameraOffset) - Rotator.Vector() * CameraDistance;
        
        FHitResult Result;
        CheckCameraSweep(Result, TargetActor, Loc, Pos);
        OutVT.POV.Location = !Result.bBlockingHit ? Pos : Result.Location;
        OutVT.POV.Rotation = Rotator;
        
        ApplyCameraModifiers(DeltaTime, OutVT.POV);
        
        // Synchronize the actor with the view target results
        SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);
    }
    else
    {
        LastThirdPersonCameraLoc = FVector::ZeroVector;
        Super::UpdateViewTarget(OutVT, DeltaTime);
        ANZCharacter* NZCharacter = Cast<ANZCharacter>(OutVT.Target);
        if (NZCharacter)
        {
            float DesiredRoll = 0.f;
            
            // todo:
            //if (NZCharacter->bApplyWallSlide)
            //{
            //    FVector Cross = NZCharacter->GetActorRotation().Vector() ^ FVector(0.f, 0.f, 1.f);
            //    DesiredRoll = -1.f * (Cross.GetSafeNormal() | NZCharacter->NZCharacterMovement->WallSlideNormal) * WallSlideCameraRoll;
            //}
            
            float AdjustRate = FMath::Min(1.f, 10.f * DeltaTime);
            CurrentCameraRoll = (1.f - AdjustRate) * CurrentCameraRoll + AdjustRate * DesiredRoll;
            OutVT.POV.Rotation.Roll = CurrentCameraRoll;
        }
    }
    
    CameraStyle = SavedCameraStyle;
}

static TAutoConsoleVariable<int32> CVarBloomDirt(TEXT("r.BloomDirt"), 0, TEXT("Enables screen dirt effect in high light/bloom"), ECVF_Default);

void ANZPlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
    Super::ApplyCameraModifiers(DeltaTime, InOutPOV);
    
    // If no PP volumes, force our default PP in at the beginning
    if (GetWorld()->PostProcessVolumes.Num() == 0)
    {
        PostProcessBlendCache.Insert(DefaultPPSettings, 0);
        PostProcessBlendCacheWeights.Insert(1.0f, 0);
    }
    
    ANZPlayerController* NZPCOwner = Cast<ANZPlayerController>(PCOwner);
    if (NZPCOwner && NZPCOwner->StylizedPPIndex != INDEX_NONE)
    {
        PostProcessBlendCache.Insert(StylizedPPSettings[NZPCOwner->StylizedPPIndex], 0);
        PostProcessBlendCacheWeights.Insert(1.0f, 0);
    }
    
    if (!CVarBloomDirt.GetValueOnGameThread())
    {
        FPostProcessSettings OverrideSettings;
        OverrideSettings.bOverride_BloomDirtMaskIntensity = true;
        OverrideSettings.BloomDirtMaskIntensity = 0.0f;
        PostProcessBlendCache.Add(OverrideSettings);
        PostProcessBlendCacheWeights.Add(1.0f);
    }
}

float ANZPlayerCameraManager::RatePlayerCamera(ANZPlayerState* InPS, ANZCharacter* Character, APlayerState* CurrentCamPS)
{
    float Score = 1.f;
    if (InPS == CurrentCamPS)
    {
        Score += CurrentCamBonus;
    }
    float LastActionTime = GetWorld()->GetTimeSeconds() - FMath::Max(Character->LastTakeHitTime, Character->LastWeaponFireTime);
    Score += FMath::Max(0.f, CurrentActionBonus - LastActionTime);
    
    if (Character->GetWeaponOverlayFlags() != 0)
    {
        Score += PowerupBonus;
    }
    
    if (CurrentCamPS)
    {
        Score += (InPS->Score > CurrentCamPS->Score) ? HigherScoreBonus : -1.f * HigherScoreBonus;
    }
    
    ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
    if (GameState)
    {
        Score += GameState->ScoreCameraView(InPS, Character);
    }
    
    return Score;
}

void ANZPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
    ANZPlayerController* NZPC = Cast<ANZPlayerController>(PCOwner);
    if (NZPC && PCOwner->PlayerState && PCOwner->PlayerState->bOnlySpectator && (NZPC->MouseButtonPressCount == 0) && (GetViewTarget() != PCOwner->GetSpectatorPawn()))
    {
        LimitViewPitch(OutViewRotation, ViewPitchMin, ViewPitchMax);
        return;
    }
    Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}

