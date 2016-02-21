// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NZCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API UNZCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UNZCharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool ClientUpdatePositionAfterServerUpdate() override;
	virtual void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration) override;
	virtual void ClientAckGoodMove_Implementation(float TimeStamp) override;

	/** Return true if it is OK to delay sending this player movement to the server to conserver bandwidth */
	virtual bool CanDelaySendingMove(const FSavedMovePtr& NewMove) override;

	virtual void NZServerMoveHandleClientError(float TimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLoc, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);

	virtual void NZCallServerMove();

	UPROPERTY(BlueprintReadOnly, Category = "Character Movement")
	float MaxPositionErrorSquared;

	/** Return true if position error exceeds max allowable amount */
	virtual bool ExceedsAllowablePositionError(FVector LocDiff) const;

	/** Process ServerMove forwarded by character */
	virtual void ProcessServerMove(float TimeStamp, FVector Accel, FVector ClientLoc, uint8 CompressedMoveFlags, float ViewYaw, float ViewPitch, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);

	/** Process ServerMove forwarded by character, without correction */
	virtual void ProcessQuickServerMove(float TimeStamp, FVector Accel, uint8 CompressedMoveFlags);

	/** Process ServerMove forwarded by character, without correction, but needs rotation */
	virtual void ProcessSavedServerMove(float TimeStamp, FVector Accel, uint8 CompressedMoveFlags, float ViewYaw, float ViewPitch);

	/** Process old ServerMove forwarded by character */
	virtual void ProcessOldServerMove(float OldTimeStamp, FVector OldAccel, float OldYaw, uint8 OldMoveFlags);

	/** Sets LastClientAdjustmentTime so there will be no delay in sending any needed adjustment */
	virtual void NeedsClientAdjustment();

	virtual void ResetPredictionData_Client() override;

	/** Bandwidth saving version, when position is not relative to base */
	UFUNCTION(Unreliable, Client)
	virtual void ClientNoBaseAdjustPosition(float TimeStamp, FVector NewLoc, FVector NewVelocity, uint8 ServerMovementMode);

	/** Last time a client adjustment was sent. Used to limit frequency (for when client hasn't had a chance to respond yet) */
	UPROPERTY()
	float LastClientAdjustmentTime;

	/** Last time a good move ack was sent. Used to limit frequency (for when client hasn't had a chance to respond yet) */
	UPROPERTY()
	float LastGoodMoveAckTime;

	/** Set if the pending client adjustment is for a large position correction */
	UPROPERTY()
	bool bLargeCorrection;

	/** Position error considered a large correction for client adjustment */
	UPROPERTY()
	float LargeCorrectionThreshold;

	/** Used to control minimum frequency of client adjustment and good move ack replication */
	UPROPERTY()
	float MinTimeBetweenClientAdjustments;

	/** Set client-side acceleration based on replicated acceldir */
	virtual void SetReplicatedAcceleration(FRotator MovementRotation, uint8 AccelDir);

	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	virtual FVector GetImpartedMovementBaseVelocity() const override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual void PerformMovement(float DeltaSeconds) override;
	virtual float ComputeAnalogInputModifier() const override;

    /** Update traversal stats */
	virtual void UpdateMovementStats(const FVector& StartLocation);

    /** Try to base on lift that just ran into me, return true if success */
	virtual bool CanBaseOnLift(UPrimitiveComponent* LiftPrim, const FVector& LiftMoveDelta);

	virtual void UpdateBasedMovement(float DeltaSeconds) override;

	virtual bool CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit, const FVector& Delta, const FVector& OldLocation, float remainingTime, float timeTick, int32 Iterations, bool bMustJump) override;

    /** If i'm on a lift, tell it to return */
	virtual void OnUnableToFollowBaseMove(const FVector& DeltaPosition, const FVector& OldLocation, const FHitResult& MoveOnBaseHit) override;

    /** Reset timers (called on pawn possessed) */
	virtual void ResetTimers();

	/** Return true if movement input should not be constrained to horizontal plane */
	virtual bool Is3DMovementMode() const;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

    /** Allows custom handling of timestamp and delta time updates, including resetting movement timers */
	virtual float UpdateTimeStampAndDeltaTime(float DeltaTime, FNetworkPredictionData_Client_Character* ClientData);

    /** Adjust movement timers after timestamp reset */
	virtual void AdjustMovementTimers(float Adjustment);

    /** Allows custom handling of timestamp and delta time updates, including resetting movement timers */
	virtual bool NZVerifyClientTimeStamp(float TimeStamp, FNetworkPredictionData_Server_Character& ServerData);

    /** For replaying moves set up */
	bool bIsSettingUpFirstReplayMove;

    /** Smoothed speed */
	UPROPERTY()
	float AvgSpeed;

    /** Max Acceleration when falling (will be scaled by AirControl property) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement", meta = (ClampMin = "0", UIMin = "0"))
	float MaxFallingAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement", meta = (ClampMin = "0", UIMin = "0"))
	float MaxSwimmingAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement", meta = (ClampMin = "0", UIMin = "0"))
	float MaxRelativeSwimmingAccelNumerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement", meta = (ClampMin = "0", UIMin = "0"))
	float MaxRelativeSwimmingAccelDenominator;

    /** Braking when walking - set to same value as BrakingDecelerationWalking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement", meta = (ClampMin = "0", UIMin = "0"))
	float DefaultBrakingDecelerationWalking;

	/** True if projectile/hitscan spawned this frame (replicated to synchronize held firing) */
	UPROPERTY()
	bool bShotSpawned;

	UPROPERTY(BlueprintReadOnly, Category = "Emote")
	bool bIsEmoting;
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

