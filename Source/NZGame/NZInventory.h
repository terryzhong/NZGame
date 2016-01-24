// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZCharacter.h"
#include "NZInventory.generated.h"

UCLASS()
class NZGAME_API ANZInventory : public AActor
{
	GENERATED_BODY()
    
    friend bool ANZCharacter::AddInventory(ANZInventory*, bool);
    friend void ANZCharacter::RemoveInventory(ANZInventory*);
    
    template<typename> friend class TInventoryIterator;
	
public:	
	// Sets default values for this actor's properties
	ANZInventory();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:
    UPROPERTY(BlueprintReadOnly, Replicated, Category = Inventory)
    ANZInventory* NextInventory;
	
    UPROPERTY(BlueprintReadOnly, Category = Inventory)
    class ANZCharacter* NZOwner;
    
    /** Called when this inventory item has been given to the specified character */
    UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly)
    void eventGivenTo(ANZCharacter* NewOwner, bool bAutoActivate);
    virtual void GivenTo(ANZCharacter* NewOwner, bool bAutoActivate);
    
    /** Called when this inventory item has been removed from its owner */
    UFUNCTION(BlueprintImplementableEvent)
    void eventRemoved();
    virtual void Removed();
    
    /** Client side handling of owner transition */
    UFUNCTION(Client, Reliable)
    void ClientGivenTo(APawn* NewInstigator, bool bAutoActivate);
    virtual void ClientGivenTo_Internal(bool bAutoActivate);
    
    /** Called only on the client that is given the item */
    UFUNCTION(BlueprintImplementableEvent)
    void eventClientGivenTo(bool bAutoActivate);
    
    UFUNCTION(Client, Reliable)
    virtual void ClientRemoved();
    
    UFUNCTION(BlueprintImplementableEvent)
    void eventClientRemoved();
    
    FTimerHandle CheckPendingClientGivenToHandle;
    void CheckPendingClientGivenTo();
    virtual void OnRep_Instigator() override;
    
    uint32 bPendingClientGivenTo : 1;
    uint32 bPendingAutoActivate : 1;
    
    UPROPERTY(EditDefaultsOnly, Category = Pickup)
    UMeshComponent* PickupMesh;
    
public:
    /** 
     * Returns next inventory item in the chain
     * NOTE: usually you should use TInventoryIterator
     */
    ANZInventory* GetNext() const
    {
        return NextInventory;
    }
    
    ANZCharacter* GetNZOwner() const
    {
        checkSlow(NZOwner == GetOwner() || Role < ROLE_Authority); // on client RPC to assign NZOwner could be delayed
        return NZOwner;
    }
    
    virtual void DropFrom(const FVector& StartLocation, const FVector& TossVelocity);
    virtual void Destroyed() override;
    
    virtual void InitializeDroppedPickup(class ANZDroppedPickup* Pickup);
    
    /** Return a component that can be instanced to be applied to pickups */
    UFUNCTION(BlueprintNativeEvent)
    UMeshComponent* GetPickupMeshTemplate(FVector& OverrideScale) const;
    
    
    
    UFUNCTION(BlueprintNativeEvent)
    bool StackPickup(ANZInventory* ContainedInv);
};




template<typename InvType = ANZInventory> class NZGAME_API TInventoryIterator
{
private:
    const ANZCharacter* Holder;
    ANZInventory* CurrentInv;
    ANZInventory* NextInv;
    int32 Count;
    
    inline bool IsValidForIteration(ANZInventory* TestInv)
    {
        return (TestInv->GetOwner() == Holder && TestInv->GetNZOwner() == Holder && TestInv->IsA(InvType::StaticClass()));
    }
public:
    TInventoryIterator(const ANZCharacter* InHolder)
    : Holder(InHolder)
    , Count(0)
    {
        if (Holder != NULL)
        {
            CurrentInv = Holder->InventoryList;
            if (CurrentInv != NULL)
            {
                NextInv = CurrentInv->NextInventory;
                if (!IsValidForIteration(CurrentInv))
                {
                    ++(*this);
                }
            }
            else
            {
                NextInv = NULL;
            }
        }
        else
        {
            CurrentInv = NULL;
            NextInv = NULL;
        }
    }
    void operator++()
    {
        do
        {
            // Bound number of items to avoid infinite loops on clients in edge cases with partial replication
            // we could keep track of all the items we've iterated but that's more expensive and probably not worth it
            Count++;
            if (Count > 100)
            {
                CurrentInv = NULL;
            }
            else
            {
                CurrentInv = NextInv;
                if (CurrentInv != NULL)
                {
                    NextInv = CurrentInv->NextInventory;
                }
            }            
        } while(CurrentInv != NULL && !IsValidForIteration(CurrentInv));
    }
    FORCEINLINE bool IsValid() const
    {
        return CurrentInv != NULL;
    }
    FORCEINLINE operator bool() const
    {
        return IsValid();
    }
    FORCEINLINE InvType* operator*() const
    {
        checkSlow(CurrentInv != NULL && CurrentInv->IsA(InvType::StaticClass()));
        return (InvType*)CurrentInv;
    }
    FORCEINLINE InvType* operator->() const
    {
        checkSlow(CurrentInv != NULL && CurrentInv->IsA(InvType::StaticClass()));
        return (InvType*)CurrentInv;
    }
};
