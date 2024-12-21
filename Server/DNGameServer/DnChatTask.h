#pragma once

#include "Task.h"
#include "GameListener.h"
#include "MultiSingleton.h"
#include "DNUserSession.h"

class CDnChatTask : public CTask,
					public CMultiSingleton<CDnChatTask, MAX_SESSION_COUNT>,
					public CGameListener, public TBoostMemoryPool <CDnChatTask>
{
public:
	CDnChatTask(CDNGameRoom * pRoom);
	virtual ~CDnChatTask ();

	bool Initialize();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual int OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char * pData, int iLen );
};
