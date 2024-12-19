
#include "stdafx.h"
#include "ServiceSession.h"
#include "DNServerPacket.h"
#include "ServiceManager.h"
#include "DNBAM.h"
#include "Log.h"
#include "DNServerDef.h"

const UINT ONEHOURTICK = (1000 * 60 * 60);
const UINT ONEMINTICK = (1000 * 60);
extern TServiceManagerConfig g_Config;
extern CLog s_CountLog;
extern CLog g_ExceptionReportLog;
extern CLog g_DBDelayLog;
extern CLog g_GameDelayLog;
extern CLog g_VillageDelayLog;
extern CLog g_DBErrorLog;
extern CLog g_DBSystemErrorLog;

CServiceSession::CServiceSession()
{
	Init(1024 * 100, 1024 * 100);
	m_nMID = 0;
	m_nCreateSessionTick = timeGetTime();
	m_bIsServiceClose = false;
	m_nLastPingTick = 0;
	m_nLastIdleTick = 0;
	m_nLastReportedTick = 0;
	m_nLastDelayedTick = 0;

	m_nLoginUserCount = 0;

	m_nVillageUserCount = 0;

	m_nGameUserCount = 0;
	m_nReportedCount = 0;
	m_nGameRoomCount = 0;
	m_nTotalRoomCount = 0;
	m_bZeroPopulation = false;
	m_bUnrecoveryBlock = false;
	m_nUnrecoveryTick = 0;
	m_nVillageFaultSafeCount = 0;
	m_bFaultSafeDump = false;

	m_nLastIdleLogTick = 0;

	m_bPermanentlyBlock = false;
}

CServiceSession::~CServiceSession()
{
	if (g_pServiceManager)
		g_pServiceManager->EjectService(m_nMID);
}

int CServiceSession::IdleProcess(ULONG nCurTick)
{
	//서로처리가 달라서 구차나서 나누어서 처리되는데 겹치는 부분이 생김...정리하자 ㅋㅋㅋ언젠가..?..-_-; -2hogi
	if (GetActive() == false || m_nCreateSessionTick == 0)
		return SERVICEPROCESS_NONE;

	if (GetType() == MANAGED_TYPE_VILLAGE)
	{
		if (m_nLastPingTick <= 0)
		{
			if (m_nLastIdleTick <= 0 || m_nLastIdleTick + DNProcessCheck::UpdateInterval < nCurTick)
			{
				SendPing();
				m_nLastIdleTick = nCurTick;
			}
		}
		else
		{
			if (m_nLastPingTick + DNProcessCheck::DisconnectInterval < nCurTick)
			{
				if (m_nVillageFaultSafeCount <= 0)
					g_Log.Log(LogType::_FILELOG, L"VillageServer FaultSafe Pre Alarm [SID:%d][ip:%S]\n", GetMID(), GetIp());

				if (m_nVillageFaultSafeCount > 1 && m_nVillageFaultSafeCount%7 == 0 && m_bFaultSafeDump == false)
				{
					SendMakeDump();					//강제덤프 발생시킨다.
					m_bFaultSafeDump = true;
				}
				m_nVillageFaultSafeCount++;
			}
			if (m_nLastPingTick + (DNProcessCheck::DisconnectInterval * 10) < nCurTick)		//좀 늘리자
			{
				return SERVICEPROCESS_DELAYED;
			}
		}
	}

	if (GetType() == MANAGED_TYPE_GAME)
	{
		//check faultsafe
		//bool bRestart = !wcsicmp(g_Config.wszRegion, L"kor") ? false : true;
		bool bRestart = true;
		if (m_bUnrecoveryBlock && bRestart && m_nUnrecoveryTick > 0)
		{
			if (ONEHOURTICK/6 + m_nUnrecoveryTick < nCurTick)
			{
				g_Log.Log(LogType::_ERROR, L"GameServer UnrecoverBlock Over10Min Check[SID:%d]\n", GetMID());

				if (ONEHOURTICK + m_nUnrecoveryTick < nCurTick || m_nGameUserCount < 20)		//한시간이 지나갔다.
					return SERVICEPROCESS_FAULTSAFE;
			}
		}
	}

	if (m_nLastIdleLogTick + ONEMINTICK < nCurTick)
	{
		switch (GetType())
		{
		case MANAGED_TYPE_GAME:
			{
				s_CountLog.Log(LogType::_FILELOG, L"gamecount [%d][%d]\n", m_nGameUserCount, GetMID());
				break;
			}

		case MANAGED_TYPE_VILLAGE:
			{
				s_CountLog.Log(LogType::_FILELOG, L"villagecount [%d][%d]\n", m_nVillageUserCount, GetMID());
				break;
			}

		case MANAGED_TYPE_LOGIN:
			{
				s_CountLog.Log(LogType::_FILELOG, L"logincount [%d][%d]\n", m_nLoginUserCount, GetMID());
				break;
			}
		}
		m_nLastIdleLogTick = nCurTick;
	}

	return SERVICEPROCESS_NONE;
}

