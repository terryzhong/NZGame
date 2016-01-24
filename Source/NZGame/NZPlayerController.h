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
    
    
    
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRestartPlayerAltFire();
    
    /** Switch between teams 0 and 1 */
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerSwitchTeam();
    
    
    
    
    virtual void SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
    virtual void ServerViewSelf_Implementation(FViewTargetTransitionParams TransitionParams) override;
    virtual void ViewSelf(FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
    
    /** 
     * Update rotation to be good view of current viewtarget.
     * UnBlockedPct is how much of the camera offset trace needs to be unlocked.
     */
    UFUNCTION()
    virtual void FindGoodView(const FVector& Targetloc, bool bIsUpdate);
    
    /** Set when request view projectile if no projectile find, keep looking */
    UPROPERTY()
    float ViewProjectileTime;
    
    
    /** Enables auto best camera for spectators */
    UPROPERTY(BlueprintReadWrite)
    bool bAutoCam;
    
    
    

	UFUNCTION(exec)
	virtual void SwitchToBestWeapon();
    
    void PrevWeapon();
    void NextWeapon();
    
    void ThrowWeapon();
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void ServerThrowWeapon();
    
    int32 PreviousWeaponGroup;
    
    virtual void SwitchWeaponInSequence(bool bPrev);
    
    UFUNCTION(Exec)
    virtual void SwitchWeapon(int32 Group);
    
    UFUNCTION(Exec)
    virtual void SwitchWeaponGroup(int32 Group);
    
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
    
    /**
     * Stores fire inputs until after movement has been executed (default would be fire -> movement -> render, this causes movement -> fire -> render)
     * Makes weapons feel a little more responsive while strafing
     */
    TArray<FDeferredFireInput, TInlineAllocator<2> > DeferredFireInputs;
    

    // Perceived latency reduction
    
    /** Return amount of time to tick or simulate to make up for network lag */
    virtual float GetPredictionTime();
    
    /** How long fake projectile should sleep before starting to simulate (because client ping is greater than MaxPredictionPing) */
    virtual float GetProjectileSleepTime();
    
    /** List of fake projectiles currently out there for this client */
    UPROPERTY()
    TArray<class ANZProjectile*> FakeProjectiles;
    
    // Ping calculation
    
    /** Guess of this player's target on last shot, used by AI */
    UPROPERTY(BlueprintReadWrite, Category = AI)
    APawn* LastShotTargetGuess;
    
    
protected:
    UPROPERTY()
    AActor* FinalViewTarget;
    
    //TSet<TWeakObjectPtr<ANZPickupWeapon> > RecentWeaponPickups;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    float MovementForwardAxis;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    float MovementStrafeAxis;
  
    
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
