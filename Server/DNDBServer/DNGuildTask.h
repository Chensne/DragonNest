#pragma once

#include "DNMessageTask.h"

class CDNGuildTask : public CDNMessageTask
{
public:
	CDNGuildTask(CDNConnection* pConnection);
	virtual ~CDNGuildTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
