#pragma once

#include "ThreadManager.h"

class CDNThreadManager: public CThreadManager
{
protected:
	
#if defined(PRE_DEL_ONNULL)
	void DoUpdate(int nThreadID, ULONG &nHeartBeatTick);
	void OnCalled(CSocketContext *pSocketContext, char *pBuffer, TThreadData* tThreadData);
#else //#if defined(PRE_DEL_ONNULL)
	void OnCalled(CSocketContext *pSocketContext, char *pBuffer, int nThreadID);
#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)	
	void OnNulled(int nThreadID, ULONG &nHeartBeatTick);
#else		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
	void OnNulled(int nThreadID);
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL) && defined(_CASHSERVER)
#endif //#if defined(PRE_DEL_ONNULL)

public:
	CDNThreadManager(void);
	virtual ~CDNThreadManager(void);
};

extern CDNThreadManager *g_pThreadManager;
