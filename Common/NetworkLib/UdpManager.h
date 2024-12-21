#pragma once

#include "Buffer.h"

class CUdpManager
{
protected:
	SOCKET m_Socket;

	virtual void OnRecv();
	virtual void OnSend();

	static UINT __stdcall SendThread(void *pParam);
	static UINT __stdcall RecvThread(void *pParam);

public:
	CUdpManager(void);
	CUdpManager(int nSize);
	~CUdpManager(void);

	void Clear();

	bool Listen(const USHORT nPort);
	void SendData(char *pData, int nSize, SOCKADDR_IN *pAddr);
};
