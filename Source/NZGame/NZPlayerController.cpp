// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZPlayerController.h"
#include "NZWeapon.h"


ANZCharacter* ANZPlayerController::GetNZCharacter()
{
	return NZCharacter;
}

void ANZPlayerController::SwitchToBestWeapon()
{
    if (NZCharacter != NULL && IsLocalPlayerController())
    {
        ANZWeapon* BestWeapon = NULL;
        float BestPriority = 0.0f;
        for (TInventoryIterator<ANZWeapon> It(NZCharacter); It; ++It)
        {
            if (It->HasAnyAmmo())
            {
                float TestPriority = It->AutoSwitchPriority;
                if (TestPriority > BestPriority)
                {
                    BestWeapon = *It;
                    BestPriority = TestPriority;
                }
            }
        }
        NZCharacter->SwitchWeapon(BestWeapon);
    }
}
