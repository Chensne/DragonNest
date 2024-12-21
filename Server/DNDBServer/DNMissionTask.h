#pragma once

#include "DNMessageTask.h"

class CDNMissionTask : public CDNMessageTask
{
public:
	CDNMissionTask(CDNConnection* pConnection);
	virtual ~CDNMissionTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
