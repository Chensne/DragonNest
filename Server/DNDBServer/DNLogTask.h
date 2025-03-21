#pragma once

#include "DNMessageTask.h"

class CDNLogTask : public CDNMessageTask
{
public:
	CDNLogTask(CDNConnection* pConnection);
	virtual ~CDNLogTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
