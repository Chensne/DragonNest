#pragma once

#include "DNMessageTask.h"

class CDNSecondarySkillTask : public CDNMessageTask
{
public:
	CDNSecondarySkillTask(CDNConnection *pConnection);
	virtual ~CDNSecondarySkillTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
