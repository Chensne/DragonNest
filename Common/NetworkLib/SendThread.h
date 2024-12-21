#pragma once

#if defined(_SERVER)

#include "Thread.h"

class CSocketContext;

class CSendThread:public Thread
{
public:

	CSendThread();
	virtual ~CSendThread();

	void			Run();	
	void			PushSendQueue( CSocketContext* pSocketContext );

private:
	
	CSocketContext*	PopSendQueue();
	void			FlushAllSendQueue();

	HANDLE						m_SendQueueEvent;
	CSyncLock					m_SendQueueLock;
	std::queue<CSocketContext*>	m_SendQueue;
};

#endif	// #if defined(_SERVER)