// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerController.h"
#include "NZLocalPlayer.h"
#include "NZWeapon.h"
#include "NZHUD.h"
#include "NZPlayerState.h"
#include "NZPlayerInput.h"
#include "NZProfileSettings.h"
#include "NZGameState.h"
#include "NZGameMode.h"
#include "NZProjectile.h"
#include "NZCarriedObject.h"
#include "NZTeamInfo.h"
#include "AudioDevice.h"
#include "ActiveSound.h"
#include "NZMobilePlayerInput.h"
#include "NZMobileInputComponent.h"


ANZPlayerController::ANZPlayerController()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

	PlayerCameraManagerClass = ANZPlayerCameraManager::StaticClass();
    
	ConfigDefaultFOV = 100.0f;
    
    WeaponBobGlobalScaling = 1.f;
    EyeOffsetGlobalScaling = 1.f;
    
    //LastEmoteTime = 0.0f;
    //EmoteCooldownTime = 0.3f;
    bSpectateBehindView = true;
    StylizedPPIndex = INDEX_NONE;

	//bPCSimulateMobile = false;
}

ANZCharacter* ANZPlayerController::GetNZCharacter()
{
	return NZCharacter;
}

UNZLocalPlayer* ANZPlayerController::GetNZLocalPlayer()
{
    return Cast<UNZLocalPlayer>(Player);
}

void ANZPlayerController::OnRep_HUDClass()
{
    ClientSetHUD_Implementation(HUDClass);
    MyNZHUD = Cast<ANZHUD>(MyHUD);
}

void ANZPlayerController::UpdateCrosshairs(ANZHUD* HUD)
{
    UNZLocalPlayer* LocalPlayer = Cast<UNZLocalPlayer>(Player);
    if (LocalPlayer)
    {
        UNZProfileSettings* Settings = LocalPlayer->GetProfileSettings();
        if (Settings)
        {
            Settings->UpdateCrosshairs(HUD);
        }
    }
}

void ANZPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

void ANZPlayerController::BeginPlay()
{
    bSpectatorMouseChangesView = false;
    Super::BeginPlay();

    // todo:
}

void ANZPlayerController::Destroyed()
{
    Super::Destroyed();

    // todo:
}

/**
 * Cache a copy of the PlayerState cast'd to ANZPlayerState for easy reference.
 * Do it both here and when the replicated copy of APlayerState arrives in OnRep_PlayerState.
 */
void ANZPlayerController::InitPlayerState()
{
    Super::InitPlayerState();
    
    // Need this until Controller::InitPlayerState() is updated
    if (PlayerState && PlayerState->PlayerName.IsEmpty())
    {
        UWorld* const World = GetWorld();
        if (World != NULL)
        {
            AGameMode* const GameMode = World->GetAuthGameMode();
            if (GameMode != NULL)
            {
                // Don't call SetPlayerName() as that will broadcast entry message but the GameMode hasn't had a chance
                // to potentially apply a player/bot name yet
                PlayerState->PlayerName = GameMode->DefaultPlayerName.ToString();
            }
        }
    }
}

void ANZPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    NZPlayerState = Cast<ANZPlayerState>(PlayerState);

    // todo:
}

void ANZPlayerController::SetPawn(APawn* InPawn)
{
    if (InPawn == NULL)
    {
        // Attempt to move the PC to the current camera location if no pawn was specified
        const FVector NewLocation = (PlayerCameraManager != NULL) ? PlayerCameraManager->GetCameraLocation() : GetSpawnLocation();
        SetSpawnLocation(NewLocation);
    }
    
    AController::SetPawn(InPawn);
    
    NZCharacter = Cast<ANZCharacter>(InPawn);
    if (Player && IsLocalPlayerController())
    {
        // Apply FOV angle if dead/spectating
        if (GetPawn() == NULL && PlayerCameraManager != NULL)
        {
            FOV(ConfigDefaultFOV);
        }

        // todo:
    }
}

UClass* ANZPlayerController::GetPlayerInputClass()
{
	FString PlatformName = UGameplayStatics::GetPlatformName();
	if (PlatformName == TEXT("Windows") || PlatformName == TEXT("Mac"))
	{
		if (!bPCSimulateMobile)
		{
			return UNZPlayerInput::StaticClass();
		}
		else
		{
			return UNZMobilePlayerInput::StaticClass();
		}
	}
	else if (PlatformName == TEXT("Android") || PlatformName == TEXT("IOS"))
	{
		return UNZMobilePlayerInput::StaticClass();
	}

	return UNZPlayerInput::StaticClass();
}

UClass* ANZPlayerController::GetInputComponentClass()
{
	FString PlatformName = UGameplayStatics::GetPlatformName();
	if (PlatformName == TEXT("Windows") || PlatformName == TEXT("Mac"))
	{
		if (!bPCSimulateMobile)
		{
			return UInputComponent::StaticClass();
		}
		else
		{
			return UNZMobileInputComponent::StaticClass();
		}
	}
	else if (PlatformName == TEXT("Android") || PlatformName == TEXT("IOS"))
	{
		return UNZMobileInputComponent::StaticClass();
	}

	return UInputComponent::StaticClass();
}

void ANZPlayerController::InitInputSystem()
{
	if (PlayerInput == NULL)
	{
		NZPlayerInput = NewObject<UNZPlayerInput>(this, GetPlayerInputClass());
		check(NZPlayerInput);
		NZPlayerInput->Initialize();
		PlayerInput = NZPlayerInput;
	}

	Super::InitInputSystem();

	// todo:
}

