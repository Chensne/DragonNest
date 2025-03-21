#pragma once

#include "DNMessageTask.h"

class CDNItemTask : public CDNMessageTask
{
public:
	CDNItemTask(CDNConnection* pConnection);
	virtual ~CDNItemTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
