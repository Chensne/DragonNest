#pragma once

#include "DNMessageTask.h"

class CDNIsolateTask : public CDNMessageTask
{
public:
	CDNIsolateTask(CDNConnection* pConnection);
	virtual ~CDNIsolateTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