void ANZPlayerController::SetupInputComponent()
{
	// A subclass could create a different InputComponent class but still want the default bindings
	if (InputComponent == NULL)
	{
		InputComponent = NewObject<UInputComponent>(this, GetInputComponentClass(), TEXT("PC_InputComponent0"));
		check(InputComponent);
		InputComponent->RegisterComponent();
	}

	Super::SetupInputComponent();
    
	InputComponent->BindAxis("MoveForward", this, &ANZPlayerController::MoveForward);
	InputComponent->BindAxis("MoveBackward", this, &ANZPlayerController::MoveBackward);
	InputComponent->BindAxis("MoveLeft", this, &ANZPlayerController::MoveLeft);
	InputComponent->BindAxis("MoveRight", this, &ANZPlayerController::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ANZPlayerController::MoveUp);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ANZPlayerController::AddYawInput);
	InputComponent->BindAxis("TurnAtRate", this, &ANZPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ANZPlayerController::AddPitchInput);
	InputComponent->BindAxis("LookUpAtRate", this, &ANZPlayerController::LookUpAtRate);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ANZPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ANZPlayerController::JumpRelease);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ANZPlayerController::Crouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &ANZPlayerController::UnCrouch);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ANZPlayerController::ToggleCrouch);

	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ANZPlayerController::PrevWeapon);
	InputComponent->BindAction("NextWeapon", IE_Released, this, &ANZPlayerController::NextWeapon);
	InputComponent->BindAction("ThrowWeapon", IE_Released, this, &ANZPlayerController::ThrowWeapon);

	InputComponent->BindAction("StartFire", IE_Pressed, this, &ANZPlayerController::OnFire);
	InputComponent->BindAction("StopFire", IE_Released, this, &ANZPlayerController::OnStopFire);
	InputComponent->BindAction("StartAltFire", IE_Pressed, this, &ANZPlayerController::OnAltFire);
	InputComponent->BindAction("StopAltFire", IE_Released, this, &ANZPlayerController::OnStopAltFire);

	InputComponent->BindAction("ChangeClip", IE_Pressed, this, &ANZPlayerController::ChangeClip);
	InputComponent->BindAction("LeftHandChangeClip", IE_Pressed, this, &ANZPlayerController::LeftHandChangeClip);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ANZPlayerController::Sprint);
	InputComponent->BindAction("UnSprint", IE_Released, this, &ANZPlayerController::UnSprint);

	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ANZPlayerController::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ANZPlayerController::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ANZPlayerController::TouchUpdate);
	}

 /*   InputComponent->BindAction("TapLeft", IE_Pressed, this, &ANZPlayerController::OnTapLeft);
    InputComponent->BindAction("TapRight", IE_Pressed, this, &ANZPlayerController::OnTapRight);
    InputComponent->BindAction("TapForward", IE_Pressed, this, &ANZPlayerController::OnTapForward);
    InputComponent->BindAction("TapBack", IE_Pressed, this, &ANZPlayerController::OnTapBack);
    InputComponent->BindAction("SingleTapDodge", IE_Pressed, this, &ANZPlayerController::OnSingleTapDodge);
    InputComponent->BindAction("HoldDodge", IE_Pressed, this, &ANZPlayerController::HoldDodge);
    InputComponent->BindAction("HoldDodge", IE_Released, this, &ANZPlayerController::ReleaseDodge);
    
    InputComponent->BindAction("TapLeftRelease", IE_Released, this, &ANZPlayerController::OnTapLeftRelease);
    InputComponent->BindAction("TapRightRelease", IE_Released, this, &ANZPlayerController::OnTapRightRelease);
    InputComponent->BindAction("TapForwardRelease", IE_Released, this, &ANZPlayerController::OnTapForwardRelease);
    InputComponent->BindAction("TapBackRelease", IE_Released, this, &ANZPlayerController::OnTapBackRelease);
	    
    InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ANZPlayerController::TouchStarted);
    
    InputComponent->BindAction("ShowScores", IE_Pressed, this, &ANZPlayerController::OnShowScores);
    InputComponent->BindAction("ShowScores", IE_Released, this, &ANZPlayerController::OnHideScores);
    
    InputComponent->BindAction("Talk", IE_Pressed, this, &ANZPlayerController::Talk);
    InputComponent->BindAction("TeamTalk", IE_Pressed, this, &ANZPlayerController::TeamTalk);
    
    InputComponent->BindAction("FasterEmote", IE_Pressed, this, &ANZPlayerController::FasterEmote);
    InputComponent->BindAction("SlowerEmote", IE_Pressed, this, &ANZPlayerController::SlowerEmote);
    InputComponent->BindAction("PlayTaunt", IE_Pressed, this, &ANZPlayerController::PlayTaunt);
    InputComponent->BindAction("PlayTaunt2", IE_Pressed, this, &ANZPlayerController::PlayTaunt2);
    
    InputComponent->BindAction("ShowBuyMenu", IE_Pressed, this, &ANZPlayerController::ShowBuyMenu);
    
    UpdateWeaponGroupKeys();*/
}

void ANZPlayerController::CreateTouchInterface()
{
	//Super::CreateTouchInterface();
}

void ANZPlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
    if (InputEnabled())
    {
        Super::ProcessPlayerInput(DeltaTime, bGamePaused);
        
        // todo:
        
        MovementForwardAxis = 0.f;
        MovementStrafeAxis = 0.f;
    }
}

void ANZPlayerController::PawnPendingDestroy(APawn* InPawn)
{
    if (IsInState(NAME_Inactive))
    {
        //UE_LOG(LogPath, Log, TEXT("PawnPendingDestroy while inactive %s"), *GetName());
    }
    
    if (InPawn == GetPawn() && InPawn != NULL)
    {
        GetPawn()->UnPossessed();
        SetPawn(NULL);
        
        FRotator AdjustedCameraRot = GetControlRotation();
        AdjustedCameraRot.Pitch = -45.0f;
        AdjustedCameraRot.Roll = 0.f;
        SetControlRotation(AdjustedCameraRot);
        
        ChangeState(NAME_Inactive);
        
        if (PlayerState == NULL)
        {
            Destroy();
        }
    }
}

void ANZPlayerController::HearSound(USoundBase* InSoundCue, AActor* SoundPlayer, const FVector& SoundLocation, bool bStopWhenOwnerDestroyed, bool bAmplifyVolume)
{
    bool bIsOccluded = false;
    //if (SoundPlayer == this || (GetViewTarget() != NULL && InSoundCue->IsAudible(SoundLocation, GetViewTarget()->GetActorLocation(), (SoundPlayer != NULL) ? SoundPlayer : this, bIsOccluded, true)))
    {
        // We don't want to replicate the location if it's the same as Actor location (so the sound gets played attached to the Actor), but we must if the source Actor isn't relevant
        UNetConnection* Conn = Cast<UNetConnection>(Player);
        FVector RepLoc = (SoundPlayer != NULL && SoundPlayer->GetActorLocation() == SoundLocation && (Conn == NULL || Conn->ActorChannels.Contains(SoundPlayer))) ? FVector::ZeroVector : SoundLocation;
        ClientHearSound(InSoundCue, SoundPlayer, RepLoc, bStopWhenOwnerDestroyed, bIsOccluded, bAmplifyVolume);
    }
}

void ANZPlayerController::ClientHearSound_Implementation(USoundBase* TheSound, AActor* SoundPlayer, FVector_NetQuantize SoundLocation, bool bStopWhenOwnerDestroyed, bool bIsOccluded, bool bAmplifyVolume)
{
    if (TheSound != NULL && (SoundPlayer != NULL || !SoundLocation.IsZero()))
    {
        bool bHRTFEnabled = false;
        if (GetWorld()->GetAudioDevice() != NULL && GetWorld()->GetAudioDevice()->IsHRTFEnabledForAll())
        {
            bHRTFEnabled = true;
        }
        
        if (!bHRTFEnabled && (SoundPlayer == this || SoundPlayer == GetViewTarget()))
        {
            // No attenuation/spatialization, full volume
            FActiveSound NewActiveSound;
            NewActiveSound.World = GetWorld();
            NewActiveSound.Sound = TheSound;
            
            NewActiveSound.VolumeMultiplier = 1.0f;
            NewActiveSound.PitchMultiplier = 1.0f;
            
            NewActiveSound.RequestedStartTime = 0.0f;
            
            NewActiveSound.bLocationDefined = false;
            NewActiveSound.bIsUISound = false;
            NewActiveSound.bHasAttenuationSettings = false;
            NewActiveSound.bAllowSpatialization = false;
            
            if (GetWorld()->GetAudioDevice() != NULL)
            {
                GetWorld()->GetAudioDevice()->AddNewActiveSound(NewActiveSound);
            }
        }
        else
        {
            USoundAttenuation* AttenuationOverride = NULL;
            if (bAmplifyVolume)
            {
                // the UGameplayStatics function copy the FAttenuationSettings by value so no need to create more than one, just reuse
                static USoundAttenuation* OverrideObj = [](){ USoundAttenuation* Result = NewObject<USoundAttenuation>(); Result->AddToRoot(); return Result; }();
                
                AttenuationOverride = OverrideObj;
                const FAttenuationSettings* DefaultAttenuation = TheSound->GetAttenuationSettingsToApply();
                if (DefaultAttenuation != NULL)
                {
                    AttenuationOverride->Attenuation = *DefaultAttenuation;
                }
                // Set minimum volume
                // We're assuming that the radius was already checked via HearSound() and thus this won't cause hearing the audio level-wide
                AttenuationOverride->Attenuation.dBAttenuationAtMax = 50.0f;
                // Move sound closer
                AActor* ViewTarget = GetViewTarget();
                if (ViewTarget != NULL)
                {
                    if (SoundLocation.IsZero())
                    {
                        SoundLocation = SoundPlayer->GetActorLocation();
                    }
                    FVector SoundOffset = GetViewTarget()->GetActorLocation() - SoundLocation;
                    SoundLocation = SoundLocation + SoundOffset.GetSafeNormal() * FMath::Min<float>(SoundOffset.Size() * 0.25f, 2000.0f);
                }
            }
            
            float VolumeMultiplier = bIsOccluded ? 0.5f : 1.0f;
            if (!SoundLocation.IsZero() && (SoundPlayer == NULL || SoundLocation != SoundPlayer->GetActorLocation()))
            {
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), TheSound, SoundLocation, VolumeMultiplier, 1.0f, 0.0f, AttenuationOverride);
            }
            else if (SoundPlayer != NULL)
            {
                UGameplayStatics::SpawnSoundAttached(TheSound, SoundPlayer->GetRootComponent(), NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, bStopWhenOwnerDestroyed, VolumeMultiplier, 1.0f, 0.0f, AttenuationOverride);
            }
        }
    }
}

