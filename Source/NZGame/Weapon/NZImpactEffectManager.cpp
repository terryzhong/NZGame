// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZImpactEffectManager.h"



TSubclassOf<ANZImpactEffect> UNZImpactEffectManager::GetImpactEffectFromPhysicalMaterial(UPhysicalMaterial* InPhysicalMaterial)
{
    if (InPhysicalMaterial != NULL)
    {
        for (int32 Index = 0; Index < ImpactEffectBindList.Num(); Index++)
        {
            if (ImpactEffectBindList[Index].PhysicalMaterial == InPhysicalMaterial)
            {
                return ImpactEffectBindList[Index].ImpactEffect;
            }
        }
    }
    
    if (ImpactEffectBindList.Num() > 0)
    {
        return ImpactEffectBindList[0].ImpactEffect;
    }
    
    return NULL;
}

