#pragma once

#include "DNMessageTask.h"

#if defined( PRE_ADD_STAMPSYSTEM )

class CDNStampSystemTask : public CDNMessageTask
{
public:
	CDNStampSystemTask(CDNConnection* pConnection);
	virtual ~CDNStampSystemTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};

#endif // #if defined( PRE_ADD_STAMPSYSTEM )
