#pragma once

#include "DNMessageTask.h"

class CDNGuildRecruitSystemTask : public CDNMessageTask
{
public:
	CDNGuildRecruitSystemTask(CDNConnection* pConnection);
	virtual ~CDNGuildRecruitSystemTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
