#pragma once

#include "ConnectionManager.h"

class CDNConnection;
class CDNConnectionManager: public CConnectionManager
{
public:
	CDNConnectionManager(void);
	virtual ~CDNConnectionManager(void);

	CConnection* AddConnection(const char *pIp, const USHORT nPort);
};

extern CDNConnectionManager* g_pConnectionManager;
