// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZWeapon_Sniper.h"


const FName NAME_SniperKills(TEXT("SniperKills"));
const FName NAME_SniperHeadshotKills(TEXT("SniperHeadshotKills"));
const FName NAME_SniperDeaths(TEXT("SniperDeaths"));
const FName NAME_SniperHeadshotDeaths(TEXT("SniperHeadshotDeaths"));
const FName NAME_SniperHits(TEXT("SniperHits"));
const FName NAME_SniperShots(TEXT("SniperShots"));

ANZWeapon_Sniper::ANZWeapon_Sniper()
{
	BringUpTime = 0.45f;
	PutDownTime = 0.4;

	KillStatsName = NAME_SniperKills;
	AltKillStatsName = NAME_SniperHeadshotKills;
	DeathStatsName = NAME_SniperDeaths;
	AltDeathStatsName = NAME_SniperHeadshotDeaths;
	HitsStatsName = NAME_SniperHits;
	ShotsStatsName = NAME_SniperShots;
}
