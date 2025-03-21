#pragma once

#include "DNMessageTask.h"

class CDNDarkLairTask : public CDNMessageTask
{
public:
	CDNDarkLairTask(CDNConnection* pConnection);
	virtual ~CDNDarkLairTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};
