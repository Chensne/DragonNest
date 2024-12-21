#include "StdAfx.h"
#include "DNShandaFCM.h"
#include "DNDivisionManager.h"
#include "DNUser.h"
#include "Log.h"

extern TMasterConfig g_Config;

#if defined(_CH) && defined(_FINAL_BUILD)

CDNShandaFCM *g_pShandaFCM = NULL;

int FCM_CallBack(const char* szUserID, const char* szRoleID, int iOnlineMinutes, int iOfflineMinutes)
{
	g_Log.Log(LogType::_NORMAL, L"[FCM_CallBack] szUserID:%S szRoleID:%S iOnlineMinutes:%d iOfflineMinutes:%d\r\n", szUserID, szRoleID, iOnlineMinutes, iOfflineMinutes);

	WCHAR wszCharName[NAMELENMAX] = { 0, };
	MultiByteToWideChar(CP_ACP, 0, szRoleID, -1, wszCharName, NAMELENMAX);
	CDNUser *pUser = g_pDivisionManager->GetUserByName(wszCharName);
	if (!pUser) return 1;

	pUser->SetFCMOnlineMin(iOnlineMinutes);

	return 1;
}

void FCM_Switch(int needFCM)
{
	g_Config.bFCM = needFCM ? true : false;
	g_Log.Log(LogType::_NORMAL, L"[FCM_Switch] needFCM:%d \r\n", needFCM);
}

void FCM_QueryCallBack(const char* szUserID, const char* szRoleID, int iState, int iOnlineMinutes, int iOfflineMinutes)
{
	g_Log.Log(LogType::_NORMAL, L"[FCM_QueryCallBack] szUserID:%S szRoleID:%S iState:%d iOnlineMinutes:%d iOfflineMinutes:%d\r\n", szUserID, szRoleID, iState, iOnlineMinutes, iOfflineMinutes);

	WCHAR wszCharName[NAMELENMAX] = { 0, };
	MultiByteToWideChar(CP_ACP, 0, szRoleID, -1, wszCharName, NAMELENMAX);
	CDNUser *pUser = g_pDivisionManager->GetUserByName(wszCharName);
	if (!pUser) return;

	pUser->SetFCMOnlineMin(iOnlineMinutes);
}

CDNShandaFCM::CDNShandaFCM(void)
{
}

CDNShandaFCM::~CDNShandaFCM(void)
{
}

bool CDNShandaFCM::InitService()
{
	// 피로도 시스템
	m_pFCM = CreateSDKInstance();
	if (!m_pFCM){
		g_Log.Log(LogType::_FILEDBLOG, L"Shanda FCM CreateSDKInstance Fail!!\r\n");
		return false;
	}

	if (!m_pFCM->initFCMSDKEASY(&FCM_CallBack, &FCM_Switch, &FCM_QueryCallBack)){
		g_Log.Log(LogType::_FILEDBLOG, L"Shanda FCM initFCMSDKEASY Fail!!\r\n");
		return false;
	}

	g_Log.Log(LogType::_FILEDBLOG, L"CDNShandaFCM::InitService OK!!\r\n");
	return true;
}

int CDNShandaFCM::FCMQueryState(const char* szUserID, const char* szRoleID, const char *IPAddr, int *OnlineTimeVal, int *OfflineTimeVal)
{
	return m_pFCM->FCMQueryState(szUserID, szRoleID, IPAddr, OnlineTimeVal, OfflineTimeVal);
}

int CDNShandaFCM::FCMQueryStateNoBlock(const char* szUserID, const char* szRoleID, const char *IPAddr)
{
	return m_pFCM->FCMQueryStateNoBlock(szUserID, szRoleID, IPAddr);
}

int CDNShandaFCM::FCMIDOnline(const char* szUserID, const char* szRoleID, const char *IPAddr)
{
	g_Log.Log(LogType::_NORMAL, L"[FCMIDOnline] szUserID:%S szRoleID:%S Ip:%S\r\n", szUserID, szRoleID, IPAddr);
	return m_pFCM->FCMIDOnline(szUserID, szRoleID, IPAddr);
}

int CDNShandaFCM::FCMIDOffline(const char* szUserID, const char* szRoleID, const char *IPAddr)
{
	g_Log.Log(LogType::_NORMAL, L"[FCMIDOffline] szUserID:%S szRoleID:%S Ip:%S\r\n", szUserID, szRoleID, IPAddr);
	return m_pFCM->FCMIDOffline(szUserID, szRoleID, IPAddr);
}

#endif	// _CH


