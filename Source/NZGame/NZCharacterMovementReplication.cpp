#include "NZGame.h"
#include "NZCharacterMovementComponent.h"
#include "NZCharacter.h"
#include "NZPlayerState.h"
#include "GameFramework/GameNetworkManager.h"


bool UNZCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	if (!HasValidData())
	{
		return false;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();

	if (!ClientData->bUpdatePosition)
	{
		return false;
	}

	// todo:

	if (ClientData->SavedMoves.Num() > 0)
	{
		bIsSettingUpFirstReplayMove = true;
		const FSavedMovePtr& FirstMove = ClientData->SavedMoves[0];
		FirstMove->PrepMoveFor(CharacterOwner);
		bIsSettingUpFirstReplayMove = false;
	}

	bool bResult = Super::ClientUpdatePositionAfterServerUpdate();

	// todo:

	return bResult;
}

void UNZCharacterMovementComponent::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	check(CharacterOwner != NULL);

	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC && PC->AcknowledgedPawn != CharacterOwner)
	{
		return;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	if (!ClientData)
	{
		return;
	}

	FSavedMovePtr NewMove = ClientData->CreateSavedMove();
	if (NewMove.IsValid() == false)
	{
		return;
	}

	NewMove->SetMoveFor(CharacterOwner, DeltaTime, NewAcceleration, *ClientData);
	NewMove->SetInitialPosition(CharacterOwner);

	Acceleration = ScaleInputAcceleration(NewMove->Acceleration);
	CharacterOwner->ClientRootMotionParams.Clear();
	PerformMovement(NewMove->DeltaTime);

	ANZCharacter* NZCharacterOwner = Cast<ANZCharacter>(CharacterOwner);
	bShotSpawned = (NZCharacterOwner && NZCharacterOwner->GetWeapon()) ? NZCharacterOwner->GetWeapon()->WillSpawnShot(DeltaTime) : false;
	NewMove->PostUpdate(CharacterOwner, FSavedMove_Character::PostUpdate_Record);

	ClientData->SavedMoves.Push(NewMove);

	NZCallServerMove();
}

