// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameViewportClient.h"




FVector UNZGameViewportClient::PaniniProjectLocation(const FSceneView* SceneView, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat) const
{
    return FVector(0.f);
}

FVector UNZGameViewportClient::PaniniProjectLocationForPlayer(ULocalPlayer* Player, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat) const
{
    return FVector(0.f);
}