int CServiceSession::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	switch (iMainCmd)
	{
		case SERVICE_SERVER_REQUESTSERVICE:
		{
			SendSessionInfo();
			return ERROR_NONE;
		}

		case SERVICE_SERVER_REGIST_MANAGEDID:
		{
			TServerManagedID * pPacket = (TServerManagedID*)pData;

			if (sizeof(TServerManagedID) - sizeof(pPacket->cWorldID) + (sizeof(BYTE) * pPacket->cWorldCount) != iLen)
			{
				g_Log.Log(LogType::_ERROR, L"OnlineServiceServer Fail Check Service[IP:%S]\n", GetIp());
				return ERROR_NONE;
			}

			m_nMID = pPacket->nMID;
			m_nManagedType = pPacket->nManagedType;
			m_nCreateSessionTick = 0;

			if (g_pServiceManager->OnlineServiceServer(pPacket->nMID, GetSessionID(), pPacket->szVersion, pPacket->szResVersion, this, pPacket->cWorldCount, pPacket->cWorldID, pPacket->bFinalBuild))
			{
				strcpy_s(m_szVersion, pPacket->szVersion);
				strcpy_s(m_szResVersion, pPacket->szResVersion);
			}
			else
				g_Log.Log(LogType::_FILELOG, L"OnlineServiceServer Fail [SID:%d]\n", pPacket->nMID);

			if( g_pServiceManager->CheckAllServiceOnline() )
				g_Log.Log(LogType::_FILELOG, L"All Services Online\n");

			return ERROR_NONE;
		}

		case SERVICE_SERVER_USUAL_LOGINREPORT:
			{
				TServiceReportLogin * pPacket = (TServiceReportLogin*)pData;

				m_nLastReportedTick = timeGetTime();
				m_nLastDelayedTick = 0;

				g_pServiceManager->ReportServiceInfoLogin(m_nMID, pPacket->nUserCount);
//#if defined(_KR)
//				g_pBAM->CurrentConnections(L"DN_LoginUser", pPacket->nUserCount);
//#endif	// #if defined(_KR)
				m_nLoginUserCount = pPacket->nUserCount;
				return ERROR_NONE;
			}

		case SERVICE_SERVER_USUAL_VILLAGEREPORT:
			{
				TServiceReportVillage * pPacket = (TServiceReportVillage*)pData;

				if (pPacket->nCount >= VILLAGECHANNELMAX)
				{
					_DANGER_POINT();
					return ERROR_NONE;
				}

				m_nVillageUserCount = pPacket->nUserCount;
				g_pServiceManager->ReportServiceInfoVillage(m_nMID, pPacket->nUserCount, pPacket->nCount, pPacket->Info);
				return ERROR_NONE;
			}

		case SERVICE_SERVER_USUAL_MASTERREPORT:
			{
				TServiceReportMaster * pPacket = (TServiceReportMaster*)pData;
				g_pServiceManager->ReportServiceMaster(GetMID(), pPacket->nWorldID, pPacket->nWaitserCount);
				g_pServiceManager->UpdateFarmStatus(pPacket);
				return ERROR_NONE;
			}

		case SERVICE_SERVER_USUAL_GAMEREPORT:
			{
				TServiceReportGame * pPacket = (TServiceReportGame*)pData;

				m_nLastReportedTick = timeGetTime();
				m_nLastDelayedTick = 0;

				g_pServiceManager->ReportServiceInfoGame(m_nMID, pPacket->nUserCount, pPacket->nRoomCount);

//#if defined(_KR)
//				g_pBAM->CurrentConnections(L"DN_GameUser", pPacket->nUserCount);
//#endif	// #if defined(_KR)

				m_nGameUserCount = pPacket->nUserCount;
				m_bZeroPopulation = pPacket->cIsZeroPopulation > 0 ? true : false;
				m_nGameRoomCount = pPacket->nRoomCount;
				m_nTotalRoomCount = pPacket->nTotalRoomCount;
				return ERROR_NONE;
			}

		case SERVICE_SERVER_SERVICE_CLOSED:
			{
				g_pServiceManager->ReportedServiceClosed(GetMID());
				return ERROR_NONE;
			}

		case SERVICE_SERVER_EXCEPTION_REPORT:
		{
			TServerException * pPacket = (TServerException*)pData;
			g_pServiceManager->SendExceptionReport(m_nMID, pPacket->nType, NULL);

			switch (pPacket->nType)
			{
			case _EXCEPTIONTYPE_ROOMCRASH:
				g_ExceptionReportLog.Log(LogType::_FILELOG, L"Server Exception Report MID[%d][Type:%s]\n", GetMID(), L"Room Crash");
				break;

			case _EXCEPTIONTYPE_SESSIONCRASH:
				g_ExceptionReportLog.Log(LogType::_FILELOG, L"Server Exception Report MID[%d][Type:%s]\n", GetMID(), L"Session Crash");
				break;

			default:
				g_ExceptionReportLog.Log(LogType::_FILELOG, L"Server Exception Report MID[%d][Type:%d]\n", GetMID(), pPacket->nType);
				break;
			}

			return ERROR_NONE;
		}

		case SERVICE_SERVER_DELAYEDREPORT:
		{
			TServiceDelayedReport * pPacket = (TServiceDelayedReport*)pData;
			
			g_pServiceManager->TerminateServiceEach(pPacket->nManagedID, NULL, true);

			g_Log.Log(LogType::_FILELOG, L"DataBase MiddleWare Delayed Over 60Second MID[%d]\n", GetMID());
			g_pServiceManager->AddServerDelayMID(SERVERTYPE_DB, m_nMID);
			g_pServiceManager->SendExceptionReport(m_nMID, _EXCEPTIONTYPE_DBMW_DELAYED, "DataBase MiddleWare Delayed Over 60Second");
			g_DBDelayLog.Log(LogType::_FILELOG, L"DataBase MiddleWare Delayed Over 60Second MID[%d] WID[%d]\n", GetMID(), g_pServiceManager->GetDBWorldIDBySID(GetMID()));
			return ERROR_NONE;
		}

		case SERVICE_SERVER_QUERYDELAYREPORT:
		{
			TQueryDelayedReport * pPacket = (TQueryDelayedReport*)pData;

			g_DBDelayLog.Log(LogType::_FILELOG, L"[Query Over Time:%S] MID:%d WID:%d ThreadID:%d time=%d ms\n", 
							pPacket->szQueryName, GetMID(), g_pServiceManager->GetDBWorldIDBySID(GetMID()), pPacket->dwThreadID, pPacket->dwElasped );
			return ERROR_NONE;
		}

		case SERVICE_SERVER_GAMEDELAYREPORT:
		{
			TGameDelayedReport* pPacket = (TGameDelayedReport*)pData;

			g_GameDelayLog.Log(LogType::_FILELOG, L"%s \r\n", pPacket->wszLog);
			g_pServiceManager->AddServerDelayMID(SERVERTYPE_GAME, m_nMID);
			return ERROR_NONE;
		}

		case SERVICE_SERVER_DBERROR_REPORT:
		{
			TQueryErrorReport* pPacket = (TQueryErrorReport*)pData;

			if (pPacket->unLogType == LogType::_DBSYSTEM_ERROR)
			{
				g_DBSystemErrorLog.Log(LogType::_FILELOG, L"[MID:%d][Server:%d][World:%d][ACC:%d][CHRID:%lld][SID:%d] %s", 
					GetMID(), pPacket->unServerType, pPacket->unWorldSetID, pPacket->uiAccountDBID, pPacket->biCharDBID, pPacket->uiSessionID, pPacket->wszBuf);
			}
			else
			{
				g_DBErrorLog.Log(LogType::_FILELOG, L"[MID:%d][Server:%d][World:%d][ACC:%d][CHRID:%lld][SID:%d] %s", 
					GetMID(), pPacket->unServerType, pPacket->unWorldSetID, pPacket->uiAccountDBID, pPacket->biCharDBID, pPacket->uiSessionID, pPacket->wszBuf);
			}

			return ERROR_NONE;
		}

		case SERVICE_SERVER_VILLAGEDELAYREPORT:
		{
			TVillageDelayedReport* pPacket = (TVillageDelayedReport*)pData;
			g_VillageDelayLog.Log(LogType::_FILELOG, L"%s \r\n", pPacket->wszLog);
			return ERROR_NONE;
		}

		case SERVICE_SERVER_UNRECOVERY:
		{
			TServiceUnrecover * pPacket = (TServiceUnrecover*)pData;
			
			if (m_bPermanentlyBlock)
			{
				g_Log.Log(LogType::_ZEROPOPULATION, L"SERVICE_SERVER_UNRECOVERY if (m_bPermanentlyBlock) bBLock[%s] bPermannently[%s]\n", pPacket->bBlock == true ? L"True" : L"False", pPacket->bPermannently == true ? L"True" : L"False");
				return ERROR_NONE;
			}

			m_bUnrecoveryBlock = pPacket->bBlock;
			m_nUnrecoveryTick = m_bUnrecoveryBlock == true ? timeGetTime() : 0;
			m_bPermanentlyBlock = pPacket->bPermannently;

			int nMasterSID[WORLDCOUNTMAX];
			memset(nMasterSID, 0, sizeof(nMasterSID));

			if (g_pServiceManager->GetMasterSIDByGameSID(GetMID(), nMasterSID))
			{
				for (int j = 0; j < WORLDCOUNTMAX; j++)
				{
					if (nMasterSID[j] <= 0) continue;
					g_pServiceManager->PopulationControl(nMasterSID[j], GetMID(), m_bUnrecoveryBlock);
				}
			}
			return ERROR_NONE;
		}

		case SERVICE_SERVER_PONG:
		{
			m_nLastPingTick = 0;
			return ERROR_NONE;
		}

		case SERVICE_SERVER_GPKRELOADFAIL:
		{
			g_Log.Log(LogType::_ERROR, L"GPK ReloadFail [SID:%d][IP:%S]\n", GetMID(), GetIp());
			return ERROR_NONE;
		}

		case SERVICE_SERVER_CHANNELINFO:
		{
			TServiceChannelInfo * pPacket = (TServiceChannelInfo*)pData;

			g_pServiceManager->UpdateChannelInfo(pPacket);
			return ERROR_NONE;
		}

		case SERVICE_SERVER_MERITINFO:
		{
			TServiceMeritInfo * pPacket = (TServiceMeritInfo*)pData;
			g_pServiceManager->UpdateMeritInfo(pPacket);
			return ERROR_NONE;
		}

		case SERVICE_SERVER_WORLDINFO:
		{
			TServiceWorldInfo * pPacket = (TServiceWorldInfo*)pData;
			g_pServiceManager->UpdateWorldMaxUser(pPacket);
			return ERROR_NONE;
		}

		case SERVICE_SERVER_EXT_RELOADRESULT:
		{
			TServerExtReloadResult * pPacket = (TServerExtReloadResult*)pData;
			g_pServiceManager->CompleteLiveExtReload(GetMID(), pPacket->bResult);
			return ERROR_NONE;
		}

		case SERVICE_SERVER_LOGIN_WORDDBRESULT:
		{
			TServerLoginWorldDResult * pPacket = (TServerLoginWorldDResult*)pData;
			switch (pPacket->nResult)
			{
			case 0:
				g_Log.Log(LogType::_NORMAL, L"LoginServer[%d] success to create WorldDB[ID:%d][IP:%S][PORT:%d]\n", GetMID(), pPacket->nWorldID, pPacket->szIp, pPacket->nPort);
				break;
			case 1:
				g_Log.Log(LogType::_ERROR, L"LoginServer[%d] fail to connect WorldDB[ID:%d][IP:%S][PORT:%d]\n", GetMID(), pPacket->nWorldID, pPacket->szIp, pPacket->nPort);
				break;
			case 2:
				g_Log.Log(LogType::_ERROR, L"LoginServer[%d] WorldDB version error[ID:%d][IP:%S][PORT:%d]\n", GetMID(), pPacket->nWorldID, pPacket->szIp, pPacket->nPort);
				break;
			case 3:
				g_Log.Log(LogType::_ERROR, L"LoginServer[%d] can not find WorldDB [ID:%d]\n", GetMID(), pPacket->nWorldID);
				break;
			case 4:
				g_Log.Log(LogType::_ERROR, L"LoginServer[%d] already create WorldDB [ID:%d][IP:%S][PORT:%d]\n", GetMID(), pPacket->nWorldID, pPacket->szIp, pPacket->nPort);
				break;
			}
				
			return ERROR_NONE;
		}
		case SERVICE_SERVER_UPDATEWORLDPVPROOM:
		{
			TApplyWorldPvPRoom * pPacket = (TApplyWorldPvPRoom*)pData;			
			if( pPacket->nRetCode )
				g_Log.Log(LogType::_FILELOG, L"Update WorldPvPRoom fail Code = %d\n", pPacket->nRetCode);
			else
				g_Log.Log(LogType::_FILELOG, L"Update WorldPvPRoom Success\n");
			return ERROR_NONE;
		}
	}
	return ERROR_NONE;
}

