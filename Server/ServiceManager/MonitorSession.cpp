
#include "stdafx.h"
#include "MonitorSession.h"
#include "DNServerPacket.h"
#include "ServiceManager.h"
#include "PatchWatcher.h"
#include "ServiceManager.h"
#include "Log.h"
extern CLog g_MonitorLog;

extern TServiceManagerConfig g_Config;

CMonitorSession::CMonitorSession()
{
	Init(1024 * 100, 1024 * 100);
	m_MonitorSessionLevel = SERVERMONITOR_LEVEL_NONE;
	memset(m_szID, 0, sizeof(m_szID));
	memset(m_szPass, 0, sizeof(m_szPass));
}

int CMonitorSession::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	if (m_MonitorSessionLevel == SERVERMONITOR_LEVEL_NONE)
	{
		//아직 접속처리가 되지 않은 녀석이다.
		if (iMainCmd == MONITOR2MANAGER_CHECK_LOGIN)
		{
			TSMCheckLogin * pPacket = (TSMCheckLogin*)pData;

			int nRet = g_pServiceManager->CheckMonitorLogin(pPacket->szID, pPacket->szPass, m_MonitorSessionLevel, pPacket->m_Version);
			SendMonitorLoginResult(nRet);
			if (nRet == ERROR_NONE)
			{
				STRCPYA(m_szID, sizeof(m_szID), pPacket->szID);
				STRCPYA(m_szPass, sizeof(m_szPass), pPacket->szPass);
			}
			
			/*if (bRet == false)
				DetachConnection(L"LoginFailed");*/
		}

		return ERROR_NONE;
	}
	else
	{
		//접속이 되어 있다면 요청이 가능한 유저인지 확인
		if (CheckAllowMsg(iMainCmd) == false)
		{
			SendOperatingResult(iMainCmd, ERROR_SM_YOU_ARE_NOT_ALLOWED_LEVEL);
			return ERROR_NONE;
		}
	}
	
	switch (iMainCmd)
	{
	case MONITOR2MANAGER_SERVERSTRUCT_REQ:
		{
			TSMServerStructReq * pPacket = (TSMServerStructReq*)pData;

			// NetLauncherList
			SendNetLauncherList(pPacket->nContainerVersion, 0);

			// MeritInfo
			SendMeritList(pPacket->nContainerVersion, 0);

			// ServerInfo
			SendServerInfoList(pPacket->nContainerVersion, 0);

			// ChannelInfo
			SendChannelInfolist(pPacket->nContainerVersion, 0);

			break;
		}

	case MONITOR2MANAGER_REALTIME_REQ:
		{
			TSMRealTimeReq * pPacket = (TSMRealTimeReq*)pData;

			// World
			SendWorldInfo(pPacket->nContainetVersion);

			// State
			SendServerState(0);

			// Login
			SendLoginUpdate(0);

			// Village
			SendVillageUpdate(0);

			// Game
			SendGameUpdate(0);

			// Farm
			SendFarmUpdate();

			break;
		}

	case MONITOR2MANAGER_STARTSERVICE:
		{
			TSMServiceStart * pPacket = (TSMServiceStart*)pData;

			int nRet = ERROR_SM_UNKNOWN_OPERATING_TYPE;

			switch (pPacket->nStartType)
			{
			case _SERVICE_START_ALL:
				{
					nRet = g_pServiceManager->StartServiceAll();
					break;
				}

			case _SERVICE_START_EACH:
				{
					for (int i = 0; i < pPacket->cCount; i++)
						nRet = g_pServiceManager->StartServiceEach(pPacket->nIDs[i], false);
					break;
				}

			case _SERVICE_START_EACH_BY_LAUNCHERID:
				{
					for (int i = 0; i < pPacket->cCount; i++)
						nRet = g_pServiceManager->StartServiceEachbyLauncher(pPacket->nIDs[i]);
					break;
				}
			}
			SendOperatingResult(iMainCmd, nRet);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MONITOR2MANAGER_STARTSERVICE [StartType:%d][Result:%d]\n", GetIp(), pPacket->nStartType, nRet);
			break;
		}

	case MONITOR2MANAGER_STOPSERVICE:
		{
			TSMServiceStop * pPacket = (TSMServiceStop*)pData;

			int nRet = ERROR_SM_UNKNOWN_OPERATING_TYPE;

			switch (pPacket->nStopType)
			{
			case _SERVICE_STOP_ALL:
				{
					nRet = g_pServiceManager->TerminateServiceAll();
					break;
				}

			case _SERVICE_STOP_ALL_FORCE:
				{
					nRet = g_pServiceManager->TerminateServiceForce();
					break;
				}

			case _SERVICE_STOP_EACH:
				{
					for (int i = 0; i < pPacket->cCount; i++)
						nRet = g_pServiceManager->TerminateServiceEach(pPacket->nIDs[i]);
					break;
				}

			case _SERVICE_STOP_EACH_FORCE:				
				{
					for (int i = 0; i < pPacket->cCount; i++)
						nRet = g_pServiceManager->TerminateServiceEachForce(pPacket->nIDs[i]);
					break;
				}

			case _SERVICE_STOP_EACH_BY_LAUNCHERID:
				{
					for (int i = 0; i < pPacket->cCount; i++)
						nRet = g_pServiceManager->TerminateServiceEachbyLauncher(pPacket->nIDs[i]);
					break;
				}

			case _SERVICE_STOP_EACH_FORCE_BY_LAUNCHERID:
				{
					for (int i = 0; i < pPacket->cCount; i++)
						nRet = g_pServiceManager->TerminateServiceForceEachbyLauncher(pPacket->nIDs[i]);
					break;
				}
			}
			SendOperatingResult(iMainCmd, nRet);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MONITOR2MANAGER_STOPSERVICE [StopType:%d][Result:%d]\n", GetIp(), pPacket->nStopType, nRet);
			break;
		}

	case MONITOR2MANAGER_MAKEDUMP:
		{
			TSMMakeDump * pPacket = (TSMMakeDump*)pData;

			int nRet = ERROR_SM_UNKNOWN_OPERATING_TYPE;
			for (int i = 0; i < pPacket->cCount; i++)
			{
				nRet = g_pServiceManager->MakeDumEach(pPacket->nSID[i]);
				if (nRet != ERROR_NONE)
					SendOperatingResult(iMainCmd, nRet);
			}
			SendOperatingResult(iMainCmd, nRet);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MONITOR2MANAGER_MAKEDUMP [Result:%d]\n", GetIp(), nRet);
			break;
		}

	case MONITOR2MANAGER_CHANNELCONTROL:
		{
			TSMChannelControl * pPacket = (TSMChannelControl*)pData;

			int nRet = ERROR_SM_UNKNOWN_OPERATING_TYPE;
			for (int i = 0; i < pPacket->cCount; i++)
			{
				int nMasterSID = g_pServiceManager->GetMasterSIDByChannelID(pPacket->Control[i].nWorldID, pPacket->Control[i].nChannelID);
				if (nMasterSID > 0)
					nRet = g_pServiceManager->ChannelControl(nMasterSID, pPacket->Control[i].nChannelID, pPacket->bVisibility, -1);
				if (nRet != ERROR_NONE)
					SendOperatingResult(iMainCmd, nRet);
			}
			SendOperatingResult(iMainCmd, nRet);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MONITOR2MANAGER_CHANNELCONTROL [Result:%d]\n", GetIp(), nRet);
			break;
		}

	case MONITOR2MANAGER_GAMECONTROL:
		{
			TSMGameControl * pPacket = (TSMGameControl*)pData;

			int nRet = ERROR_SM_UNKNOWN_OPERATING_TYPE;
			int nMasterSID[WORLDCOUNTMAX];
			memset(nMasterSID, 0, sizeof(nMasterSID));

			for (int i = 0; i < pPacket->cCount; i++)
			{
				memset(nMasterSID, 0, sizeof(nMasterSID));
				if (g_pServiceManager->GetMasterSIDByGameSID(pPacket->nSID[i], nMasterSID))
				{
					for (int j = 0; j < WORLDCOUNTMAX; j++)
					{
						if (nMasterSID[j] <= 0) continue;
						nRet = g_pServiceManager->PopulationControl(nMasterSID[j], pPacket->nSID[i], pPacket->bClose);
						if (nRet != ERROR_NONE)
							SendOperatingResult(iMainCmd, nRet);
					}
				}
			}

			SendOperatingResult(iMainCmd, nRet);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MONITOR2MANAGER_GAMECONTROL\n", GetIp());
			break;
		}
	case MANAGER2MONITOR_SERVER_DEALYINFO_REQ:
		{
			TMSServerDealyReq* pPacket = (TMSServerDealyReq*)pData;
			for (int i=0; i<SERVICEMANAGER_SERVERDEALY_SIZE; i++)
				g_pServiceManager->AddServerDelayMID(pPacket->nType, pPacket->nSID);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MANAGER2MONITOR_SERVER_DEALYINFO_REQ\n", GetIp());
		}
		break;

	case MONITOR2MANAGER_CHANGEPASSWORD:
		{
			TChangePassword* pPacket = (TChangePassword*)pData;
			int nRet = CDataManager::GetInstance()->ChangeUserPassword(pPacket);
			SendOperatingResult(iMainCmd, nRet);
			g_MonitorLog.Log(LogType::_FILELOG, L"[IP:%S] MONITOR2MANAGER_CHANGEPASSWORD [User:%S]\n", GetIp(), pPacket->szUserName);
		}
		break;

#if defined (_SERVICEMANAGER_EX)
	case MONITOR2MANAGER_REPORT_REQ:
		{
			TSMReportReq* pPacket = (TSMReportReq*)pData;
			if (pPacket->bBuild)
			{
				if (!m_ReportBuffer.empty())
					break;

				g_pServiceManager->BuildExceptionReport((size_t)pPacket->cDays, m_ReportBuffer);
			}

			SendServiceManagerReport(pPacket->nPage);
		}
		break;
#endif // #if defined (_SERVICEMANAGER_EX)

	}
	return ERROR_NONE;
}

