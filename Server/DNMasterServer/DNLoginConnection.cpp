#include "StdAfx.h"
#include "DNLoginConnection.h"
#include "DNVillageConnection.h"
#include "DNIocpManager.h"
#include "DNDivisionManager.h"
#include "DNUser.h"
#include "DNExtManager.h"
#include "Log.h"
#include "DNWaitUserManager.h"

#if defined(_KR)
#include "DNNexonAuth.h"
#elif defined (_JP) && defined (WIN64)
#include "DNNHNNetCafe.h"
#elif defined(_TH)
#include "DNAsiaSoftPCCafe.h"
#elif defined(_ID)
#include "DNKreonPCCafe.h"
#endif	// _KR
#if defined(_WORK)
#include "PsUpdater.h"
#endif	// #if defined(_WORK)

extern TMasterConfig g_Config;

CDNLoginConnection::CDNLoginConnection(void): m_dwReconnectTick(0),m_nServerID(0), m_nCurConCount(0), m_nMaxConCount(0)
{
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)
}

CDNLoginConnection::~CDNLoginConnection(void)
{
}

void CDNLoginConnection::Reconnect(DWORD CurTick)
{
	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;

		if (!GetActive() && !GetConnecting() && m_wPort	&& g_Config.nWorldSetID > 0)
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_LOGIN, m_szIP, m_wPort) < 0)
			{
				SetConnecting(false);
#if !defined( STRESS_TEST )
				g_Log.Log(LogType::_FILEDBLOG, L"LoginServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
#endif
			}
			else 
			{
				g_Log.Log(LogType::_FILEDBLOG, L"LoginServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}
	}
}

int CDNLoginConnection::MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	switch (iMainCmd)
	{
	case LOMA_REGIST:
		{
/*#if defined(_WORK)
			if (!g_PsUpdater)
				g_PsUpdater = new(std::nothrow) CPsUpdater;

			g_PsUpdater->AsyncUpdate ();
#endif	// #if defined(_WORK)
			*/
			LOMARegist *pRegist = (LOMARegist*)pData;

			m_nServerID = pRegist->nServerID;

			SendRegistWorldID();
			g_pDivisionManager->SendVillageInfo();
			g_pDivisionManager->SendFarmInfo();

			g_Log.Log(LogType::_FILEDBLOG, g_Config.nWorldSetID, 0, 0, 0, L"[LOMA_REGIST] WorldID:%d\r\n", g_Config.nWorldSetID);
			return ERROR_NONE;
		}
		break;

	case LOMA_ADDUSER:
		{
			LOMAAddUser *pPacket = (LOMAAddUser*)pData;
			return OnRecvAdduser(pPacket, false);
		}

#if !defined(PRE_MOD_SELECT_CHAR)

	case LOMA_USERINFO:
		{
			LOMAUserInfo *pUserInfo = (LOMAUserInfo*)pData;

			if (pUserInfo->bTutorial){
				int nGameServerID = 0;
				CDNGameConnection* pGameCon = g_pDivisionManager->GetFreeGameConnection(&nGameServerID, REQINFO_TYPE_SINGLE);
				if( !pGameCon ){
					SendUserInfo(pUserInfo->nAccountDBID, pUserInfo->nSessionID, ERROR_GENERIC_GAMECON_NOT_FOUND);
					return ERROR_NONE;
				}
			}

			if (!g_pDivisionManager->GetVillageConnectionByVillageID(pUserInfo->cVillageID)){
				SendUserInfo(pUserInfo->nAccountDBID, pUserInfo->nSessionID, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
				g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, pUserInfo->nAccountDBID, 0, pUserInfo->nSessionID, L"ADBID:%u [LOMA_USERINFO] VillageCon not found (%s, %s)\r\n", pUserInfo->nAccountDBID, pUserInfo->wszAccountName, pUserInfo->wszCharacterName);
				return ERROR_GENERIC_UNKNOWNERROR;
			}

			BYTE szTempMID[MACHINEIDMAX];
			memset(szTempMID, 0, sizeof(szTempMID));
			DWORD dwGRC = 0;
			if (g_pDivisionManager->SetUserInfo(pUserInfo, 0, 0, szTempMID, dwGRC) == true)
			{
#if defined(_KR) && defined(_FINAL_BUILD)	// nexon pc bang
				CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pUserInfo->nAccountDBID);
				if (pUser)
					g_pNexonAuth->SendLogin(pUser);

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
				char szTempAccountName[IDLENMAX * 2];
				WideCharToMultiByte(CP_ACP, 0, pUserInfo->wszAccountName, -1, szTempAccountName, IDLENMAX * 2, NULL, NULL);

				bool bTempNetCafe = false;
				char szNHNNetCafeCode[NHNNETCAFECODEMAX];
				char szNHNProductCode[NHNNETCAFECODEMAX];

				if (g_pNHNNetCafe->NetCafe_UserLogin(szTempAccountName, pUserInfo->szIP, bTempNetCafe))
				{
					if (bTempNetCafe)
					{
						if (g_pNHNNetCafe->NetCafe_GetNetCafeCode(szTempAccountName, szNHNNetCafeCode, sizeof(szNHNNetCafeCode)) == false)
							memset(szNHNNetCafeCode, 0, sizeof(szNHNNetCafeCode));

						if (g_pNHNNetCafe->NetCafe_GetProductCode(szTempAccountName, szNHNProductCode, sizeof(szNHNProductCode)) == false)
							memset(szNHNProductCode, 0, sizeof(szNHNProductCode));

						if (g_pDivisionManager->SetNHNNetCafeInfo(pUserInfo->nAccountDBID, bTempNetCafe, szNHNNetCafeCode, szNHNProductCode) == false)
							_DANGER_POINT();
					}
				}
#elif defined(_TH) && defined(_FINAL_BUILD)
				CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pUserInfo->nAccountDBID);
				if (pUser)								
					g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 1, pUser->GetAccountDBID() );				
#elif defined(_ID) && defined(_FINAL_BUILD)
				CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pUserInfo->nAccountDBID);
				if (pUser)					
					g_pKreonPCCafe->AddProcessCall(pUser->GetAccountDBID());			
