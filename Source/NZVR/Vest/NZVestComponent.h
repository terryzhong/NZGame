/************************************************************************/
/* 力反馈背心                                                                     */
/************************************************************************/
#pragma once

#include "Components/ActorComponent.h"
#include "NZVestComponent.generated.h"

/**
* 震动位置与MotorPosition相同
*/
UENUM(BlueprintType)
enum EVestPosition
{
	EVP_Default,
	EVP_RUp,
	EVP_LUp,
	EVP_RMid,
	EVP_LMid,
	EVP_RDown,
	EVP_LDown,
	EVP_RBack,
	EVP_LBack,
};

/**震动模式， 与StereotypedMode对应*/
UENUM(BlueprintType)
enum EVestMode
{
	EVM_SinglePoint = 0xc9,	///One point vibration 50ms
	EVM_AllPoint = 0xd1,	///All point vibration 150ms
};

USTRUCT(BlueprintType)
struct  FVestPositionSocket
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vest")
	TEnumAsByte<EVestPosition> pos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vest")
	FName SocketName;
};

//UCLASS(Config=Vest, ClassGroup="Interaction", editinlinenew, meta = (DisplayName = "NZ Vest Component", BlueprintSpawnableComponent))
UCLASS(Config = Vest, Blueprintable)
class NZVR_API UNZVestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNZVestComponent(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Vest|NZVestComponent")
	void ConnectVest();

	UFUNCTION(BlueprintImplementableEvent, Category = "Vest|NZVestComponent")
	bool IsConnected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Vest|NZVestComponent")
	void CloseVest();

	UFUNCTION(BlueprintImplementableEvent, Category = "Vest|NZVestComponent")
	void Virbration(int Position, int Mode);

	UFUNCTION(BlueprintCallable, Category = "Vest|NZVestComponent")
	virtual int32 GetConfigSerialPort();

	UFUNCTION(BlueprintCallable, Category = "Vest|NZVestComponent")
	virtual void OnTakePointDamage(float Damage, FVector& HitLocation, AActor* DamageCauser);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vest|NZVestComponent")
	TArray<FVestPositionSocket> VestSockets;
	
private:
	UPROPERTY(globalconfig)
	int32 ConfigSerialPort;
};