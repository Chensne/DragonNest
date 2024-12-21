#pragma once

#include "DNMessageTask.h"

#if defined( PRE_ADD_BESTFRIEND )

class CDNBestFriendTask : public CDNMessageTask
{
public:

	CDNBestFriendTask(CDNConnection* pConnection);
	virtual ~CDNBestFriendTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};

#endif // #if defined( PRE_ADD_BESTFRIEND )