void CMonitorSession::SendMonitorLoginResult(int nRetCode)
{
	TMSLoginResult packet;
	memset(&packet, 0, sizeof(packet));

	packet.nRetCode = nRetCode;
	packet.nMonitorLevel = m_MonitorSessionLevel;
	packet.nContainerVersion = g_pServiceManager->GetContainerVersion();

	AddSendData(MANAGER2MONITOR_LOGIN_RESULT, 0, (char*)&packet, sizeof(packet));
}

void CMonitorSession::SendMeritList(int nContainerVersion, int nLastRecvID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSMeritList packet;
		memset(&packet, 0, sizeof(TMSMeritList));

		packet.nWholeCount = g_pServiceManager->GetMeritList(nContainerVersion, nLastRecvID, packet.cIsLast, packet.nCount, packet.Info, sizeof(packet.Info)/sizeof(*packet.Info));
		packet.nLastReqMeritID = nLastRecvID;
		packet.nContainerVersion = nContainerVersion;
		if (g_pServiceManager->GetContainerVersion() != nContainerVersion)
			packet.nRet = ERROR_SM_UNMATCHED_REQ;

		cIsLast = packet.cIsLast;
		if (packet.nCount > 0)
			nLastRecvID = packet.Info[packet.nCount-1].nID;

		AddSendData(MANAGER2MONITOR_MERITLIST, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Info) + (sizeof(sMonitorChannelMeritInfo) * packet.nCount));	
	}
}