static void HideComponentTree(const UPrimitiveComponent* Primitive, TSet<FPrimitiveComponentId>& HiddenComponents)
{
    if (Primitive != NULL)
    {
        HiddenComponents.Add(Primitive->ComponentId);
        TArray<USceneComponent*> Children;
        Primitive->GetChildrenComponents(true, Children);
        for (int32 i = 0; i < Children.Num(); i++)
        {
            UPrimitiveComponent* ChildPrimitive = Cast<UPrimitiveComponent>(Children[i]);
            if (ChildPrimitive != NULL)
            {
                HiddenComponents.Add(ChildPrimitive->ComponentId);
            }
        }
    }
}

void ANZPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& HiddenComponents)
{
    Super::UpdateHiddenComponents(ViewLocation, HiddenComponents);
    
    // todo:
    
    // Hide all components that shouldn't be shown in the current 1P/3P state
    // with bOwnerNoSee/bOnlyOwnerSee not being propagated to children this method is much easier to maintain
    // although slightly less efficient
    ANZCharacter* P = Cast<ANZCharacter>(GetViewTarget());
    if (IsBehindView())
    {
        // Hide first person weapon
        if (P != NULL)
        {
            HiddenComponents.Add(P->FirstPersonMesh->ComponentId);
            if (P->GetWeapon() != NULL)
            {
                TArray<UMeshComponent*> Meshes = P->GetWeapon()->Get1PMeshes();
                for (UMeshComponent* WeaponMesh : Meshes)
                {
                    if (WeaponMesh != NULL)
                    {
                        HideComponentTree(WeaponMesh, HiddenComponents);
                    }
                }
            }
        }
    }
    else if (P != NULL)
    {
        // Hide first person mesh (but not attachments) if hidden weapons
        if (GetWeaponHand() == HAND_Hidden || (P->GetWeapon() != NULL && P->GetWeapon()->ZoomState != EZoomState::EZS_NotZoomed))
        {
            HiddenComponents.Add(P->FirstPersonMesh->ComponentId);
            if (P->GetWeapon() != NULL)
            {
                TArray<UMeshComponent*> Meshes = P->GetWeapon()->Get1PMeshes();
                for (UMeshComponent* WeaponMesh : Meshes)
                {
                    if (WeaponMesh != NULL)
                    {
                        HiddenComponents.Add(WeaponMesh->ComponentId);
                    }
                }
            }
        }
        // terryzhong@2016.3.18: Always show hands
        //else if (P->GetWeapon() == NULL || P->GetWeapon()->HandsAttachSocket == NAME_None)
        //{
        //    // Weapon doesn't use hands
        //    HiddenComponents.Add(P->FirstPersonMesh->ComponentId);
        //}
        
        // Hide third person character model
        HideComponentTree(P->GetMesh(), HiddenComponents);
        
        // Hide flag
        // todo:
/*        if (P->GetCarriedObject() != NULL)
        {
            HideComponentTree(Cast<UPrimitiveComponent>(P->GetCarriedObject()->GetRootComponent()), HiddenComponents);
        }*/
    }
    else if (GetViewTarget() != NULL)
    {
        // For others we can't just hide everything because we don't know where the camera component is and we don't want to hide its attachments
        // so just hide root
        UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(GetViewTarget()->GetRootComponent());
        if (RootPrimitive != NULL)
        {
            HiddenComponents.Add(RootPrimitive->ComponentId);
        }
    }
    
    // Hide other pawns' first person hands/weapons
    for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
    {
        if (It->IsValid() && It->Get() != GetViewTarget() && It->Get() != GetPawn())
        {
            ANZCharacter* OtherP = Cast<ANZCharacter>(It->Get());
            if (OtherP != NULL)
            {
                HideComponentTree(OtherP->FirstPersonMesh, HiddenComponents);
            }
        }
    }
}


void ANZPlayerController::ToggleBehindView()
{
    bSpectateBehindView = !bSpectateBehindView;
    BehindView(bSpectateBehindView);
    
    ANZCarriedObject* NZFlag = Cast<ANZCarriedObject>(GetViewTarget());
    if (!bSpectateBehindView && NZFlag && NZFlag->Holder)
    {
        ServerViewFlagHolder(NZFlag->GetTeamNum());
    }
}

void ANZPlayerController::SpawnPlayerCameraManager()
{
    Super::SpawnPlayerCameraManager();
    // Init configured FOV angle
    if (PlayerCameraManager != NULL)
    {
        PlayerCameraManager->DefaultFOV = ConfigDefaultFOV;
    }
}


void ANZPlayerController::ServerRestartPlayerAltFire_Implementation()
{
    if (NZCharacter != NULL)
    {
        NZPlayerState->bChosePrimaryRespawnChoice = false;
        NZPlayerState->ForceNetUpdate();
    }
    
    ANZGameState* GameState = GetWorld()->GetGameState<ANZGameState>();
    if (GameState && !GameState->HasMatchStarted())
    {
        if (GameState->GetMatchState() != MatchState::CountdownToBegin && GameState->GetMatchState() != MatchState::PlayerIntro)
        {
            ServerSwitchTeam();
        }
    }
    else
    {
        ANZGameMode* GameMode = GetWorld()->GetAuthGameMode<ANZGameMode>();
        if (GameMode && !GameMode->PlayerCanAltRestart(this))
        {
            return;
        }
    }
    
    Super::ServerRestartPlayer_Implementation();
}

bool ANZPlayerController::ServerRestartPlayerAltFire_Validate()
{
    return true;
}

void ANZPlayerController::ServerSwitchTeam_Implementation()
{
    if (NZPlayerState != NULL && NZPlayerState->Team != NULL)
    {
        uint8 NewTeam = (NZPlayerState->Team->TeamIndex + 1) % GetWorld()->GetGameState<ANZGameState>()->Teams.Num();
    }
}

bool ANZPlayerController::ServerSwitchTeam_Validate()
{
    return true;
}

void ANZPlayerController::BehindView(bool bWantBehindView)
{
    if (GetPawn() != NULL && !GetPawn()->bTearOff && !bAllowPlayingBehindView && GetNetMode() != NM_Standalone && (GetWorld()->WorldType != EWorldType::PIE))
    {
        bWantBehindView = false;
    }
    if (IsInState(NAME_Spectating))
    {
        bSpectateBehindView = bWantBehindView;
    }
    else
    {
        bPlayBehindView = bWantBehindView;
    }
    SetCameraMode(bWantBehindView ? FName(TEXT("FreeCam")) : FName(TEXT("Default")));
    if (Cast<ANZCharacter>(GetViewTarget()) != NULL)
    {
        ((ANZCharacter*)GetViewTarget())->BehindViewChange(this, bWantBehindView);
    }
    
    // Make sure we don't have leftover zoom
    if (bWantBehindView && PlayerCameraManager != NULL)
    {
        PlayerCameraManager->UnlockFOV();
        PlayerCameraManager->DefaultFOV = ConfigDefaultFOV;
    }
}

bool ANZPlayerController::IsBehindView()
{
    if (PlayerCameraManager != NULL)
    {
		static FName NAME_FreeCam(TEXT("FreeCam"));
        
		ANZPlayerCameraManager* NZCam = Cast<ANZPlayerCameraManager>(PlayerCameraManager);
        FName CameraStyle = (NZCam != NULL) ? NZCam->GetCameraStyleWithOverrides() : PlayerCameraManager->CameraStyle;
        
        return CameraStyle == NAME_FreeCam;
    }
    else
    {
        return false;
    }
}

