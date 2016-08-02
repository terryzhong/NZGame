#pragma once

#include "Engine.h"
#include "CSerial.h"
#include "PortControlBPLibrary.h"

/**
*
*/
class ThreadTest : public FRunnable
{
	static ThreadTest* Runnable;

	static UPortControlBPLibrary* BP;

	FString* theData;

/*
	UPortControlBPLibrary* TheBP;
*/

	FRunnableThread* Thread;

	FThreadSafeCounter StopTaskCounter;

	static CSerial *SP;

public:

	ThreadTest(UPortControlBPLibrary* IN_BP);
	virtual ~ThreadTest();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	void EnsureCompletion();

	static ThreadTest* ThreadLauncher(UPortControlBPLibrary* IN_BP, int32 portNum, int32 baudRate);

	static void Shutdown();

};