void CMonitorSession::SendNetLauncherList(int nContainerVersion, int nLastRecvLauncherID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSNetLauncherList packet;
		memset(&packet, 0, sizeof(TMSNetLauncherList));

		packet.nWholeCount = g_pServiceManager->GetNetLauncherList(nContainerVersion, nLastRecvLauncherID, packet.cIsLast, packet.nCount, packet.Info, sizeof(packet.Info)/sizeof(*packet.Info));
		packet.nLastReqNLID = nLastRecvLauncherID;
		packet.nContainerVersion = nContainerVersion;
		if (g_pServiceManager->GetContainerVersion() != nContainerVersion)
			packet.nRet = ERROR_SM_UNMATCHED_REQ;

		cIsLast = packet.cIsLast;

		if (packet.nCount > 0)
			nLastRecvLauncherID = packet.Info[packet.nCount-1].nID;

		AddSendData(MANAGER2MONITOR_NETLAUNCHERLIST, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Info) + (sizeof(sMonitorNetLauncherInfo) * packet.nCount));	
	}
}

void CMonitorSession::SendChannelInfolist(int nContainerVersion, INT64 nLastRecvCHID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSMonitorChannelList packet;
		memset(&packet, 0, sizeof(packet));

		packet.nWholeCount = g_pServiceManager->GetChannelList(nContainerVersion, nLastRecvCHID, packet.cIsLast, packet.nCount, packet.Info, sizeof(packet.Info)/sizeof(*packet.Info));
		packet.nLastReqCHID = nLastRecvCHID;
		packet.nContainerVersion = nContainerVersion;
		if (g_pServiceManager->GetContainerVersion() != nContainerVersion)
			packet.nRet = ERROR_SM_UNMATCHED_REQ;

		cIsLast = packet.cIsLast;
		if (packet.nCount > 0)
			nLastRecvCHID = MAKE_SM_SVRCHN(packet.Info[packet.nCount-1].nSID, packet.Info[packet.nCount-1].nChannelID);

		AddSendData(MANAGER2MONITOR_CHANNELINFO_LIST, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Info) + (sizeof(sMonitorChannelInfo) * packet.nCount));
	}
}

