#pragma once

#include "DNMessageTask.h"

class CDNPvPTask : public CDNMessageTask
{
public:
	CDNPvPTask(CDNConnection *pConnection);
	virtual ~CDNPvPTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
