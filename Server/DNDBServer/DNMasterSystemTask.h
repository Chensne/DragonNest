#pragma once

#include "DNMessageTask.h"

class CDNMasterSystemTask : public CDNMessageTask
{
public:
	CDNMasterSystemTask(CDNConnection *pConnection);
	virtual ~CDNMasterSystemTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData);
};
