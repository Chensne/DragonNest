#pragma once

#include "DNMessageTask.h"

class CDNAuthTask : public CDNMessageTask
{
public:
	CDNAuthTask(CDNConnection* pConnection);
	virtual ~CDNAuthTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
