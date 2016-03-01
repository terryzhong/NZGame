// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "NZWeaponViewKickComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NZGAME_API UNZWeaponViewKickComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    virtual void BeginFiringSequence(uint8 FireModeNum, bool bClientFired) {}
    
    virtual void EndFiringSequence(uint8 FireModeNum) {}

    virtual void FireShot() {}

    virtual void KickBackTheView() {}

    virtual void CalcCamera(float DeltaTime, FVector& OutCamLoc, FRotator& OutCamRot) {}
    
    virtual bool DrawCrosshair_Normal(class UNZCrosshair* Crosshair, UCanvas* Canvas, float DeltaTime, float Scale, FLinearColor Color)
    {
        return false;
    }
	
};
