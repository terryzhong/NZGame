// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Navigation/RecastNavMesh.h"
#include "NZRecastNavMesh.generated.h"

/**
 * 
 */
UCLASS()
class NZGAME_API ANZRecastNavMesh : public ARecastNavMesh
{
	GENERATED_BODY()
	
public:
    
    virtual void AddToNavigation(AActor* NewPOI);
	
	
};

inline ANZRecastNavMesh* GetNZNavData(UWorld* World)
{
    if (World->GetNavigationSystem() == NULL)
    {
        World->SetNavigationSystem(NewObject<UNavigationSystem>(World, GEngine->NavigationSystemClass));
    }
    return Cast<ANZRecastNavMesh>(World->GetNavigationSystem()->GetMainNavData(FNavigationSystem::ECreateIfEmpty::DontCreate));
}
