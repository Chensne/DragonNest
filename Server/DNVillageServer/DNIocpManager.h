#pragma once

#include "IocpManager.h"

class CDNIocpManager: public CIocpManager
{
private:
	unsigned int m_nProcessThreadID;
	queue<CSocketContext*> m_ProcessCalls;

	CSyncLock m_ProcessLock;
	bool m_bAcceptVerifyList[ACCEPTOPEN_VERIFY_TYPE_MAX];
	bool m_bClientAcceptOpened;
	static volatile ULONG m_nLasterProcessTick;

protected:
	static UINT __stdcall ProcessThread(void *pParam);
	static UINT __stdcall UpdateThread(void *pParam);
	static UINT __stdcall ReconnectThread(void *pParam);
	static UINT __stdcall ProcessChecker(void *pParam);

	static HANDLE m_hSignalProcess;
	//static HANDLE m_hSignalRecv;
	//static HANDLE m_hSignalUpdate;

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

	void AddProcessCall(CSocketContext *pSocketContext );
	inline unsigned int GetProcessThreadId() { return m_nProcessThreadID; }

	void ProcessDelay(int nConKey, int nMainCmd, int nSubCmd, int nTick);
	void StagnatePacket(int nCnt, int nTick, CSocketContext *pSocketContext);
	
	void VerifyAccept(int nVerifyType);

	HANDLE m_hProcessThread;
	HANDLE m_hUpdateThread;
	HANDLE m_hReconnectThread;
	HANDLE m_hProcessCheckerThread;
};

extern CDNIocpManager* g_pIocpManager;
