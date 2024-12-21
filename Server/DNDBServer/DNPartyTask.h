#pragma once

#include "DNMessageTask.h"

#if defined( PRE_PARTY_DB )

class CDNPartyTask : public CDNMessageTask
{
public:

	CDNPartyTask(CDNConnection* pConnection);
	virtual ~CDNPartyTask();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};

#endif // #if defined( PRE_PARTY_DB )
