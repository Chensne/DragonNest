#pragma once

#include "DNMessageTask.h"

class CDNMarketTask : public CDNMessageTask
{
public:
	CDNMarketTask(CDNConnection* pConnection);
	virtual ~CDNMarketTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
