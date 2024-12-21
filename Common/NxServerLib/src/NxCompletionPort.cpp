#include "stdafx.h"
#include "NxCompletionPort.h"
#include "NxAsyncEvent.h"
#include "NxWorkerThread.h"


NxCompletionPort::NxCompletionPort()
: m_hCompletionPort( INVALID_HANDLE_VALUE ), m_nWorkerThreadCnt(0)
{

}
	
NxCompletionPort::~NxCompletionPort()
{
	Destroy();
}


bool
NxCompletionPort::Create(int nThreadCnt, const char* szThreadNick /* = "Worker" */)
{
	if( m_hCompletionPort != INVALID_HANDLE_VALUE )
		return false;

	m_hCompletionPort = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, nThreadCnt );
	
	if( m_hCompletionPort == 0 )
	{
		m_hCompletionPort = INVALID_HANDLE_VALUE;
		return false;
	}

	m_nWorkerThreadCnt = nThreadCnt;

	for ( int i = 0 ; i < m_nWorkerThreadCnt ; i++ )
	{
		NxWorkerThread* pWorkerThead = new NxWorkerThread(this, szThreadNick);
		pWorkerThead->Start();
		m_WorkerThreadList.push_back(pWorkerThead);
	}
	

	return true;

}

bool
NxCompletionPort::Destroy()
{
	for ( int i = 0 ; i < m_nWorkerThreadCnt ; i++ )
	{
		PostCompletion(NULL, NULL);
	}

	for ( int i = 0 ; i < m_nWorkerThreadCnt ; i++ )
	{
		SAFE_DELETE( m_WorkerThreadList.at(i) )	;
	}
	m_WorkerThreadList.clear();

	if( m_hCompletionPort == INVALID_HANDLE_VALUE )
		return false;

	::CloseHandle( m_hCompletionPort );
	m_hCompletionPort = INVALID_HANDLE_VALUE;

	return true;

}

bool	
NxCompletionPort::AddCompletionPort(SOCKET hSocket, NxAsyncEventHandler* pHandler)
{
	HANDLE ret;

	ret = ::CreateIoCompletionPort( ( HANDLE )hSocket, m_hCompletionPort,
									(ULONG_PTR)pHandler, 0 );

	return ( ret != 0 );
}

NxAsyncEventHandler*
NxCompletionPort::GetCompletionEvent( NxAsyncEvent** ppEvent, int ms )
{
	if( ppEvent == 0 || m_hCompletionPort == INVALID_HANDLE_VALUE )
		return 0;

	NxAsyncEventHandler*	pHandler = 0;
	DWORD			nTransBytes = 0;
	ULONG			ulKey = 0;
	BOOL			bRet = FALSE;

	bRet = ::GetQueuedCompletionStatus( m_hCompletionPort, &nTransBytes, &ulKey, ( LPOVERLAPPED* )ppEvent, ms );
	pHandler = ( NxAsyncEventHandler* )ulKey;

	DWORD nErr = GetLastError();

	if( bRet == TRUE ) 
	{
		if ( ulKey != 0 && (*ppEvent) != 0  && nTransBytes != 0 )
		{
			( *ppEvent )->nTransBytes	= nTransBytes;
			( *ppEvent )->nError		= ::GetLastError();
			return pHandler;
		}
		else
		{
			int a = 0;
			
			
		}
	}
	else
	{
		DWORD nErr = GetLastError();
		if( ulKey != 0 && nErr != ERROR_OPERATION_ABORTED && *ppEvent != 0 )
		{
			( *ppEvent )->nEventType = NxAsyncEventType::Event_Close;
			( *ppEvent )->nError	 = ::GetLastError();
			return pHandler;
		}
	}
	
	return pHandler;
}

void
NxCompletionPort::PostCompletion( NxAsyncEventHandler* pHandler, NxAsyncEvent* pEvent )
{
	BOOL bResult = FALSE;
	if( pEvent != 0 )
	{
		bResult = ::PostQueuedCompletionStatus( m_hCompletionPort, pEvent->nTransBytes,
		( ULONG_PTR )( pHandler ), pEvent );
		
	}
	else
	{
		::PostQueuedCompletionStatus( m_hCompletionPort, 0,
		( ULONG_PTR )( pHandler ), 0 );
	}
}

