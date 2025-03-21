#pragma once

#include "DNMessageTask.h"

class CDNJobSystemTask : public CDNMessageTask
{
public:
	CDNJobSystemTask(CDNConnection* pConnection);
	virtual ~CDNJobSystemTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
