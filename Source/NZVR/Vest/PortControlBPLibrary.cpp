// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "NZVR.h"
#include <vector>
#include <queue>
#include "ThreadTest.h"

#include "PortControlBPLibrary.h"

CSerial *UPortControlBPLibrary::SP = new CSerial();
UPortControlBPLibrary *UPortControlBPLibrary::BP = NULL;
FString returnIt;
FString sendChar;

unsigned char sendHex[200] = {};
TQueue<FString> Order_Send_List;//////

std::vector<TQueue<FString>> Vibr_Order_List(8);
std::queue<unsigned char> vibra_mode;
std::queue<MotorPosition> vibra_position;


FString testStr;
bool serialShutdown;
bool threadShutdown;
bool threadIsOpen;
bool serialIsOpen;

bool isAutoOPen;

UPortControlBPLibrary::UPortControlBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	vibra_mode.empty();
	vibra_position.empty();
	isAutoOPen = false;
}

UPortControlBPLibrary::UPortControlBPLibrary()
{
	vibra_mode.empty();
	vibra_position.empty();
	isAutoOPen = false;
}

int HexChar(char c)
{
	if ((c >= '0') && (c <= '9'))
		return (c - 0x30);
	else if ((c >= 'A') && (c <= 'F'))
		return (c - 'A' + 10);
	else if ((c >= 'a') && (c <= 'f'))
		return (c - 'a' + 10);
	else
		return (0x10);
}

//将一个字符串作为十六进制串转化为一个字节数组，字节间可用空格分隔，
//返回转换后的字节数组长度，同时字节数组长度自动设置。
int Str2Hex(FString str, unsigned char data[])
{
	int t = 0;
	int t1 = 0;
	int rlen = 0;
	int32 len = str.Len();

	//data.SetSize(len/2);

	for (int i = 0; i < len;)
	{
		char l = ' ';
		char h = (char)str[i];
		if (h == ' ')
		{
			i++;
			continue;
		}

		i++;
		if (i >= len)
			break;

		l = (char)str[i];
		t = HexChar(h);
		t1 = HexChar(l);

		if ((t == 16) || (t1 == 16))
			break;
		else
			t = t * 16 + t1;
		i++;

		data[rlen] = (unsigned char)t;
		rlen++;
	}

	return rlen;

}

FString virArry[8];
void clearVirArry()
{
	FString str132 = "ff";
	bool have = false;

	for (int j = 0; j < getArrayLen(virArry); j++) {
		if (virArry[j] != "") {
			str132 += virArry[j];
			have = true;
			virArry[j] = "";
		}
		else
			str132 += "0000";
	}
	str132 += "ff";

	if (have) {
		Order_Send_List.Enqueue(str132);
		have = false;
	}
}

void OrderToList()
{
	for (int i = 0; i < getArrayLen(virArry); i++) {
		if (vibra_position.size() > 0)
		{
			unsigned char vm;
			vm = vibra_mode.front();
			vibra_mode.pop();

			MotorPosition mp;
			mp = vibra_position.front();
			vibra_position.pop();

			int _mp;
			_mp = (int)mp - 1;

			/*if (vm == VibraMode::all_Vib || vm == VibraMode::front_V || vm == VibraMode::rear_V) {
				clearVirArry();
				Order_Send_List.Enqueue("ff0" + FString::FormatAsNumber(vm) + "0000000000000000000000000000ff");
				return;
			}*/
			if (virArry[_mp] != "") {
				clearVirArry();
				virArry[_mp] = "01" + FString::Printf(TEXT("%02X"), vm);
				return;
			}

			virArry[_mp] = "01" + FString::Printf(TEXT("%02X"), vm);
		}
		else
			clearVirArry();
	}

}


void UPortControlBPLibrary::Vibration(int32 _motorPosition, int32 _vibraMode)
{
	//MotorPosition
	//StereotypedMode
	if (serialIsOpen) 
	{
		if (_vibraMode == StereotypedMode::Blast_AllPoint)
		{
			UPortControlBPLibrary::CustomizeVibration((MotorPosition)1, uint8(_vibraMode));
			return;
		}

		UPortControlBPLibrary::CustomizeVibration(MotorPosition(_motorPosition), uint8(_vibraMode));
	}
}

void UPortControlBPLibrary::CustomizeVibration(MotorPosition _motorPosition, uint8 _vibraTime)
{
	if (serialIsOpen)
	{
		vibra_mode.push(_vibraTime);
		vibra_position.push(_motorPosition);
	}
}


// void UPortControlBPLibrary::AutoOpenSerial()
// {
//  BP = UPortControlBPLibrary::GetInstance();
// 	isAutoOPen = true;
// 	BP->StartThread(0, 0);
// }

void UPortControlBPLibrary::OpenSerial(int32 portNum)
{
	int32 baudRate = 9600;
	isAutoOPen = false;
	BP = UPortControlBPLibrary::GetInstance();
	BP->StartThread(portNum, baudRate);
}

void UPortControlBPLibrary::CloseSerial()
{
	ThreadTest::Shutdown();

	vibra_mode.empty();
	vibra_position.empty();
	isAutoOPen = false;

	if (threadShutdown)
	{
		UE_LOG(LogTemp, Log, TEXT("THREAD CLOSED!!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: THREAD NOT CLOSED!!"));
	}

	if (serialShutdown)
	{
		UE_LOG(LogTemp, Log, TEXT("SERIAL PORT CLOSED!!"));
		delete SP;
		SP = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR: SERIAL PORT NOT CLOSED!!"));
	}

}

FString UPortControlBPLibrary::ReadSerial()
{
	return returnIt;
}

void UPortControlBPLibrary::WriteSerial(FString writeData)
{
	sendChar = writeData;
}

bool UPortControlBPLibrary::IsSerialPortOpen()
{
	if (serialIsOpen && threadIsOpen)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UPortControlBPLibrary::InitVestControl(UObject* inOwner)
{
	if (BP == NULL)
	{
		//BP = NewObject<UPortControlBPLibrary>(inOwner, TEXT("PortControlBPLibrary"));
		//BP->AddToRoot();
	}
}

int32 UPortControlBPLibrary::GetConfigSerialPort()
{
	int32 Port = GetDefault<UPortControlBPLibrary>()->ConfigSerialPort;
	return Port;
}

UPortControlBPLibrary * UPortControlBPLibrary::UPortControlBPLibrary::GetInstance()
{
	if (BP == NULL)
	{
		BP = NewObject<UPortControlBPLibrary>();//	BP = new UPortControlBPLibrary();
	}
	return BP;
}

void UPortControlBPLibrary::StartThread(int32 portNum, int32 baudRate)
{
	if (ThreadTest::ThreadLauncher(this, portNum, baudRate) != NULL)
	{
		if (portNum != 0)
		{
			UE_LOG(LogTemp, Log, TEXT("THREAD STARTED!!"));
		}
		threadIsOpen = true;
	}
	else
	{
		if (portNum != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ERROR: THREAD NOT STARTED!!"));
		}
		threadIsOpen = false;
	}
	if (serialIsOpen)
	{
		if (portNum != 0)
		{
			UE_LOG(LogTemp, Log, TEXT("SERIAL PORT OPENED!!---COM%d"), portNum);
		}
	}
	else
	{
		if (portNum != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ERROR: SERIAL PORT NOT OPENED!!---COM%d"), portNum);
		}
	}

}

FString UPortControlBPLibrary::GetData(unsigned char* data)
{
	FString temp((char*)data);
	returnIt = temp;
	return temp;
}

FString UPortControlBPLibrary::SendData()
{
	FString temp = sendChar;
	sendChar = "";
	return temp;

}

int UPortControlBPLibrary::SendData_Hex(unsigned char* Comsenddata)
{
	OrderToList();

	FString	mes;
	if (!Order_Send_List.Dequeue(mes))
	{
		return 0;
	}

	int len = Str2Hex(mes, sendHex);
	memcpy(Comsenddata, sendHex, sizeof(sendHex));
	*sendHex = {};

	return len;

}

void UPortControlBPLibrary::IsThreadClosed(bool threadClosed)
{
	threadShutdown = threadClosed;
}

void UPortControlBPLibrary::IsSerialClosed(bool serialClosed)
{
	serialShutdown = serialClosed;
}

void UPortControlBPLibrary::SerialOpened(bool serialOpen)
{
	serialIsOpen = serialOpen;
}

bool UPortControlBPLibrary::IsAutoOpenSerial()
{
	return isAutoOPen;
}

void UPortControlBPLibrary::NotAutoVest()
{
	UE_LOG(LogTemp, Warning, TEXT("ERROR: Not Find the Vest Device!!!Please connect !"));
}
