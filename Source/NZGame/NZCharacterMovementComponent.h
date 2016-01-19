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
	
    
    /** Return world time on client, CurrentClientTimeStamp on server */
    virtual float GetCurrentMovementTime() const;
    
    /** Return synchronized time (timestamp currently being used on server, timestamp being sent on client) */
    virtual float GetCurrentSynchTime() const;
};
