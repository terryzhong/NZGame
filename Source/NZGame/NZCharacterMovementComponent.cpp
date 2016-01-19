// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZCharacterMovementComponent.h"



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
