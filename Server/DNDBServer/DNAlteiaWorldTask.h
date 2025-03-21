#pragma once

#include "DNMessageTask.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDNAlteiaWorldTask : public CDNMessageTask
{
public:

	CDNAlteiaWorldTask(CDNConnection* pConnection);
	virtual ~CDNAlteiaWorldTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override; 
};

#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )