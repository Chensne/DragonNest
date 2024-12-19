
#pragma once

#include "DNUserEventHandler.h"

class CDNVillageUserEventHandler:public CDNUserEventHandler,public TBoostMemoryPool<CDNVillageUserEventHandler>
{
public:
	CDNVillageUserEventHandler( CDNUserSession* pSession ):CDNUserEventHandler(pSession){}
	
	virtual void OnFinalize()
	{
		CDNUserEventHandler::OnFinalize();
	}
};