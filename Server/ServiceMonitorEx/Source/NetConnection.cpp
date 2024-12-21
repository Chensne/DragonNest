#include "StdAfx.h"
#include "NetConnection.h"
#include "NetSession.h"
#include "ServiceMonitor.h"
#include "ErrorLogMngr.h"
#include "MainFrm.h"
#include "BaseErrorLogView.h"
#include "BaseInformationView.h"
#include "BasePartitionView.h"
#include <MMSystem.h>	// PlaySound

#include "ServiceInfo.h"

#pragma comment(lib, "winmm.lib")

#define DNPRJSAFE
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"
#include "ConnectionInfoMngr.h"

CNetConnection::CNetConnection() : m_IsOpen(FALSE)
{
	m_pNetSession = NULL;
}

CNetConnection::~CNetConnection()
{

}

DWORD CNetConnection::Open()
{

	m_PacketParser.SetPacketNotifyProc(PacketNotify);

	m_TcpClient.SetErrorNotifyProc(TcpClientErrorNotifyProc);

	BOOL bRetVal = m_TcpClient.Start();
	if (!bRetVal)
		BASE_RETURN(HASERROR+1);
	
	m_IsOpen = TRUE;

	return 0;
}

VOID CNetConnection::Close()
{
	m_TcpClient.End();

	m_IsOpen = FALSE;
}

CNetSession* CNetConnection::GetSession()
{
	if (!m_pNetSession)
	{
		m_pNetSession = m_TcpClient.GetSession(
			TcpClientSessionBaseNotifyProc,
			CNetPacketParser::PacketParse,
			NULL,
			reinterpret_cast<LPVOID>(&m_PacketParser),
			DF_NETCLIENTSESSION_DEFAULT_RECVBUFSIZE,
			DF_NETCLIENTSESSION_DEFAULT_SENDBUFSIZE
			);
	}
	
	if (!m_pNetSession)
		BASE_RETURN(NULL);
	
	return m_pNetSession;
}

VOID CNetConnection::ReleaseSession(CNetSession* pNetSession)
{
	m_TcpClient.ReleaseSession(pNetSession);
}

CServiceInfo* CNetConnection::GetServiceInfo ()
{
	return m_pNetSession->GetServiceInfo();
}

VOID CNetConnection::TcpClientErrorNotifyProc(LPVOID pModule, INT /*pErrorCode*/, LPCTSTR pErrorMessage, LPVOID /*pErrorNotifyProcParam*/)
{
	CEventSelectTcpClient<CNetSession>* aNetClient = static_cast<CEventSelectTcpClient<CNetSession>*>(pModule);

	if (pErrorMessage) 
	{
		// 에러 출력 필요
		pErrorMessage;
	}
}

VOID CNetConnection::TcpClientSessionBaseNotifyProc(EF_NETWORK_BASE_NOFITY pNotifyType, CSessionBase* pSession, LPVOID pResultParam, LPVOID /*pBaseNotifyProcParam*/)
{
	CNetSession* pNetSession = static_cast<CNetSession*>(pSession);
	ULONG nConID = (UINT)pNetSession->GetRemoteIpAddressN();

	switch(pNotifyType) {
	case EV_NBN_ERROR:
		{
		
			CBaseErrorLogView* aBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
			if (!aBaseErrorLogView)
				return;

			aBaseErrorLogView->AddGridList(0, reinterpret_cast<LPCTSTR>(pResultParam), TRUE);

			CConnectionDialog& aConnectionDialog = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetConnectionDialog();
			if (::IsWindow(aConnectionDialog.GetSafeHwnd())) {
				aConnectionDialog.EnableButton(FALSE);	// 네트워크 모듈 내부에서 소켓을 닫았을 수 있으니 일단 연결 버튼 활성화
			}

			pNetSession->GetServiceInfo()->ResetAllRecvMode();
		}
		break;
	case EV_NBN_SESSCNNT: 
		{

			CConnectionDialog& aConnectionDialog = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetConnectionDialog();
			if (::IsWindow(aConnectionDialog.GetSafeHwnd())) {
				aConnectionDialog.EnableButton(TRUE);
				aConnectionDialog.SaveConfig();
				aConnectionDialog.ShowWindow(SW_HIDE);
			}

			pNetSession->GetServiceInfo()->ResetAllRecvMode();
			USES_CONVERSION;

			TSMCheckLogin aReq;
			aReq.m_Version = GET_SM_VERSION();

			stConnectionInfo ConInfo;
			if (CConnectionInfoMngr::GetInstancePtr()->GetConInfo(pNetSession->GetRemoteIpAddressN(), ConInfo))
			{
				strncpy_s(aReq.szID, ConInfo.szUserName, _countof(aReq.szID));
				strncpy_s(aReq.szPass, ConInfo.szPassWord, _countof(aReq.szPass));
			}
			else
			{
				strncpy_s(aReq.szID, CT2A(aConnectionDialog.GetID()), _countof(aReq.szID));
				strncpy_s(aReq.szPass, CT2A(aConnectionDialog.GetPass()), _countof(aReq.szPass));
			}
			
			pNetSession->SendData(MONITOR2MANAGER_CHECK_LOGIN, 0, reinterpret_cast<CHAR*>(&aReq), sizeof(aReq));

			::PlaySound(NULL, ::AfxGetInstanceHandle(), SND_ASYNC);
		}
		break;
	case EV_NBN_SESSDCNT: 
		{
			BASE_TRACE(_T("Session Disconnected (%d)\n"), pNetSession->GetSocket());

			CConnectionDialog& aConnectionDialog = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetConnectionDialog();
			if (::IsWindow(aConnectionDialog.GetSafeHwnd())) {
				aConnectionDialog.EnableButton(FALSE);
//				aConnectionDialog.ShowWindow(SW_SHOW);
			}

			pNetSession->GetServiceInfo()->ResetAllRecvMode();

			::PlaySound(NULL, ::AfxGetInstanceHandle(), SND_ASYNC);
		}
		break;
	default:
		break;
	}
	
}