void UNZCharacterMovementComponent::ClientAckGoodMove_Implementation(float TimeStamp)
{
	if (!HasValidData())
	{
		return;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	check(ClientData);

	ANZPlayerState* NZPS = Cast<ANZPlayerState>(CharacterOwner->PlayerState);
	if (NZPS)
	{
		NZPS->CalculatePing(GetCurrentMovementTime() - TimeStamp);
	}

	// Ack move if it has not expired
	int32 MoveIndex = ClientData->GetSavedMoveIndex(TimeStamp);

	// It's legit to sometimes have moves be already gone (after client adjustment called)
	if (MoveIndex == INDEX_NONE)
	{
		return;
	}
	ClientData->AckMove(MoveIndex);
}

bool UNZCharacterMovementComponent::CanDelaySendingMove(const FSavedMovePtr& NewMove)
{
	// Don't delay if just spawned shot or other
	return !NewMove.IsValid() || !((FSavedMove_NZCharacter*)(NewMove.Get()))->NeedsRotationSent();
}

void UNZCharacterMovementComponent::NZServerMoveHandleClientError(float TimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLoc, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	// Don't prevent more recent updates from being sent if received this frame.
	// We're going to send out an update anyway, might as well be the most recent one.
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if ((ServerData->LastUpdateTime != GetWorld()->TimeSeconds) && GetDefault<AGameNetworkManager>()->WithinUpdateDelayBounds(PC, ServerData->LastUpdateTime))
	{
		return;
	}

	// Offset may be relative to base component
	FVector ClientLoc = RelativeClientLoc;
	if (MovementBaseUtility::UseRelativeLocation(ClientMovementBase))
	{
		FVector BaseLocation;
		FQuat BaseRotation;
		MovementBaseUtility::GetMovementBaseTransform(ClientMovementBase, ClientBaseBoneName, BaseLocation, BaseRotation);
		ClientLoc += BaseLocation;
	}

	// Compute the client error from the server's position
	const FVector LocDiff = CharacterOwner->GetActorLocation() - ClientLoc;
	const uint8 CurrentPackedMovementMode = PackNetworkMovementMode();
	bool bMovementModeDiffers = (CurrentPackedMovementMode != ClientMovementMode);
	if (bMovementModeDiffers)
	{
		// Don't differentiate between falling and walking for this check
		// TODO: FIXME - Maybe make sure the transition isn't more than 1 frame off?
		if (CurrentPackedMovementMode == 2)
		{
			bMovementModeDiffers = (ClientMovementMode != 1);
		}
		else if (CurrentPackedMovementMode == 1)
		{
			bMovementModeDiffers = (ClientMovementMode != 2);
		}
	}

	// If client has accumulated a noticeable positional error, correct him.
	if (ExceedsAllowablePositionError(LocDiff) || bMovementModeDiffers)
	{
		UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
		ServerData->PendingAdjustment.NewVel = Velocity;
		ServerData->PendingAdjustment.NewRot = CharacterOwner->GetActorRotation();

		ServerData->PendingAdjustment.bBaseRelativePosition = MovementBaseUtility::UseRelativeLocation(MovementBase);
		if (ServerData->PendingAdjustment.bBaseRelativePosition)
		{
			// Relative location
			ServerData->PendingAdjustment.NewLoc = CharacterOwner->GetBasedMovement().Location;
			ServerData->PendingAdjustment.NewBase = MovementBase;
			ServerData->PendingAdjustment.NewBaseBoneName = CharacterOwner->GetBasedMovement().BoneName;
		}
		else
		{
			ServerData->PendingAdjustment.NewLoc = CharacterOwner->GetActorLocation();
			ServerData->PendingAdjustment.NewBase = NULL;
			ServerData->PendingAdjustment.NewBaseBoneName = NAME_None;
		}

		// TODO: FIXME - Configurable property controlled
		bLargeCorrection = (LocDiff.Size() > LargeCorrectionThreshold);

		ServerData->LastUpdateTime = GetWorld()->TimeSeconds;
		ServerData->PendingAdjustment.DeltaTime = DeltaTime;
		ServerData->PendingAdjustment.TimeStamp = TimeStamp;
		ServerData->PendingAdjustment.bAckGoodMove = false;
		ServerData->PendingAdjustment.MovementMode = CurrentPackedMovementMode;
	}
	else
	{
		// Acknowledge receipt of this successful ServerMove()
		ServerData->PendingAdjustment.TimeStamp = TimeStamp;
		ServerData->PendingAdjustment.bAckGoodMove = true;
	}
}

void UNZCharacterMovementComponent::NZCallServerMove()
{
	ANZCharacter* NZCharacterOwner = Cast<ANZCharacter>(CharacterOwner);
	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	if (!NZCharacterOwner || !ClientData || (ClientData->SavedMoves.Num() == 0))
	{
		return;
	}
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());

	// Decide whether to hold off on move
	// never delay if spawning shot must sync
	const FSavedMovePtr& NewMove = ClientData->SavedMoves.Last();
	if (CanDelaySendingMove(NewMove))
	{
		UPlayer* Player = (PC ? PC->Player : NULL);
		int32 CurrentNetSpeed = Player ? Player->CurrentNetSpeed : 0;
		// TODO:FIXME - base acceptable netmovedelta on CurrentNetSpeed
		float NetMoveDelta = NewMove->IsImportantMove(ClientData->LastAckedMove) ? 0.017f : 0.033f;

		if (((NewMove->TimeStamp - ClientData->ClientUpdateTime) * CharacterOwner->GetWorldSettings()->GetEffectiveTimeDilation() < NetMoveDelta))
		{
			return;
		}
	}

	// Find the oldest unacknowledged sent important move (OldMove).
	// Don't include the last move because it may be combined with the next new move.
	// A saved move is interesting if it differs significantly from the last acknowledged move.
	FSavedMovePtr OldMovePtr = NULL;
	if (ClientData->LastAckedMove.IsValid())
	{
		for (int32 i = 0; i < ClientData->SavedMoves.Num() - 1; i++)
		{
			const FSavedMovePtr& CurrentMove = ClientData->SavedMoves[i];
			if (CurrentMove->TimeStamp > ClientData->ClientUpdateTime)
			{
				// Move hasn't been sent yet
				break;
			}
			else if (CurrentMove->IsImportantMove(ClientData->LastAckedMove))
			{
				OldMovePtr = CurrentMove;
			}
		}
	}

	// Send old move if it exists
	if (OldMovePtr.IsValid())
	{
		const FSavedMove_Character* OldMove = OldMovePtr.Get();
		NZCharacterOwner->NZServerMoveOld(OldMove->TimeStamp, OldMove->Acceleration, OldMove->SavedControlRotation.Yaw, OldMove->GetCompressedFlags());
	}

	for (int32 i = 0; i < ClientData->SavedMoves.Num() - 1; i++)
	{
		const FSavedMovePtr& MoveToSend = ClientData->SavedMoves[i];
		if (MoveToSend.IsValid() && (MoveToSend->TimeStamp > ClientData->ClientUpdateTime))
		{
			ClientData->ClientUpdateTime = MoveToSend->TimeStamp;
			if (((FSavedMove_NZCharacter*)(MoveToSend.Get()))->NeedsRotationSent())
			{
				NZCharacterOwner->NZServerMoveSaved(MoveToSend->TimeStamp, MoveToSend->Acceleration, MoveToSend->GetCompressedFlags(), MoveToSend->SavedControlRotation.Yaw, MoveToSend->SavedControlRotation.Pitch);
			}
			else
			{
				NZCharacterOwner->NZServerMoveQuick(MoveToSend->TimeStamp, MoveToSend->Acceleration, MoveToSend->GetCompressedFlags());
			}
		}
	}

	if (NewMove.IsValid() && (NewMove->TimeStamp > ClientData->ClientUpdateTime))
	{
		// Determine if we send absolute or relative location
		UPrimitiveComponent* ClientMovementBase = NewMove->EndBase.Get();
		bool bUseRelativeLocation = MovementBaseUtility::UseRelativeLocation(ClientMovementBase);
		const FVector SendLocation = bUseRelativeLocation ? NewMove->SavedRelativeLocation : NewMove->SavedLocation;
		if (!bUseRelativeLocation)
		{
			// Location isn't relative, don't need to replicate base
			ClientMovementBase = NULL;
			NewMove->EndBoneName = NAME_None;
		}

		ClientData->ClientUpdateTime = NewMove->TimeStamp;
		NZCharacterOwner->NZServerMove(
			NewMove->TimeStamp,
			NewMove->Acceleration,
			SendLocation,
			NewMove->GetCompressedFlags(),
			NewMove->SavedControlRotation.Yaw,
			NewMove->SavedControlRotation.Pitch,
			ClientMovementBase,
			NewMove->EndBoneName,
			NewMove->MovementMode
			);
	}

	APlayerCameraManager* PlayerCameraManager = (PC ? PC->PlayerCameraManager : NULL);
	if (PlayerCameraManager != NULL && PlayerCameraManager->bUseClientSideCameraUpdates)
	{
		//UE_LOG(NZ, Warning, TEXT("WTF WTF WTF WTF!!!!!!!!!!!!!!!!"));
		PlayerCameraManager->bShouldSendClientSideCameraUpdate = true;
	}
}

