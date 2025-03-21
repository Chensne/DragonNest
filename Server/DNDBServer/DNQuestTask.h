#pragma once

#include "DNMessageTask.h"

class CDNQuestTask : public CDNMessageTask
{
public:
	CDNQuestTask(CDNConnection *pConnection);
	virtual ~CDNQuestTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
