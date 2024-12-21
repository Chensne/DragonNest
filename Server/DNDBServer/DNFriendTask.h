#pragma once

#include "DNMessageTask.h"

class CDNFriendTask : public CDNMessageTask
{
public:
	CDNFriendTask(CDNConnection* pConnection);
	virtual ~CDNFriendTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
