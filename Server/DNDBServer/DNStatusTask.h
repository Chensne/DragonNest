#pragma once

#include "DNMessageTask.h"

class CDNStatusTask : public CDNMessageTask
{
public:
	CDNStatusTask(CDNConnection *pConnection);
	virtual ~CDNStatusTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