#endif	// _KR
				g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, pUserInfo->nAccountDBID, 0, pUserInfo->nSessionID, L"ADBID:%u [LOMA_USERINFO] %s, %s\r\n", pUserInfo->nAccountDBID, pUserInfo->wszAccountName, pUserInfo->wszCharacterName);
				SendUserInfo(pUserInfo->nAccountDBID, pUserInfo->nSessionID, ERROR_NONE);
				return ERROR_NONE;
			}
			else
			{
				SendUserInfo(pUserInfo->nAccountDBID, pUserInfo->nSessionID, ERROR_GENERIC_USER_NOT_FOUND);

				//이경우 각 서버에 유저가 남아 있을 수 있습니다. 클라이언트는 로그인에서 월드에 진입하였고 채널을 고른상태임 로그인에서 끊기게 처리하고 각 서버 컨넥션을 정리한번 합니다.
				//정상적인 상태인경우 여기에 진입하면 안됩니다. 빌리지 또는 게임서버에서 정상적으로 끊김처리가 발생하지 않은 겁니다.
				g_pDivisionManager->SendDetachUser(pUserInfo->nAccountDBID);
				g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, pUserInfo->nAccountDBID, 0, pUserInfo->nSessionID, L"ADBID:%u [LOMA_USERINFO] FAIL!!!! %s, %s\r\n", pUserInfo->nAccountDBID, pUserInfo->wszAccountName, pUserInfo->wszCharacterName);
				return ERROR_GENERIC_UNKNOWNERROR;
			}
			return ERROR_NONE;
		}
		break;