void CMonitorSession::SendServerInfoList(int nContainerVersion, int nLastRecvSID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMonitorServerInfoList packet;
		memset(&packet, 0, sizeof(packet));

		packet.nWholeCount = g_pServiceManager->GetServerList(nContainerVersion, nLastRecvSID, packet.cIsLast, packet.nCount, packet.Info, sizeof(packet.Info)/sizeof(*packet.Info));
		packet.nLastReqSID = nLastRecvSID;
		packet.nContainerVersion = nContainerVersion;
		if (g_pServiceManager->GetContainerVersion() != nContainerVersion)
			packet.nRet = ERROR_SM_UNMATCHED_REQ;

		cIsLast = packet.cIsLast;

		if (packet.nCount > 0)
			nLastRecvSID = packet.Info[packet.nCount-1].nSID;

		AddSendData(MANAGER2MONITOR_SERVERINFO_LIST, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Info) + (sizeof(sMonitorServerInfo) * packet.nCount));
	}
}

void CMonitorSession::SendUpdateNotice(int nUpdateType)
{
	TMSUpdateNotice packet;
	memset(&packet, 0, sizeof(packet));

	packet.nNoticeType = nUpdateType;
	packet.nContainerVersion = g_pServiceManager->GetContainerVersion();

	AddSendData(MANAGER2MONITOR_UPDATE_NOTICE, 0, (char*)&packet, sizeof(packet));
}

void CMonitorSession::SendUpdateServerState(int nSID, int nServerState, int nServiceException, const char * pDetail)
{
	TMSServerStateUpdate packet;
	memset(&packet, 0, sizeof(packet));

	packet.nSID = nSID;
	packet.nServerState = nServerState;
	packet.nServerException = nServiceException;
	if (pDetail)
		STRCPYA(packet.szDetail, 128, pDetail);

	AddSendData(MANAGER2MONITOR_UPDATE_SERVERSTATE, 0, (char*)&packet, sizeof(packet));
}
void CMonitorSession::SendServerDelayInfo(int nType, std::vector<int>& vServerDelayMIDs)
{
	int nCount = (int)vServerDelayMIDs.size();
	if (nCount > SERVICEMANAGER_SERVERDEALY_SIZE)
		nCount = SERVICEMANAGER_SERVERDEALY_SIZE;

	TMSServerDealyInfo packet;
	memset(&packet, 0, sizeof(packet));

	packet.nType = nType;
	packet.nCount = nCount;
	memcpy (packet.nMIDs, &vServerDelayMIDs[0], sizeof(int)*nCount);

	AddSendData(MANAGER2MONITOR_SERVER_DEALYINFO, 0, (char*)&packet, sizeof(packet));
}