void CServiceSession::SendSessionInfo()
{
	TServiceConnected packet;

	packet.nSessionID = GetSessionID();

	AddSendData(NETSERVICE_CONNECTED, 0, (char*)&packet, sizeof(packet));
}

void CServiceSession::SendNoticeMassage(int nNoticeType, int nManagedID, int nChannelID, int nMapIdx, int nSec, const char * pMsg)
{
	TServiceNotice packet;
	memset(&packet, 0, sizeof(packet));	

	packet.nNoticeType = nNoticeType;
	packet.nManagedID = nManagedID;
	packet.nChannelID = nChannelID;
	packet.nMapIdx = nMapIdx;
	packet.nSlideShowSec = nSec;
	MultiByteToWideChar(CP_ACP, 0, pMsg, -1, packet.wszMsg, CHATLENMAX);
	packet.nLen = (USHORT)wcslen(packet.wszMsg);

	AddSendData(NETSERVICE_NOTICE, 0, (char*)&packet, sizeof(packet) - sizeof(packet.wszMsg) + (packet.nLen * sizeof(WCHAR)));
}

void CServiceSession::SendNoticeCancel()
{
	AddSendData(NETSERVICE_CANCELNOTICE, 0, NULL, 0);
}

void CServiceSession::SendMakeDump()
{
	AddSendData(NETSERVICE_MAKE_DUMP, 0, NULL, 0);
}