#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	case LOMA_DETACHUSER:
		{
			LOMADetachUser *pDetach = (LOMADetachUser*)pData;
			bool bIsReconnectLogin = pDetach->cIsReconnectLogin == 1 ? true : false;

			BYTE cSelectedLanguage = MultiLanguage::eDefaultLanguage;
#ifdef PRE_ADD_MULTILANGUAGE
			CDNUser * pLangUser = g_pDivisionManager->GetUserByAccountDBID(pDetach->nAccountDBID);
			if (pLangUser)
				cSelectedLanguage = pLangUser->m_eSelectedLanguage;

#endif		//#ifdef PRE_ADD_MULTILANGUAGE
			bool bRet = g_pDivisionManager->SendDetachUser(pDetach->nAccountDBID, bIsReconnectLogin, pDetach->bIsDuplicate, pDetach->nSessionID);
			// 존재하지 않는 유저임..
			if( bRet == false)
				SendDuplicateLogin(pDetach->nAccountDBID, pDetach->nSessionID);


#if !defined(PRE_MOD_SELECT_CHAR)
			if (bIsReconnectLogin)
			{
				BYTE szTempMID[MACHINEIDMAX];
				memset(szTempMID, 0, sizeof(szTempMID));
				DWORD dwGRC = 0;
#if defined(_KR)
				memcpy(szTempMID, pDetach->szMID, sizeof(szTempMID));
				dwGRC = pDetach->dwGRC;
#endif	// #if defined(_KR)

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
				g_pDivisionManager->AddUser(GetServerID(), pDetach->nAccountDBID, szTempMID, dwGRC, pDetach->szIP, cSelectedLanguage, pDetach->cAccountLevel);
#else		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
				g_pDivisionManager->AddUser(GetServerID(), pDetach->nAccountDBID, szTempMID, dwGRC, NULL, cSelectedLanguage, pDetach->cAccountLevel);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
			}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

			g_pWaitUserManager->DelWaitUser(GetServerID(), pDetach->nAccountDBID);
			return ERROR_NONE;
		}
		break;

	case LOMA_REQTUTORIALGAMEID:	// 게임서버 정보 얻기
		{
			LOMAReqTutorialGameID* pPacket = reinterpret_cast<LOMAReqTutorialGameID*>(pData);
			if( g_pDivisionManager->RequestGameRoom( REQINFO_TYPE_SINGLE, pPacket->nAccountDBID, timeGetTime(), pPacket->nTutorialMapIndex, pPacket->cTutorialGateNo, 0, true, true, GetServerID()) == false) {
				SendSetTutorialGameID( pPacket->nSessionID, 0, 0, 0, 0, 0, 0, ERROR_GENERIC_GAMECON_NOT_FOUND );
				// _DANGER_POINT();
			}
			return ERROR_NONE;
		}
		break;

	case LOMA_CONCOUNTINFO:
		{
			// 20091230 MA LO 병렬화 변경부분 !!!	// 완료 !!!

			const LOMAConCountInfo *pConCountInfo = reinterpret_cast<LOMAConCountInfo*>(pData);
			DN_ASSERT(NULL != pConCountInfo,					"Invalid!");
			DN_ASSERT(pConCountInfo->nServerID == m_nServerID,	"Invalid!");

			m_nCurConCount = pConCountInfo->nCurConCount;
			m_nMaxConCount = pConCountInfo->nMaxConCount;
			return ERROR_NONE;
		}
		break;

	case LOMA_ADDWAITUSER:
		{
			LOMAAddWaitUser * pPacket = (LOMAAddWaitUser*)pData;

			USHORT nTicketNum = 0, nEstimateCalcTime = 0;
			short nRet = (short)g_pWaitUserManager->AddWaitUser(pPacket->nServerID, pPacket->nAccountDBID, nTicketNum, nEstimateCalcTime);
			SendWaitUserResult(pPacket->nAccountDBID, nTicketNum, nRet);

			return ERROR_NONE;
		}
		break;

	case LOMA_DELWAITUSER:
		{
			LOMADelWaitUser * pPacket = (LOMADelWaitUser*)pData;
			g_pWaitUserManager->DelWaitUser(pPacket->nServerID, pPacket->nAccountDBID);
			return ERROR_NONE;
		}
		break;

	case LOMA_DETACHUSEROTHER:
		{
			LOMADetachUserOther* pPacket = (LOMADetachUserOther*)pData;
			g_pDivisionManager->SendLoginUserDetach(pPacket->nAccountDBID, pPacket->nLoginServerID);
			return ERROR_NONE;
		}
		break;

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	case LOMA_DETACHUSER_BY_IP:
		{
			LOMADetachUserbyIP * pPacket = (LOMADetachUserbyIP*)pData;
			g_pDivisionManager->DetachUserByIP(pPacket->szIP);
			return ERROR_NONE;
		}
		break;
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#ifdef PRE_ADD_BEGINNERGUILD
	case LOMA_ADDGUILDMEMB:
		{
			g_pDivisionManager->SendAddGuildMember(reinterpret_cast<MAAddGuildMember*>(pData));
		}
		break;
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
	}
	return ERROR_UNKNOWN_HEADER;
}