bool UNZCharacterMovementComponent::ExceedsAllowablePositionError(FVector LocDiff) const
{
	return (LocDiff | LocDiff) > MaxPositionErrorSquared;
}

void UNZCharacterMovementComponent::ProcessServerMove(float TimeStamp, FVector InAccel, FVector ClientLoc, uint8 MoveFlags, float ViewYaw, float ViewPitch, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	if (!HasValidData() || !IsComponentTickEnabled())
	{
		return;
	}

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!NZVerifyClientTimeStamp(TimeStamp, *ServerData))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	bool bServerReadyForClient = PC ? PC->NotifyServerReceivedClientData(CharacterOwner, TimeStamp) : true;
	const FVector Accel = bServerReadyForClient ? InAccel : FVector::ZeroVector;

	// Save move parameters
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(TimeStamp) * CharacterOwner->CustomTimeDilation;
	ServerData->CurrentClientTimeStamp = TimeStamp;
	ServerData->ServerTimeStamp = GetWorld()->GetTimeSeconds();
	if (PC)
	{
		FRotator ViewRot;
		ViewRot.Pitch = ViewPitch;
		ViewRot.Yaw = ViewYaw;
		ViewRot.Roll = 0.f;
		PC->SetControlRotation(ViewRot);
	}

	if (!bServerReadyForClient)
	{
		return;
	}

	// Perform actual movement
	if ((CharacterOwner->GetWorldSettings()->Pauser == NULL) && (DeltaTime > 0.f))
	{
		if (PC)
		{
			PC->UpdateRotation(DeltaTime);
		}
		MoveAutonomous(TimeStamp, DeltaTime, MoveFlags, Accel);

		FVector CurrLoc = CharacterOwner->GetActorLocation();
		if (InAccel.IsZero() && (MovementMode == MOVE_Walking) && (ClientLoc.X == CurrLoc.X) && (ClientLoc.Y == CurrLoc.Y) && (FMath::Abs(CurrLoc.Z - ClientLoc.Z) < 0.1f))
		{
			CharacterOwner->SetActorLocation(ClientLoc);
		}
	}

	UE_LOG(LogNetPlayerMovement, Verbose, TEXT("ServerMove Time %f Acceleration %s Position %s DeltaTime %f"),
		TimeStamp, *Accel.ToString(), *CharacterOwner->GetActorLocation().ToString(), DeltaTime);

	NZServerMoveHandleClientError(TimeStamp, DeltaTime, Accel, ClientLoc, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

void UNZCharacterMovementComponent::ProcessQuickServerMove(float TimeStamp, FVector InAccel, uint8 MoveFlags)
{
	if (!HasValidData() || !IsComponentTickEnabled())
	{
		return;
	}

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!NZVerifyClientTimeStamp(TimeStamp, *ServerData))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	bool bServerReadyForClient = PC ? PC->NotifyServerReceivedClientData(CharacterOwner, TimeStamp) : true;
	const FVector Accel = bServerReadyForClient ? InAccel : FVector::ZeroVector;

	// Save move parameters.
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(TimeStamp) * CharacterOwner->CustomTimeDilation;
	ServerData->CurrentClientTimeStamp = TimeStamp;
	ServerData->ServerTimeStamp = GetWorld()->GetTimeSeconds();

	if (!bServerReadyForClient)
	{
		return;
	}

	// Perform actual movement
	if ((CharacterOwner->GetWorldSettings()->Pauser == NULL) && (DeltaTime > 0.f))
	{
		MoveAutonomous(TimeStamp, DeltaTime, MoveFlags, Accel);
	}

	UE_LOG(LogNetPlayerMovement, Verbose, TEXT("QuickServerMove Time %f Acceleration %s Position %s DeltaTime %f"),
		TimeStamp, *Accel.ToString(), *CharacterOwner->GetActorLocation().ToString(), DeltaTime);
}

