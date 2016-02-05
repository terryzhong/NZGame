// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZBasePlayerController.h"
#include "NZPlayerController.generated.h"


#define FOV_CONFIG_MIN 80.0f
#define FOV_CONFIG_MAX 120.0f

struct FDeferredFireInput
{
    /** The fire mode */
    uint8 FireMode;
    
    /** If true, call StartFire(), false call StopFire() */
    bool bStartFire;
    
    FDeferredFireInput(uint8 InFireMode, bool bInStartFire)
        : FireMode(InFireMode)
        , bStartFire(bInStartFire)
    {}
};

/** Controls location and orientation of first person weapon */
UENUM()
enum EWeaponHand
{
    HAND_Right,
    HAND_Left,
    HAND_Center,
    HAND_Hidden,
};

/**
 * 
 */
UCLASS()
class NZGAME_API ANZPlayerController : public ANZBasePlayerController
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class ANZCharacter* NZCharacter;
	
public:
    ANZPlayerController();
    
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	virtual ANZCharacter* GetNZCharacter();
    
    UFUNCTION(BlueprintCallable, Category = PlayerController)
    virtual UNZLocalPlayer* GetNZLocalPlayer();
    
    UPROPERTY(ReplicatedUsing = OnRep_HUDClass)
    TSubclassOf<class AHUD> HUDClass;
    
    UFUNCTION()
    virtual void OnRep_HUDClass();
    
    UPROPERTY()
    class ANZHUD* MyNZHUD;
    
    

    
    virtual void BeginPlay() override;
    virtual void Destroyed() override;
    virtual void InitInputSystem() override;
    virtual void InitPlayerState();
    virtual void OnRep_PlayerState();
    virtual void SetPawn(APawn* InPawn);
    virtual void SetupInputComponent() override;
    virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
    virtual void PawnPendingDestroy(APawn* InPawn) override;
    
    /** 167 */
    virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& HiddenComponents);
    
    /** 179
     Attempts to restart this player, generally called from the client upon respawn request */
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRestartPlayerAltFire();
    
    /** Switch between teams 0 and 1 */
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerSwitchTeam();
    
    /** 193 */
    UFUNCTION(exec)
    virtual void BehindView(bool bWantBehindView);
    
    virtual bool IsBehindView();
    virtual void SetCameraMode(FName NewCamMode);
    virtual void ClientSetCameraMode_Implementation(FName NewCamMode) override;
    virtual void ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner) override;
    
	/** Handles bWantsBehindView */
	virtual void ResetCameraMode() override;

	/** Switch to best current camera while spectating */
	virtual void ChooseBestCamera();
    
    virtual void SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
    virtual void ServerViewSelf_Implementation(FViewTargetTransitionParams TransitionParams) override;
    virtual void ViewSelf(FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
    
    /** 
     * Update rotation to be good view of current viewtarget.
     * UnBlockedPct is how much of the camera offset trace needs to be unlocked.
     */
    UFUNCTION()
    virtual void FindGoodView(const FVector& Targetloc, bool bIsUpdate);
    
    
    /** View player holding flag specified by TeamIndex */
    UFUNCTION(Unreliable, Server, WithValidation)
    void ServerViewFlagHolder(uint8 TeamIndex);
    
    /** 256
     Set when request view projectile if no projectile find, keep looking */
    UPROPERTY()
    float ViewProjectileTime;
    
    
    /** Enables auto best camera for spectators */
    UPROPERTY(BlueprintReadWrite)
    bool bAutoCam;
    

	virtual void Possess(APawn* PawnToPossess) override;
	virtual void PawnLeavingGame() override;

	/** 313
	 We override player tick to keep updating the player's rotation when the game is over */
	virtual void PlayerTick(float DeltaTime) override;
    
    
protected:
    UPROPERTY(GlobalConfig, BlueprintReadOnly, Category = Weapon)
    TEnumAsByte<EWeaponHand> WeaponHand;
public:
    inline EWeaponHand GetWeaponHand() const
    {
        // Spectators always see right handed weapons
        return IsInState(NAME_Spectating) ? HAND_Right : GetPreferredWeaponHand();
    }
    
    inline EWeaponHand GetPreferredWeaponHand() const
    {
        return WeaponHand;
    }
    
    UFUNCTION(BlueprintCallable, Category = Weapon)
    void SetWeaponHand(EWeaponHand NewHand);
    
    UFUNCTION(Reliable, Server, WithValidation)
    void ServerSetWeaponHand(EWeaponHand NewHand);
    
    
    
    

	UFUNCTION(exec)
	virtual void SwitchToBestWeapon();

	/** Forces SwitchToBestWeapon() call, should only be used after granting startup inventory */
	UFUNCTION(Client, Reliable)
	virtual void ClientSwitchToBestWeapon();
    
    void PrevWeapon();
    void NextWeapon();
    
    void ThrowWeapon();
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerThrowWeapon();
    
    int32 PreviousWeaponGroup;
    
    virtual void SwitchWeaponInSequence(bool bPrev);
    
    /** Switches weapons using classic groups */
    UFUNCTION(Exec)
    virtual void SwitchWeapon(int32 Group);
    
    /** Switches weapons using modern groups */
    UFUNCTION(Exec)
    virtual void SwitchWeaponGroup(int32 Group);
    
    virtual void CheckAutoWeaponSwitch(class ANZWeapon* TestWeapon);
    
    UPROPERTY(GlobalConfig, BlueprintReadOnly, Category = Weapon)
    bool bAutoWeaponSwitch;
    
    /** Weapon fire input handling -- NOTE: Just forward to the pawn */
    virtual void OnFire();
    virtual void OnStopFire();
    virtual void OnAltFire();
    virtual void OnStopAltFire();

	/** Handles moving forward */
	virtual void MoveForward(float Value);

	/** Handles moving backward */
	virtual void MoveBackward(float Value);

	/** Handles strafing movement left */
	virtual void MoveLeft(float Value);

	/** Handles strafing movement right */
	virtual void MoveRight(float Value);

	/** Up and down when flying or swimming */
	virtual void MoveUp(float Value);

	/** Called to set the jump flag from input */
	virtual void Jump();

	/** Called when jump is released */
	virtual void JumpRelease();
    
	virtual void Crouch();
	virtual void UnCrouch();
	virtual void ToggleCrouch();

	virtual void AddYawInput(float Value) override;
	virtual void AddPitchInput(float Value) override;
    
    /** 813
     * Stores fire inputs until after movement has been executed (default would be fire -> movement -> render, this causes movement -> fire -> render)
     * Makes weapons feel a little more responsive while strafing
     */
    TArray<FDeferredFireInput, TInlineAllocator<2> > DeferredFireInputs;

	void ApplyDeferredFireInputs();

	bool HasDeferredFireInputs();
    

    // Perceived latency reduction
    
    /** 518
     Used to correct prediction error */
    UPROPERTY(EditAnywhere, Replicated, GlobalCOnfig, Category = Network)
    float PredictionFudgeFactor;
    
    /** Negotiated max amount of ping to predict ahead for */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Network)
    float MaxPredictionPing;
    
    /** User configurable desired prediction ping (will be negotiated with server) */
    UPROPERTY(BlueprintReadOnly, GLobalConfig, Category = Network)
    float DesiredPredictionPing;
    
    
    /** Return amount of time to tick or simulate to make up for network lag */
    virtual float GetPredictionTime();
    
    /** How long fake projectile should sleep before starting to simulate (because client ping is greater than MaxPredictionPing) */
    virtual float GetProjectileSleepTime();
    
    /** List of fake projectiles currently out there for this client */
    UPROPERTY()
    TArray<class ANZProjectile*> FakeProjectiles;
    
    // Ping calculation

	UPROPERTY()
	float LastPingCalcTime;

	/** Client sends ping request to server - used when servermoves aren't happening */
	UFUNCTION(unreliable, server, WithValidation)
	virtual void ServerBouncePing(float TimeStamp);

	/** Server bounces ping request back to client - used when servermoves aren't happening */
	UFUNCTION(unreliable, client)
	virtual void ClientReturnPing(float TimeStamp);

	/** Client informs server of new ping update */
	UFUNCTION(unreliable, server, WithValidation)
	virtual void ServerUpdatePing(float ExactPing);
    
    /** Guess of this player's target on last shot, used by AI */
    UPROPERTY(BlueprintReadWrite, Category = AI)
    APawn* LastShotTargetGuess;

	virtual float GetWeaponAutoSwitchPriority(FString WeaponClassname, float DefaultPriority);
	virtual void SetWeaponGroup(class ANZWeapon* InWeapon);
    
	virtual void ClientRequireContentItemListComplete_Implementation() override;


    