void CServiceSession::SendChangeWorldMaxuser(UINT nWorldMaxUser)
{
	TServiceWordlMaxUser packet;
	memset(&packet, 0, sizeof(TServiceWordlMaxUser));

	packet.nChangeMaxUser = nWorldMaxUser;

	AddSendData(NETSERVICE_WORLDMAXUSER, 0, (char*)&packet, sizeof(TServiceWordlMaxUser));
}

void CServiceSession::SendChannelControl(int nChannelID, bool bVisibility, int nChannelPopulation)
{
	TServiceChannelControl packet;
	memset(&packet, 0, sizeof(packet));

	packet.nChannelID = nChannelID;
	packet.cVisibility = bVisibility == true ? 1 : 0;
	packet.nChannelPopulation = nChannelPopulation;

	AddSendData(NETSERVICE_CHANNEL_CONTROL, 0, (char*)&packet, sizeof(packet));
}

void CServiceSession::SendPopulationControl(int nServerType, int nManagedID, bool bZeroPopulation)
{
	TServicePopulationControl packet;
	memset(&packet, 0, sizeof(packet));

	packet.nServerType = nServerType;
	packet.nManagedID = nManagedID;
	packet.cZeroPopulation = bZeroPopulation == true ? 1 : 0;

	AddSendData(NETSERVICE_POPULATION_CONTROL, 0, (char*)&packet, sizeof(packet));
}