void UNZCharacterMovementComponent::ProcessSavedServerMove(float TimeStamp, FVector InAccel, uint8 MoveFlags, float ViewYaw, float ViewPitch)
{
	if (!HasValidData() || !IsComponentTickEnabled())
	{
		return;
	}

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!NZVerifyClientTimeStamp(TimeStamp, *ServerData))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	bool bServerReadyForClient = PC ? PC->NotifyServerReceivedClientData(CharacterOwner, TimeStamp) : true;
	const FVector Accel = bServerReadyForClient ? InAccel : FVector::ZeroVector;

	// Save move parameters.
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(TimeStamp) * CharacterOwner->CustomTimeDilation;
	ServerData->CurrentClientTimeStamp = TimeStamp;
	ServerData->ServerTimeStamp = GetWorld()->GetTimeSeconds();

	if (PC)
	{
		FRotator ViewRot;
		ViewRot.Pitch = ViewPitch;
		ViewRot.Yaw = ViewYaw;
		ViewRot.Roll = 0.f;
		PC->SetControlRotation(ViewRot);
	}

	if (!bServerReadyForClient)
	{
		return;
	}

	// Perform actual movement
	if ((CharacterOwner->GetWorldSettings()->Pauser == NULL) && (DeltaTime > 0.f))
	{
		if (PC)
		{
			PC->UpdateRotation(DeltaTime);
		}
		MoveAutonomous(TimeStamp, DeltaTime, MoveFlags, Accel);
	}

	UE_LOG(LogNetPlayerMovement, Verbose, TEXT("QuickServerMove Time %f Acceleration %s Position %s DeltaTime %f"),
		TimeStamp, *Accel.ToString(), *CharacterOwner->GetActorLocation().ToString(), DeltaTime);
}

