// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, NZGame, "NZGame" );




#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleModuleRequired.h"
#include "particles/ParticleLODLevel.h"
bool IsLoopingParticleSystem(const UParticleSystem* PSys)
{
    for (int32 i = 0; i < PSys->Emitters.Num(); i++)
    {
        if (PSys->Emitters[i]->GetLODLevel(0)->RequiredModule->EmitterLoops <= 0 &&
            PSys->Emitters[i]->GetLODLevel(0)->RequiredModule->bEnabled)
        {
            return true;
        }
    }
    return false;
}

