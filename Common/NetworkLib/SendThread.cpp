#include "Stdafx.h"
#include "SendThread.h"
#include "Connection.h"

#if defined(_SERVER)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CSendThread::CSendThread():Thread( "SendThread" )
{
	m_SendQueueEvent = CreateEvent( NULL, false, false, NULL );
}

CSendThread::~CSendThread()
{
	m_bRunning = false;
	WaitForTerminate( INFINITE );

	FlushAllSendQueue();

	CloseHandle( m_SendQueueEvent );
	m_SendQueueEvent = NULL;
}

void CSendThread::PushSendQueue( CSocketContext* pSocketContext )
{
	ScopeLock<CSyncLock> Lock( m_SendQueueLock );

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
	if( Scope.bIsDelete() )
		return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	// SendQueue
	CConnection* pCon = static_cast<CConnection*>(pSocketContext->GetParam());
	if (pCon == NULL) return;

	if( pCon->bIsUserSession() && pCon->bIsPushSendQueue() )
		return;
	
	m_SendQueue.push( pSocketContext );

	pCon->SetPushSendQueue( true );

	SetEvent( m_SendQueueEvent );
}

CSocketContext*	CSendThread::PopSendQueue()
{
	// 함수호출전에 empty() 체크 한다.
	ScopeLock<CSyncLock> Lock( m_SendQueueLock );

	CSocketContext* pSocketContext = m_SendQueue.front();
	m_SendQueue.pop();

#if !defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	// SendQueue
	CConnection* pCon = static_cast<CConnection*>(pSocketContext->GetParam());
	if (pCon)
		pCon->SetPushSendQueue( false );
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	return pSocketContext;
}

// SendThread 종료 전에 SendQueue 모두 비워준다.
void CSendThread::FlushAllSendQueue()
{
	ScopeLock<CSyncLock> Lock( m_SendQueueLock );

	while( !m_SendQueue.empty() )
	{
		CSocketContext* pSocketContext = m_SendQueue.front();
		m_SendQueue.pop();

		CConnection* pCon = static_cast<CConnection*>(pSocketContext->GetParam());
		if( pCon && pSocketContext->m_Socket != INVALID_SOCKET && !pCon->GetDelete() )
		{
			pCon->FlushSendData();
		}
	}
}

// SendThread MainLoop
void CSendThread::Run()
{
	while( m_bRunning )
	{
		if( !m_SendQueue.empty() )
		{
			CConnection* pDeleteCon = NULL;
			{
				CSocketContext* pSocketContext = PopSendQueue();
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
				CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
				if( Scope.bIsDelete() )
					continue;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

				CConnection* pCon = static_cast<CConnection*>(pSocketContext->GetParam());
				if( pCon && pSocketContext->m_Socket != INVALID_SOCKET && !pCon->GetDelete() )
				{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
					pCon->SetPushSendQueue( false );
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

					int nRet = pCon->FlushSendData();
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
					if( nRet == IN_DISCONNECT )
						pDeleteCon = pCon;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

					if (nRet > 0)
					{
						PushSendQueue( pSocketContext );
					}
				}
			}
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			if( pDeleteCon )
			{
				pDeleteCon->OnDisconnected();
			}
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		}
		else
		{
			WaitForSingleObject( m_SendQueueEvent, 100 );
		}
	}
}

#endif	// #if defined(_SERVER)