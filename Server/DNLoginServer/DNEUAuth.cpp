#include "StdAfx.h"
#include "DNEUAuth.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"

#if defined(_EU) && defined(_FINAL_BUILD)

CDNEUAuth *g_pEUAuth = NULL;

void AuthenCallback(unsigned __int64 nSessionId, int nResult, const char *pszUsername, const char *pszUserId, const char *pszUserCountry, const char *pszStartDate, const char *pszEndDate)
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif

	int nSession = (UINT)nSessionId;
	CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(nSession));
	if (!pUserCon){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, nSession, L"[SID:%u] UserCon NULL Result:%d \r\n", nSession);
		return;
	}
	g_Log.Log(LogType::_ERROR, 0, 0, 0, nSession, L"AuthenCallback SessionID:%d Result:%d %S %S %S\r\n", nSession, nResult, pszUsername, pszUserId, pszUserCountry);

	pUserCon->CheckLoginEU(nResult, pszUsername);
}

CDNEUAuth::CDNEUAuth(void)
{
}

CDNEUAuth::~CDNEUAuth(void)
{
}

bool CDNEUAuth::Init()
{
	if (SDGASInit(AuthenCallback) == FALSE){
		g_Log.Log(LogType::_FILEDBLOG, L"SDGASInit Fail!!\r\n");
		return false;
	}

	return true;
}

void CDNEUAuth::Authen(unsigned __int64 nSessionId, const char *pszUsername, const char *pszPassword, const char *pszClientip)
{
	SDGASAuthen(nSessionId, pszUsername, pszPassword, pszClientip);
}

void CDNEUAuth::Terminate()
{
	SDGASTerminate();
}

#endif	// #if defined(_EU)