void ANZPlayerController::SetCameraMode(FName NewCamMode)
{
    ClientSetCameraMode_Implementation(NewCamMode);
    
    if (GetNetMode() == NM_DedicatedServer)
    {
        ClientSetCameraMode(NewCamMode);
    }
}

void ANZPlayerController::ClientSetCameraMode_Implementation(FName NewCamMode)
{
    if (PlayerCameraManager)
    {
        PlayerCameraManager->CameraStyle = NewCamMode;
    }
}

void ANZPlayerController::ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner)
{
    ChangeState(FName(TEXT("GameOver")));
    FinalViewTarget = EndGameFocus;
    BehindView(true);
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ANZPlayerController::ShowEndGameScoreboard, 10.f, false);
    Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);
    
    TurnOffPawns();
}

void ANZPlayerController::ShowEndGameScoreboard()
{
    // todo:
    check(false);
}

void ANZPlayerController::TurnOffPawns()
{
    // Freezze all Pawns locally
    for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
    {
        if (It->IsValid() && !Cast<ASpectatorPawn>(It->Get()))
        {
            It->Get()->TurnOff();
        }
    }
}



void ANZPlayerController::ResetCameraMode()
{
    bool bBehindView;
    
    if (IsInState(NAME_Spectating))
    {
        bBehindView = bSpectateBehindView;
    }
    else if (!bAllowPlayingBehindView && GetNetMode() != NM_Standalone && GetWorld()->WorldType != EWorldType::PIE)
    {
        bBehindView = false;
    }
    else
    {
        bBehindView = bPlayBehindView;
    }
    
    if (bBehindView)
    {
        SetCameraMode(FName(TEXT("FreeCam")));
    }
    else
    {
        Super::ResetCameraMode();
    }
}

void ANZPlayerController::ChooseBestCamera()
{
    // For now, choose just between live players. Eventually also use level cameras, etc.
    float BestScore = 0.f;
    APlayerState* BestPS = LastSpectatedPlayerState;
    ANZPlayerCameraManager* NZCam = Cast<ANZPlayerCameraManager>(PlayerCameraManager);
    if (NZCam)
    {
        for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
        {
            ANZCharacter* CamPawn = Cast<ANZCharacter>(*Iterator);
            ANZPlayerState* NextPlayerState = (CamPawn && (CamPawn->Health > 0)) ? Cast<ANZPlayerState>(CamPawn->PlayerState) : NULL;
            if (NextPlayerState)
            {
                float NewScore = NZCam->RatePlayerCamera(NextPlayerState, CamPawn, LastSpectatedPlayerState);
                if (NewScore > BestScore)
                {
                    BestScore = NewScore;
                    BestPS = NextPlayerState;
                }
            }
        }
    }

    if (BestPS && (BestPS != LastSpectatedPlayerState))
    {
        ViewPlayerState(BestPS);
        BehindView(bSpectateBehindView);
    }
}


void ANZPlayerController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
    // Remove any FOV shifts when changing targets (e.g. sniper zoom)
    if (PlayerCameraManager != NULL)
    {
        PlayerCameraManager->UnlockFOV();
    }
    
    if (FinalViewTarget != NULL)
    {
        NewViewTarget = FinalViewTarget;
    }
    
    //ANZViewPlaceholder* NZPlaceholder = Cast<ANZViewPlaceholder>(GetViewTarget());
    
    Super::SetViewTarget(NewViewTarget, TransitionParams);
    
    AActor* UpdatedViewTarget = GetViewTarget();
    //if (NZPlaceholder != NULL && UpdatedViewTarget != NZPlaceholder)
    //{
    //    NZPlaceholder->Destroy();
    //}
    
    if (StateName == NAME_Spectating)
    {
        ANZCharacter* Char = Cast<ANZCharacter>(UpdatedViewTarget);
        if (Char)
        {
            ViewProjectileTime = 0.f;
            LastSpectatedPlayerState = Cast<ANZPlayerState>(Char->PlayerState);
            if (LastSpectatedPlayerState)
            {
                LastSpectatedPlayerId = LastSpectatedPlayerState->SpectatingID;
            }
        }
        else if (!Cast<ANZProjectile>(UpdatedViewTarget) && (UpdatedViewTarget != this))
        {
            LastSpectatedPlayerState = NULL;
            LastSpectatedPlayerId = -1;
        }
        
        if (IsLocalController() && bSpectateBehindView && PlayerState && PlayerState->bOnlySpectator && (NewViewTarget != GetSpectatorPawn()) && NewViewTarget)
        {
            // Pick good starting rotation
            FindGoodView(NewViewTarget->GetActorLocation(), false);
        }
    }
}

void ANZPlayerController::ServerViewSelf_Implementation(FViewTargetTransitionParams TransitionParams)
{
    Super::ServerViewSelf_Implementation(TransitionParams);

    // todo:
}

void ANZPlayerController::ViewSelf(FViewTargetTransitionParams TransitionParams)
{
    ServerViewSelf(TransitionParams);
}

void ANZPlayerController::FindGoodView(const FVector& TargetLoc, bool bIsUpdate)
{
    AActor* TestViewTarget = GetViewTarget();
    if (!TestViewTarget || !PlayerCameraManager || (TestViewTarget == this) || (TestViewTarget == GetSpectatorPawn()) || Cast<ANZProjectile>(TestViewTarget)/* || Cast<ANZViewPlaceholder>(TestViewTarget)*/)
    {
        // No rotation change
        return;
    }
    
    FRotator BestRot = GetControlRotation();
    // Always start looking down slightly
    BestRot.Pitch = -10.f;
    BestRot.Roll = 0.f;
    BestRot.Yaw = FMath::UnwindDegrees(BestRot.Yaw);
    float CurrentYaw = BestRot.Yaw;
    
    BestRot.Yaw = FMath::UnwindDegrees(TestViewTarget->GetActorRotation().Yaw) + 15.f;
    if (bIsUpdate)
    {
        float DesiredYaw = BestRot.Yaw;
        
        // Check if too far to change directly
        if (DesiredYaw - CurrentYaw > 180.f)
        {
            DesiredYaw -= 360.f;
        }
        else if (CurrentYaw - DesiredYaw > 180.f)
        {
            CurrentYaw -= 360.f;
        }
    }
    
    ANZPlayerCameraManager* CamMgr = Cast<ANZPlayerCameraManager>(PlayerCameraManager);
    static const FName NAME_GameOver = FName(TEXT("GameOver"));
    bool bGameOver = (GetStateName() == NAME_GameOver);
    float CameraDistance = bGameOver ? CamMgr->EndGameFreeCamDistance : CamMgr->FreeCamDistance;
    FVector CameraOffset = bGameOver ? CamMgr->EndGameFreeCamOffset : CamMgr->FreeCamOffset;
    float UnBlockedPct = (Cast<APawn>(TestViewTarget) && (CameraDistance > 0.f)) ? 96.f / CameraDistance : 1.f;
    
    // todo:
    
    if ((TestViewTarget == FinalViewTarget) && Cast<ANZCharacter>(TestViewTarget))
    {
        UnBlockedPct = 1.f;
        BestRot.Yaw += 180.f;
    }
    
    // Look for acceptable view
    float YawIncrement = 30.f;
    BestRot.Yaw = int32(BestRot.Yaw / 5.f) * 5.f;
    BestRot.Yaw = FMath::UnwindDegrees(BestRot.Yaw);
    if ((FMath::Abs(LastGoalYaw - BestRot.Yaw) < 6.f) || (FMath::Abs(LastGoalYaw - BestRot.Yaw) > 354.f))
    {
        // Prevent jitter when can't settle on good view
        BestRot.Yaw = LastGoalYaw;
    }
    float OffsetMag = -60.f;
    float YawOffset = 0.f;
    bool bFoundGoodView = false;
    float BestView = BestRot.Yaw;
    float BestDist = 0.f;
    float StartYaw = BestRot.Yaw;
    int32 IncrementCount = 1;
    while (!bFoundGoodView && (IncrementCount < 12) && CamMgr)
    {
        BestRot.Yaw = StartYaw + YawOffset;
        FVector Pos = TargetLoc + FRotationMatrix(BestRot).TransformVector(CameraOffset) - BestRot.Vector() * CameraDistance;
        FHitResult Result(1.f);
        CamMgr->CheckCameraSweep(Result, TestViewTarget, TargetLoc, Pos);
        float NewDist = (Result.Location - TargetLoc).SizeSquared();
        bFoundGoodView = Result.Time > UnBlockedPct;
        if (NewDist > BestDist)
        {
            BestDist = NewDist;
            BestView = BestRot.Yaw;
        }
        float NewOffset = (YawIncrement * IncrementCount);
        if ((IncrementCount % 2) == 0)
        {
            NewOffset *= -1.f;
        }
        IncrementCount++;
        YawOffset += NewOffset;
    }
    if (!bFoundGoodView)
    {
        BestRot.Yaw = CurrentYaw;
    }
    LastGoalYaw = BestRot.Yaw;
    if ((FMath::Abs(CurrentYaw - BestRot.Yaw) > 60.f) && (FMath::Abs(CurrentYaw - BestRot.Yaw) < 300.f))
    {
        if (BestRot.Yaw < 0.f)
        {
            BestRot.Yaw += 360.f;
        }
        if (CurrentYaw < 0.f)
        {
            CurrentYaw += 360.f;
        }
        if (CurrentYaw > BestRot.Yaw)
        {
            if (360.f - CurrentYaw + BestRot.Yaw < CurrentYaw - BestRot.Yaw)
            {
                BestRot.Yaw = CurrentYaw + 30.f;
            }
            else
            {
                BestRot.Yaw = CurrentYaw - 30.f;
            }
        }
        else
        {
            if (360.f - BestRot.Yaw + CurrentYaw < BestRot.Yaw - CurrentYaw)
            {
                BestRot.Yaw = CurrentYaw - 30.f;
            }
            else
            {
                BestRot.Yaw = CurrentYaw + 30.f;
            }
        }
    }
    SetControlRotation(BestRot);
}

