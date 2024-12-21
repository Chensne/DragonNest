#pragma once

#include "IocpManager.h"

class CDNIocpManager: public CIocpManager
{
private:

	HANDLE	m_hTerminateEvent;
	HANDLE	m_hReconnectThread;

	bool m_bAcceptVerifyList[ACCEPTOPEN_VERIFY_TYPE_MAX];
	bool m_bClientAcceptOpened;

#if defined( PRE_PARTY_DB )
	bool m_bFirstDBConnect;
#endif

protected:
	static UINT __stdcall ReconnectThread(void *pParam);

	void OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort);
	void OnConnected(CSocketContext *pSocketContext);
	void OnDisconnected(CSocketContext *pSocketContext);
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);
	void OnConnectFail(CSocketContext *pSocketContext);

public:
	CDNIocpManager(void);
	~CDNIocpManager(void);

	int Init(int nSocketCountMax, int nWorkerThreadSize = 0);
	void Final();
	void FinalReconnectThread();

	int CreateThread();
	HANDLE	GetTerminateEvent(){ return m_hTerminateEvent; }
#if defined( PRE_WORLDCOMBINE_PARTY )
	void VerifyAccept(int nVerifyType, BYTE cWorldSetID=0 );
#else
	void VerifyAccept(int nVerifyType);
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
};

extern CDNIocpManager* g_pIocpManager;
