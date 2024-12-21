#pragma once

#include "DNMessageTask.h"

class CDNEtcTask : public CDNMessageTask
{
public:
	CDNEtcTask(CDNConnection* pConnection);
	virtual ~CDNEtcTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
