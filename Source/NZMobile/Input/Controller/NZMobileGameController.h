// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "NZMobileGameView.h"
#include "NZMobileGameController.generated.h"

/**
 * 
 */
UCLASS()
class NZMOBILE_API UNZMobileGameController : public UObject
{
	GENERATED_BODY()
	
public:
    UNZMobileGameController()
    {
        if (ViewClass)
        {
            View = NewObject<UNZMobileGameView>(this, ViewClass);
        }
    }
    
    class UNZMobileGameView* GetView() { return View; }
    
protected:
    TSubclassOf<class UNZMobileGameView> ViewClass;
    
    UNZMobileGameView* View;
};