void UNZCharacterMovementComponent::ProcessOldServerMove(float OldTimeStamp, FVector OldAccel, float OldYaw, uint8 OldMoveFlags)
{
	if (!HasValidData() || !IsComponentTickEnabled())
	{
		return;
	}

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!NZVerifyClientTimeStamp(OldTimeStamp, *ServerData))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC)
	{
		FRotator ViewRot;
		ViewRot.Pitch = PC->GetControlRotation().Pitch;
		ViewRot.Yaw = OldYaw;
		ViewRot.Roll = 0.f;
		PC->SetControlRotation(ViewRot);
	}

	const float MaxResponseTime = ServerData->MaxResponseTime * CharacterOwner->GetWorldSettings()->GetEffectiveTimeDilation();

	MoveAutonomous(OldTimeStamp, FMath::Min(OldTimeStamp - ServerData->CurrentClientTimeStamp, MaxResponseTime), OldMoveFlags, OldAccel);
	ServerData->CurrentClientTimeStamp = OldTimeStamp;
}

void UNZCharacterMovementComponent::NeedsClientAdjustment()
{
	LastClientAdjustmentTime = -1.f;
}

void UNZCharacterMovementComponent::ResetPredictionData_Client()
{
	Super::ResetPredictionData_Client();
	ResetTimers();
}

void UNZCharacterMovementComponent::ClientNoBaseAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVelocity, uint8 ServerMovementMode)
{
	if (!HasValidData() || !IsComponentTickEnabled())
	{
		return;
	}
	if (MovementBaseUtility::UseRelativeLocation(CharacterOwner->GetMovementBase()))
	{
		// Need to change to no base, cause server doesn't think I'm based on something relative
		SetBase(NULL);
	}
	ClientAdjustPosition_Implementation(TimeStamp, NewLoc, NewVelocity, CharacterOwner->GetMovementBase(), NAME_None, (CharacterOwner->GetMovementBase() != NULL), false, ServerMovementMode);
}

void UNZCharacterMovementComponent::SetReplicatedAcceleration(FRotator MovementRotation, uint8 CompressedAccel)
{
	MovementRotation.Pitch = 0.f;
	FVector CurrentDir = MovementRotation.Vector();
	FVector SideDir = (CurrentDir ^ FVector(0.f, 0.f, 1.f)).GetSafeNormal();

	FVector AccelDir(0.f);
	if (CompressedAccel & 1)
	{
		AccelDir += CurrentDir;
	}
	else if (CompressedAccel & 2)
	{
		AccelDir -= CurrentDir;
	}
	if (CompressedAccel & 4)
	{
		AccelDir += SideDir;
	}
	else if (CompressedAccel & 8)
	{
		AccelDir -= SideDir;
	}
	bIsSprinting = ((MovementMode == MOVE_Walking) && (Velocity.SizeSquared() > FMath::Square<float>(MaxWalkSpeed)));
	Acceleration = GetMaxAcceleration() * AccelDir.GetSafeNormal();
}

void UNZCharacterMovementComponent::DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{

}

FVector UNZCharacterMovementComponent::GetImpartedMovementBaseVelocity() const
{
	FVector Result = Super::GetImpartedMovementBaseVelocity();

	if (!Result.IsZero())
	{
		// Clamp total velocity to GroundSpeed + JumpZ + Imparted total, TODO: Separate XY and Z
		float XYSpeed = ((Result.X == 0.f) && (Result.Y == 0.f)) ? 0.f : Result.Size2D();
		float MaxSpeedSq = FMath::Square(MaxWalkSpeed + Result.Size2D()) + FMath::Square(JumpZVelocity + Result.Z);
		if ((Velocity + Result).SizeSquared() > MaxSpeedSq)
		{
			Result = (Velocity + Result).GetSafeNormal() * FMath::Sqrt(MaxSpeedSq) - Velocity;
		}
		Result.Z = FMath::Max(Result.Z, 0.f);
	}

	return Result;
}

bool UNZCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return CanEverCrouch() && IsMovingOnGround();
}