int CDNLoginConnection::OnRecvAdduser(LOMAAddUser *pPacket, bool bRecusive)
{
#if defined(PRE_MOD_SELECT_CHAR)

	if (pPacket->bTutorial){
		int nGameServerID = 0;
		CDNGameConnection* pGameCon = g_pDivisionManager->GetFreeGameConnection(&nGameServerID, REQINFO_TYPE_SINGLE);
		if( !pGameCon ){
			SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, ERROR_GENERIC_GAMECON_NOT_FOUND);
			return ERROR_NONE;
		}
	}

	if (!g_pDivisionManager->GetVillageConnectionByVillageID(pPacket->cVillageID)){
		SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
		g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID,
			L"ADBID:%u [LOMA_USERINFO] VillageCon not found (%s, %s)\r\n", pPacket->nAccountDBID, pPacket->wszAccountName, pPacket->wszCharacterName);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	int nRet = g_pDivisionManager->AddUser(pPacket, GetServerID());
	switch (nRet)
	{
	case ERROR_NONE:
		{
#if defined(_KR) && defined(_FINAL_BUILD)	// nexon pc bang
			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
			if (pUser)
				g_pNexonAuth->SendLogin(pUser);

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
			if (pUser){
				bool bTempNetCafe = false;
				char szNHNNetCafeCode[NHNNETCAFECODEMAX];
				char szNHNProductCode[NHNNETCAFECODEMAX];

				if (g_pNHNNetCafe->NetCafe_UserLogin(pUser->GetAccountNameA(), pPacket->szIP, bTempNetCafe))
				{
					if (bTempNetCafe)
					{
						if (g_pNHNNetCafe->NetCafe_GetNetCafeCode(pUser->GetAccountNameA(), szNHNNetCafeCode, sizeof(szNHNNetCafeCode)) == false)
							memset(szNHNNetCafeCode, 0, sizeof(szNHNNetCafeCode));

						if (g_pNHNNetCafe->NetCafe_GetProductCode(pUser->GetAccountNameA(), szNHNProductCode, sizeof(szNHNProductCode)) == false)
							memset(szNHNProductCode, 0, sizeof(szNHNProductCode));

						if (g_pDivisionManager->SetNHNNetCafeInfo(pPacket->nAccountDBID, bTempNetCafe, szNHNNetCafeCode, szNHNProductCode) == false)
							_DANGER_POINT();
					}
				}
			}

#elif defined(_TH) && defined(_FINAL_BUILD)
			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
			if (pUser)								
				g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 1, pUser->GetAccountDBID());				
#elif defined(_ID) && defined(_FINAL_BUILD)
			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
			if (pUser)				
				g_pKreonPCCafe->AddProcessCall(pUser->GetAccountDBID());			
#endif	// _KR

#ifdef PRE_ADD_STEAM_USERCOUNT
			CDNUser * pSteamUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
			if (pSteamUser)
			{
				pSteamUser->m_bSteamUser = pPacket->bSteamUser;
				if (pSteamUser->m_bSteamUser)
					g_pDivisionManager->IncreaseSteamUser();
			}
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

			SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, nRet);

			g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID,
				L"ADBID:%u [LOMA_ADDUSER] %s, %s\r\n", pPacket->nAccountDBID, pPacket->wszAccountName, pPacket->wszCharacterName);

			return ERROR_NONE;
		}

	case ERROR_GENERIC_WORLDFULLY_CAPACITY:
		{
			SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, nRet);
			return ERROR_NONE;
		}

	case ERROR_GENERIC_DUPLICATEUSER:
		{
			if (bRecusive == false)
			{
				g_pDivisionManager->SendDetachUser(pPacket->nAccountDBID);
				OnRecvAdduser(pPacket, true);
			}
			else
			{
				//이쪽으로 진입이 불가능 합니다. 나오면 낭패 혹시나 해서 다시 객체는 정리 합니다.
				_DANGER_POINT();
				SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, ERROR_GENERIC_USER_NOT_FOUND);
				g_pDivisionManager->SendDetachUser(pPacket->nAccountDBID);
			}
			return ERROR_NONE;
		}

	default:
		{
			SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, ERROR_GENERIC_USER_NOT_FOUND);
			g_pDivisionManager->SendDetachUser(pPacket->nAccountDBID);
			g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID,
				L"ADBID:%u [LOMA_ADDUSER] FAIL!!!! %s, %s\r\n", pPacket->nAccountDBID, pPacket->wszAccountName, pPacket->wszCharacterName);
			return ERROR_NONE;
		}
		break;
	}

	return ERROR_NONE;

#else	// #if defined(PRE_MOD_SELECT_CHAR)

	BYTE szMID[MACHINEIDMAX] = {0,};
	DWORD dwGRC = 0;
#if defined(_KR)
	memcpy(szMID, pPacket->szMID, sizeof(szMID));
	dwGRC = pPacket->dwGRC;
#endif	// #if defined(_KR)

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#ifdef PRE_ADD_MULTILANGUAGE
	int nRet = g_pDivisionManager->AddUser(GetServerID(), pPacket->nAccountDBID, szMID, dwGRC, pPacket->szIP, pPacket->cSelectedLanguage, pPacket->cAccountLevel, pPacket->bForce );