DWORD CNetConnection::PacketNotify(LPVOID pSession, SHORT pMainCmd, SHORT pSubCmd, LPBYTE pBuffer, SHORT pPacketLen)
{
	if (pPacketLen <= 0) 
		return NOERROR;

	if (sizeof(DNTPacketHeader) >= pPacketLen) 
		return NOERROR;

	if (pBuffer == NULL) 
		return NOERROR;

	CNetSession* pNetSession = static_cast<CNetSession*>(pSession);
	if (!pNetSession) 
		return NOERROR;

	ULONG nConID = pNetSession->GetRemoteIpAddressN();

	TRACE(CVarArg<MAX_PATH>(_T("Recv Packet : %d / %d\n"), pMainCmd, pSubCmd));

	switch(pMainCmd) 
	{
	case MANAGER2MONITOR_LOGIN_RESULT:	// // 로그인
		{
			const TMSLoginResult* aAck = reinterpret_cast<const TMSLoginResult*>(pBuffer);

			// 실패
			if (ERROR_NONE != aAck->nRetCode) 
			{
				pNetSession->Disconnect();

				CBaseErrorLogView* pBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
				if (!pBaseErrorLogView)
					return NOERROR;

				if (::IsWindow(pBaseErrorLogView->GetSafeHwnd()) == FALSE)
					return NOERROR;

				pBaseErrorLogView->AddGridList(aAck->nRetCode, CVarArg<MAX_PATH>(_T("Service Manager Login Failed (%02d)"), aAck->nRetCode), TRUE);

				switch(aAck->nRetCode) 
				{
				case ERROR_SM_LOGINFAIL_VERSIONUNMATCHED:
					{
						::AfxMessageBox(CVarArg<MAX_PATH>(_T("Version is not match. (%d.%d.%d)\nUse new version of client to connect."), g_SMBuildVersionMajor, g_SMBuildVersionMinor, g_SMBuildVersionRevision), MB_ICONINFORMATION | MB_OK);
						::ExitProcess(0);
					}
					return NOERROR;
				default:
					break;
				}
			}
			else 
			{
				// 성공
				CBaseErrorLogView* pBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
				if (!pBaseErrorLogView)
					return NOERROR;

				if (::IsWindow(pBaseErrorLogView->GetSafeHwnd()) == FALSE)
					return NOERROR;

				if (SERVERMONITOR_LEVEL_NONE >= aAck->nMonitorLevel) 
				{
					pBaseErrorLogView->AddGridList(aAck->nRetCode, CVarArg<MAX_PATH>(_T("Account Monitor Level Error (%d)"), aAck->nMonitorLevel), TRUE);
					pNetSession->Disconnect();
					break;
				}

				pNetSession->SetMonitorLevel(aAck->nMonitorLevel);

				SendRequestServiceStruct(pNetSession, aAck->nContainerVersion);

				pBaseErrorLogView->AddGridList(aAck->nRetCode, CVarArg<MAX_PATH>(_T("Service Manager Login Succeeded (Monitor Level : %d)"), pNetSession->GetMonitorLevel()), FALSE);
			}
		}
		break;

		//================================================================================

		// 서버구조구축
	case MANAGER2MONITOR_NETLAUNCHERLIST:		// NET LUNCHER			// 순서1
		{
			const TMSNetLauncherList* aAck = reinterpret_cast<const TMSNetLauncherList*>(pBuffer);
			if (ERROR_NONE != aAck->nRet)
				break;

			INT aContainerVersion = aAck->nContainerVersion;
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				if (pNetSession->GetServiceInfo()->GetContainerVersion() != aAck->nContainerVersion)
					break;

				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) 
				{
					NETLUNCHERINFO NetLuncherInfo(aAck->Info[nIndex].nID, ::inet_addr(CW2A(aAck->Info[nIndex].wszIP)), ::inet_addr(CW2A(aAck->Info[nIndex].wszPublicIP)));
					if (NOERROR != pNetSession->GetServiceInfo()->AddNetLuncher(EV_SMT_TEMP, &NetLuncherInfo, FALSE))
					{
						::AfxMessageBox(_T("Invalid NetLauncher(1) Information, click confirm to reconnect"), MB_ICONERROR);
						return HASERROR;
					}
				}
			}
		}
		break;
	case MANAGER2MONITOR_MERITLIST:				// MERIT				// 순서2
		{
			const TMSMeritList* aAck = reinterpret_cast<const TMSMeritList*>(pBuffer);
			if (ERROR_NONE != aAck->nRet)
				break;

			INT aContainerVersion = aAck->nContainerVersion;
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				if (pNetSession->GetServiceInfo()->GetContainerVersion() != aAck->nContainerVersion)
					break;

				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) {
					MERITINFO aMeritInfo(aAck->Info[nIndex].nID, aAck->Info[nIndex].nMinLevel, aAck->Info[nIndex].nMaxLevel, aAck->Info[nIndex].nMeritType, aAck->Info[nIndex].nExtendValue);

					if (NOERROR != pNetSession->GetServiceInfo()->AddMerit(EV_SMT_TEMP, &aMeritInfo, FALSE))
					{
						::AfxMessageBox(_T("Invalid Merit(1) Information, click confirm to reconnect"), MB_ICONERROR);
						return HASERROR;
					}
				}
			}
		}
		break;
	case MANAGER2MONITOR_SERVERINFO_LIST:		// SERVER				// 순서3
		{
			const TMonitorServerInfoList* aAck = reinterpret_cast<const TMonitorServerInfoList*>(pBuffer);
			if (ERROR_NONE != aAck->nRet)
				break;

			INT aContainerVersion = aAck->nContainerVersion;
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());
				if (pNetSession->GetServiceInfo()->GetContainerVersion() != aAck->nContainerVersion)
					break;


				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) 
				{
					const NETLUNCHERINFO* pNetLuncherInfo = pNetSession->GetServiceInfo()->AtNetLuncher(EV_SMT_TEMP, aAck->Info[nIndex].nManagedLauncherID, FALSE);
					if (!pNetLuncherInfo || !pNetLuncherInfo->IsSet()) 
						continue;

					INT WorldID[WORLDCOUNTMAX] = { EV_WORLDID_DEF, };
					for (int nCount = 0 ; WORLDCOUNTMAX > nCount ; ++nCount) 
					{
						if (!aAck->Info[nIndex].cWorldIDs[nCount]) 
							break;

						WorldID[nCount] = aAck->Info[nIndex].cWorldIDs[nCount];

						WORLDINFO WorldInfo(static_cast<CHAR>(WorldID[nCount]));
						WorldInfo.m_MaxUserLimit = 0;
						pNetSession->GetServiceInfo()->AddWorld(EV_SMT_TEMP, &WorldInfo, FALSE);
					}

					switch(aAck->Info[nIndex].nServerType) 
					{
					case EV_SVT_MA:
					case EV_SVT_GA:
					case EV_SVT_VI:
					case EV_SVT_CA:
						{
							if (EV_WORLDID_DEF == aAck->Info[nIndex].cWorldIDs[0]) 
							{
								::AfxMessageBox(_T("Invalid Server(1) Information, click confirm to reconnect"), MB_ICONERROR);
								return HASERROR;
							}
						}
						break;
					}

					SERVERINFO ServerInfo(static_cast<EF_SERVERTYPE>(aAck->Info[nIndex].nServerType), aAck->Info[nIndex].nSID, aAck->Info[nIndex].nManagedLauncherID, 
					CA2T(aAck->Info[nIndex].szResourceRevision), CA2T(aAck->Info[nIndex].szExeVersion), WorldID, pNetLuncherInfo->m_IpAddr, pNetLuncherInfo->m_PublicIP);

					if (NOERROR != pNetSession->GetServiceInfo()->AddServer(EV_SMT_TEMP, &ServerInfo, FALSE))
					{
						::AfxMessageBox(_T("Invalid Server(2) Information, click confirm to reconnect"), MB_ICONERROR);
						return HASERROR;
					}
				}
			}
		}
		break;
	case MANAGER2MONITOR_CHANNELINFO_LIST:		// CHANNEL				// 순서4
		{
			const TMSMonitorChannelList* aAck = reinterpret_cast<const TMSMonitorChannelList*>(pBuffer);
			if (ERROR_NONE != aAck->nRet)
				break;

			INT aContainerVersion = aAck->nContainerVersion;
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				if (pNetSession->GetServiceInfo()->GetContainerVersion() != aAck->nContainerVersion) 
					break;

				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) 
				{
					const SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_TEMP, aAck->Info[nIndex].nSID, FALSE);

					if (!pServerInfo || !pServerInfo->IsSet() || !pServerInfo->FindWorldID(aAck->Info[nIndex].nWorldID))
					{
						::AfxMessageBox(_T("Invalid Channel(1) Information, click confirm to reconnect"), MB_ICONERROR);
						return HASERROR;
					}

					CHANNELINFO ChannelInfo(aAck->Info[nIndex].nSID, aAck->Info[nIndex].nChannelID, aAck->Info[nIndex].nWorldID, aAck->Info[nIndex].nMapIdx, aAck->Info[nIndex].nMaxUserCount, aAck->Info[nIndex].nChannelAttribute, aAck->Info[nIndex].wszMapName);
					if (NOERROR != pNetSession->GetServiceInfo()->AddChannel(EV_SMT_TEMP, &ChannelInfo, FALSE))
					{
						::AfxMessageBox(_T("Invalid Channel(2) Information, click confirm to reconnect"), MB_ICONERROR);
						return HASERROR;
					}
				}
			}

			if (aAck->cIsLast) 
			{
				// 모든서버정보구축완료
				{
					// 각월드뷰의정보갱신을위해임시→실제목록으로이전

					CServiceInfo::TP_LISTNTLCAUTO aListNtlc;
					CServiceInfo::TP_LISTMRITAUTO aListMrit;
					{
						CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());
						pNetSession->GetServiceInfo()->ApplyListTempToReal(FALSE);
						pNetSession->GetServiceInfo()->GetNetLuncherList(EV_SMT_REAL, aListNtlc, FALSE);
						pNetSession->GetServiceInfo()->GetMeritList(EV_SMT_REAL, aListMrit, FALSE);
					}

					CBaseInformationView* pBaseInformationView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetInformationView(nConID);
					if (!pBaseInformationView)
						return NOERROR;

					if (::IsWindow(pBaseInformationView->GetSafeHwnd()) == FALSE)
						return NOERROR;

					{
						pBaseInformationView->ClearGridList(CBaseInformationView::EV_SFT_NLC);
						CServiceInfo::TP_LISTNTLCAUTO_CTR aIt = aListNtlc.begin();
						for (; aListNtlc.end() != aIt ; ++aIt) 
						{
							const NETLUNCHERINFO* pNetLuncherInfo = (&(*aIt));
							if (!pNetLuncherInfo || !pNetLuncherInfo->IsSet())
								continue;

							pBaseInformationView->AddGridList(pNetLuncherInfo);
						}
					}

					{
						pBaseInformationView->ClearGridList(CBaseInformationView::EV_SFT_MRT);
						CServiceInfo::TP_LISTMRITAUTO_CTR aIt = aListMrit.begin();
						for (; aListMrit.end() != aIt ; ++aIt) 
						{
							const MERITINFO* pMeritInfo = (&(*aIt));
							if (!pMeritInfo || !pMeritInfo->IsSet())
								continue;

							pBaseInformationView->AddGridList(pMeritInfo);
						}
					}

					pNetSession->GetServiceInfo()->SetRecvMode(EV_SRT_STRUCT, TRUE);
				}
			}

		}
		break;

		//================================================================================

		// 서버정보 실시간수신
	case MANAGER2MONITOR_WORLDINFO_RESULT:		// WORLD				// 순서1
		{
			if (!pNetSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
			{
				// 서버정보구축이되어있지않으면서버정보실시간수신불가
				break;
			}

			const TMSWorldInfo* aAck = reinterpret_cast<const TMSWorldInfo*>(pBuffer);
			if (ERROR_NONE != aAck->nRet)
				break;

			if (aAck->cCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				for (int nIndex = 0 ; aAck->cCount > nIndex ; ++nIndex) 
				{
					WORLDINFO* WorldInfo = pNetSession->GetServiceInfo()->AtWorld(EV_SMT_REAL, aAck->WorldInfo[nIndex].nWorldID, FALSE);
					if (!WorldInfo) 
						continue;

					WorldInfo->m_MaxUserLimit = aAck->WorldInfo[nIndex].nWorldMaxUser;
					::wcsncpy_s(WorldInfo->m_WorldName, aAck->WorldInfo[nIndex].wszWorldName, _countof(WorldInfo->m_WorldName));
				}
			}
			// 여기서 농장 정보를 초기화하면, 모니터 화면 갱신시 농장 서버가 표기되지 않는 문제가 있어서
			// MANAGER2MONITOR_FARM_UPDATEINFO(농장 정보를 서비스매니저에서 수신)로 초기화 위치를 변경합니다
			// 농장 정보가 여러번 전송되기 때문에, 초기화를 한번만 하도록 플래그로 관리합니다.
			// 플래그 초기화는 서버 정보 갱신에서 해주도록 구현하였습니다		- 2013-01-18		karl
			//pNetSession->GetServiceInfo()->ResetFarmInfo(EV_SMT_REAL);		// 농장정보 초기화
			pNetSession->GetServiceInfo()->SetFarmInfoResetFlag(EV_SMT_REAL, FALSE);
		}
		break;
	case MANAGER2MONITOR_SERVERSTATE_LIST:		// SERVER				// 순서2
		{
			if (!pNetSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
			{
				// 서버정보구축이되어있지않으면서버정보실시간수신불가
				break;
			}

			const TMSServerStateList* aAck = reinterpret_cast<const TMSServerStateList*>(pBuffer);			
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) {
					SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aAck->State[nIndex].nSID, FALSE);
					if (!pServerInfo)
						continue;

					pServerInfo->m_ServerState = static_cast<EF_SERVERSTATE>(aAck->State[nIndex].nServerState);

					// 서버타입별개별처리
					switch(pServerInfo->m_ServerType) 
					{
					case EV_SVT_GA:
						{
							switch(pServerInfo->m_ServerState) 
							{
							case EV_SVS_CNNT:
								{
									if (!pServerInfo->m_Visibility) 
									{
										// 게임서버의경우만HIDE 시서버상태도EV_SVS_HIDE 가됨
										pServerInfo->m_ServerState = EV_SVS_HIDE;
									}
								}
								break;
							}

							TRACE(CVarArg<MAX_PATH>(_T("Server State : %d\n"), pServerInfo->m_ServerState));
						}
						break;
					}
				}
			}
		}
		break;
	case MANAGER2MONITOR_LOGIN_UPDATEINFO:		// LOGIN				// 순서3
		{
			if (!pNetSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
			{
				// 서버정보구축이되어있지않으면서버정보실시간수신불가
				break;
			}

			const TMSLoginUserCount* aAck = reinterpret_cast<const TMSLoginUserCount*>(pBuffer);
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) 
				{
					SERVERINFO* ServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aAck->Update[nIndex].nSID, FALSE);
					if (!ServerInfo || EV_SVT_LO != ServerInfo->m_ServerType)
						continue;

					ServerInfo->m_CurUserCount = aAck->Update[nIndex].nUserCount;
				}
			}
		}
		break;
	case MANAGER2MONITOR_VILLAGE_UPDATEINFO:	// VILLAGE				// 순서4
		{
			if (!pNetSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
			{
				// 서버정보구축이되어있지않으면서버정보실시간수신불가
				break;
			}

			const TMSVillageUserCount* aAck = reinterpret_cast<const TMSVillageUserCount*>(pBuffer);

			if (aAck->cCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				for (int nIndex1 = 0 ; aAck->cCount > nIndex1 ; ++nIndex1) 
				{
					const sChannelUpdateInfo* pUpdate = &aAck->Update[nIndex1];


					SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, pUpdate->nSID, FALSE);
					if (!pServerInfo || EV_SVT_VI != pServerInfo->m_ServerType)
						continue;


					CHANNELINFO* pChannelInfo = pNetSession->GetServiceInfo()->AtChannel(EV_SMT_REAL, pUpdate->nSID, pUpdate->nChannelID, FALSE);
					if (!pChannelInfo)
						continue;


					pChannelInfo->m_CurUserCount	= pUpdate->nChannelUserCount;
					pChannelInfo->m_MeritID			= pUpdate->nMeritBonusID;
					pChannelInfo->m_Visibility		= pUpdate->bVisibility;
					pChannelInfo->m_LimitLevel		= pUpdate->nLimitLevel;
					pChannelInfo->m_IsMidShow		= pUpdate->bShow;
					pChannelInfo->m_nServerID		= pUpdate->nServerID;
					pChannelInfo->m_cThreadID		= pUpdate->cThreadID;
				}
			}

			if (aAck->cIsLast) 
			{
				// 빌리지서버각각사용자총합을계산
				{
					typedef std::map<INT, INT>						TP_SVRUSRCNT;
					typedef TP_SVRUSRCNT::iterator					TP_SVRUSRCNT_ITR;
					typedef TP_SVRUSRCNT::const_iterator			TP_SVRUSRCNT_CTR;

					TP_SVRUSRCNT ServerUserCountList;

					CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());
					{
						CServiceInfo::TP_LISTCHNL_ITR aIt1 = pNetSession->GetServiceInfo()->BeginChannel(EV_SMT_REAL, FALSE);
						for (; pNetSession->GetServiceInfo()->EndChannel(EV_SMT_REAL, FALSE) != aIt1 ; ++aIt1) 
						{
							CHANNELINFO* pChannelInfo = (&aIt1->second);

							TP_SVRUSRCNT_ITR aIt2 = ServerUserCountList.find(pChannelInfo->m_ServerID);
							if (ServerUserCountList.end() != aIt2) 
							{
								INT* pUserCount = (&aIt2->second);
								(*pUserCount) += pChannelInfo->m_CurUserCount;
							}
							else 
							{
								std::pair<TP_SVRUSRCNT_CTR, bool> aRetVal = ServerUserCountList.insert(TP_SVRUSRCNT::value_type(pChannelInfo->m_ServerID, pChannelInfo->m_CurUserCount));
								if (!aRetVal.second) 
								{
									::AfxMessageBox(_T("Invalid VillageUpdate(2) Information, click confirm to reconnect"), MB_ICONERROR);
									return HASERROR;
								}
							}
						}
					}

					{
						TP_SVRUSRCNT_CTR aIt = ServerUserCountList.begin();
						for (; ServerUserCountList.end() != aIt ; ++aIt) 
						{
							SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aIt->first, FALSE);
							if (EV_SVT_VI != pServerInfo->m_ServerType) 
								continue;

							pServerInfo->m_CurUserCount = aIt->second;
						}						
					}
				}
			}
		}
		break;
	case MANAGER2MONITOR_GAME_UPDATEINFO:		// GAME					// 순서4
		{
			if (!pNetSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
			{
				// 서버정보구축이되어있지않으면서버정보실시간수신불가
				break;
			}
			
			const TMSGameUserCount* aAck = reinterpret_cast<const TMSGameUserCount*>(pBuffer);
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) 
				{
					SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aAck->Update[nIndex].nSID, FALSE);
					if (!pServerInfo || EV_SVT_GA != pServerInfo->m_ServerType)
						continue;

					pServerInfo->m_Visibility = aAck->Update[nIndex].cIsOpened;
					pServerInfo->m_CurUserCount = aAck->Update[nIndex].nUserCount;
					pServerInfo->m_CurRoomCount = aAck->Update[nIndex].nRoomCount;
					pServerInfo->m_MaxRoomCount = aAck->Update[nIndex].nTotalRoomCount;

				}
			}
		}
		break;

	case MANAGER2MONITOR_FARM_UPDATEINFO:
		{
			if (!pNetSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
				break;

			pNetSession->GetServiceInfo()->ResetFarmInfo(EV_SMT_REAL);		// 농장정보 초기화
			const TMSFarmUserCount* aAck = reinterpret_cast<const TMSFarmUserCount*>(pBuffer);
			if (aAck->nCount > 0) 
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				int nCount = 0;
				for (int nIndex = 0 ; aAck->nCount > nIndex ; ++nIndex) 
				{
					SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aAck->Update[nIndex].nManagedID, FALSE);
					if (!pServerInfo)
						continue;
					
					nCount = pServerInfo->m_MaxFarmCount;
					pServerInfo->m_FarmDBID[nCount] = aAck->Update[nIndex].nFarmDBID;
					pServerInfo->m_CurFarmUserCount[nCount] = aAck->Update[nIndex].nFarmCurUserCount;
					pServerInfo->m_bActivate[nCount] = aAck->Update[nIndex].bActivate;
					pServerInfo->m_TotalFarmUserCount += aAck->Update[nIndex].nFarmCurUserCount;
					pServerInfo->m_MaxFarmCount++;
				}
			}
			else
			{
				// 모든서버정보실시간수신완료	
				pNetSession->GetServiceInfo()->SetRecvMode(EV_SRT_REALTIME, FALSE);
			}
		}
		break;

		//================================================================================

		// 서버구조변경통지
	case MANAGER2MONITOR_UPDATE_NOTICE:
		{
			const TMSUpdateNotice* aAck = reinterpret_cast<const TMSUpdateNotice*>(pBuffer);
			SendRequestServiceStruct(pNetSession, aAck->nContainerVersion);
		}
		break;

		// 서버오류정보통지
	case MANAGER2MONITOR_UPDATE_SERVERSTATE:
		{
			const TMSServerStateUpdate* aAck = reinterpret_cast<const TMSServerStateUpdate*>(pBuffer);

			CHAR cServerType = EV_SVT_CNT;
			INT aWorldID[WORLDCOUNTMAX] = { EV_WORLDID_DEF, };
			{

				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				SERVERINFO* pServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aAck->nSID, FALSE);
				if (pServerInfo) 
				{
					pServerInfo->m_ServerState = static_cast<EF_SERVERSTATE>(aAck->nServerState);

					if (aAck->nServerException == _EXCEPTIONTYPE_GAME_DELAYED)
					{
						pServerInfo->m_FrameDelay[SERVERTYPE_GAME]++;
					}

					cServerType = static_cast<CHAR>(pServerInfo->m_ServerType);	// 서버타입형식은같음
					for (int nIndex = 0 ; WORLDCOUNTMAX > nIndex ; ++nIndex) 
					{
						if (EV_WORLDID_DEF == pServerInfo->m_WorldID[nIndex])
							break;
						aWorldID[nIndex] = pServerInfo->m_WorldID[nIndex];
					}
				}
			}

			CBaseErrorLogView* pBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
			if (!pBaseErrorLogView)
				return NOERROR;

			if (::IsWindow(pBaseErrorLogView->GetSafeHwnd()) == FALSE)
				return NOERROR;

			pBaseErrorLogView->AddGridList(aAck->nSID, cServerType, aWorldID, static_cast<EF_SERVERSTATE>(aAck->nServerState), aAck->nServerException, CA2T(aAck->szDetail), TRUE);
		}
		break;
	case MANAGER2MONITOR_SERVER_DEALYINFO:
		{
			const TMSServerDealyInfo* aAck = reinterpret_cast<const TMSServerDealyInfo*>(pBuffer);

			std::map<int, SERVERINFO*> mServerInfo;

			CHAR cServerType = EV_SVT_CNT;
			INT nWorldID[WORLDCOUNTMAX] = { EV_WORLDID_DEF, };
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

				for (int i=0; i<aAck->nCount; i++)
				{
					SERVERINFO* aServerInfo = pNetSession->GetServiceInfo()->AtServer(EV_SMT_REAL, aAck->nMIDs[i], FALSE);
					if (aServerInfo)
					{
						mServerInfo[aAck->nMIDs[i]] = aServerInfo;
						aServerInfo->m_FrameDelay[aAck->nType]++;
					}
				}

				CBaseErrorLogView* pBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
				if (!pBaseErrorLogView)
					return NOERROR;

				if (::IsWindow(pBaseErrorLogView->GetSafeHwnd()) == FALSE)
					return NOERROR;

				
				for(std::map<int, SERVERINFO*>::iterator iter = mServerInfo.begin(); iter!=mServerInfo.end(); iter++ )
				{
					SERVERINFO* pServerInfo = iter->second;
					if (aAck->nType == SERVERTYPE_GAME)
						pBaseErrorLogView->AddGridList(pServerInfo->m_ServerID, (CHAR)pServerInfo->m_ServerType, pServerInfo->m_WorldID, pServerInfo->m_ServerState, _EXCEPTIONTYPE_GAME_DELAYED, CA2T("GameServer Frame Delayed"), TRUE);
					else if (aAck->nType == SERVERTYPE_DB)
						pBaseErrorLogView->AddGridList(pServerInfo->m_ServerID, (CHAR)pServerInfo->m_ServerType, pServerInfo->m_WorldID, pServerInfo->m_ServerState, _EXCEPTIONTYPE_DBMW_DELAYED, CA2T("DB Middleware Delayed"), TRUE);
				}
				
			}
		}
		break;

		// 요청한명령에대한결과
	case MANAGER2MONITOR_OPERATING_RESULT:
		{
			const TMSOperatingResult* aAck = reinterpret_cast<const TMSOperatingResult*>(pBuffer);

			CBaseErrorLogView* pBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
			if (!pBaseErrorLogView)
				return NOERROR;

			if (::IsWindow(pBaseErrorLogView->GetSafeHwnd()) == FALSE)
				return NOERROR;

			pBaseErrorLogView->ShowOperatingResult(aAck->nCmd, aAck->nRet);
		}
		break;

	case MANAGER2MONITOR_REPORT_RESULT:
		{
			const TMSReportReslut* aAck = reinterpret_cast<const TMSReportReslut*>(pBuffer);

			CBasePartitionView* pBasePartitionView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetPartitionView(nConID);
			if (pBasePartitionView)
				pBasePartitionView->ShowReportStatus(aAck->nPage, aAck->nMaxPage);

			CBaseErrorLogView* pBaseErrorLogView = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetErrorLogView(nConID);
			if (!pBaseErrorLogView)
				return NOERROR;

			if (::IsWindow(pBaseErrorLogView->GetSafeHwnd()) == FALSE)
				return NOERROR;

			pBaseErrorLogView->RecvReportData(aAck->szReport);

			int nPage = aAck->nPage;
			if (!aAck->bEnd)
			{
				TSMReportReq TxPacket;
				TxPacket.bBuild = false;
				TxPacket.cDays = 0;
				TxPacket.nPage = ++nPage;
				pNetSession->SendData(MONITOR2MANAGER_REPORT_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));
			}
			else
			{
				pBaseErrorLogView->SaveToClipboard();
			}
		}
		break;

	default:
		break;
	}

	return NOERROR;	
}