void CMonitorSession::SendWorldInfo(int nContainerVersion)
{
	TMSWorldInfo packet;
	memset(&packet, 0, sizeof(packet));

	g_pServiceManager->GetWorldInfo(nContainerVersion, packet.cCount, packet.WorldInfo);
	if (g_pServiceManager->GetContainerVersion() != nContainerVersion)
		packet.nRet = ERROR_SM_UNMATCHED_REQ;

	AddSendData(MANAGER2MONITOR_WORLDINFO_RESULT, 0, (char*)&packet, sizeof(packet) - sizeof(packet.WorldInfo) + (sizeof(sMonitorWorldInfo) * packet.cCount));
}

void CMonitorSession::SendLoginUpdate(int nLastRecvLoginSID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSLoginUserCount packet;
		memset(&packet, 0, sizeof(packet));

		packet.nWholeCount = g_pServiceManager->GetLoginUpdateList(nLastRecvLoginSID, packet.cIsLast, packet.nCount, packet.Update, sizeof(packet.Update)/sizeof(*packet.Update));
		packet.nLastReqLoginSID = nLastRecvLoginSID;

		cIsLast = packet.cIsLast;
		if (packet.nCount > 0)
			nLastRecvLoginSID = packet.Update[packet.nCount-1].nSID;

		AddSendData(MANAGER2MONITOR_LOGIN_UPDATEINFO, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Update) + (sizeof(sUserCountInfo) * packet.nCount));
	}
}

void CMonitorSession::SendVillageUpdate(INT64 uiLastRecvCHID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSVillageUserCount packet;
		memset(&packet, 0, sizeof(packet));

		packet.nWholeCount = g_pServiceManager->GetVillageUpdateList(uiLastRecvCHID, packet.cIsLast, packet.cCount, packet.Update, sizeof(packet.Update)/sizeof(*packet.Update));
		packet.uiLastReqCHID = uiLastRecvCHID;

		cIsLast = packet.cIsLast;
		if (packet.cCount > 0)
			uiLastRecvCHID = MAKE_SM_SVRCHN(packet.Update[packet.cCount - 1].nSID, packet.Update[packet.cCount - 1].nChannelID);

		AddSendData(MANAGER2MONITOR_VILLAGE_UPDATEINFO, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Update) + (sizeof(sChannelUpdateInfo) * packet.cCount));
	}
}

void CMonitorSession::SendGameUpdate(int nLastRecvGameSID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSGameUserCount packet;
		memset(&packet, 0, sizeof(packet));

		packet.nWholeCount = g_pServiceManager->GetGameUpdateList(nLastRecvGameSID, packet.cIsLast, packet.nCount, packet.Update, sizeof(packet.Update)/sizeof(*packet.Update));
		packet.nLastReqGameSID = nLastRecvGameSID;

		cIsLast = packet.cIsLast;
		if (packet.nCount > 0)
			nLastRecvGameSID = packet.Update[packet.nCount - 1].nSID;

		AddSendData(MANAGER2MONITOR_GAME_UPDATEINFO, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Update) + (sizeof(sGameUserInfo) * packet.nCount));
	}
}

void CMonitorSession::SendFarmUpdate()
{
	BYTE cIsLast = 0;
	int nFarmCount = 0;
	while (!cIsLast)
	{
		TMSFarmUserCount packet;
		memset(&packet, 0, sizeof(packet));

		g_pServiceManager->GetFarmUpdateList(nFarmCount, packet.nCount, packet.Update, sizeof(packet.Update)/sizeof(*packet.Update));
		
		if (packet.nCount > 0)
			nFarmCount += packet.nCount;
		else
			cIsLast = true;
		
		AddSendData(MANAGER2MONITOR_FARM_UPDATEINFO, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Update) + (sizeof(sGameUserInfo) * packet.nCount));
	}
}


