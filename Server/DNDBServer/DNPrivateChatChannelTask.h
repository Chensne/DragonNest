#pragma once

#include "DNMessageTask.h"

#if defined( PRE_PRIVATECHAT_CHANNEL )

class CDNPrivateChatChannelTask : public CDNMessageTask
{
public:

	CDNPrivateChatChannelTask(CDNConnection* pConnection);
	virtual ~CDNPrivateChatChannelTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override; 
};

#endif // #if defined( PRE_PRIVATECHAT_CHANNEL )
