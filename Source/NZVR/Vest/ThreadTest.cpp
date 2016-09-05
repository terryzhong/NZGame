// Fill out your copyright notice in the Description page of Project Settings.

#include "NZVR.h"
#include "ThreadTest.h"
#include "CString.h"

ThreadTest* ThreadTest::Runnable = NULL;

CSerial *ThreadTest::SP = new CSerial();

UPortControlBPLibrary *ThreadTest::BP = NULL;

unsigned char incomingData[5] = {};			// don't forget to pre-allocate memory
unsigned char outData[10] = {};
char writeBuffer[250] = {};
int len = 0;
int32 thePort;
int32 theBaud;

std::queue<int> com_Arry;

ThreadTest::ThreadTest(UPortControlBPLibrary* IN_BP)
	:/* TheBP(IN_BP)
	, */StopTaskCounter(0)

{
	Thread = FRunnableThread::Create(this, TEXT("ThreadTest"), 0, TPri_BelowNormal);
}

ThreadTest::~ThreadTest()
{
	delete Thread;
	Thread = NULL;
}

bool ThreadTest::Init()
{
	// TODO: Initialization

	return true;
}

uint32 ThreadTest::Run()
{
	if (BP->IsAutoOpenSerial())
	{
		unsigned char _CheckCode[5] = { 0x32, 0x34, 0x39, 0x32, 0x39 };//校验码

		SP->EnumSerialPorts(com_Arry);

		for (; StopTaskCounter.GetValue() == 0 && com_Arry.size() > 0;)
		{
			int cpm = com_Arry.front();
			com_Arry.pop();

			if (!SP->Open(cpm))
			{
				continue;
			}

			SP->SendData_Hex(_CheckCode, 5);
			FPlatformProcess::Sleep(1);

			int nBytesRead = SP->ReadData(incomingData, 5);

			if (!(nBytesRead < 5))
				//如果接受到5个以上字符
			{
				int cout_check = 0;
				for (int i = 0; i < nBytesRead; i++)
				{

					if (_CheckCode[i] == incomingData[i])
					{
						cout_check++;
					}
				}
				if (cout_check == 5) {
					BP->StartThread(cpm, 9600);
					break;
				}

			}
			SP->Close();
		}
		if (!SP->IsOpened())
		{
			BP->NotAutoVest();
		}

	}
	while (StopTaskCounter.GetValue() == 0 && SP->IsOpened())
	{

		FPlatformProcess::Sleep(0.01);

		FString writeData = BP->SendData();
		int len = FCStringAnsi::Strlen(TCHAR_TO_ANSI(*writeData));
		FCStringAnsi::Strcpy(writeBuffer, len+1, TCHAR_TO_ANSI(*writeData));
		//strcpy_s(writeBuffer, TCHAR_TO_ANSI(*writeData));
		SP->SendData(writeBuffer, FCStringAnsi::Strlen(writeBuffer));

		unsigned char test[200] = {};
		int size_Hex = BP->SendData_Hex(test);
		if (size_Hex > 0)
		{
			SP->SendData_Hex(test, size_Hex);
		}
	}
	return 0;
}

void ThreadTest::Stop()
{
	StopTaskCounter.Increment();
}

ThreadTest* ThreadTest::ThreadLauncher(UPortControlBPLibrary* IN_BP, int32 portNum, int32 baudRate)
{
	thePort = portNum;
	theBaud = baudRate;
	if (BP == NULL)	{
		BP = UPortControlBPLibrary::GetInstance();
	}

	if (SP->Open(thePort, theBaud))	{
		BP->SerialOpened(true);
		BP->IsSerialClosed(false);
	}
	else	{
		BP->IsSerialClosed(true);
		BP->SerialOpened(false);
	}

	if (!Runnable && FPlatformProcess::SupportsMultithreading()){
		Runnable = new ThreadTest(IN_BP);
	}

	return Runnable;
}

void ThreadTest::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void ThreadTest::Shutdown()
{

	if (SP->Close())
	{
		BP->IsSerialClosed(true);
		BP->SerialOpened(false);
	}
	else
	{
		BP->IsSerialClosed(false);
	}
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
	if (Runnable == NULL)
	{
		BP->IsThreadClosed(true);
	}
	else
	{
		BP->IsThreadClosed(false);
	}

}

