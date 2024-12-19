#include "stdafx.h"
#include "NxWorkerThread.h"
#include "NxCompletionPort.h"
#include "NxAsyncEvent.h"

NxWorkerThread::NxWorkerThread(NxCompletionPort* pCompletionHandler, const char* szThreadNick)
:NxThread(szThreadNick), m_pCompletionHandler(pCompletionHandler)
{

}

NxWorkerThread::~NxWorkerThread()
{
	m_bRunning = false;
	WaitForTerminate(1000);
}

void 
NxWorkerThread::Run()
{
	NxAsyncEventHandler*	pHandler = NULL;
	NxAsyncEvent*	pEvent = NULL;

	while( m_bRunning )
	{
		pHandler = m_pCompletionHandler->GetCompletionEvent( &pEvent, INFINITE );
	
		if( pHandler == NULL )
		{
			break;
		}
		else
		{
			pHandler->HandleEvent( pEvent );
		}
	}
}