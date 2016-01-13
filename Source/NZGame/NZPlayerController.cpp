// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerController.h"
#include "NZLocalPlayer.h"
#include "NZWeapon.h"
#include "NZHUD.h"
#include "NZPlayerState.h"
#include "NZPlayerInput.h"
#include "NZProfileSettings.h"


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

void ANZPlayerController::InitInputSystem()
{
    if (PlayerInput == NULL)
    {
        PlayerInput = NewObject<UNZPlayerInput>(this, UNZPlayerInput::StaticClass());
    }
    
    Super::InitInputSystem();
    
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

void ANZPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
/*    InputComponent->BindAxis("MoveForward", this, &ANZPlayerController::MoveForward);
    InputComponent->BindAxis("MoveBackward", this, &ANZPlayerController::MoveBackward);
    InputComponent->BindAxis("MoveLeft", this, &ANZPlayerController::MoveLeft);
    InputComponent->BindAxis("MoveRight", this, &ANZPlayerController::MoveRight);
    InputComponent->BindAxis("MoveUp", this, &ANZPlayerController::MoveUp);
    
    InputComponent->BindAction("Jump", IE_Pressed, this, &ANZPlayerController::Jump);
    InputComponent->BindAction("Jump", IE_Released, this, &ANZPlayerController::JumpRelease);
    InputComponent->BindAction("Crouch", IE_Pressed, this, &ANZPlayerController::Crouch);
    InputComponent->BindAction("Crouch", IE_Released, this, &ANZPlayerController::UnCrouch);
    InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ANZPlayerController::ToggleCrouch);
    
    InputComponent->BindAction("TapLeft", IE_Pressed, this, &ANZPlayerController::OnTapLeft);
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
    
    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
    InputComponent->BindAxis("TurnRate", this, &ANZPlayerController::TurnAtRate);
    InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);
    InputComponent->BindAxis("LookUpRate", this, &ANZPlayerController::LookUpAtRate);
    
    InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ANZPlayerController::PrevWeapon);
    InputComponent->BindAction("NextWeapon", IE_Released, this, &ANZPlayerController::NextWeapon);
    InputComponent->BindAction("ThrowWeapon", IE_Released, this, &ANZPlayerController::ThrowWeapon);
    
    InputComponent->BindAction("StartFire", IE_Pressed, this, &ANZPlayerController::OnFire);
    InputComponent->BindAction("StopFire", IE_Released, this, &ANZPlayerController::OnStopFire);
    InputComponent->BindAction("StartAltFire", IE_Pressed, this, &ANZPlayerController::OnAltFire);
    InputComponent->BindAction("StopAltFire", IE_Released, this, &ANZPlayerController::OnStopAltFire);
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

void ANZPlayerController::SpawnPlayerCameraManager()
{
    Super::SpawnPlayerCameraManager();
    // Init configured FOV angle
    if (PlayerCameraManager != NULL)
    {
        PlayerCameraManager->DefaultFOV = ConfigDefaultFOV;
    }
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

void ANZPlayerController::PrevWeapon()
{
}

void ANZPlayerController::NextWeapon()
{
}

void ANZPlayerController::ThrowWeapon()
{
}

void ANZPlayerController::ServerThrowWeapon_Implementation()
{
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
/*                    if (Weap->FollowsInList(CurrentWeapon) == bPrev)
                    {
                        if (WraparoundChoice == NULL || (Weap->FollowsInList(WraparoundChoice) == bPrev))
                        {
                            WraparoundChoice = Weap;
                        }
                    }
                    else if (Best == NULL || (Weap->FollowsInList(Best) == bPrev))
                    {
                        Best = Weap;
                    }*/
                }
            }
            
            if (Best == NULL)
            {
                Best = WraparoundChoice;
            }
            
            NZCharacter->SwitchWeapon(Best);
        }
    }
}

void ANZPlayerController::SwitchWeapon(int32 Group)
{
    
}

void ANZPlayerController::SwitchWeaponGroup(int32 Group)
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
