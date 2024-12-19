
#pragma once

#include "DNUserEventHandler.h"

class CDNGameUserEventHandler:public CDNUserEventHandler,public TBoostMemoryPool<CDNGameUserEventHandler>
{
public:

	CDNGameUserEventHandler( CDNUserSession* pSession ):CDNUserEventHandler(pSession){}

	virtual void OnFinalize()
	{
		CDNUserEventHandler::OnFinalize();
	}
};