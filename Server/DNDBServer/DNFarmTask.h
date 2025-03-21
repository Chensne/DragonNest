#pragma once

#include "DNMessageTask.h"

class CDNFarmTask : public CDNMessageTask
{
public:
	CDNFarmTask(CDNConnection* pConnection);
	virtual ~CDNFarmTask();

public:
	void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