#else		//#ifdef PRE_ADD_MULTILANGUAGE
	int nRet = g_pDivisionManager->AddUser(GetServerID(), pPacket->nAccountDBID, szMID, dwGRC, pPacket->szIP, 0, pPacket->cAccountLevel, pPacket->bForce );
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#else		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#ifdef PRE_ADD_MULTILANGUAGE
	int nRet = g_pDivisionManager->AddUser(GetServerID(), pPacket->nAccountDBID, szMID, dwGRC, NULL, pPacket->cSelectedLanguage, pPacket->cAccountLevel, pPacket->bForce );
#else		//#ifdef PRE_ADD_MULTILANGUAGE
	int nRet = g_pDivisionManager->AddUser(GetServerID(), pPacket->nAccountDBID, szMID, dwGRC, NULL, 0, pPacket->cAccountLevel, pPacket->bForce );
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#ifdef PRE_ADD_COMEBACK
	//adduser안에서 할때마다 인자로 받아야 해서 디파인 늘어갈때마다 코드가 지저분 한번더 찾더라도 외부에서 세팅으로 변신 - 2hogi
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
	if (pUser)
	{
		pUser->m_bComebackUser = pPacket->bComebackUser;
#if defined(PRE_ADD_GAMEQUIT_REWARD)
		pUser->m_bReConnectNewbieReward = pPacket->bReConnectNewbieReward;
		pUser->m_eUserGameQuitRewardType = pPacket->eUserGameQuitReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef PRE_ADD_STEAM_USERCOUNT
		pUser->m_bSteamUser = pPacket->bSteamUser;
		if (pUser->m_bSteamUser)
			g_pDivisionManager->IncreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
	}
#else	//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	//adduser안에서 할때마다 인자로 받아야 해서 디파인 늘어갈때마다 코드가 지저분 한번더 찾더라도 외부에서 세팅으로 변신 - 2hogi
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
	if (pUser)
	{
		pUser->m_bReConnectNewbieReward = pPacket->bReConnectNewbieReward;
		pUser->m_eUserGameQuitRewardType = pPacket->eUserGameQuitReward;
#ifdef PRE_ADD_STEAM_USERCOUNT
		pUser->m_bSteamUser = pPacket->bSteamUser;
		if (pUser->m_bSteamUser)
			g_pDivisionManager->IncreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
	}
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#endif		//#ifdef PRE_ADD_COMEBACK

	switch (nRet)
	{
	case ERROR_NONE:
	case ERROR_GENERIC_WORLDFULLY_CAPACITY:
		{
			SendAddUser(pPacket->nAccountDBID, pPacket->nSessionID, pPacket->cWorldID, nRet);
			return ERROR_NONE;
		}

	case ERROR_GENERIC_DUPLICATEUSER:
		{
			if (bRecusive == false)
			{
				g_pDivisionManager->SendDetachUser(pPacket->nAccountDBID);
				OnRecvAdduser(pPacket, true);
			}
			else
			{
				//이쪽으로 진입이 불가능 합니다. 나오면 낭패 혹시나 해서 다시 객체는 정리 합니다.
				_DANGER_POINT();
				SendUserInfo(pPacket->nAccountDBID, pPacket->nSessionID, ERROR_GENERIC_USER_NOT_FOUND);
				g_pDivisionManager->SendDetachUser(pPacket->nAccountDBID);
			}
			return ERROR_NONE;
		}

	case ERROR_GENERIC_UNKNOWNERROR:
		{
			SendUserInfo(pPacket->nAccountDBID, pPacket->nSessionID, ERROR_GENERIC_USER_NOT_FOUND);
			g_pDivisionManager->SendDetachUser(pPacket->nAccountDBID);
			g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, pPacket->nAccountDBID, 0, 0, L"ADBID:%u [LOMA_ADDUSER] FAIL!!!! %s, %s\r\n", pPacket->nAccountDBID, pPacket->wszAccountName, L"UNKNOWNERROR");
			return ERROR_NONE;
		}
	}
	return ERROR_NONE;

#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

void CDNLoginConnection::SendRegistWorldID()
{
	MALORegist Regist;
	memset(&Regist, 0, sizeof(MALORegist));

	Regist.cWorldSetID = g_Config.nWorldSetID;
	Regist.nWorldMaxUser = g_pExtManager->GetWorldMaxUser(g_Config.nWorldSetID);

	AddSendData(MALO_REGIST, 0, (char*)&Regist, sizeof(MALORegist));
}

