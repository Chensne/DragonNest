#if defined(_SERVER)

#pragma once

#include "SocketContext.h"

using namespace std;

class CThreadManager;
struct TThreadData
{
	int nThreadID;
	CThreadManager *pThreadManager;
#if !defined( _LOGSERVER ) && !defined(_CASHSERVER)
	HANDLE hEvent;
#endif // #if !defined( _LOGSERVER )
#if (defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)) || defined(PRE_DEL_ONNULL)
	ULONG nLastHearbeatTick;
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
};

class CThreadManager
{
private:
#if defined(_LOGSERVER) || defined(_CASHSERVER)
	queue<CSocketContext*> m_ThreadCalls;	
#else
	queue<UINT> m_ThreadCalls;	
#endif // #if !defined( _LOGSERVER )

	list<HANDLE> m_hThreadList;
	list<TThreadData*> m_ThreadDataList;

	CSyncLock m_ThreadLock;
	LONG m_WaitThreadCount;

#if defined(_LOGSERVER) || defined(_CASHSERVER)
	HANDLE m_Handles[2];
#else
public:
	HANDLE m_Handles[2];
#endif // #if defined(_LOGSERVER) || defined(_CASHSERVER)
	

protected:
	static UINT __stdcall WorkerThread(void *pParam);
#if defined(PRE_DEL_ONNULL)
	virtual void OnCalled(CSocketContext *pSocketContext, char *pBuffer, TThreadData* pThreadData);	
#else
	virtual void OnCalled(CSocketContext *pSocketContext, char *pBuffer, int nThreadID);	
#endif

#if !defined(PRE_DEL_ONNULL)
#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
	virtual void OnNulled(int nThreadID, ULONG &nHeartBeatTick);	
#else		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
	virtual void OnNulled(int nThreadID);
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
#endif //#if !defined(PRE_DEL_ONNULL)

public:
	CThreadManager(void);
	virtual ~CThreadManager(void);

	bool Init(int nThreadCount);
	void Final();
#if defined(_LOGSERVER) || defined(_CASHSERVER)
	void AddThreadCall(CSocketContext *pSocketContext);
	CSocketContext* WaitProcess(bool& bEndThread);
#else
	void AddThreadCall( UINT uiIndex );
	int WaitProcess( UINT uiThreadID );
#endif // #if defined(_LOGSERVER) || defined(_CASHSERVER)
	int GetWaitThreadCount();
};


#endif	// #if defined(_SERVER)