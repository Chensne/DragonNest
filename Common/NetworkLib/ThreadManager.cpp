#include "StdAfx.h"
#include "ThreadManager.h"

#if defined(_SERVER)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CThreadManager::CThreadManager(void): m_WaitThreadCount(0)
{
	m_Handles[0] = INVALID_HANDLE_VALUE;
	m_Handles[1] = INVALID_HANDLE_VALUE;
	m_WaitThreadCount = 0;

	Final();
}

CThreadManager::~CThreadManager(void)
{
	Final();
}

bool CThreadManager::Init(int nThreadCount)
{
	if (nThreadCount <= 0) nThreadCount = 1;

	Final();

	m_Handles[0] = CreateEvent(NULL, true, false, NULL);
	m_Handles[1] = CreateSemaphore(NULL, 0, 1, NULL);

	if (m_Handles[0] == INVALID_HANDLE_VALUE || m_Handles[1] == INVALID_HANDLE_VALUE) return false;

	HANDLE hThread;
	UINT nThreadID = 0;

	TThreadData *pThreadData = NULL;
	for (int i = 0; i < nThreadCount; i++){
		pThreadData = new TThreadData;
		if (!pThreadData) return false;

		pThreadData->nThreadID = i;
		pThreadData->pThreadManager = this;
#if !defined(_LOGSERVER) && !defined(_CASHSERVER)
		pThreadData->hEvent = CreateEvent( NULL, false, false, NULL );
#endif // #if !defined( _LOGSERVER )
#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
		pThreadData->nLastHearbeatTick = timeGetTime();
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)

		m_ThreadDataList.push_back(pThreadData);

		hThread = (HANDLE)_beginthreadex(NULL, 0, &WorkerThread, pThreadData, 0, &nThreadID);
#if defined( _DBSERVER )
		SetThreadPriorityBoost( hThread, true );
#endif // #if defined( _DBSERVER )
		if (hThread != INVALID_HANDLE_VALUE) m_hThreadList.push_back(hThread);
	}

	return true;
}

void CThreadManager::Final()
{
	m_ThreadLock.Lock();
	while(!m_ThreadCalls.empty()){
		m_ThreadCalls.pop();
	}
	m_ThreadLock.UnLock();

#if !defined(_LOGSERVER) && !defined(_CASHSERVER)
	if (!m_ThreadDataList.empty() )
	{
		list<TThreadData*>::iterator iter;
		for (iter = m_ThreadDataList.begin(); iter != m_ThreadDataList.end(); ++iter)
		{
			TThreadData* pThreadData = *iter;
			SetEvent( pThreadData->hEvent );
		}
	}

#endif // #if !defined( _LOGSERVER )

	if (!m_hThreadList.empty()){
		list<HANDLE>::iterator iter;
		for (iter = m_hThreadList.begin(); iter != m_hThreadList.end(); ++iter){
			SetEvent(m_Handles[0]);
		}

		for (iter = m_hThreadList.begin(); iter != m_hThreadList.end(); ++iter){
			WaitForSingleObject(*iter, INFINITE);
			CloseHandle(*iter);
		}
		m_hThreadList.clear();
	}

	TThreadData *pThreadData = NULL;
	if (!m_ThreadDataList.empty()){
		list<TThreadData*>::iterator iter;
		for (iter = m_ThreadDataList.begin(); iter != m_ThreadDataList.end(); ++iter){
			pThreadData = *iter;
#if !defined( _LOGSERVER ) && !defined(_CASHSERVER)
			if (pThreadData)
				CloseHandle( pThreadData->hEvent );
#endif // #if !defined( _LOGSERVER )
			SAFE_DELETE(pThreadData);
		}
		m_ThreadDataList.clear();
	}

	for (int i = 0; i < 2; i++){
		if (m_Handles[i] != INVALID_HANDLE_VALUE){
			CloseHandle(m_Handles[i]);
			m_Handles[i] = INVALID_HANDLE_VALUE;
		}
	}
}

#if defined(_LOGSERVER) || defined(_CASHSERVER)
CSocketContext* CThreadManager::WaitProcess(bool& bEndThread)
{
	DWORD nWait = 0;

	if (!m_ThreadCalls.empty()){
		m_ThreadLock.Lock();
		nWait = WAIT_OBJECT_0 + 1;
		m_ThreadLock.UnLock();
	}

	CSocketContext *pSocketContext = NULL;

	while(1){
		if (nWait == 0)
			nWait = WaitForMultipleObjects(2, m_Handles, false, INFINITE);

		switch(nWait)
		{
		case WAIT_OBJECT_0:
			{
				bEndThread = true;				
			}
			return NULL;
		case WAIT_OBJECT_0 + 1:
			{
				m_ThreadLock.Lock();
				if (m_ThreadCalls.empty()){
					nWait = 0; 
					m_ThreadLock.UnLock();
					continue;
				}

				pSocketContext = m_ThreadCalls.front();
				m_ThreadCalls.pop();
				m_ThreadLock.UnLock();

				return pSocketContext;
			}
		}
	}
	return NULL;
}
void CThreadManager::AddThreadCall(CSocketContext *pSocketContext)
{
	m_ThreadLock.Lock();
	m_ThreadCalls.push(pSocketContext);
	m_ThreadLock.UnLock();

	ReleaseSemaphore(m_Handles[1], 1, NULL);
}
#else
int CThreadManager::WaitProcess( UINT uiThreadID )
{
	while(1)
	{
		DWORD dwRet = WaitForMultipleObjects(2, m_Handles, false, 1 );

		switch(dwRet)
		{
			// 종료 이벤트
			case WAIT_OBJECT_0:
			{
				return -1;
			}
			// Queue 이벤트
			case WAIT_TIMEOUT:
			case WAIT_OBJECT_0 + 1:
			{
				m_ThreadLock.Lock();
				if (m_ThreadCalls.empty())
				{
					m_ThreadLock.UnLock();
				}
				else
				{
					UINT uiRet = m_ThreadCalls.front();
					bool bRet = (uiRet == uiThreadID); 
					if( bRet )
						m_ThreadCalls.pop();
					m_ThreadLock.UnLock();

					return  bRet ? uiThreadID : -2;
				}
				break;
			}
		}
	}
	return -1;
}