void CDNLoginConnection::SendVillageInfoDelete(int nVillageID)
{
	MALOVillageInfoDelete packet;
	memset(&packet, 0, sizeof(packet));
	packet.nVillageID = nVillageID;
	AddSendData(MALO_VILLAGEINFODEL, 0, (char*)&packet, sizeof(packet));
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDNLoginConnection::SendAddUser(UINT nAccountDBID, UINT nSessionID, int nRet)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
void CDNLoginConnection::SendAddUser(UINT nAccountDBID, UINT nSessionID, char cWorldID, int nRet)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	MALOAddUser packet;
	memset(&packet, 0, sizeof(MALOAddUser));
	packet.nAccountDBID = nAccountDBID;
	packet.nSessionID = nSessionID;
#if !defined(PRE_MOD_SELECT_CHAR)
	packet.cWorldID = cWorldID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	packet.nRet = nRet;
	AddSendData(MALO_ADDUSER, 0, (char*)&packet, sizeof(MALOAddUser));
}

#if !defined(PRE_MOD_SELECT_CHAR)
void CDNLoginConnection::SendUserInfo(UINT nAccountDBID, UINT nSessionID, int nRet, const char * pszIP/* = NULL*/)
{
	MALOUserInfo UserInfo;
	memset(&UserInfo, 0, sizeof(MALOUserInfo));

	UserInfo.nAccountDBID = nAccountDBID;
	UserInfo.nSessionID = nSessionID;
	UserInfo.nRet = nRet;
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	if (pszIP != NULL)
		_strcpy(UserInfo.szIP, IPLENMAX, pszIP, IPLENMAX);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

	AddSendData(MALO_USERINFO, 0, (char*)&UserInfo, sizeof(MALOUserInfo));
}
#endif	// #if !defined(PRE_MOD_SELECT_CHAR)

void CDNLoginConnection::SendDetachUser(UINT nAccountDBID)
{
	MALODetachUser packet;
	memset(&packet, 0, sizeof(MALODetachUser));
	packet.nAccountDBID = nAccountDBID;
	AddSendData(MALO_DETACHUSER, 0, (char*)&packet, sizeof(MALODetachUser));
}

void CDNLoginConnection::SendSetTutorialGameID(UINT nSessionID, ULONG nIP, USHORT nPort, USHORT nTcpPort, int iServerIdx, USHORT wGameID, int nRoomID, int nRet)
{
	MALOSetTutorialGameID GameID;
	memset(&GameID, 0, sizeof(MALOSetTutorialGameID));

	GameID.nSessionID = nSessionID;
	GameID.nIP = nIP;
	GameID.nPort = nPort;
	GameID.nTcpPort = nTcpPort;
	GameID.cServerIdx = iServerIdx;
	GameID.wGameID = wGameID;
	GameID.nRoomID = nRoomID;
	GameID.nRet = nRet;

	AddSendData(MALO_SETTUTORIALGAMEID, 0, (char*)&GameID, sizeof(MALOSetTutorialGameID));
}

void CDNLoginConnection::SendWaitUserResult(UINT nAccountDBID, USHORT nTicketNum, short nRetCode)
{
	MALOAddWaitUser packet;
	memset(&packet, 0, sizeof(MALOAddWaitUser));

	packet.nAccountDBID = nAccountDBID;
	packet.nTicketNum = nTicketNum;
	packet.nRetCode = nRetCode;

	AddSendData(MALO_ADDWAITUSER, 0, (char*)&packet, sizeof(packet));
}

void CDNLoginConnection::SendWaitProcess(MALOWaitUserProcess * pProcess)
{
	if (pProcess == NULL)
	{
		_DANGER_POINT();
		return;
	}

	AddSendData(MALO_WAITUSERPROCESS, 0, (char*)pProcess, sizeof(MALOWaitUserProcess) - sizeof(pProcess->nAccountArr) + (sizeof(UINT) * pProcess->nCount));
}

void CDNLoginConnection::SendDuplicateLogin(UINT nAccountDBID, UINT nSessionID)
{
	MALODuplicateLogin packet;
	memset(&packet, 0, sizeof(MALODetachUser));
	packet.nAccountDBID = nAccountDBID;	
	packet.nSessionID = nSessionID;
	AddSendData(MALO_DUPLICATE_LOGIN, 0, (char*)&packet, sizeof(MALODuplicateLogin));
}