public:

	/** Add an impulse, damped if player would end up going too fast */
	UFUNCTION(BlueprintCallable, Category = "Impulse")
	virtual void AddDampedImpulse(FVector Impulse, bool bSelfInflicted);


	virtual void HandleCrouchRequest();

	virtual void HandleUnCrouchRequest();

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual bool IsCrouching() const override;

	/** How long you have to be running/grounded before auto-sprint engages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autosprint", meta = (DisplayName = "Auto Sprint Delay Interval"))
	float AutoSprintDelayInterval;

	/** Max speed when sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autosprint", meta = (DisplayName = "Sprint Speed"))
	float SprintSpeed;

	/** Acceleration when sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autosprint", meta = (DisplayName = "Sprint Acceleration"))
	float SprintAccel;

	/** Max dotproduct of wall surface sprinter ran into that doesn't stop sprint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Autosprint")
	float SprintMaxWallNormal;

	/** World time when sprinting can start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Sprint Start Time"))
	float SprintStartTime;

	/** True when sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Autosprint", meta = (DisplayName = "Is Sprinting"))
	bool bIsSprinting;

	/** Reset sprint start if braking */
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

	virtual float GetMaxAcceleration() const override;

	virtual bool CanSprint() const;

	virtual float GetMaxSpeed() const override;

	/** If true, the player is against the wall and WallSlideNormal will describe the touch */
	UPROPERTY(BlueprintReadOnly, Category = "Wall Slide")
	bool bIsAgainstWall;

	/** Used to gate client-side checking whether other characters are falling against a wall */
	UPROPERTY()
	float LastCheckedAgainstWall;

	/** Normal of the wall we are sliding against */
	UPROPERTY(BlueprintReadOnly, Category = "Wall Slide")
	FVector WallSlideNormal;
	

	// Networking
	virtual void SendClientAdjustment() override;

	virtual void SmoothClientPosition(float DeltaTime) override;

	virtual void SimulateMovement(float DeltaTime) override;
	virtual void SimulateMovement_Internal(float DeltaTime);

    /** Used for remote client simulation */
	UPROPERTY()
	FVector SimulatedVelocity;

    /** Time server is using for this move, from TimeStamp passed by client */
	UPROPERTY()
	float CurrentServerMoveTime;

    /** Return world time on client, CurrentClientTimeStamp on server */
    virtual float GetCurrentMovementTime() const;
    
    /** Return synchronized time (timestamp currently being used on server, timestamp being sent on client) */
    virtual float GetCurrentSynchTime() const;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode) override;

    /** Accumulated TimeStamp error */
	float TotalTimeStampError;

    /** True if currently clearing potential speed hack */
	bool bClearingSpeedHack;

	virtual void StopActiveMovement() override;
};

/**
 * Networking support
 */
class NZGAME_API FSavedMove_NZCharacter : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	FSavedMove_NZCharacter()
	{
		AccelMagThreshold = 2000.f;
		AccelDotThreshold = 0.8f;
		bShotSpawned = false;
		bSavedIsSprinting = false;
		SavedSprintStartTime = 0.f;
	}

	/** True if projectile/hitscan spawned this frame, not from firing press/release */
	bool bShotSpawned;

	bool bSavedIsSprinting;

	float SavedSprintStartTime;

	/** Return true if rotation affects this moves implementation */
	virtual bool NeedsRotationSent() const;

	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InPawn, float MaxDelta) const override;
	virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const override;
	virtual void PostUpdate(ACharacter* C, EPostUpdateMode PostUpdateMode) override;
	virtual void PrepMoveFor(ACharacter* C) override;
};

class NZGAME_API FNetworkPredictionData_Client_NZCharacter : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_NZCharacter(const UCharacterMovementComponent& ClientMovement)
		: FNetworkPredictionData_Client_Character(ClientMovement)
	{}

	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class */
	virtual FSavedMovePtr AllocateNewMove() override;
};
