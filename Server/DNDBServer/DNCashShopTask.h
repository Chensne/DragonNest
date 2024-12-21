#pragma once

#include "DNMessageTask.h"

class CDNCashShopTask : public CDNMessageTask
{
public:
	CDNCashShopTask(CDNConnection* pConnection);
	virtual ~CDNCashShopTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
