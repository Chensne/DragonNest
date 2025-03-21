#pragma once

#include "NxThread.h"

class NxWorkerThread : public NxThread
{
	friend class NxCompletionPort;

public:

	virtual	void Run();

private:
	NxWorkerThread(NxCompletionPort* pCompletionHandler, const char* szThreadNick = "Worker" );
	virtual ~NxWorkerThread();

	NxCompletionPort*	m_pCompletionHandler;
	int					m_nWaitCompletionTime;


};