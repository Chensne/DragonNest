#pragma once

#include "DNMessageTask.h"

class CDNSkillTask : public CDNMessageTask
{
public:
	CDNSkillTask(CDNConnection *pConnection);
	virtual ~CDNSkillTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
