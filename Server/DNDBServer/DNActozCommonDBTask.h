#pragma once

#include "DNMessageTask.h"

#if defined(_KRAZ)

class CDNActozCommonDBTask : public CDNMessageTask
{
public:
	CDNActozCommonDBTask(CDNConnection* pConnection);
	virtual ~CDNActozCommonDBTask(void);

	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};

#endif	// #if defined(_KRAZ)