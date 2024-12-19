#pragma once

#include "ThreadManager.h"

class CDNThreadManager: public CThreadManager
{
protected:
#if defined(PRE_DEL_ONNULL)
	void OnCalled(CSocketContext *pSocketContext, char *pBuffer, TThreadData* pThreadData);	
#else
	void OnCalled(CSocketContext *pSocketContext, char *pBuffer, int nThreadID);
#endif

public:
	CDNThreadManager(void);
	virtual ~CDNThreadManager(void);
};

extern CDNThreadManager g_ThreadManager;
