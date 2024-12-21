#include "StdAfx.h"
#include "DNConnectionManager.h"
#include "DNConnection.h"
#include "Util.h"

CDNConnectionManager* g_pConnectionManager = NULL;

CDNConnectionManager::CDNConnectionManager(void): CConnectionManager()
{

}

CDNConnectionManager::~CDNConnectionManager(void)
{
	
}

CConnection* CDNConnectionManager::AddConnection(const char *pIp, const USHORT nPort)
{
	CDNConnection *pCon = new CDNConnection;
	if (!pCon) return NULL;

	// 그냥 마스터 커넥션 ID발행..
	pCon->SetSessionID(g_IDGenerator.GetMasterConnectionID());
	pCon->SetIp(pIp);
	pCon->SetPort(nPort);

	PushConnection(pCon);

	return pCon;
}

