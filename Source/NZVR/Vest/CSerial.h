// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13


#include "Internationalization.h"
#include "SlateBasics.h"

#include <string>
#include <queue>

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include   <windows.h>   
#include   <setupapi.h>   
#include   <devguid.h>   
#endif // PLATFORM_WINDOWS


//#pragma   comment(lib,   "setupapi")  ///

/**
*
*/


class CSerial
{

public:
	CSerial();
	~CSerial();

	void EnumSerialPorts(std::queue<int>&);

	bool Open(int nPort = 2, int nBaud = 9600);
	void SetDTR();
	bool Close(void);

	int ReadData(unsigned char *, int);
	int SendData(const char *, int);
	bool SendData_Hex(unsigned char *, int);
	int ReadDataWaiting(void);

	bool IsOpened(void) 
	{
#if PLATFORM_WINDOWS

		return(m_bOpened); 
#endif //  PLATFORM_WINDOWS
		return false;
	}

protected:
	bool WriteCommByte(unsigned char);

#if PLATFORM_WINDOWS
	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	bool m_bOpened;
#endif

};

#endif