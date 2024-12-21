#pragma once

#include "DNMessageTask.h"

class CDNAdjustTask : public CDNMessageTask
{
public:
	CDNAdjustTask(CDNConnection* pConnection);
	virtual ~CDNAdjustTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
