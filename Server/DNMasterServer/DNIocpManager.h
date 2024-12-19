#pragma once

#include "IocpManager.h"

class CDNIocpManager: public CIocpManager
{
private:
	queue<CSocketContext*> m_ProcessCalls;
	CSyncLock m_ProcessLock;

	HANDLE m_hProcessThread;
	HANDLE m_hIdleThread;

protected:
	static UINT __stdcall ProcessThread(void *pParam);
	static UINT __stdcall IdleThread(void *pParam);

	static HANDLE m_hSignalProcess;

	void OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort);
	void OnConnected(CSocketContext *pSocketContext);
	void OnDisconnected(CSocketContext *pSocketContext);
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);
	void OnConnectFail(CSocketContext *pSocketContext);

public:
	CDNIocpManager(void);
	virtual ~CDNIocpManager(void);

	int Init(int nSocketCountMax);
	void Final();

	int CreateThread();

	void AddProcessCall(CSocketContext *pSocketContext);
};

extern CDNIocpManager* g_pIocpManager;