protected:
    /** If set, this will be the final viewtarget this pawn can see */
    UPROPERTY()
    AActor* FinalViewTarget;
    
    /** List of weapon pickups that my Pawn has recently picked up, so we can hide the weapon mesh per player */
    TSet<TWeakObjectPtr<class ANZPickupWeapon> > RecentWeaponPickups;
    
    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;
    
    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    float MovementForwardAxis;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    float MovementStrafeAxis;  
    
public:
    /** Whether player wants behindview when spectating */
    UPROPERTY(BlueprintReadWrite, GlobalConfig)
    bool bSpectateBehindView;
    
    /** Whether player wants behindview when playing (subject to server and game mode restrictions) */
    UPROPERTY(BlueprintReadOnly)
    bool bPlayBehindView;
    
    UPROPERTY(BlueprintReadOnly)
    bool bRequestingSlideOut;
    
    /** True when spectator has used a spectating camera bind */
    UPROPERTY()
    bool bHasUsedSpectatingBind;
    
    UPROPERTY()
    bool bShowCameraBinds;
    
    UPROPERTY()
    bool bShowPowerupTimes;
    
    /** Toggle behindview for spectators */
    UFUNCTION(exec)
    virtual void ToggleBehindView();
    
    /** User configurable FOV setting */
    UPROPERTY(BlueprintReadOnly, GlobalConfig, Category = Camera)
    float ConfigDefaultFOV;
    
    virtual void SpawnPlayerCameraManager() override;
    virtual void FOV(float NewFOV) override;
    
    
    
    UPROPERTY()
    class ANZPlayerState* LastSpectatedPlayerState;
    
    UPROPERTY()
    int32 LastSpectatedPlayerId;
    
    
    
    UFUNCTION(exec)
    virtual void Suicide();
    
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSuicide();


protected:
    bool bSpectatorMouseChangesView;
    void SetSpectatorMouseChangesView(bool bNewValue);
    
};
