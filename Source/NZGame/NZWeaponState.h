// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZWeapon.h"
#include "NZWeaponState.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, CustomConstructor, Within = NZWeapon)
class NZGAME_API UNZWeaponState : public UObject
{
	GENERATED_BODY()
	
public:
    // Accessor for convenience
    inline class ANZCharacter* GetNZOwner()
    {
        return GetOuterANZWeapon()->GetNZOwner();
    }
	
	virtual UWorld* GetWorld() const override
    {
        return GetOuterANZWeapon()->GetWorld();
    }
    
    virtual void BeginState(const UNZWeaponState* PrevState) {}
    virtual void EndState() {}
    
};
