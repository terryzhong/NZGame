// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "NZATypes.h"

#define COLLISION_PROJECTILE    ECC_GameTraceChannel1
#define COLLISION_TRACE_WEAPON  ECC_GameTraceChannel2
#define COLLISION_PROJECTILE_SHOOTABLE  ECC_GameTraceChannel3
#define COLLISION_TELEPORTING_OBJECT    ECC_GameTraceChannel4
#define COLLISION_PAWNOVERLAP   ECC_GameTraceChannel5
#define COLLISION_TRACE_WEAPONNOCHARACTER   ECC_GameTraceChannel6
#define COLLISION_TRANSDISK     ECC_GameTraceChannel7


/** Utility to find out if a particle system loops */
extern bool IsLoopingParticleSystem(const UParticleSystem* PSys);

/** Utility to retrieve the highest priority physics volume overlapping the passed in primitive */
extern APhysicsVolume* FindPhysicsVolume(UWorld* World, const FVector& TestLoc, const FCollisionShape& Shape);

/**  */
extern float GetAnimLengthForScaling(UAnimMontage* WeaponAnim, UAnimMontage* HandsAnim);
