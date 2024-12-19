#pragma once

#include "AuthManager.h"
#include "MultiSync.h"

class CDNUserConnection;

class CDNAuthManager : public CAuthManager
{
public:
	CDNAuthManager();

	int QueryStoreAuth(char cLastServerType, CDNUserConnection *pUserCon);
	int QueryResetAuth(char cWorldID, UINT nAccountDBID, UINT nSessionID);

	int QuerySetWorldIDAuth(char cWorldID, UINT nAccountDBID);
	int QueryResetAuthServerClose();

private:
	int QueryResetAuthServer();
};

extern CDNAuthManager* g_pAuthManager;