void CServiceSession::SendGameAffinity(int nManagedID, int nType)
{
	TServiceAffinityControl packet;
	memset(&packet, 0, sizeof(packet));

	packet.nManagedID = nManagedID;
	packet.cAffinityType = (BYTE)nType;
	
	AddSendData(NETSERVICE_AFFINITY_CONTROL, 0, (char*)&packet, sizeof(packet));
}

void CServiceSession::SendUpdateEvent()
{
	AddSendData(NETSERVICE_EVENTUPDATE, 0, NULL, 0);
}

void CServiceSession::SendUpdateSimpleConfig()
{
	AddSendData(NETSERVICE_SIMPLECONFIGUPDATE, 0, NULL, 0);
}

void CServiceSession::SendCloseService()
{
	m_bIsServiceClose = true;
	AddSendData(NETSERVICE_DETACHALLUSER, 0, NULL, 0);
}

void CServiceSession::SendUserRestraint(UINT nAccountDBID)
{
	TServiceUserRestraint packet;
	memset(&packet, 0, sizeof(TServiceUserRestraint));

	packet.nAccountDBID = nAccountDBID;

	AddSendData(NETSERVICE_USERRESTRAINT, 0, (char*)&packet, sizeof(packet));
}

void CServiceSession::SendUserBan(const WCHAR * pName, UINT nAccountDBID, eBanFileType eType)
{
	TServiceUserBan packet;
	memset(&packet, 0, sizeof(TServiceUserBan));

	packet.eType = eType;
	packet.nAccountDBID = nAccountDBID;
	if (pName)
	{
		if (wcslen(pName) >= NAMELENMAX)
		{
			g_Log.Log(LogType::_FILELOG, L"ChracterName Size OverFlow [%s]\n", pName);
			return ;
		}
		::wcsncpy_s(packet.wszName, _countof(packet.wszName), pName, _countof(packet.wszName));
	}	

	AddSendData(NETSERVICE_USERBAN, 0, (char*)&packet, sizeof(packet));	
}

