#include "Stdafx.h"
#include "DNAuthManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNGameDataManager.h"
#include "DNLogConnection.h"
#include "DNUserSession.h"

#if defined(_GAMESERVER)
extern TGameConfig g_Config;
#endif // #if defined(_GAMESERVER)

CDNAuthManager* g_pAuthManager = NULL;

CDNAuthManager::CDNAuthManager()
{
	
}

int CDNAuthManager::QueryStoreAuth(char cLastServerType, CDNUserSession *pUserSesion)
{
	pUserSesion->FinalizeEvent();

	// StoreAuth 는 반드시 기존 DB 패킷들과 직렬화가 이루어져야한다.
	if (pUserSesion->GetDBConnection()) 
		pUserSesion->GetDBConnection()->QueryStoreAuth(cLastServerType, pUserSesion);

	g_Log.Log(LogType::_NORMAL, pUserSesion, L"[Auth] StoreAuth (REQ) - nAccountDBID:%d (%s/%s), CertifyingKey:%I64d\r\n", 
		pUserSesion->GetAccountDBID(), pUserSesion->GetAccountName(), pUserSesion->GetCharacterName(), pUserSesion->GetCertifyingKey());

	return ERROR_NONE;
}

int CDNAuthManager::QueryCheckAuth(INT64 biCertifyingKey, CDNUserSession *pUserSesion)
{
	if (pUserSesion->GetDBConnection())
		pUserSesion->GetDBConnection()->QueryCheckAuth(biCertifyingKey, pUserSesion);

	g_Log.Log(LogType::_NORMAL, pUserSesion, L"[Auth] CheckAuth (REQ) - nAccountDBID:%d, CertifyingKey:%I64d, CerServerID:%d\r\n", pUserSesion->GetAccountDBID(), biCertifyingKey, GetServerID());

	return ERROR_NONE;
}

int CDNAuthManager::QueryCheckAuth(CDNDBConnection * pDBCon, INT64 biCertifyingKey, BYTE cThreadID, int nAccountDBID, int nSessionID)
{
	if (pDBCon)
		pDBCon->QueryCheckAuth(biCertifyingKey, cThreadID, nAccountDBID, nSessionID);

	g_Log.Log(LogType::_NORMAL, 0, nAccountDBID, 0, nSessionID, L"[Auth] CheckAuth (REQ) - nAccountDBID:%d, CertifyingKey:%I64d, CerServerID:%d\r\n", nAccountDBID, biCertifyingKey, GetServerID());

	return ERROR_NONE;
}

int CDNAuthManager::QueryResetAuth(char cWorldSetID, UINT nAccountDBID, UINT nSessionID)
{
	BYTE cThreadID;
	CDNDBConnection *pDBCon = static_cast<CDNDBConnection*>(g_pDBConnectionManager->GetDBConnection(cThreadID));
	if (pDBCon){
		pDBCon->QueryResetAuth(cThreadID, nAccountDBID, nSessionID);
	}

#if defined(_GAMESERVER)
	g_Log.Log(LogType::_NORMAL, cWorldSetID, nAccountDBID, 0, nSessionID, L"[Auth][%d] ResetAuth (REQ) - nAccountDBID:%d\r\n", g_Config.nManagedID, nAccountDBID);
#else
	g_Log.Log(LogType::_NORMAL, cWorldSetID, nAccountDBID, 0, nSessionID, L"[Auth] ResetAuth (REQ) - nAccountDBID:%d\r\n", nAccountDBID);
#endif // #if defined(_GAMESERVER)

	return ERROR_NONE;
}

int CDNAuthManager::QueryResetAuthServer()
{
	int nServerID = GetServerID();
	if (!nServerID) {
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	BYTE cThreadID;
	CDNDBConnection *pDBCon = static_cast<CDNDBConnection*>(g_pDBConnectionManager->GetDBConnection(cThreadID));
	if (pDBCon) {
		pDBCon->QueryResetAuthServer(cThreadID,GetServerID());
	}

	return ERROR_NONE;
}

int CDNAuthManager::QueryResetAuthServerClose()
{
	g_Log.Log(LogType::_NORMAL, L"QueryResetAuthServerClose %d \r\n", GetServerID());

	return QueryResetAuthServer();
}

