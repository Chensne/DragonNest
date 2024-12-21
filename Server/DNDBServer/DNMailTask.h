#pragma once

#include "DNMessageTask.h"

class CDNMailTask : public CDNMessageTask
{
public:
	CDNMailTask(CDNConnection* pConnection);
	virtual ~CDNMailTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