void UNZCharacterMovementComponent::PerformMovement(float DeltaSeconds)
{
	if (!CharacterOwner)
	{
		return;
	}

	ANZCharacter* NZOwner = Cast<ANZCharacter>(CharacterOwner);
	if (!NZOwner || !NZOwner->IsRagdoll())
	{
		float RealGroundFriction = GroundFriction;
		if (Acceleration.IsZero())
		{
			GroundFriction = BrakingFriction;
		}
		
		bool bSavedWantsToCrouch = bWantsToCrouch;
		bForceMaxAccel = false;

		FVector StartMoveLoc = GetActorLocation();
		Super::PerformMovement(DeltaSeconds);
		UpdateMovementStats(StartMoveLoc);

		bWantsToCrouch = bSavedWantsToCrouch;
		GroundFriction = RealGroundFriction;
		BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	}

	if (NZOwner != NULL)
	{
		NZOwner->PositionUpdated(bShotSpawned);
		bShotSpawned = false;
		// Tick movement reduction timer
		NZOwner->WalkMovementReductionTime = FMath::Max(0.f, NZOwner->WalkMovementReductionTime - DeltaSeconds);
		if (NZOwner->WalkMovementReductionTime <= 0.f)
		{
			NZOwner->WalkMovementReductionPct = 0.f;
		}
	}
}






bool FSavedMove_NZCharacter::NeedsRotationSent() const
{
	return (bShotSpawned);
}

void FSavedMove_NZCharacter::Clear()
{
	Super::Clear();

	bShotSpawned = false;
	bSavedIsSprinting = false;
	SavedSprintStartTime = 0.f;
}

void FSavedMove_NZCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UNZCharacterMovementComponent* NZCharMov = Cast<UNZCharacterMovementComponent>(Character->GetCharacterMovement());
	if (NZCharMov)
	{
		bSavedIsSprinting = NZCharMov->bIsSprinting;
		SavedSprintStartTime = NZCharMov->SprintStartTime;
	}

	Acceleration.X = FMath::RoundToFloat(Acceleration.X);
	Acceleration.Y = FMath::RoundToFloat(Acceleration.Y);
	Acceleration.Z = FMath::RoundToFloat(Acceleration.Z);
}

uint8 FSavedMove_NZCharacter::GetCompressedFlags() const
{
	uint8 Result = 0;
	if (bPressedJump) { Result |= 1; }
	if (bWantsToCrouch) { Result |= 2; }
	if (bSavedIsSprinting) { Result |= (5 << 2); }
	return Result;
}

bool FSavedMove_NZCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (bSavedIsSprinting != ((FSavedMove_NZCharacter*)&NewMove)->bSavedIsSprinting)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

bool FSavedMove_NZCharacter::IsImportantMove(const FSavedMovePtr& ComparedMove) const
{
	if (bShotSpawned)
	{
		return true;
	}

	if (!ComparedMove.IsValid())
	{
		// If no previous move to compare, always send impulses
		return bPressedJump;
	}

	// Check if any important movement flags have changed status
	if ((bPressedJump && (bPressedJump != ComparedMove->bPressedJump)) || (bWantsToCrouch != ComparedMove->bWantsToCrouch))
	{
		return true;
	}

	if (MovementMode != ComparedMove->MovementMode)
	{
		return true;
	}

	// Check if acceleration has changed significantly
	if (Acceleration != ComparedMove->Acceleration)
	{
		// Compare magnitude and orientation
		if ((FMath::Abs(AccelMag - ComparedMove->AccelMag) > AccelMagThreshold) || ((AccelNormal | ComparedMove->AccelNormal) < AccelDotThreshold))
		{
			return true;
		}
	}

	return false;
}

void FSavedMove_NZCharacter::PostUpdate(ACharacter* Character, EPostUpdateMode PostUpdateMode)
{
	// Set flag if weapon is shooting on client this frame not from new fire press/release (to keep client and server synchronized)
	UNZCharacterMovementComponent* NZCharMov = Character ? Cast<UNZCharacterMovementComponent>(Character->GetCharacterMovement()) : NULL;
	bShotSpawned = NZCharMov->bShotSpawned;
	MovementMode = NZCharMov->PackNetworkMovementMode();
	Super::PostUpdate(Character, PostUpdateMode);
}

void FSavedMove_NZCharacter::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UNZCharacterMovementComponent* NZCharMov = Cast<UNZCharacterMovementComponent>(Character->GetCharacterMovement());
	if (NZCharMov)
	{
		if (NZCharMov->bIsSettingUpFirstReplayMove)
		{
			NZCharMov->SprintStartTime = SavedSprintStartTime;
		}
		else
		{
			SavedSprintStartTime = NZCharMov->SprintStartTime;
		}
	}
}

FSavedMovePtr FNetworkPredictionData_Client_NZCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_NZCharacter());
}
