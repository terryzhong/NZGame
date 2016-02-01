// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



namespace InventoryEventName
{
    const FName Landed = FName(TEXT("Landed"));
    const FName LandedWater = FName(TEXT("LandedWater"));
    const FName FiredWeapon = FName(TEXT("FiredWeapon"));
    const FName Jump = FName(TEXT("Jump"));
    const FName MultiJump = FName(TEXT("MultiJump"));
    const FName Dodge = FName(TEXT("Dodge"));
}


/**
 * 
 */
class NZGAME_API NZTypes
{
public:
	NZTypes();
	~NZTypes();
};
