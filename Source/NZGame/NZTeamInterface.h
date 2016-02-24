// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NZTeamInterface.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI)
class UNZTeamInterface : public UInterface
{
	GENERATED_BODY()
};

class NZGAME_API INZTeamInterface
{
	GENERATED_BODY()

public:
	/** If this returns true then the Actor is considered to be on all teams simultaneously (OnSameTeam() will always return true) */
	virtual bool IsFriendlyToAll() const
	{
		return false;
	}

	/** Version of GetTeamNum() for blueprints */
	UFUNCTION(BlueprintImplementableEvent, meta = (DiaplayName = "GetTeamNum"))
	uint8 ScriptGetTeamNum();

	/** Return team number the object is on, or 255 for no team */
	virtual uint8 GetTeamNum() const PURE_VIRTUAL(INZTeamInterface::GetTeamNum, return 255;);

	UFUNCTION(BlueprintNativeEvent, Category = Team)
	void SetTeamForSideSwap(uint8 NewTeamNum);
};