void ANZPlayerController::ClientViewSpectatorPawn_Implementation(FViewTargetTransitionParams TransitionParams)
{
    
}

void ANZPlayerController::ViewPlayerNum(int32 Index, uint8 TeamNum)
{
    ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
    if (GS != NULL)
    {
        APlayerState** PlayerToView = NULL;
        if ((TeamNum == 255) || !GS->Teams.IsValidIndex(TeamNum))
        {
            if (TeamNum == 0)
            {
                Index += 1;
            }
            else if (TeamNum == 1)
            {
                Index += 6;
            }
            int32 MaxSpectatingId = GS->GetMaxSpectatingId();
            while ((Index <= MaxSpectatingId) && (PlayerToView == NULL))
            {
                PlayerToView = GS->PlayerArray.FindByPredicate([=](const APlayerState* TestItem) -> bool
                {
                    const ANZPlayerState* PS = Cast<ANZPlayerState>(TestItem);
                    return (PS != NULL && PS->SpectatingID == Index);
                });
                Index += 5;
            }
        }
        else
        {
            int32 MaxSpectatingId = GS->GetMaxTeamSpectatingId(TeamNum);
            while ((Index <= MaxSpectatingId) && (PlayerToView == NULL))
            {
                PlayerToView = GS->PlayerArray.FindByPredicate([=](const APlayerState* TestItem)->bool
                {
                    const ANZPlayerState* PS = Cast<ANZPlayerState>(TestItem);
                    return (PS != NULL && PS->SpectatingIDTeam == Index && PS->GetTeamNum() == TeamNum);
                });
                Index += 5;
            }
        }
        if (PlayerToView != NULL)
        {
            bAutoCam = false;
            BehindView(bSpectateBehindView);
            ViewPlayerState(*PlayerToView);
        }
    }
}

void ANZPlayerController::ViewNextPlayer()
{
    bAutoCam = false;
    BehindView(bSpectateBehindView);
    ServerViewNextPlayer();
}

void ANZPlayerController::ServerViewFlagHolder_Implementation(uint8 TeamIndex)
{
    // todo:
}

bool ANZPlayerController::ServerViewFlagHolder_Validate(uint8 TeamIndex)
{
    return true;
}

void ANZPlayerController::ServerViewProjectile_Implementation()
{
    
}

bool ANZPlayerController::ServerViewProjectile_Validate()
{
    return true;
}

void ANZPlayerController::ViewProjectile()
{
    
}

void ANZPlayerController::ServerViewProjectileShim()
{
    
}

void ANZPlayerController::ServerViewPlayerState_Implementation(APlayerState* PS)
{
    
}

bool ANZPlayerController::ServerViewPlayerState_Validate(APlayerState* PS)
{
    return true;
}

void ANZPlayerController::ViewPlayerState(APlayerState* PS)
{
    
}

void ANZPlayerController::ViewClosestVisiblePlayer()
{
    
}

void ANZPlayerController::ViewFlag(uint8 Index)
{
    
}

void ANZPlayerController::ViewCamera(int32 Index)
{
    
}

FRotator ANZPlayerController::GetSpectatingRotation(const FVector& ViewLoc, float DeltaTime)
{
    if (IsInState(NAME_Spectating))
    {
        ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
        if (GS && (!GS->IsMatchInProgress() || GS->IsMatchIntermission()))
        {
            return GetControlRotation();
        }
        float OldYaw = FMath::UnwindDegrees(GetControlRotation().Yaw);
        FindGoodView(ViewLoc, true);
        FRotator NewRotation = GetControlRotation();
        float NewYaw = FMath::UnwindDegrees(NewRotation.Yaw);
        if (FMath::Abs(NewYaw - OldYaw) < 60.f)
        {
            NewRotation.Yaw = (1.f - 7.f * DeltaTime) * OldYaw + 7.f * DeltaTime * NewYaw;
            SetControlRotation(NewRotation);
        }
    }
    return GetControlRotation();
}




void ANZPlayerController::Possess(APawn* PawnToPossess)
{
	Super::Possess(PawnToPossess);

	// todo:
}

void ANZPlayerController::PawnLeavingGame()
{
	if (NZCharacter != NULL)
	{
		NZCharacter->PlayerSuicide();
	}
	else
	{
		UnPossess();
	}
}

void ANZPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (NZPlayerInput)
	{
		NZPlayerInput->UpdatePlayerInput(DeltaTime);
	}

	if (StateName == FName(TEXT("GameOver")))
	{
		UpdateRotation(DeltaTime);
	}

	// If we have no NZCharacterMovementComponent, we need to apply firing here since it won't happen from the component
	if (GetPawn() == NULL || Cast<UNZCharacterMovementComponent>(GetPawn()->GetMovementComponent()) == NULL)
	{
		ApplyDeferredFireInputs();
	}

	// Force ping update if servermoves aren't triggering it
	if ((GetWorld()->GetTimeSeconds() - LastPingCalcTime > 0.5f) && (GetNetMode() == NM_Client))
	{
		LastPingCalcTime = GetWorld()->GetTimeSeconds();
		ServerBouncePing(GetWorld()->GetTimeSeconds());
	}

	APawn* ViewTargetPawn = PlayerCameraManager->GetViewTargetPawn();

	if (Cast<ASpectatorPawn>(ViewTargetPawn) == NULL && bSpectatorMouseChangesView)
	{
		// If we aren't in free-cam then turn off movement via mouse
		SetSpectatorMouseChangesView(false);
	}

	ANZCharacter* ViewTargetCharacter = Cast<ANZCharacter>(ViewTargetPawn);
	if (IsInState(NAME_Spectating) && bAutoCam && (!ViewTargetCharacter || !ViewTargetCharacter->IsRecentlyDead()))
	{
		// Possibly switch cameras
		ChooseBestCamera();
	}

	// Follow the last spectated player again when they respawn
	if (IsInState(NAME_Spectating) && LastSpectatedPlayerId >= 0 && IsLocalController() && (!Cast<ANZProjectile>(GetViewTarget()) || GetViewTarget()->IsPendingKillPending()))
	{
		ViewTargetPawn = PlayerCameraManager->GetViewTargetPawn();
		ViewTargetCharacter = Cast<ANZCharacter>(ViewTargetPawn);
		if (!ViewTargetPawn || (ViewTargetCharacter && ViewTargetCharacter->IsDead() && !ViewTargetCharacter->IsRecentlyDead()))
		{
			for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
			{
				APawn* Pawn = *Iterator;
				if (Pawn != NULL)
				{
					ANZPlayerState* PS = Cast<ANZPlayerState>(Pawn->PlayerState);
					if (PS && PS->SpectatingID == LastSpectatedPlayerId)
					{
						ANZCharacter* TargetCharacter = Cast<ANZCharacter>(Pawn);
						if (TargetCharacter && TargetCharacter->DrivenVehicle && !TargetCharacter->DrivenVehicle->IsPendingKillPending())
						{
							ViewPawn(TargetCharacter->DrivenVehicle);
						}
						else
						{
							ViewPawn(*Iterator);
						}
						break;
					}
				}
			}
		}
	}
	else if (PlayerState && PlayerState->bOnlySpectator && (GetViewTarget() == this))
	{
		ClientViewSpectatorPawn(FViewTargetTransitionParams());
	}

	// todo:
}

void ANZPlayerController::NotifyTakeHit_Implementation(AController* InstigatedBy, int32 Damage, FVector Momentum, const FDamageEvent& DamageEvent)
{
    APlayerState* InstigatedByState = (InstigatedBy != NULL) ? InstigatedBy->PlayerState : NULL;
    FVector RelHitLocation(FVector::ZeroVector);
    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        RelHitLocation = ((FPointDamageEvent*)&DamageEvent)->HitInfo.Location - GetViewTarget()->GetActorLocation();
    }
    else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID) && ((FRadialDamageEvent*)&DamageEvent)->ComponentHits.Num() > 0)
    {
        RelHitLocation = ((FRadialDamageEvent*)&DamageEvent)->ComponentHits[0].Location - GetViewTarget()->GetActorLocation();
    }
    ANZGameState* GS = GetWorld()->GetGameState<ANZGameState>();
    bool bFriendlyFire = InstigatedByState != PlayerState && GS != NULL && GS->OnSameTeam(InstigatedByState, this);
    uint8 RepDamage = FMath::Clamp(Damage, 0, 255);
    ClientNotifyTakeHit(bFriendlyFire, RepDamage, RelHitLocation);
}

void ANZPlayerController::NotifyCausedHit_Implementation(APawn* HitPawn, int32 Damage)
{
	ClientNotifyCausedHit(HitPawn, Damage);
}

void ANZPlayerController::ClientNotifyTakeHit_Implementation(bool bFriendlyFire, uint8 Damage, FVector_NetQuantize RelHitLocation)
{
    if (MyNZHUD != NULL)
    {
        // todo:
        //MyNZHUD->PawnDamaged(((GetPawn() != NULL) ? GetPawn()->GetActorLocation() : GetViewTarget()->GetActorLocation()) + RelHitLocation, Damage, bFriendlyFire);
    }
}

void ANZPlayerController::ClientNotifyCausedHit_Implementation(APawn* HitPawn, uint8 Damage)
{
    // By default we only show HUD hitconfirms for hits that the player could conceivably see (i.e. target is in LOS)
    if (HitPawn != NULL && HitPawn->GetRootComponent() != NULL && GetPawn() != NULL && MyNZHUD != NULL)
    {
        float VictimLastRenderTime = -1.0f;
        TArray<USceneComponent*> Components;
        HitPawn->GetRootComponent()->GetChildrenComponents(true, Components);
        for (int32 i = 0; i < Components.Num(); i++)
        {
            UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Components[i]);
            if (Prim != NULL)
            {
                VictimLastRenderTime = FMath::Max<float>(VictimLastRenderTime, Prim->LastRenderTime);
            }
        }
        if (GetWorld()->TimeSeconds - VictimLastRenderTime < 0.15f)
        {
            MyNZHUD->CausedDamage(HitPawn, Damage);
        }
    }
}


void ANZPlayerController::SetStylizedPP(int32 NewPP)
{
    ANZPlayerCameraManager* NZCM = Cast<ANZPlayerCameraManager>(PlayerCameraManager);
    if (NZCM)
    {
        if (NewPP == INDEX_NONE || NewPP < NZCM->StylizedPPSettings.Num())
        {
            StylizedPPIndex = NewPP;
        }
    }
}



void ANZPlayerController::SetWeaponHand(EWeaponHand NewHand)
{
    WeaponHand = NewHand;
    ANZCharacter* NZCharTarget = Cast<ANZCharacter>(GetViewTarget());
    if (NZCharTarget != NULL && NZCharTarget->GetWeapon() != NULL)
    {
        NZCharTarget->GetWeapon()->UpdateWeaponHand();
    }
    if (IsTemplate() || IsLocalPlayerController())
    {
        SaveConfig();
    }
    if (!IsTemplate() && Role < ROLE_Authority)
    {
        ServerSetWeaponHand(NewHand);
    }
}

void ANZPlayerController::ServerSetWeaponHand_Implementation(EWeaponHand NewHand)
{
    SetWeaponHand(NewHand);
}

bool ANZPlayerController::ServerSetWeaponHand_Validate(EWeaponHand NewHand)
{
    return true;
}





void ANZPlayerController::SwitchToBestWeapon()
{
    if (NZCharacter != NULL && IsLocalPlayerController())
    {
        ANZWeapon* BestWeapon = NULL;
        float BestPriority = 0.0f;
        for (TInventoryIterator<ANZWeapon> It(NZCharacter); It; ++It)
        {
            if (It->HasAnyAmmo())
            {
                float TestPriority = It->AutoSwitchPriority;
                if (TestPriority > BestPriority)
                {
                    BestWeapon = *It;
                    BestPriority = TestPriority;
                }
            }
        }
        NZCharacter->SwitchWeapon(BestWeapon);
    }
}

void ANZPlayerController::ClientSwitchToBestWeapon_Implementation()
{
	SwitchToBestWeapon();
}

void ANZPlayerController::PrevWeapon()
{
    SwitchWeaponInSequence(true);
}

void ANZPlayerController::NextWeapon()
{
    SwitchWeaponInSequence(false);
}

void ANZPlayerController::ThrowWeapon()
{
    if (NZCharacter != NULL && IsLocalPlayerController() && !NZCharacter->IsRagdoll())
    {
        if (NZCharacter->GetWeapon() != NULL && NZCharacter->GetWeapon()->DroppedPickupClass != NULL && NZCharacter->GetWeapon()->bCanThrowWeapon)
        {
            ServerThrowWeapon();
        }
    }
}

void ANZPlayerController::ServerThrowWeapon_Implementation()
{
    if (NZCharacter != NULL && !NZCharacter->IsRagdoll())
    {
        if (NZCharacter->GetWeapon() != NULL && NZCharacter->GetWeapon()->DroppedPickupClass != NULL && NZCharacter->GetWeapon()->bCanThrowWeapon && !NZCharacter->GetWeapon()->IsFiring())
        {
            NZCharacter->TossInventory(NZCharacter->GetWeapon(), FVector(400.0f, 0.0f, 200.0f));
        }
    }
}

bool ANZPlayerController::ServerThrowWeapon_Validate()
{
    return true;
}