VOID CNetConnection::SendRequestServiceStruct(CNetSession* pNetSession, INT pContainerVersion)
{
	if (!pNetSession->IsConnect()) {
		return;
	}

	pNetSession->GetServiceInfo()->ResetAllRecvMode();
	{
		// 임시 서버 구조 초기화
		CServiceInfo::TP_LOCKAUTO AutoLock(pNetSession->GetServiceInfo()->GetLock());

		pNetSession->GetServiceInfo()->ClearAllList(EV_SMT_TEMP, FALSE);

		if (0 <= pContainerVersion &&
			pNetSession->GetServiceInfo()->GetContainerVersion() < pContainerVersion
			)
		{
			pNetSession->GetServiceInfo()->SetContainerVersion(pContainerVersion);
		}
		
		pContainerVersion = pNetSession->GetServiceInfo()->GetContainerVersion();
	}

	{
		// 서버 구조 구축 정보 요청
		TSMServerStructReq TxPacket;
		TxPacket.nContainerVersion = pContainerVersion;
		pNetSession->SendData(MONITOR2MANAGER_SERVERSTRUCT_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));
	}
}

//-----------------------------------------------------------------------------------------------------------------------

DEFINE_SINGLETON_CLASS(CNetConnectionMgr);

CNetConnectionMgr::CNetConnectionMgr()
{
	m_nConnectionID = 0;
	m_MapNetConnections.clear();
	m_CurrentSession = NULL;

	m_Lock.Open ();
}

