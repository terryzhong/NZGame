// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZCarriedObject.generated.h"

UCLASS()
class NZGAME_API ANZCarriedObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANZCarriedObject();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
    /** Holds the NZPlayerState of the person currently holding this object */
	UPROPERTY(BlueprintReadOnly, Category = GameObject)
    class ANZPlayerState* Holder;
    
    /** Holds the NZPlayerState of the last person to hold this object */
    UPROPERTY(BlueprintReadOnly, Category = GameObject)
    class ANZPlayerState* LastHolder;
    
    
    /** 75
     Holds the team that this object belongs to */
    UPROPERTY(Replicated, BlueprintReadOnly, Category = GameObject)
    class ANZTeamInfo* Team;
    
    /** 138
     Returns the team number of the team that owns this object */
    UFUNCTION()
    virtual uint8 GetTeamNum() const;
};
