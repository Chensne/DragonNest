#pragma once

#include "DNMessageTask.h"

class CDNAppellationTask : public CDNMessageTask
{
public:
	CDNAppellationTask(CDNConnection* pConnection);
	virtual ~CDNAppellationTask();

public:
	virtual void OnRecvMessage(int nThread, int nMainCmd, int nSubCmd, char* pData) override;
};
