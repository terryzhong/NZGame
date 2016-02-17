// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCharacterMovementComponent.h"
#include "NZCharacter.h"



UNZCharacterMovementComponent::UNZCharacterMovementComponent()
{
	MaxWalkSpeedSprinting = 800;
}

void UNZCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	ANZCharacter* NZOwner = Cast<ANZCharacter>(CharacterOwner);
	UMovementComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	bool bOwnerIsRagdoll = NZOwner && NZOwner->IsRagdoll();
	if (bOwnerIsRagdoll)
	{
		// Ignore jump key presses this frame since the character is in ragdoll and they don't apply
		NZOwner->bPressedJump = false;
		if (!NZOwner->GetController())
		{
			return;
		}
	}

	const FVector InputVector = ConsumeInputVector();
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime) || UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	if (CharacterOwner->Role > ROLE_SimulatedProxy)
	{
		if (CharacterOwner->Role == ROLE_Authority)
		{
			// Check we are still in the world, and stop simulating if not
			const bool bStillInWorld = (bCheatFlying || CharacterOwner->CheckStillInWorld());
			if (!bStillInWorld || !HasValidData())
			{
				return;
			}
		}

		// If we are a client we might have received an update from the server
		const bool bIsClient = (GetNetMode() == NM_Client && CharacterOwner->Role == ROLE_AutonomousProxy);
		if (bIsClient && !bOwnerIsRagdoll)
		{
			ClientUpdatePositionAfterServerUpdate();
		}

		// Allow root motion to move characters that have no controller
		if (CharacterOwner->IsLocallyControlled() || bRunPhysicsWithNoController || (!CharacterOwner->Controller && CharacterOwner->IsPlayingRootMotion()))
		{
			FNetworkPredictionData_Client_Character* ClientData = ((CharacterOwner->Role < ROLE_Authority) && (GetNetMode() == NM_Client)) ? GetPredictionData_Client_Character() : NULL;
			if (ClientData)
			{
				// Update our delta time for physics simulation
				DeltaTime = UpdateTimeStampAndDeltaTime(DeltaTime, ClientData);
				CurrentServerMoveTime = ClientData->CurrentTimeStamp;
			}
			else
			{
				CurrentServerMoveTime = GetWorld()->GetTimeSeconds();
			}

			// We need to check the jump state before adjusting input acceleration, 
			// to minimize latency and to make sure acceleration respects our potentially new falling state
			CharacterOwner->CheckJumpInput(DeltaTime);

			// Apply input to acceleration
			Acceleration = ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
			AnalogInputModifier = ComputeAnalogInputModifier();

			if ((CharacterOwner->Role == ROLE_Authority) && !bOwnerIsRagdoll)
			{
				PerformMovement(DeltaTime);
			}
			else if (bIsClient)
			{
				ReplicateMoveToServer(DeltaTime, Acceleration);
			}
		}
		else if ((CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy) && !bOwnerIsRagdoll)
		{
			// Server ticking for remote client
			// Between net updates from the client we need to update position if based on another object,
			// otherwise the object will move on intermediate frames and we won't follow it
			MaybeUpdateBasedMovement(DeltaTime);
			SaveBaseLocation();
		}
		else if (!CharacterOwner->Controller && (CharacterOwner->Role == ROLE_Authority) && !bOwnerIsRagdoll)
		{
			// Still update forces
			ApplyAccumulatedForces(DeltaTime);
			PerformMovement(DeltaTime);
		}
	}
	else if (!bOwnerIsRagdoll && CharacterOwner->Role == ROLE_SimulatedProxy)
	{
		AdjustProxyCapsuleSize();
		SimulatedTick(DeltaTime);
		CharacterOwner->RecalculateBaseEyeHeight();
	}

	if (bEnablePhysicsInteraction && !bOwnerIsRagdoll)
	{
		if (CurrentFloor.HitResult.IsValidBlockingHit())
		{
			// Apply downwards force when walking on top of physics objects
			if (UPrimitiveComponent* BaseComp = CurrentFloor.HitResult.GetComponent())
			{
				if (StandingDownwardForceScale != 0.f && BaseComp->IsAnySimulatingPhysics())
				{
					const float GravZ = GetGravityZ();
					const FVector ForceLocation = CurrentFloor.HitResult.ImpactPoint;
					BaseComp->AddForceAtLocation(FVector(0.f, 0.f, GravZ * Mass * StandingDownwardForceScale), ForceLocation, CurrentFloor.HitResult.BoneName);
				}
			}
		}
	}

	if (bOwnerIsRagdoll)
	{
		// Ignore jump key presses this frame since the character is in ragdoll and they don't apply
		NZOwner->bPressedJump = false;
	}
	AvgSpeed = AvgSpeed * (1.f - 2.f * DeltaTime) + 2.f * DeltaTime * Velocity.Size2D();
	if (CharacterOwner != NULL)
	{
		ANZPlayerController* PC = Cast<ANZPlayerController>(CharacterOwner->Controller);
		if (PC != NULL && PC->PlayerInput != NULL)
		{
			PC->ApplyDeferredFireInputs();
		}
	}
}




float UNZCharacterMovementComponent::GetMaxSpeed() const
{
	ANZCharacter* NZCharacter = Cast<ANZCharacter>(CharacterOwner);
	if (NZCharacter != NULL && NZCharacter->bSprinting)
	{
		return MaxWalkSpeedSprinting;
	}
	else
	{
		return Super::GetMaxSpeed();
	}
}

bool UNZCharacterMovementComponent::Is3DMovementMode() const
{
	return (MovementMode == MOVE_Flying) || (MovementMode == MOVE_Swimming);
}

float UNZCharacterMovementComponent::GetCurrentMovementTime() const
{
    return 0.f;
}

float UNZCharacterMovementComponent::GetCurrentSynchTime() const
{
    return 0.f;
}
