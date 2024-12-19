#pragma once
#include <vector>
#include "NxSocket.h"

class NxWorkerThread;
class NxAsyncEventHandler;


class NxCompletionPort
{
public:
	NxCompletionPort();
	virtual ~NxCompletionPort();

	bool	Create(int nThreadCnt, const char* szThreadNick = "Worker" );
	bool	Destroy();

	bool	AddCompletionPort(SOCKET hSocket, NxAsyncEventHandler* pHandler);


	NxAsyncEventHandler*	GetCompletionEvent( NxAsyncEvent** ppEvent, int ms );
	void					PostCompletion( NxAsyncEventHandler* pHandler, NxAsyncEvent* pEvent );

private:
	
	HANDLE	m_hCompletionPort;
	int		m_nWorkerThreadCnt;
	std::vector<NxWorkerThread*>	m_WorkerThreadList;
};

