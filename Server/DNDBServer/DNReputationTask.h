#pragma once

#include "DNMessageTask.h"

class CDNReputationTask : public CDNMessageTask
{
public:
	CDNReputationTask(CDNConnection* pConnection);
	virtual ~CDNReputationTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
