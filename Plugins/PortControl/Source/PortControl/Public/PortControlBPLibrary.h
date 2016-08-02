// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

// #include "CSerial.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CSerial.h"

#include "PortControlBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum StereotypedMode/* : unsigned char*/
{
	Hit_SinglePoint = 0xc9,
	///One point vibration 50ms

	Blast_AllPoint = 0xd1,
	///All point vibration 150ms
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum  MotorPosition
{
	defau = 0,
        rUp = 1,
        lUp = 2,
        rMid = 3,
        lMid = 4,
        rDown = 5,
        lDown = 6,
        rBack = 7,
	lBack = 8,

};


UCLASS(Config = Vest)
class UPortControlBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UPortControlBPLibrary();

	UFUNCTION(BlueprintCallable,  Category = "Vest_Control_Serve")
		static void Vibration(MotorPosition _motorPosition, StereotypedMode _vibraMode);

	UFUNCTION(BlueprintCallable, Category = "Vest_Control_Serve")
		static void CustomizeVibration(MotorPosition _motorPosition, uint8 _vibraTime);

// 	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Auto-Connect the Vest"), Category = "Vest_Control_Serve")
// 		static void AutoOpenSerial();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Manual-Connect the Vest"), Category = "Vest_Control_Serve")
		static void OpenSerial(int32 portNum);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close the Vest"), Category = "Vest_Control_Serve")
		static void CloseSerial();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Data from Vest"), Category = "Vest_Control_Serve")
		static FString ReadSerial();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Message to Vest"), Category = "Vest_Control_Serve")
		static void WriteSerial(FString writeData);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Connected to Vest?"), Category = "Vest_Control_Serve")
		static bool IsSerialPortOpen();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Init Vest Control"), Category = "Vest_Control_Serve")
		static void InitVestControl(UObject* inOwner);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Config Serial Port"), Category = "Vest_Control_Serve")
		static int32 GetConfigSerialPort();

	static UPortControlBPLibrary * GetInstance();


	void StartThread(int32 portNum, int32 baudRate);
	FString GetData(unsigned char*);
	FString SendData();
	int SendData_Hex(unsigned char*);
	void IsThreadClosed(bool threadClosed);
	void IsSerialClosed(bool serialClosed);
	void SerialOpened(bool serialOpen);

	bool IsAutoOpenSerial();
	void NotAutoVest();

private:

	static CSerial *SP;
	static UPortControlBPLibrary* BP;

	UPROPERTY(globalconfig)
	int32 ConfigSerialPort;


/*
	class CGarbo   //它的唯一工作就是在析构函数中删除CSingleton的实例  
	{
	public:
		~CGarbo()
		{
			if (UPortControlBPLibrary::BP)
				delete UPortControlBPLibrary::BP;
		}
	};
	static CGarbo Garbo;  //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数  
*/

};

template <class T>
int getArrayLen(T& array)

{//使用模板定义一 个函数getArrayLen,该函数将返回数组array的长度

	return (sizeof(array) / sizeof(array[0]));

}