void ANZPlayerController::SwitchWeaponInSequence(bool bPrev)
{
    if (NZCharacter != NULL && IsLocalPlayerController() && /*NZCharacter->EmoteCount == 0 &&*/ !NZCharacter->IsRagdoll())
    {
        if (NZCharacter->GetWeapon() == NULL)
        {
            SwitchToBestWeapon();
        }
        else
        {
            UNZProfileSettings* ProfileSettings = NULL;
            
            if (Cast<UNZLocalPlayer>(Player))
            {
                ProfileSettings = Cast<UNZLocalPlayer>(Player)->GetProfileSettings();
            }
            
            ANZWeapon* Best = NULL;
            ANZWeapon* WraparoundChoice = NULL;
            ANZWeapon* CurrentWeapon = (NZCharacter->GetPendingWeapon() != NULL) ? NZCharacter->GetPendingWeapon() : NZCharacter->GetWeapon();
            for (TInventoryIterator<ANZWeapon> It(NZCharacter); It; ++It)
            {
                ANZWeapon* Weap = *It;
                if (Weap != CurrentWeapon && Weap->HasAnyAmmo())
                {
                    if (Weap->FollowsInList(CurrentWeapon) == bPrev)
                    {
                        // Remember last weapon in list as possible wraparound choice
                        if (WraparoundChoice == NULL || (Weap->FollowsInList(WraparoundChoice) == bPrev))
                        {
                            WraparoundChoice = Weap;
                        }
                    }
                    else if (Best == NULL || (Weap->FollowsInList(Best) == bPrev))
                    {
                        Best = Weap;
                    }
                }
            }
            
            if (Best == NULL)
            {
                Best = WraparoundChoice;
            }
            
            NZCharacter->SwitchWeapon(Best);
        }
    }
    else if (PlayerState && PlayerState->bIsSpectator && PlayerCameraManager)
    {
        float Offset = 10000.f * GetWorld()->GetDeltaSeconds();
        if (bPrev)
        {
            Offset *= -1.f;
        }
        
        ASpectatorPawn* Spectator = Cast<ASpectatorPawn>(GetViewTarget());
        if (!Spectator)
        {
            PlayerCameraManager->FreeCamDistance = FMath::Clamp(PlayerCameraManager->FreeCamDistance + Offset, 16.f, 2048.f);
        }
        else
        {
            USpectatorPawnMovement* SpectatorMovement = Cast<USpectatorPawnMovement>(Spectator->GetMovementComponent());
            if (SpectatorMovement)
            {
                SpectatorMovement->MaxSpeed = FMath::Clamp(SpectatorMovement->MaxSpeed + 5.f * Offset, 200.f, 6000.f);
            }
        }
    }
}

void ANZPlayerController::SwitchWeapon(int32 Group)
{
    SwitchWeaponGroup(Group);
}

void ANZPlayerController::SwitchWeaponGroup(int32 Group)
{
    if (NZCharacter != NULL && IsLocalPlayerController() && /*NZCharacter->EmoteCount == 0 &&*/ !NZCharacter->IsRagdoll())
    {
        // If Current weapon isn't in the specified group, pick lowest GroupSlot in that group
        // If it is, then pick next highest slot, or wrap around to lowest if no higher slot
        ANZWeapon* CurrWeapon = (NZCharacter->GetPendingWeapon() != NULL) ? NZCharacter->GetPendingWeapon() : NZCharacter->GetWeapon();
        ANZWeapon* LowestSlotWeapon = NULL;
        ANZWeapon* NextSlotWeapon = NULL;
        for (TInventoryIterator<ANZWeapon> It(NZCharacter); It; ++It)
        {
            ANZWeapon* Weap = *It;
            if (Weap != NZCharacter->GetWeapon() && Weap->HasAnyAmmo())
            {
                if (Weap->Group == Group)
                {
                    if (LowestSlotWeapon == NULL || LowestSlotWeapon->GroupSlot > Weap->GroupSlot)
                    {
                        LowestSlotWeapon = Weap;
                    }
                    if (CurrWeapon != NULL && CurrWeapon->Group == Group && Weap->GroupSlot > CurrWeapon->GroupSlot && (NextSlotWeapon == NULL || NextSlotWeapon->GroupSlot > Weap->GroupSlot))
                    {
                        NextSlotWeapon = Weap;
                    }
                }
            }
        }
        if (NextSlotWeapon != NULL)
        {
            NZCharacter->SwitchWeapon(NextSlotWeapon);
        }
        else if (LowestSlotWeapon != NULL)
        {
            NZCharacter->SwitchWeapon(LowestSlotWeapon);
        }
    }
}

void ANZPlayerController::CheckAutoWeaponSwitch(class ANZWeapon* TestWeapon)
{
    if (NZCharacter != NULL && IsLocalPlayerController())
    {
        ANZWeapon* CurWeapon = NZCharacter->GetPendingWeapon();
        if (CurWeapon == NULL)
        {
            CurWeapon = NZCharacter->GetWeapon();
        }
        if (CurWeapon == NULL ||
            (bAutoWeaponSwitch && !NZCharacter->IsPendingFire(CurWeapon->GetCurrentFireMode()) && TestWeapon->AutoSwitchPriority > CurWeapon->AutoSwitchPriority))
        {
            NZCharacter->SwitchWeapon(TestWeapon);
        }
    }
}


void ANZPlayerController::OnFire()
{
    if (GetPawn() != NULL)
    {
        new(DeferredFireInputs) FDeferredFireInput(0, true);
    }
    else if (IsInState(NAME_Spectating))
    {
        // todo:
        
        if ((PlayerState == NULL || !PlayerState->bOnlySpectator) && bPlayerIsWaiting)
        {
            ServerRestartPlayer();
        }
    }
    else
    {
        // todo:
        
        ServerRestartPlayer();
    }
}

void ANZPlayerController::OnStopFire()
{
    if (GetPawn() != NULL)
    {
        new(DeferredFireInputs) FDeferredFireInput(0, false);
    }
}

void ANZPlayerController::OnAltFire()
{
    if (GetPawn() != NULL)
    {
        new(DeferredFireInputs) FDeferredFireInput(1, true);
    }
    else if (IsInState(NAME_Spectating) &&
             GetWorld()->GetGameState() != NULL && GetWorld()->GetGameState()->HasMatchStarted())
    {
        // todo:
        
        if ((PlayerState == NULL || !PlayerState->bOnlySpectator) && bPlayerIsWaiting)
        {
            ServerRestartPlayer();
        }
        else
        {
            bAutoCam = false;
            ViewSelf();
        }
    }
    else
    {
        // todo:
        
        ServerRestartPlayerAltFire();
    }
}

void ANZPlayerController::OnStopAltFire()
{
    if (GetPawn() != NULL)
    {
        new(DeferredFireInputs) FDeferredFireInput(1, false);
    }
}

void ANZPlayerController::ChangeClip()
{
	if (NZCharacter != NULL)
	{
		NZCharacter->ChangeClip();
	}
}

void ANZPlayerController::LeftHandChangeClip()
{
	if (NZCharacter != NULL)
	{
		NZCharacter->LeftHandChangeClip();
	}
}

void ANZPlayerController::Sprint()
{
	if (NZCharacter != NULL)
	{
		NZCharacter->Sprint();
	}
}

void ANZPlayerController::UnSprint()
{
	if (NZCharacter != NULL)
	{
		NZCharacter->UnSprint();
	}
}

void ANZPlayerController::MoveForward(float Value)
{
	if (Value != 0.0f && NZCharacter != NULL)
	{
		MovementForwardAxis = Value;
		NZCharacter->MoveForward(Value);
	}
	else if (GetSpectatorPawn() != NULL)
	{
		GetSpectatorPawn()->MoveForward(Value);
	}
}

void ANZPlayerController::MoveBackward(float Value)
{
	MoveForward(Value * -1);
}

void ANZPlayerController::MoveLeft(float Value)
{
	MoveRight(Value * -1);
}