void CMonitorSession::SendServerState(int nLastRecvSID)
{
	BYTE cIsLast = 0;
	while (!cIsLast)
	{
		TMSServerStateList packet;
		memset(&packet, 0, sizeof(packet));

		packet.nWholeCount = g_pServiceManager->GetServerStateList(nLastRecvSID, packet.cIsLast, packet.nCount, packet.State, sizeof(packet.State)/sizeof(*packet.State));
		packet.nLastReqSID = nLastRecvSID;

		cIsLast = packet.cIsLast;
		if (packet.nCount > 0)
			nLastRecvSID = packet.State[packet.nCount-1].nSID;

		AddSendData(MANAGER2MONITOR_SERVERSTATE_LIST, 0, (char*)&packet, sizeof(packet) - sizeof(packet.State) + (sizeof(sServerStateInfo) * packet.nCount));
	}
}

void CMonitorSession::SendOperatingResult(int nMainCmd, int nRetCode)
{
	TMSOperatingResult packet;
	memset(&packet, 0, sizeof(packet));

	packet.nCmd = nMainCmd;
	packet.nRet = nRetCode;

	AddSendData(MANAGER2MONITOR_OPERATING_RESULT, 0, (char*)&packet, sizeof(packet));
}

void CMonitorSession::SendPatchState()
{
}

bool CMonitorSession::CheckAllowMsg(int nMainCmd)
{
	if (SERVERMONITOR_LEVEL_SUPERADMIN == m_MonitorSessionLevel)
		return true;

	if (SERVERMONITOR_LEVEL_ADMIN == m_MonitorSessionLevel)
	{
		switch (nMainCmd)
		{
		case MONITOR2MANAGER_CHECK_LOGIN:
		case MONITOR2MANAGER_SERVERSTRUCT_REQ:
		case MONITOR2MANAGER_REALTIME_REQ:
		case MONITOR2MANAGER_STARTSERVICE:
			return true;
		}
	}
	else if (SERVERMONITOR_LEVEL_MONITOR == m_MonitorSessionLevel)
	{
		switch (nMainCmd)
		{
		case MONITOR2MANAGER_CHECK_LOGIN:
		case MONITOR2MANAGER_SERVERSTRUCT_REQ:
		case MONITOR2MANAGER_REALTIME_REQ:
			return true;
		}
	}
	return false;
}

#if defined (_SERVICEMANAGER_EX)
void CMonitorSession::SendServiceManagerReport(size_t nPage)
{
	TMSReportReslut packet;
	memset(&packet, 0, sizeof(packet));
	packet.nPage = (int)nPage;
	packet.nMaxPage = (int)m_ReportBuffer.size() / SERVICEMANAGER_REPORT_SIZE;
	if (m_ReportBuffer.size() % SERVICEMANAGER_REPORT_SIZE > 0)
		++packet.nMaxPage;

	packet.bEnd = ReadServiceManagerReport(nPage, packet.szReport);
	if (packet.bEnd)
		m_ReportBuffer.clear();

	AddSendData(MANAGER2MONITOR_REPORT_RESULT, 0, (char*)&packet, sizeof(packet));
	g_Log.Log(LogType::_NORMAL, L"Send service manager report to service monitor [ip: %s] [page: %d / %d]", GetwszIp(), packet.nPage, packet.nMaxPage);
}

bool CMonitorSession::ReadServiceManagerReport(size_t nPage, OUT wchar_t* pBuffer) const
{
	size_t size = m_ReportBuffer.size();
	size_t pos = (nPage - 1) * SERVICEMANAGER_REPORT_SIZE;
	if (size < pos)
		return true;

	if (size < pos + SERVICEMANAGER_REPORT_SIZE)
	{
		std::copy(m_ReportBuffer.begin() + pos, m_ReportBuffer.begin() + pos + (size - pos), pBuffer);
		return true;
	}

	std::copy(m_ReportBuffer.begin() + pos, m_ReportBuffer.begin() + pos + SERVICEMANAGER_REPORT_SIZE, pBuffer);
	pBuffer[SERVICEMANAGER_REPORT_SIZE] = '\0';
	return false;
}
#endif // #if defined (_SERVICEMANAGER_EX)