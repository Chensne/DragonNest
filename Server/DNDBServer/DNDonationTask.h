#pragma once

#include "DNMessageTask.h"

class CDNDonationTask : public CDNMessageTask
{
public:
	CDNDonationTask(CDNConnection* pConnection);
	virtual ~CDNDonationTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