CNetConnectionMgr::~CNetConnectionMgr()
{
	m_Lock.Close ();
}

CNetConnection* CNetConnectionMgr::CreateConnection()
{
	CNetConnection* pNetCon = new CNetConnection();
	pNetCon->Open ();

	CNetSession* pSession = pNetCon->GetSession();
	m_CurrentSession = pSession;
	
	return pNetCon;
}

CNetConnection* CNetConnectionMgr::GetConnection (UINT nConID)
{
	CLockAutoEx<CCriticalSection> AutoLock (m_Lock);

	std::map<UINT, CNetConnection*>::iterator iter = m_MapNetConnections.find(nConID);

	if (iter != m_MapNetConnections.end())
		return iter->second;

	return NULL;
}

void CNetConnectionMgr::RemoveConnection (UINT nConID)
{
	m_MapNetConnections.erase(nConID);
}

CNetSession* CNetConnectionMgr::GetCurrentSession()
{
	return m_CurrentSession;
}

void CNetConnectionMgr::RegistConnection (CNetConnection* pNetCon)
{
	CNetSession* pSession = pNetCon->GetSession();

	ULONG nConID = pSession->GetRemoteIpAddressN();

	CLockAutoEx<CCriticalSection> AutoLock (m_Lock);
	m_MapNetConnections[nConID] = pNetCon;
}