void ANZPlayerController::MoveRight(float Value)
{
	if (Value != 0.0f && NZCharacter != NULL)
	{
		MovementStrafeAxis = Value;
		NZCharacter->MoveRight(Value);
	}
	else if (GetSpectatorPawn() != NULL)
	{
		GetSpectatorPawn()->MoveRight(Value);
	}
}

void ANZPlayerController::MoveUp(float Value)
{
	if (Value != 0.0f && NZCharacter != NULL)
	{
		NZCharacter->MoveUp(Value);
	}
	else if (GetSpectatorPawn() != NULL)
	{
		GetSpectatorPawn()->MoveUp_World(Value);
	}
}

void ANZPlayerController::Jump()
{
	if (NZCharacter != NULL && !IsMoveInputIgnored())
	{
		NZCharacter->bPressedJump = true;

		// todo:
	}
}

void ANZPlayerController::JumpRelease()
{

}

void ANZPlayerController::Crouch()
{
	if (!IsMoveInputIgnored())
	{
		// todo:

		if (NZCharacter != NULL)
		{
			NZCharacter->Crouch(false);
		}
	}
}

void ANZPlayerController::UnCrouch()
{
	// todo:

	if (NZCharacter != NULL)
	{
		NZCharacter->UnCrouch(false);
	}
}

void ANZPlayerController::ToggleCrouch()
{
	if (GetCharacter() != NULL)
	{
		GetCharacter()->bIsCrouched ? UnCrouch() : Crouch();
	}
}

void ANZPlayerController::AddYawInput(float Value)
{
    if (Value != 0.f)
    {
        Super::AddYawInput(Value);
    }
}

void ANZPlayerController::AddPitchInput(float Value)
{
    if (Value != 0.f)
    {
        Super::AddPitchInput(Value);
    }
}

void ANZPlayerController::TurnAtRate(float Rate)
{
    // Calculate delta for this frame from the rate information
    AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANZPlayerController::LookUpAtRate(float Rate)
{
    //
    AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANZPlayerController::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (NZPlayerInput)
	{
		NZPlayerInput->BeginTouch(FingerIndex, Location);
	}
}

void ANZPlayerController::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (NZPlayerInput)
	{
		NZPlayerInput->EndTouch(FingerIndex, Location);
	}
}

void ANZPlayerController::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (NZPlayerInput)
	{
		NZPlayerInput->TouchUpdate(FingerIndex, Location);
	}
}

void ANZPlayerController::ApplyDeferredFireInputs()
{
	for (FDeferredFireInput& Input : DeferredFireInputs)
	{
		if (Input.bStartFire)
		{
			if (!IsMoveInputIgnored())
			{
				if (NZCharacter != NULL)
				{
					if (StateName == NAME_Playing)
					{
						NZCharacter->StartFire(Input.FireMode);
					}
				}
				else if (GetPawn() != NULL)
				{
					GetPawn()->PawnStartFire(Input.FireMode);
				}
			}
		}
		else if (NZCharacter != NULL)
		{
			NZCharacter->StopFire(Input.FireMode);
		}
	}
	DeferredFireInputs.Empty();
}

bool ANZPlayerController::HasDeferredFireInputs()
{
	for (FDeferredFireInput& Input : DeferredFireInputs)
	{
		if (Input.bStartFire)
		{
			return true;
		}
	}
	return false;
}

void ANZPlayerController::SetViewedScorePS(ANZPlayerState* ViewedPS, uint8 NewStatsPage)
{
    
}


float ANZPlayerController::GetPredictionTime()
{
    // Exact ping is in msec, devide by 1000 to get time in seconds
    //if (Role == ROLE_Authority) { UE_LOG(NZ, Warning, TEXT("Server ExactPing %f"), PlayerState->ExactPing); }
    return (PlayerState && (GetNetMode() != NM_Standalone)) ? (0.0005f * FMath::Clamp(PlayerState->ExactPing - PredictionFudgeFactor, 0.f, MaxPredictionPing)) : 0.f;
}

float ANZPlayerController::GetProjectileSleepTime()
{
    return 0.001f * FMath::Max(0.f, PlayerState->ExactPing - PredictionFudgeFactor - MaxPredictionPing);
}

void ANZPlayerController::ServerBouncePing_Implementation(float TimeStamp)
{
	ClientReturnPing(TimeStamp);
}

bool ANZPlayerController::ServerBouncePing_Validate(float TimeStamp)
{
	return true;
}

void ANZPlayerController::ClientReturnPing_Implementation(float TimeStamp)
{
	ANZPlayerState* NZPS = Cast<ANZPlayerState>(PlayerState);
	if (NZPS)
	{
		NZPS->CalculatePing(GetWorld()->GetTimeSeconds() - TimeStamp);
	}
}

void ANZPlayerController::ServerUpdatePing_Implementation(float ExactPing)
{
	if (PlayerState)
	{
		PlayerState->ExactPing = ExactPing;
		PlayerState->Ping = FMath::Min(255, (int32)(ExactPing * 0.25f));
	}
}

bool ANZPlayerController::ServerUpdatePing_Validate(float ExactPing)
{
	return true;
}

float ANZPlayerController::GetWeaponAutoSwitchPriority(FString WeaponClassname, float DefaultPriority)
{
	return DefaultPriority;
}

void ANZPlayerController::SetWeaponGroup(class ANZWeapon* InWeapon)
{
	if (Cast<UNZLocalPlayer>(Player))
	{
        UNZProfileSettings* ProfileSettings = Cast<UNZLocalPlayer>(Player)->GetProfileSettings();
        if (ProfileSettings)
        {
            FString WeaponClassName = GetNameSafe(InWeapon);
            if (ProfileSettings->WeaponGroupLookup.Contains(WeaponClassName))
            {
                InWeapon->Group = ProfileSettings->WeaponGroupLookup[WeaponClassName].Group;
            }
        }
	}
}

void ANZPlayerController::ClientRequireContentItemListComplete_Implementation()
{

}







void ANZPlayerController::FOV(float NewFOV)
{
    if (NewFOV != ConfigDefaultFOV)
    {
        ConfigDefaultFOV = FMath::Clamp<float>(NewFOV, FOV_CONFIG_MIN, FOV_CONFIG_MAX);
        if (PlayerCameraManager != NULL)
        {
            PlayerCameraManager->DefaultFOV = ConfigDefaultFOV;
        }
        if (GetPawn() != NULL && GetNetMode() != NM_Standalone)
        {
            Suicide();
        }
        SaveConfig();
    }
}

void ANZPlayerController::SetMouseSensitivityNZ(float NewSensitivity)
{
    PlayerInput->SetMouseSensitivity(NewSensitivity);
    
    UInputSettings* InputSettings = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
    for (FInputAxisConfigEntry& Entry : InputSettings->AxisConfig)
    {
        if (Entry.AxisKeyName == EKeys::MouseX || Entry.AxisKeyName == EKeys::MouseY)
        {
            Entry.AxisProperties.Sensitivity = NewSensitivity;
        }
    }
    
    InputSettings->SaveConfig();
}


void ANZPlayerController::ServerViewPawn_Implementation(APawn* PawnToView)
{
    // Don't view other pawns when we already have a pawn
    if (GetPawn() != NULL)
    {
        return;
    }
    
    if (PawnToView)
    {
        SetViewTarget(PawnToView->PlayerState);
    }
}

bool ANZPlayerController::ServerViewPawn_Validate(APawn* PawnToView)
{
    return true;
}

void ANZPlayerController::ViewPawn(APawn* PawnToView)
{
    ServerViewPawn(PawnToView);
}


void ANZPlayerController::Suicide()
{
    ServerSuicide();
}

void ANZPlayerController::ServerSuicide_Implementation()
{
    
    
}

bool ANZPlayerController::ServerSuicide_Validate()
{
    return true;
}



void ANZPlayerController::SetSpectatorMouseChangesView(bool bNewValue)
{
    
}
