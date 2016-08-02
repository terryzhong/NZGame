// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PortControlPrivatePCH.h"
#include "CSerial.h"


CSerial::CSerial()
{

	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	m_hIDComDev = NULL;
	m_bOpened = FALSE;

}

CSerial::~CSerial()
{

	Close();
}

void CSerial::EnumSerialPorts(std::queue<int> &com_num)
{
	//   得到设备信息集   
	while (!com_num.empty())
	{
		com_num.pop();
	}
	HDEVINFO   hDevInfo = SetupDiGetClassDevs(
		(LPGUID)&GUID_DEVCLASS_PORTS,
		NULL,
		0,
		DIGCF_PRESENT/*   |   DIGCF_ALLCLASSES*/);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error!   SetupDiGetClassDevs()   return   %d"), GetLastError());

		SetupDiDestroyDeviceInfoList(hDevInfo);
		UE_LOG(LogTemp, Warning, TEXT("has erro"));
	}

	TCHAR   szBuf[MAX_PATH];
	SP_DEVINFO_DATA   spDevInfoData = { sizeof(SP_DEVINFO_DATA) };

	//   开始列举设备   
	unsigned long i = 0;
	for (i = 1; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++)
	{

		//   得到设备型号   
		if (SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&spDevInfoData,
			(0x0000000C),
			NULL,
			(PBYTE)szBuf,
			MAX_PATH,
			NULL))
		{
			// 			UE_LOG(LogTemp, Warning, TEXT("%03d   :   FriendName   =   %s"), i, szBuf);

			FString mes(szBuf);
			int j = mes.Find("com", ESearchCase::IgnoreCase, ESearchDir::FromStart, -1) + 3;
			int num = mes.Len() - j - 1;
			mes = mes.Mid(j, num);

			if (num > 3)////用于虚拟串口，测试用，应该注释掉
			{
				j = mes.Find("->", ESearchCase::IgnoreCase, ESearchDir::FromStart, -1);
				mes = mes.Mid(0, j);
			}
			com_num.push(FCString::Atoi(*mes));
			// 			UE_LOG(LogTemp, Warning, TEXT("%03d   :   FriendName   =   %d"), i, FCString::Atoi(*mes));

		}
	}
	i--;
}

BOOL CSerial::Open(int nPort, int nBaud)
{

	if (m_bOpened) return(TRUE);

	TCHAR szPort[15];
	TCHAR szComParams[50];
	DCB dcb;

	wsprintf(szPort, _T("\\\\.\\COM%d"), nPort);
	m_hIDComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hIDComDev == NULL) return(FALSE);

	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));

	COMMTIMEOUTS CommTimeOuts;
	//CommTimeOuts.ReadIntervalTimeout = 10;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);

	wsprintf(szComParams, _T("COM%d:%d,n,8,1"), nPort, nBaud);

	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);

	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;

	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	unsigned char ucSet;
	ucSet = (unsigned char)((FC_RTSCTS & FC_DTRDSR) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_RTSCTS) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_XONXOFF) != 0);
	if (!SetCommState(m_hIDComDev, &dcb) ||
		!SetupComm(m_hIDComDev, 10000, 10000) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL) {
		DWORD dwError = GetLastError();
		if (m_OverlappedRead.hEvent != NULL) CloseHandle(m_OverlappedRead.hEvent);
		if (m_OverlappedWrite.hEvent != NULL) CloseHandle(m_OverlappedWrite.hEvent);
		CloseHandle(m_hIDComDev);
		return(FALSE);
	}

	m_bOpened = TRUE;
	Sleep(50);
	return(m_bOpened);

}

void CSerial::SetDTR()
{

	COMMCONFIG _config;
	DWORD size = sizeof(_config);
	if (!GetCommConfig(m_hIDComDev, &_config, &size))
	{
		return;
	}
	_config.dcb.fDtrControl = 1;
	_config.dcb.fRtsControl = 1;
	if (!SetCommConfig(m_hIDComDev, &_config, sizeof(_config))) {
		return;
	}
}

BOOL CSerial::Close(void)
{

	if (!m_bOpened || m_hIDComDev == NULL) return(TRUE);

	if (m_OverlappedRead.hEvent != NULL) CloseHandle(m_OverlappedRead.hEvent);
	if (m_OverlappedWrite.hEvent != NULL) CloseHandle(m_OverlappedWrite.hEvent);
	CloseHandle(m_hIDComDev);
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return(TRUE);

}

BOOL CSerial::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile(m_hIDComDev, &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING)) {
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, 1000)) dwBytesWritten = 0;
		else {
			GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return(TRUE);

}

int CSerial::SendData(const char *buffer, int size)
{

	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	DWORD dwBytesWritten = 0;
	int i;
	for (i = 0; i < size; i++) {
		WriteCommByte(buffer[i]);
		dwBytesWritten++;
	}

	return((int)dwBytesWritten);

}

BOOL CSerial::SendData_Hex(unsigned char *buffer, int size)
{
	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile(m_hIDComDev, buffer, size, &dwBytesWritten, &m_OverlappedWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING)) {
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, 1000)) dwBytesWritten = 0;
		else {
			GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return(TRUE);
}

int CSerial::ReadDataWaiting(void)
{

	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);

	return((int)ComStat.cbInQue);

}

int CSerial::ReadData(unsigned char *buffer, int limit)
{

	if (!m_bOpened || m_hIDComDev == NULL) return(0);

	BOOL bReadStatus;
	DWORD dwBytesRead; /*dwErrorFlags, nTotalRead;*/
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	dwBytesRead = (DWORD)ComStat.cbInQue;
	dwBytesRead = ((DWORD)limit) < (dwBytesRead) ? ((DWORD)limit) : (dwBytesRead);

	bReadStatus = ReadFile(m_hIDComDev, buffer,
		dwBytesRead, &dwBytesRead,
		&m_OverlappedRead);
	if (!bReadStatus)// / 如果ReadFile函数返回FALSE
	{
		if (GetLastError() == ERROR_IO_PENDING)
			//GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作 
		{
			DWORD a_a = WaitForSingleObject(m_OverlappedRead.hEvent, INFINITE);
			// 			使用WaitForSingleObject函数等待，直到 INFINITE 读操作完成   或 2000 延时已达到2秒钟
			// 			当串口读操作进行完毕后，的hEvent事件会变为有信号

			switch (a_a)
			{
			case WAIT_FAILED:
				// 				UE_LOG(LogTemp, Warning, TEXT("[读线程] 等待失败"));
				return(0);
			case 0:
				// 				UE_LOG(LogTemp, Warning, TEXT("[读线程] 收到退出事件"));
				// 				return(0);
			case 1:
				bReadStatus = GetOverlappedResult(m_hIDComDev, &m_OverlappedRead, &dwBytesRead, FALSE);
				if (!bReadStatus)
				{
					// 					UE_LOG(LogTemp, Warning, TEXT("[读线程] GetOverlappedResult失败!"));
					return(0);
				}
			}
		}
	}
	// 	PurgeComm(m_hIDComDev, PURGE_TXABORT |
	// 		PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	return dwBytesRead;

}