// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NZCharacter.h"
#include "NZDroppedPickup.h"
#include "NZInventory.generated.h"

UCLASS(Blueprintable, Abstract, notplaceable, meta = (ChildCanTick))
class NZGAME_API ANZInventory : public AActor
{
	GENERATED_BODY()
    
    friend bool ANZCharacter::AddInventory(ANZInventory*, bool);
    friend void ANZCharacter::RemoveInventory(ANZInventory*);
    template<typename> friend class TInventoryIterator;
	
public:	
	// Sets default values for this actor's properties
	ANZInventory(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitProperties() override;
	virtual void PreInitializeComponents() override;

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
    
	/** Initialized dropped pickup holding this inventory item */
    virtual void InitializeDroppedPickup(class ANZDroppedPickup* Pickup);
    
    /** Return a component that can be instanced to be applied to pickups */
    UFUNCTION(BlueprintNativeEvent)
    UMeshComponent* GetPickupMeshTemplate(FVector& OverrideScale) const;
    
    /** Call AddOverlayMaterial() on the GRI to add any character or weapon overlay materials; this registration is required to replicate overlays */
	UFUNCTION(BlueprintNativeEvent)
	void AddOverlayMaterials(ANZGameState* GameState) const;

	/** Human readable localized name for the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText DisplayName;
    
	/** Respawn time for level placed pickups of this type */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	float RespawnTime;

	/** If set, item starts off not being avaliable when placed in the level (must wait RespawnTime from start of match) */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	bool bDelayedSpawn;

	/** If set, item is always dropped when its holder dies if uses/charges/etc remain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	bool bAlwaysDropOnDeath;

    /** Particles played when picked up */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	UParticleSystem* PickupEffect;

    /** Sound played when pickup up */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	USoundBase* PickupSound;

    /** Sound played when owner takes damage (armor hit sound, for example) */
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	USoundBase* ReceivedDamageSound;

    /** Class used when this item is dropped by its holder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	TSubclassOf<class ANZDroppedPickup> DroppedPickupClass;

    /**
     * Called by pickups when another inventory of same class will be given, 
     * allowing the item to simply stack instance values instead of spawning a new item
     * ContainedInv may be NULL if it's a pickup that spawns new instead of containing a partially used existing item
     * Return true to prevent giving/spawning a new inventory item
     */
    UFUNCTION(BlueprintNativeEvent)
    bool StackPickup(ANZInventory* ContainedInv);
    
    /** If set, inventory gets the ModifyDamageTaken() and PreventHeadShot() functions/events when the holder takes damage */
    UPROPERTY(EditDefaultsOnly, Category = Events)
    uint32 bCallDamageEvents : 1;
    
    /** It set, receive OwnerEvent() calls for various holder events (jump, land, fire, etc) */
    UPROPERTY(EditDefaultsOnly, Category = Events)
    uint32 bCallOwnerEvent : 1;
    
    /** NOTE: Return value is a workaround for blueprint bugs involving ref parameters and is not used */
    UFUNCTION(BlueprintNativeEvent)
    bool ModifyDamageTaken(UPARAM(ref) int32& Damage, UPARAM(ref) FVector& Momentum, UPARAM(ref) ANZInventory*& HitArmor, AController* InstigatedBy, const FHitResult& HitInfo, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
    
    UFUNCTION(BlueprintNativeEvent)
    bool PlayArmorEffects(ANZCharacter* HitPawn) const;
    
    /** Handles any C++ generated effects, calls blueprint PlayArmorEffects */
    virtual bool HandleArmorEffects(ANZCharacter* HitPawn) const;
    
    /**
     * Return true to prevent an incoming head shot if bConsumeArmor is true, 
     * prevention should also consume the item (or a charge or whatever mechanic of degradation is being used)
     */
    UFUNCTION(BlueprintNativeEvent)
    bool PreventHeadShot(FVector HitLocation, FVector ShotDirection, float WeaponHeadScaling, bool bConsumeArmor);
    
    /** Return true to display armor hit effects */
    UFUNCTION(BlueprintNativeEvent)
    bool ShouldDisplayHitEffect(int32 AttemptedDamage, int32 DamageAmount, int32 FinalHealth, int32 FinalArmor);
    
    UFUNCTION(BlueprintNativeEvent)
    int32 GetEffectiveHealthModifier(bool bOnlyVisible) const;
    
    UFUNCTION(BlueprintNativeEvent)
    void OwnerEvent(FName EventName);
    
    /**
     * Draws any relevant HUD that should be drawn whenever this item is held
     * NOTE: Not called by default, generally a HUD widget will call this for item types that are relevant for its area
     */
    UFUNCTION(BlueprintNativeEvent)
    void DrawInventoryHUD(class UNZHUDWidget* Widget, FVector2D Pos, FVector2D Size);
    
    /** Base AI desireability for picking up this item on the ground */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AI)
    float BasePickupDesireability;
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AI)
    float BotDesireability(APawn* Asker, AActor* Pickup, float PathDistance) const;
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = AI)
    float DetourWeight(APawn* Asker, AActor* Pickup, float PathDistance) const;
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
    FLinearColor IconColor;
    
    /** Icon for drawing time remaining on the HUD */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HUD)
    FCanvasIcon HUDIcon;
    
    /** Icon for minimap when this item is in a pickup (uses HUDIcon if not specified) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HUD)
    FCanvasIcon MinimapIcon;
    
    /** Whether to show timer for this on spectator slide out HUD */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HUD)
    bool bShowPowerupTimer;
    
    /** How important is this inventory item when rendering a group of them */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HUD)
    float HUDRenderPriority;
    
    /** Returns the HUD text to display for this item */
    virtual FText GetHUDText()
    {
        return FText::GetEmpty();
    }
    
    /** Allows inventory items to decide if a widget should be allowed to render them */
    virtual bool HUDShouldRender(UNZHUDWidget* TargetWidget)
    {
        return false;
    }
    
    /** Used to initially flash icon if HUD rendered */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
    float InitialFlashTime;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
    float InitialFlashScale;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderObject)
    FLinearColor InitialFlashColor;
    
    float FlashTimer;
    
public:
    /** Holds a reference to the menu */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
    UTexture2D* MenuGraphic;
    
    /** This is the long description that will be displayed in the menu */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
    FText MenuDescription;
    
    /** The stat for how many times this was pickup up */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Powerup)
    FName StatsNameCount;
};

/**
 * Template to access a character's inventory
 * This class automatically handles ignoring invalid (not fully replicated or initialized) inventory items
 * It is also resilient against the currently iterated item being destroyed
 */
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
