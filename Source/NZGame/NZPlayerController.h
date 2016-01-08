// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZBasePlayerController.h"
#include "NZPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZPlayerController : public ANZBasePlayerController
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class ANZCharacter* NZCharacter;
	
public:
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	virtual ANZCharacter* GetNZCharacter();

	UFUNCTION(exec)
	virtual void SwitchToBestWeapon();


};