void CServiceSession::SendPing()
{
	AddSendData(NETSERVICE_PING, 0, NULL, 0);
	if (m_nLastPingTick <= 0)
	m_nLastPingTick = timeGetTime();
}

void CServiceSession::SendSelectJoin(int nSID, const WCHAR * pwszCharacterName)
{
	TServiceSelectjoin packet;
	memset(&packet, 0, sizeof(TServiceSelectjoin));

	packet.nSID = nSID;
	if (wcslen(pwszCharacterName) >= NAMELENMAX)
	{
		g_Log.Log(LogType::_FILELOG, L"ChracterName Size OverFlow [%s]\n", pwszCharacterName);
		return ;
	}
	::wcsncpy_s(packet.wszCharacterName, _countof(packet.wszCharacterName), pwszCharacterName, wcslen(pwszCharacterName));
	
	AddSendData(NETSERVICE_SELECTJOIN, 0, (char*)&packet, sizeof(TServiceSelectjoin));
}

void CServiceSession::SendClearSelectjoin()
{
	AddSendData(NETSERVICE_CLEAR_SELECTJOIN, 0, NULL, 0);
}

void CServiceSession::SendScheduleCloseService(__time64_t _tOderedTime, __time64_t _tCloseTime)
{
	TScheduleServiceClose packet;
	memset(&packet, 0, sizeof(packet));

	packet._tOderedTime = _tOderedTime;
	packet._tCloseTime = _tCloseTime;
	
	AddSendData(NETSERVICE_SCHEDULE_SERVERCLOSE, 0, (char*)&packet, sizeof(TScheduleServiceClose));
}

void CServiceSession::SendUpdateSaleData()
{
	AddSendData(NETSERVICE_UPDATESALEDATA, 0, NULL, 0);
}

void CServiceSession::SendUpdateFarm(int nWorldID, bool bForceStart)
{
	TUpdateFarm packet;
	memset(&packet, 0, sizeof(TUpdateFarm));

	packet.nWorldID = nWorldID;
	packet.bForceStart = bForceStart;

	AddSendData(NETSERVICE_UPDATEFARM, 0, (char*)&packet, sizeof(TUpdateFarm));
}

