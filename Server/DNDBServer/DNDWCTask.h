#pragma once

#include "DNMessageTask.h"

#if defined( PRE_ADD_DWC)

class CDNDWCTask : public CDNMessageTask
{
public:
	CDNDWCTask (CDNConnection* pConnection);
	virtual ~CDNDWCTask ();

public:
	virtual void OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) override;
};

#endif // #if defined( PRE_ADD_DWC )
