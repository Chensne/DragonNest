
#pragma once

#include <map>
#include "IocpManager.h"
#include "CriticalSection.h"
#include "ServiceManager.h"

const int IDLEDISTRIBUTING_TIME = 5 * 1000;

class CServiceServer : public CIocpManager
{
public:
	CServiceServer();
	~CServiceServer();

	bool Initialize(int nSocketCountMax, int nLauncherPort, int nServicePort, int nServicePatcherPort, int nMonitorPort);

	void AddProcessCall(CSocketContext *pSocketContext);
	void AddSyncCall();
	void Close();

	void ThreadDestroyEvent(int nThreadID){ SetEvent(m_hDestroyThreadEvent[nThreadID]); }
protected:
	static UINT __stdcall ProcessThread(void *pParam);
	static UINT __stdcall SyncTaskProcess(void * pParam);

	void OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort);
	void OnConnected(CSocketContext *pSocketContext);
	void OnDisconnected(CSocketContext *pSocketContext);
	void OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred);

	static HANDLE m_hSignalProcess;
	static HANDLE m_hSignalSyncTask;
	HANDLE m_hDestroyThreadEvent[2];
private:
	queue<CSocketContext*> m_ProcessCalls;
	CSyncLock m_ProcessLock;

	volatile bool m_bTaskAdded;
	CSyncLock m_SyncTaskLock;

	bool CreateThread();
};

extern CServiceServer * g_pServiceServer;