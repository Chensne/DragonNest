#pragma once

#include "IocpManager.h"

class CDNIocpManager: public CIocpManager
{
protected:
	static UINT __stdcall ReconnectThread(void *pParam);

	void OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort);
	void OnConnected(CSocketContext *pSocketContext);
	void OnDisconnected(CSocketContext *pSocketContext);
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);

public:
	CDNIocpManager(void);
	virtual ~CDNIocpManager(void);

	int CreateThread();
	int CreateUpdateThread();
	static UINT __stdcall UpdateThread(void *pParam);
};

extern CDNIocpManager* g_pIocpManager;
