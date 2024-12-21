#pragma once

#include "IocpManager.h"

class CDNIocpManager: public CIocpManager
{
private:

#ifndef _USE_ACCEPTEX
	queue<CSocketContext*> m_ProcessCalls;
	CSyncLock m_ProcessLock;
#else
	CSyncLock m_DeleteLock;
#endif

protected:

#ifndef _USE_ACCEPTEX
	static UINT __stdcall ProcessThread(void *pParam);
#endif
	static UINT __stdcall UpdateThread(void *pParam);
	static UINT __stdcall ReconnectThread(void *pParam);

	void OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort);
	void OnConnected(CSocketContext *pSocketContext);
	void OnDisconnected(CSocketContext *pSocketContext);
	void OnConnectFail(CSocketContext *pSocketContext);

#ifdef _USE_ACCEPTEX
public:
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);
protected:
#else
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);
#endif
	
#ifdef _USE_ACCEPTEX
	HANDLE m_hReconnectThread;
	HANDLE m_hUpdateThread;
#else
	static HANDLE m_hSignalProcess;
#endif

public:
	CDNIocpManager(void);
	virtual ~CDNIocpManager(void);

	int Init(int nSocketCountMax,int nWorkerThreadCount);
	void Final();

	int CreateThread();

#ifdef _USE_ACCEPTEX
	void PostDisconnect( CSocketContext *pSocketContext );
	virtual void ThreadStop();
#else
	void AddProcessCall(CSocketContext *pSocketContext);
#endif

	void DetachSocket (CSocketContext *pSocketContext, wchar_t *pwszIdent);
};

extern CDNIocpManager* g_pIocpManager;

