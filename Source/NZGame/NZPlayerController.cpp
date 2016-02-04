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



ANZPlayerController::ANZPlayerController()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
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
    
	InputComponent->BindAxis("MoveForward", this, &ANZPlayerController::MoveForward);
	InputComponent->BindAxis("MoveBackward", this, &ANZPlayerController::MoveBackward);
	InputComponent->BindAxis("MoveLeft", this, &ANZPlayerController::MoveLeft);
	InputComponent->BindAxis("MoveRight", this, &ANZPlayerController::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ANZPlayerController::MoveUp);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ANZPlayerController::AddYawInput);
	//InputComponent->BindAxis("TurnRate", this, &ANZPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ANZPlayerController::AddPitchInput);
	//InputComponent->BindAxis("LookUpRate", this, &ANZPlayerController::LookUpAtRate);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ANZPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ANZPlayerController::JumpRelease);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ANZPlayerController::Crouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &ANZPlayerController::UnCrouch);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ANZPlayerController::ToggleCrouch);
    
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
        else if (P->GetWeapon() == NULL || P->GetWeapon()->HandsAttachSocket == NAME_None)
        {
            // Weapon doesn't use hands
            HiddenComponents.Add(P->FirstPersonMesh->ComponentId);
        }
        
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
    
}

bool ANZPlayerController::ServerSwitchTeam_Validate()
{
    return true;
}

void ANZPlayerController::BehindView(bool bWantBehindView)
{
}

bool ANZPlayerController::IsBehindView()
{
    if (PlayerCameraManager != NULL)
    {
/*        static FName NAME_FreeCam(TEXT("FreeCam"));
        
        ANZPlayerController* NZCam = Cast<ANZPlayerCameraManager>(PlayerCameraManager);
        FName CameraStyle = (NZCam != NULL) ? NZCam->GetCameraStyleWithOverrides() : PlayerCameraManager->CameraStyle;
        
        return CameraStyle == NAME_FreeCam;*/
        // todo:
        return false;
    }
    else
    {
        return false;
    }
}

void ANZPlayerController::SetCameraMode(FName NewCamMode)
{
}

void ANZPlayerController::ClientSetCameraMode_Implementation(FName NewCamMode)
{
}

void ANZPlayerController::ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner)
{
    
}



void ANZPlayerController::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
    // remove any FOV shifts when changing targets (e.g. sniper zoom)
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

void ANZPlayerController::FindGoodView(const FVector& Targetloc, bool bIsUpdate)
{
    // todo:
}

void ANZPlayerController::ServerViewFlagHolder_Implementation(uint8 TeamIndex)
{
    // todo:
}

bool ANZPlayerController::ServerViewFlagHolder_Validate(uint8 TeamIndex)
{
    return true;
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
        
        // todo:
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
	// todo:

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

float ANZPlayerController::GetPredictionTime()
{
    // todo:
    check(false);
    return 0.f;
    // Exact ping is in msec, devide by 1000 to get time in seconds
    //if (Role == ROLE_Authority) { UE_LOG(NZ, Warning, TEXT("Server ExactPing %f"), PlayerState->ExactPing); }
    //return (PlayerState && (GetNetMode() != NM_Standalone)) ? (0.0005f * FMath::Clamp(PlayerState->ExactPing - PredictionFudgeFactor, 0.f, MaxPredictionPing)) : 0.f;
}

float ANZPlayerController::GetProjectileSleepTime()
{
    // todo:
    check(false);
    return 0.f;
    //return 0.001f * FMath::Max(0.f, PlayerState->ExactPing - PredictionFudgeFactor - MaxPredictionPing);
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