void CServiceSession::SendUpdateGuildWarSchedule()
{
	AddSendData(NETSERVICE_UPDATEGUILDWAR_SCHEDULE, 0, NULL, 0);
}

void CServiceSession::SendUpdateGuildWare(int nGuildID)
{
	TUPdateGuildWare packet;
	memset(&packet, 0, sizeof(TUPdateGuildWare));

	packet.nGuildID = nGuildID;

	AddSendData(NETSERVICE_UPDATEGUILDWARE, 0, (char*)&packet, sizeof(TUPdateGuildWare));
}

void CServiceSession::SendExtReload(int nType)
{
	TExtReload packet;
	memset(&packet, 0, sizeof(TExtReload));

	packet.nType = nType;

	AddSendData(NETSERVICE_EXTRELOAD, 0, (char*)&packet, sizeof(TExtReload));
}

void CServiceSession::SendCreatEachDB(int nWorldID)
{
	TCreateDB packet;
	memset(&packet, 0, sizeof(TCreateDB));

	packet.nWorldID = nWorldID;

	AddSendData(NETSERVICE_CREATEWORLDDB, 0, (char*)&packet, sizeof(TCreateDB));
}

void CServiceSession::SendReloadSchedule(int nWorldID)
{
	TReloadGuildwarSchedule packet;
	memset(&packet, 0, sizeof(TReloadGuildwarSchedule));

	packet.nWorldID = nWorldID;

	AddSendData(NETSERVICE_RELOADGUILDWARSCHEDULE, 0, (char*)&packet, sizeof(TReloadGuildwarSchedule));
}

void CServiceSession::SendChangeLimitedItemMax(int nSN, int nLimitMax)
{
	TChangeLimitItemMax packet;
	memset(&packet, 0, sizeof(TChangeLimitItemMax));

	packet.nSN = nSN;
	packet.nLimiteMax = nLimitMax;

	AddSendData(NETSERVICE_LIMITEITEM_CHANGEQUANTITY, 0, (char*)&packet, sizeof(TReloadGuildwarSchedule));
}

void CServiceSession::SendReloadDynCode()
{
	AddSendData(NETSERVICE_RELOADDYNCODE, 0, NULL, 0);
}

bool CServiceSession::IsCompleteConnected(ULONG nCurTick)
{
	if (m_nCreateSessionTick <= 0)
		return true;

	if (m_nCreateSessionTick + ONEMINTICK < nCurTick)
		return false;
	return true;
}

void CServiceSession::SendUpdateWorldPvPRoom()
{
	TApplyWorldPvPRoom packet;
	memset(&packet, 0, sizeof(TApplyWorldPvPRoom));	

	AddSendData(NETSERVICE_UPDATEWORLDPVPROOM, 0, (char*)&packet, sizeof(TApplyWorldPvPRoom));
}

void CServiceSession::SendCreateGambleRoom(int nRoomIndex, int nRegulation, int nGambleType, int nPrice )
{
	TCreateGambleRoom packet;
	memset(&packet, 0, sizeof(TCreateGambleRoom));

	
	packet.nRoomIndex = nRoomIndex;	
	packet.bRegulation = (bool)nRegulation;
	packet.nGambleType = nGambleType;
	packet.nPrice = nPrice;

	AddSendData(NETSERVICE_CREATEGAMBLEROOM, 0, (char*)&packet, sizeof(TCreateGambleRoom));
}

void CServiceSession::SendStopGambleRoom()
{
	TStopGambleRoom packet;
	memset(&packet, 0, sizeof(TStopGambleRoom));
	
	AddSendData(NETSERVICE_STOPGAMBLEROOM, 0, (char*)&packet, sizeof(TStopGambleRoom));
}

void CServiceSession::SendDelGambleRoom()
{
	TDelGambleRoom packet;
	memset(&packet, 0, sizeof(TDelGambleRoom));

	AddSendData(NETSERVICE_DELGAMBLEROOM, 0, (char*)&packet, sizeof(TDelGambleRoom));
}

void CServiceSession::SendUpdateDWCStatus()
{
	TUpdateDWCStatus packet;
	memset(&packet, 0, sizeof(TUpdateDWCStatus));

	AddSendData(NETSERVICE_UPDATE_DWCSTATUS, 0, (char*)&packet, sizeof(TUpdateDWCStatus));
}
