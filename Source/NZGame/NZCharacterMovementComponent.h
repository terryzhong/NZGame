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

	
	/** Return true if movement input should not be constrained to horizontal plane */
	virtual bool Is3DMovementMode() const;
	
};