void CThreadManager::AddThreadCall( UINT uiIndex )
{
	m_ThreadLock.Lock();
	m_ThreadCalls.push(uiIndex);
	m_ThreadLock.UnLock();

	ReleaseSemaphore(m_Handles[1], 1, NULL);
}
#endif // #if defined(_LOGSERVER) || defined(_CASHSERVER)

int CThreadManager::GetWaitThreadCount()
{
	m_ThreadLock.Lock();
	int nCount = (int)m_ThreadCalls.size();
	m_ThreadLock.UnLock();

	return nCount;
}
#if defined(_LOGSERVER) || defined(_CASHSERVER)
UINT __stdcall CThreadManager::WorkerThread(void *pParam)
{
	TThreadData *pThreadData = (TThreadData*)pParam;

	CSocketContext *pSocketContext = NULL;
	CThreadManager *pThreadManager = pThreadData->pThreadManager;
#if defined(_MASTERSERVER) || defined(_DBSERVER) || defined(_LOGSERVER) || defined(_CASHSERVER)
	char buffer[PROCESSBUFFERLENMAX];
#else
	char buffer[WORKERBUFFERLENMAX];
#endif

	while (1){
		bool bEndThread = false;
		InterlockedIncrement(&pThreadManager->m_WaitThreadCount);
		pSocketContext = pThreadManager->WaitProcess(bEndThread);
		InterlockedDecrement(&pThreadManager->m_WaitThreadCount);

		if (pSocketContext == NULL) 
		{
			if( bEndThread ) // 쓰레드 종료
				break;
#if !defined(PRE_DEL_ONNULL)
#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
			pThreadManager->OnNulled(pThreadData->nThreadID, pThreadData->nLastHearbeatTick);
#else		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
			pThreadManager->OnNulled(pThreadData->nThreadID);
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
#if defined(_CASHSERVER)
			continue;
#else	// #if defined(_CASHSERVER)
			break;
#endif	// #if defined(_CASHSERVER)
#endif //#if !defined(PRE_DEL_ONNULL)
		}		
#if defined(PRE_DEL_ONNULL)
#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
		if( pSocketContext == reinterpret_cast<CSocketContext*>(reinterpret_cast<void*>(DNProcessCheck::HeartbeatKey)))		
		{
			pThreadManager->OnCalled(pSocketContext, buffer, pThreadData);		
			continue;
		}
#endif //#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
		if ((pSocketContext->AddRef() > 0) && (pSocketContext->m_Socket != INVALID_SOCKET)){
			pThreadManager->OnCalled(pSocketContext, buffer, pThreadData);
		}
#else //#if defined(PRE_DEL_ONNULL)
		if ((pSocketContext->AddRef() > 0) && (pSocketContext->m_Socket != INVALID_SOCKET)){
			pThreadManager->OnCalled(pSocketContext, buffer, pThreadData->nThreadID);
		}
#endif //#if defined(PRE_DEL_ONNULL)
	}

	_endthreadex(0);

	return 1;
}
#else
UINT __stdcall CThreadManager::WorkerThread(void *pParam)
{
	TThreadData *pThreadData = (TThreadData*)pParam;

	CSocketContext *pSocketContext = NULL;
	CThreadManager *pThreadManager = pThreadData->pThreadManager;
	
	while (1)
	{
		// 종료
		DWORD dwRet = ::WaitForSingleObject( pThreadData->hEvent, 1 );
		if( dwRet == WAIT_OBJECT_0 )
			break;

		//InterlockedIncrement(&pThreadManager->m_WaitThreadCount);
		//int iRet = pThreadManager->WaitProcess( pThreadData->nThreadID );
		//InterlockedDecrement(&pThreadManager->m_WaitThreadCount);

		// 종료
		//if( iRet == -1 )
		//	break;
		// 내쓰레드 이벤트 아님
		//if( iRet == -2 )
		//	continue;
#if defined(PRE_DEL_ONNULL)
		pThreadManager->OnCalled( NULL,NULL,pThreadData);
#else
		pThreadManager->OnCalled( NULL,NULL,pThreadData->nThreadID );
#endif
	}

	_endthreadex(0);

	return 1;
}
#endif // #if defined(_LOGSERVER) || defined(_CASHSERVER)

#if defined(PRE_DEL_ONNULL)
void CThreadManager::OnCalled(CSocketContext *pSocketContext, char *pBuffer, TThreadData* pThreadData)
{
}
#else
void CThreadManager::OnCalled(CSocketContext *pSocketContext, char *pBuffer, int nThreadID)
{
}
#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
void CThreadManager::OnNulled(int nThreadID, ULONG &nHeartBeatTick)
#else		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
void CThreadManager::OnNulled(int nThreadID)
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
{
}
#endif //#if !defined(PRE_DEL_ONNULL)

#endif	// #if defined(_SERVER)