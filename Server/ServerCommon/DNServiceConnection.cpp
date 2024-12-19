
#include "stdafx.h"
#include "DNIocpManager.h"
#include "DNServiceConnection.h"
#include "Log.h"
#include "DNServerPacket.h"
#include "DNEvent.h"
#if defined (_GAMESERVER)
extern TGameConfig g_Config;
#include "DNGameServerManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "CloseSystem.h"
#include "DNAuthManager.h"
#elif defined (_MASTERSERVER)
#include "DNDivisionManager.h"
#include "DNLoginConnection.h"
#include "DNUser.h"
#include "DNWaitUserManager.h"
#include "DNExtManager.h"
#include "DNWaitUserManager.h"
#include "DNVillageConnection.h"
#include "DNGuildWarManager.h"
extern TMasterConfig g_Config;
#elif defined (_VILLAGESERVER)
extern TVillageConfig g_Config;
#include "DNUserSessionManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNCashConnection.h"
#include "CloseSystem.h"
#include "DNAuthManager.h"
#include "DNGameDataManager.h"
#elif defined (_DBSERVER)
#include "DNExtManager.h"
extern TDBConfig g_Config;
#elif defined (_LOGINSERVER)
extern TLoginConfig g_Config;
#include "DNUserConnectionManager.h"
#include "CloseSystem.h"
#include "DNMasterConnectionManager.h"
#include "DNAuthManager.h"
#include "DNSQLWorldManager.h"
#if defined( PRE_ADD_DWC )
#include "DNDWCChannelManager.h"
#endif // #if defined( PRE_ADD_DWC )
#elif defined (_LOGSERVER)
extern TLogConfig g_Config;
#elif defined(_CASHSERVER)
#include "DNExtManager.h"
extern TCashConfig g_Config;
#if defined(PRE_ADD_LIMITED_CASHITEM)
#include "DNLimitedCashItemRepository.h"
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
#endif

#if defined( PRE_PVP_GAMBLEROOM )
#if defined( _VILLAGESERVER )
#include "DNPvPRoomManager.h"
#endif
#endif

CDNServiceConnection * g_pServiceConnection = NULL;

CDNServiceConnection::CDNServiceConnection(int nManagedID)
{
	m_nManagedID = nManagedID;
	m_dwReconnectTick = 0;
#if defined(_FINAL_BUILD)
	Init( 1024*1024, 1024*1024 );
#else	// #if defined(_FINAL_BUILD)
	Init( 1024*100, 1024*100 );
#endif	// #if defined(_FINAL_BUILD)
	m_bIsInit = false;

	m_nScopeCnt = 0;
	memset(&m_nScopeElapsed, 0, sizeof(m_nScopeElapsed));

#if defined (_MASTERSERVER)
	m_nManagedType = MANAGED_TYPE_MASTER;
#elif defined (_LOGINSERVER)
	m_nManagedType = MANAGED_TYPE_LOGIN;
#elif defined (_GAMESERVER)
	m_nManagedType = MANAGED_TYPE_GAME;
#elif defined (_VILLAGESERVER)
	m_nManagedType = MANAGED_TYPE_VILLAGE;
#elif defined (_DBSERVER)
	m_nManagedType = MANAGED_TYPE_DB;
#elif defined (_CASHSERVER)
	m_nManagedType = MANAGED_TYPE_CASH;
#else
	m_nManagedType = MANAGED_TYPE_LOG;
#endif
	m_bCloseServer = false;
}

CDNServiceConnection::~CDNServiceConnection()
{
	m_dwReconnectTick = 0;
	m_bIsInit = false;	

	m_nScopeCnt = 0;
	memset(&m_nScopeElapsed, 0, sizeof(m_nScopeElapsed));
}

int CDNServiceConnection::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	switch (iMainCmd)
	{
		case NETSERVICE_CONNECTED:
		{
			TServiceConnected * pPacket = (TServiceConnected*)pData;
			
			SetSessionID(pPacket->nSessionID);
#if defined(_GAMESERVER)
			BYTE cCount = 0;
			BYTE cWorldID[WORLDCOUNTMAX];
			memset(&cWorldID, 0, sizeof(cWorldID));

			cCount = g_pMasterConnectionManager->GetWorldSetIDs(cWorldID);

			SendRegistManager(m_nManagedID, m_nManagedType, g_Config.szVersion, g_Config.szResVersion, 0, cCount, cWorldID);
#elif defined(_VILLAGESERVER) || defined(_MASTERSERVER) || defined(_CASHSERVER)
			SendRegistManager(m_nManagedID, m_nManagedType, g_Config.szVersion, g_Config.szResVersion, g_Config.nWorldSetID, 0, NULL);
#ifdef _MASTERSERVER
			SendMeritInfo();
			SendWorldInfo();
#endif
#else
			SendRegistManager(m_nManagedID, m_nManagedType, g_Config.szVersion, g_Config.szResVersion, 0, 0, NULL);
#endif

			m_bIsInit = true;
			return ERROR_NONE;
		}

		case NETSERVICE_MAKE_DUMP:
		{
			g_pIocpManager->StoreMiniDump();
			return ERROR_NONE;
		}

		case NETSERVICE_DETACHALLUSER:
		{
#if defined(_GAMESERVER) || defined (_VILLAGESERVER) || defined (_LOGINSERVER)

#ifdef _VILLAGESERVER

			g_pUserSessionManager->DetachAllConnection();
#ifdef PRE_ADD_SEETMAINTENANCEFLAG
			BYTE cThreadID;
			CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
			if (pDBCon)
			{
				pDBCon->QuerySetMaintenanceFlag(cThreadID, 1);
			}
#endif		//#ifdef PRE_ADD_SEETMAINTENANCEFLAG

#elif _GAMESERVER
			g_pGameServerManager->DestroyAllGameRoom();
#elif _LOGINSERVER
#if defined(_TW) && defined(_FINAL_BUILD)
			g_pUserConnectionManager->SendAllLogOutTW();
#endif	// #if defined(_TW) 
#endif	// _LOGINSERVER

			//2010.11.19 haling 서버죽을때 인증 초기화..동접 카운트 위해?
			g_pAuthManager->QueryResetAuthServerClose();

#ifdef _LOGINSERVER
			//ServiceClose Flow
			//로그인서버인 경우에는 쿼리문만 보장하면 덴다 바로 종료리포팅
			SendServiceClosed();
#endif	//#ifdef _LOGINSERVER

#endif
			m_bCloseServer = true;
			return ERROR_NONE;
		}		

		case NETSERVICE_EVENTUPDATE:
		{
			LoadWorldEvent();
			return ERROR_NONE;
		}

		case NETSERVICE_SIMPLECONFIGUPDATE:
		{
//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
//			LoadSimpleConfig();
//#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
			return ERROR_NONE;
		}

		case NETSERVICE_PING:
		{
			SendPong();
			return ERROR_NONE;
		}

#ifdef _MASTERSERVER
		case NETSERVICE_NOTICE:
		{
			if (!g_pDivisionManager) return ERROR_NONE; 
			TServiceNotice * pPacket = (TServiceNotice*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pPacket->wszMsg, pPacket->nLen);

			//타입별 공지 종류 변경
			switch (pPacket->nNoticeType)
			{
			case _NOTICETYPE_WORLD: 
				g_pDivisionManager->Notice(wszChatMsg, pPacket->nLen, pPacket->nSlideShowSec); 
				break;

			case _NOTICETYPE_EACHSERVER: 
				g_pDivisionManager->NoticeServer(pPacket->nManagedID, wszChatMsg, pPacket->nLen, pPacket->nSlideShowSec); 
				break;

			case _NOTICETYPE_CHANNEL:
				g_pDivisionManager->NoticeChannel(pPacket->nChannelID, wszChatMsg, pPacket->nLen, pPacket->nSlideShowSec);
				break;

			case _NOTICETYPE_ZONE:
				g_pDivisionManager->NoticeZone(pPacket->nMapIdx, wszChatMsg, pPacket->nLen, pPacket->nSlideShowSec);
				break;
			}
			return ERROR_NONE;
		}

		case NETSERVICE_SELECTJOIN:
		{
			TServiceSelectjoin * pPacket = (TServiceSelectjoin*)pData;

			if (g_pDivisionManager->AddSelectJoin(pPacket->nSID, pPacket->wszCharacterName) == false)
				_DANGER_POINT();
			return ERROR_NONE;
		}

		case NETSERVICE_CLEAR_SELECTJOIN:
		{
			g_pDivisionManager->ClearSelectJoin();
			return ERROR_NONE;
		}

		case NETSERVICE_CANCELNOTICE:
		{
			g_pDivisionManager->NoticeCancel();
			return ERROR_NONE;
		}

		case NETSERVICE_CHANNEL_CONTROL:
		{
			TServiceChannelControl * pPacket = (TServiceChannelControl*)pData;
			if (g_pDivisionManager->ChannelControl(pPacket->nChannelID, pPacket->cVisibility == 0 ? false : true, pPacket->nChannelPopulation) == true)
				g_pDivisionManager->SendVillageInfo();
			return ERROR_NONE;
		}

		case NETSERVICE_POPULATION_CONTROL:
		{
			TServicePopulationControl * pPacket = (TServicePopulationControl*)pData;
			g_pDivisionManager->PopulationControl(pPacket->nServerType, pPacket->nManagedID, pPacket->cZeroPopulation == 0 ? false : true);
			
			return ERROR_NONE;
		}

		case NETSERVICE_AFFINITY_CONTROL:			
		{
			TServiceAffinityControl * pPacket = (TServiceAffinityControl*)pData;
			g_pDivisionManager->SetGameConAffinityType(pPacket->nManagedID, pPacket->cAffinityType);
			return ERROR_NONE;
		}

		case NETSERVICE_USERRESTRAINT:
		{
			TServiceUserRestraint * pPacket = (TServiceUserRestraint*)pData;
			g_pDivisionManager->UpdateUserRestraint(pPacket->nAccountDBID);
			return ERROR_NONE;
		}

		case NETSERVICE_USERBAN:
		{
			TServiceUserBan * pPacket = (TServiceUserBan*)pData;

			CDNUser * pUser = NULL;
			UINT nBanAccountDBID = 0;
			switch (pPacket->eType)
			{
			case _BANTYPE_ACCOUNTNAME:
				{
					pUser = g_pDivisionManager->GetUserByAccountName(pPacket->wszName);
					if (pUser)
						nBanAccountDBID = pUser->GetAccountDBID();
					break;
				}
			case _BANTYPE_ACCOUNTID:
				{
					nBanAccountDBID = pPacket->nAccountDBID;
					break;
				}
			case _BANTYPE_CHARACTERNAME:
				{
					pUser = g_pDivisionManager->GetUserByName(pPacket->wszName);
					if (pUser)
						nBanAccountDBID = pUser->GetAccountDBID();
					break;
				}
			}

			if (nBanAccountDBID > 0)
			{
				g_pDivisionManager->SendDetachUser(nBanAccountDBID);
				g_pDivisionManager->SendAllLoginServerDetachUser(nBanAccountDBID);
			}

			return ERROR_NONE;
		}

		case NETSERVICE_WORLDMAXUSER:
		{
			TServiceWordlMaxUser * pPacket = (TServiceWordlMaxUser*)pData;
			g_pWaitUserManager->Initialize(pPacket->nChangeMaxUser);
			g_Log.Log(LogType::_FILEDBLOG, L"WorldMaxUser Changed [%d]\n", pPacket->nChangeMaxUser);

			SendWorldInfo();
			return ERROR_NONE;
		}

		case NETSERVICE_RELOADGUILDWARSCHEDULE:
		{
			TReloadGuildwarSchedule * pPacket = (TReloadGuildwarSchedule*)pData;
			if( pPacket->nWorldID == 0 || pPacket->nWorldID == g_Config.nWorldSetID)
			{
				g_pGuildWarManager->SetResetSchedule(true);
				g_Log.Log(LogType::_GUILDWAR, 0, 0, 0, 0, L"[GUILDWAR] NETSERVICE_RELOADGUILDWARSCHEDULE !!!\n");
			}
			return ERROR_NONE;
		}
#endif

#if defined(_GPK)
#if defined(_LOGINSERVER) || defined (_GAMESERVER) || defined(_VILLAGESERVER)
		case NETSERVICE_RELOADDYNCODE:
		{
			char szSvrDir[MAX_PATH] = { 0, };
			char szCltDir[MAX_PATH] = { 0, };
#if defined(WIN64)
			sprintf_s(szSvrDir, MAX_PATH - 1, "./DynCodeBin64/Server");
			sprintf_s(szCltDir, MAX_PATH - 1, "./DynCodeBin64/Client");
#else	// WIN64
#if defined(BIT64)
			sprintf(szSvrDir, "./DynCodeBin/Server64");
#else
			sprintf(szSvrDir, "./DynCodeBin/Server");
#endif
			sprintf(szCltDir, "./DynCodeBin/Client");
#endif	// WIN64

			int nBinCount = g_Config.pDynCode->LoadBinary(szSvrDir, szCltDir);
			if (nBinCount == 0){
				g_Log.Log(LogType::_FILEDBLOG, L"Load DynCode failed!!!\r\n");
				SendGPKLoadFail();
				return false;
			}
			return ERROR_NONE;
		}
#endif	//#if defined(_LOGINSERVER) || defined (_GAMESERVER) || defined(_VILLAGESERVER)
#endif	//defined(_GPK)

		case NETSERVICE_SCHEDULE_SERVERCLOSE:
		{
#if defined(_GAMESERVER) || defined (_VILLAGESERVER) || defined (_LOGINSERVER)
			TScheduleServiceClose * pPacket = (TScheduleServiceClose*)pData;

			if (g_pCloseSystem == NULL)
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}
			
			if (g_pCloseSystem->AddClose(pPacket->_tOderedTime, pPacket->_tCloseTime) == false)
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}

			__time64_t _tNow;
			time(&_tNow);

#ifdef _VILLAGESERVER
			g_pUserSessionManager->SendCloseService(_tNow, pPacket->_tOderedTime, pPacket->_tCloseTime);			
#elif _GAMESERVER
			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, iMainCmd, 0, pData, iLen, EXTERNALTYPE_SERVICEMANAGER);
			}
#endif

#endif	//#if defined(_GAMESERVER) || defined (_VILLAGESERVER) || defined (_LOGINSERVER)
			return ERROR_NONE;
		}

		case NETSERVICE_UPDATESALEDATA:
		{
#if defined(_CASHSERVER) || defined(_DBSERVER)
			// 캐쉬리스트 판매중지 있는지 디비에서 체크
			g_pExtManager->SetProhibitSaleList();

#elif defined(_VILLAGESERVER)
			g_pCashConnection->SendSaleAbortList();
#endif
			return ERROR_NONE;
		}

		case NETSERVICE_UPDATEFARM:
		{
			TUpdateFarm * pPacket = (TUpdateFarm*)pData;
#ifdef _GAMESERVER
			if (pPacket->bForceStart)
			{
				for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
				{
					CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
					if (pServer)
						pServer->StoreExternalBuffer(0, iMainCmd, 0, pData, iLen, EXTERNALTYPE_SERVICEMANAGER);
				}
			}
			else
			{
				BYTE cThreadID;
				CDNDBConnection * pDBCon = g_pDBConnectionManager->GetDBConnection(cThreadID);
				if (pDBCon)
				{
					if (pPacket->nWorldID == 0)
					{
						BYTE cWorldID[WORLDCOUNTMAX];
						memset(&cWorldID, 0, sizeof(cWorldID));

						BYTE cCount = g_pMasterConnectionManager->GetWorldSetIDs(cWorldID);
						for (int i = 0; i < cCount && i < WORLDCOUNTMAX; i++)
						{
							pDBCon->QueryFarmList(cThreadID, cWorldID[i]);
						}
					}
					else
					{
						pDBCon->QueryFarmList(cThreadID, pPacket->nWorldID);
					}
				}
				else{
					g_Log.Log(LogType::_FARM, L"UpdateFarm Failed [WorldID:%d] Con Not Found\n", pPacket->nWorldID);
					g_Log.Log(LogType::_FILELOG, L"UpdateFarm Failed [WorldID:%d] Con Not Found\n", pPacket->nWorldID);
				}
			}
#elif _VILLAGESERVER
			if (pPacket->bForceStart == true)
				return ERROR_NONE;

			BYTE cThreadID;
			CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
			if (pDBCon)
			{
				pDBCon->QueryFarmList(cThreadID, g_Config.nWorldSetID);
#ifdef PRE_ADD_SEETMAINTENANCEFLAG
				pDBCon->QuerySetMaintenanceFlag(cThreadID, 0);
#endif		//#ifdef PRE_ADD_SEETMAINTENANCEFLAG
			}
#endif

			return ERROR_NONE;
		}
		case NETSERVICE_UPDATEGUILDWAR_SCHEDULE:
		{			
#ifdef _MASTERSERVER
			// 콜로세움 빌리지한테 요청
			//CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
			//if( pVillageConnection == NULL || pVillageConnection->GetActive() == false )
			//	return ERROR_NONE;
			g_pGuildWarManager->SetResetSchedule(true);
			g_Log.Log(LogType::_GUILDWAR, 0, 0, 0, 0, L"[GUILDWAR] NETSERVICE_UPDATEGUILDWAR_SCHEDULE !!!\n");
			//pVillageConnection->SendGetGuildWarSchedule(); // 스케쥴 정보 가져오기 요청
#endif			
			return ERROR_NONE;
		}
#if defined (_LOGINSERVER)
		case NETSERVICE_CREATEWORLDDB:
		{
			TCreateDB * pPacket = (TCreateDB*)pData;
			if (g_pSQLWorldManager)
			{
				int nIndex = -1;
				int nResult = g_pSQLWorldManager->CreateEachDB (pPacket->nWorldID, nIndex);

				if (nIndex == -1)
					SendLoginWorldDBResult(nResult, pPacket->nWorldID, NULL, 0);
				else
					SendLoginWorldDBResult(nResult, g_Config.WorldDB[nIndex].nWorldSetID, g_Config.WorldDB[nIndex].szIP,g_Config.WorldDB[nIndex].nPort);

			}	

			return ERROR_NONE;
		}
#endif // #if defined(_LOGINSERVER)

		case NETSERVICE_EXTRELOAD:
		{
			TExtReload * pPacket = (TExtReload*)pData;
			
			bool bReload = false;
#ifdef _VILLAGESERVER
			bReload = g_pDataManager->LoadCashCommodityData(true);
			if (bReload == false)
			{
				SendReloadResult(bReload);
				return ERROR_NONE;
			}

			bReload = g_pDataManager->LoadCashPackageData(true);
			if (bReload == false)
			{
				SendReloadResult(bReload);
				return ERROR_NONE;
			}
			SendReloadResult(bReload);
#elif defined (_CASHSERVER) || defined (_DBSERVER)
			bReload = g_pExtManager->ReLoadCashCommodity();
			if (bReload == false)
			{
				SendReloadResult(bReload);
				return ERROR_NONE;
			}

			bReload = g_pExtManager->ReLoadCashPackage();
			if (bReload == false)
			{
				SendReloadResult(bReload);
				return ERROR_NONE;
			}
			SendReloadResult(bReload);
#else
			SendReloadResult(true);
#endif
			return ERROR_NONE;
		}

		case NETSERVICE_UPDATEGUILDWARE:
		{
#if defined (_MASTERSERVER)
			TUPdateGuildWare * pPacket = (TUPdateGuildWare*)pData;
			if (g_pDivisionManager)
				g_pDivisionManager->SendUpdateGuildWare(pPacket->nGuildID);
#endif		//#if defined (_MASTERSERVER)
			return ERROR_NONE;
		}

		case NETSERVICE_LIMITEITEM_CHANGEQUANTITY:
		{
#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)
			TChangeLimitItemMax * pPacket = (TChangeLimitItemMax*)pData;
			if (g_pLimitedCashItemRepository)
			{
				if (g_pLimitedCashItemRepository->UpdateChangedLimitedItemMax(pPacket->nSN, pPacket->nLimiteMax) == false)
					_DANGER_POINT_MSG(L"NETSERVICE_LIMITEITEM_CHANGEQUANTITY Fail");
			}
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)
			return ERROR_NONE;
		}
		case NETSERVICE_UPDATEWORLDPVPROOM:
		{
			TApplyWorldPvPRoom * pPacket = (TApplyWorldPvPRoom*)pData;
#if defined( _VILLAGESERVER )	
#if defined( PRE_WORLDCOMBINE_PVP )
			if( g_Config.nCombinePartyWorld != g_Config.nWorldSetID )
			{
				g_Log.Log(LogType::_NORMAL, L"UpdateWorldPvPRoom fail CombineWorld=%d WorldSetID=%d \n", g_Config.nCombinePartyWorld, g_Config.nWorldSetID);
				break;
			}
			BYTE cThreadID;
			CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
			if (pDBCon)	
			{
				pDBCon->QueryUpdateWorldPvPRoom(cThreadID, g_Config.nWorldSetID);
				g_Log.Log(LogType::_NORMAL, L"UpdateWorldPvPRoom[WorldID:%d] sucess\n", g_Config.nWorldSetID);
			}
#endif
#endif
			
		}
		break;
		case NETSERVICE_CREATEGAMBLEROOM:
			{
				TCreateGambleRoom * pPacket = (TCreateGambleRoom*)pData;
#if defined( _VILLAGESERVER )	
#if defined( PRE_PVP_GAMBLEROOM )
				CDNPvPRoomManager::GetInstance().SetGambleRoomData(pPacket->nRoomIndex, pPacket->bRegulation, pPacket->nGambleType, pPacket->nPrice, true);
#endif
#endif
				return ERROR_NONE;
			}
			break;
		case NETSERVICE_STOPGAMBLEROOM:
			{				
#if defined( _VILLAGESERVER )	
#if defined( PRE_PVP_GAMBLEROOM )
				CDNPvPRoomManager::GetInstance().SetGambleRoomData(0, true, 0, 0, false);
#endif
#endif
				return ERROR_NONE;
			}
			break;
		case NETSERVICE_DELGAMBLEROOM:
			{				
#if defined( _VILLAGESERVER )
#if defined( PRE_PVP_GAMBLEROOM )
				CDNPvPRoomManager::GetInstance().DelGambleRoom();
#endif
#endif
				return ERROR_NONE;
			}
			break;
		case NETSERVICE_UPDATE_DWCSTATUS:
			{
#if defined(PRE_ADD_DWC) && defined(_VILLAGESERVER)
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)	
				{
					pDBCon->QueryGetDWCChannelInfo(cThreadID, g_Config.nWorldSetID);
					g_Log.Log(LogType::_NORMAL, L"Update DWC Status\n");
				}
#endif
#if defined(PRE_ADD_DWC) && defined(_LOGINSERVER)
				g_pDWCChannelManager->UpdateInfo();
				g_Log.Log(LogType::_NORMAL, L"Update DWC Status\n");
#endif
				return ERROR_NONE;
			}
			break;
	}
	return ERROR_NONE;
}

void CDNServiceConnection::Reconnect(DWORD CurTick)
{
	if (m_dwReconnectTick == 0)
	{
		m_dwReconnectTick = CurTick;
		return;
	}

	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;
		if (!GetActive() && !GetConnecting())
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_SERVICEMANAGER, m_szIP, m_wPort) < 0)
			{
				SetConnecting(false);
				g_Log.Log(LogType::_FILEDBLOG, L"ServiceManager Connect Failed(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else {
				g_Log.Log(LogType::_FILEDBLOG, L"ServiceManager Connected(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}
		else
		{
			if (GetActive())
			{
				SendReqeustServiceInfo();
				SendHeartBeat(); // ServiceManager가 셧다운 되었을경우 Socket에서 감지하지 못할때가 있음. 일방적인 Ping임.
			}

			//여기 빌붙어요~
			static int s_nCallCnt = 0;
#ifdef _CH	
			//중국은 빠른 업데이트
			if ((s_nCallCnt%4) == 0)
#else
			if ((s_nCallCnt%12) == 0)
#endif
			{
				ReportProcess();
			}			
			
			if (s_nCallCnt%720 == 0)
			{
				LoadWorldEvent();
			}

//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
//			if (s_nCallCnt%1440 == 0)
//				LoadSimpleConfig();
//#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

			s_nCallCnt++;				
		}
	}
}

void CDNServiceConnection::Disconnected()
{
	m_dwReconnectTick = 0;
	m_bIsInit = false;

	m_nScopeCnt = 0;
	memset(&m_nScopeElapsed, 0, sizeof(m_nScopeElapsed));
}

void CDNServiceConnection::ScopeElapsedTick(ULONG nDelta)
{
	m_Sync.Lock();
	int cnt = m_nScopeCnt++&31;
	m_nScopeElapsed[cnt] = nDelta;
	m_Sync.UnLock();
}

void CDNServiceConnection::LoadWorldEvent()
{
#ifdef _GAMESERVER
	BYTE cThreadID;
	CDNDBConnection * pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
	if (pDBCon == NULL || g_pMasterConnectionManager == NULL || g_pEvent == NULL)
		g_Log.Log(LogType::_EVENTLIST, L"EventQuery Failed GameServer\n");

	if (pDBCon && g_pMasterConnectionManager && g_pEvent)
	{
		std::vector <int> vList;
		g_pMasterConnectionManager->GetConnectedWorldID(&vList);

		for (int i = 0; i < (int)vList.size(); i++)
			pDBCon->QueryEventList(cThreadID, vList[i]);
	}
#elif _VILLAGESERVER
	if (g_Config.nWorldSetID <= 0 || g_pEvent == NULL)
		g_Log.Log(LogType::_EVENTLIST, L"EventQuery Failed VillageServer\n");

	if (g_Config.nWorldSetID > 0 && g_pEvent)
	{
		BYTE cThreadID;
		CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
		pDBCon->QueryEventList( cThreadID, g_Config.nWorldSetID);
	}
#endif
}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
void CDNServiceConnection::LoadSimpleConfig()
{
#ifdef _VILLAGESERVER
	BYTE cThreadID;
	CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
	pDBCon->QuerySimpleConfig(cThreadID);
#endif		//#ifdef _VILLAGESERVER
}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

void CDNServiceConnection::SendServiceClosed()
{
	AddSendData(SERVICE_SERVER_SERVICE_CLOSED, 0, NULL, 0);
}

#if defined (_MASTERSERVER)
void CDNServiceConnection::SendWorldInfo()
{
	TServiceWorldInfo packet;
	memset(&packet, 0, sizeof(packet));

	packet.nWorldID = g_Config.nWorldSetID;
	packet.nWorldMaxuser = g_pWaitUserManager->GetWorldMaxUser();
	
	AddSendData(SERVICE_SERVER_WORLDINFO, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendMeritInfo()
{
	std::vector <TMeritInfo> vList;
	g_pExtManager->GetMeritList(&vList);

	TServiceMeritInfo packet;
	memset(&packet, 0, sizeof(packet));

	for (int i = 0; i < (int)vList.size(); i++)
	{
		memcpy(&packet.Info[packet.cCount], &vList[i], sizeof(TMeritInfo));
		packet.cCount++;

		if (packet.cCount >= MERITINFOMAX)
			break;
	}

	AddSendData(SERVICE_SERVER_MERITINFO, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Info) + (sizeof(TMeritInfo) * packet.cCount));	
}

void CDNServiceConnection::SendVillageInfo(UINT nUserCnt, std::vector<TVillageInfo> * vList)
{
	std::vector <TVillageInfo>::iterator ii;
	for (ii = vList->begin(); ii != vList->end(); ii++)
	{
		TServiceChannelInfo packet;
		memset(&packet, 0, sizeof(packet));

		packet.nManagedID = (*ii).nManagedID;
		packet.nWorldID = g_Config.nWorldSetID;
		for (int j = 0; j < (int)(*ii).vOwnedChannelList.size(); j++)
		{
			packet.Info[packet.cCount].nChannelID = (*ii).vOwnedChannelList[j].nChannelID;
			packet.Info[packet.cCount].nChannelIdx = (*ii).vOwnedChannelList[j].nChannelIdx;
			packet.Info[packet.cCount].nMapIdx = (*ii).vOwnedChannelList[j].nMapIdx;
			packet.Info[packet.cCount].nCurrentUserCount = (*ii).vOwnedChannelList[j].nCurrentUser;
			packet.Info[packet.cCount].nMaxUserCount = (*ii).vOwnedChannelList[j].nChannelMaxUser;
			packet.Info[packet.cCount].nChannelAttribute = (*ii).vOwnedChannelList[j].nAttribute;
			packet.Info[packet.cCount].nMeritBonusID = (*ii).vOwnedChannelList[j].nMeritBonusID;
			packet.Info[packet.cCount].bVisibility = (*ii).vOwnedChannelList[j].bVisibility;
			_strcpy(packet.Info[packet.cCount].szIP, IPLENMAX, (*ii).szIP, (int)strlen((*ii).szIP));
			packet.Info[packet.cCount].nPort = (*ii).nPort;
			packet.Info[packet.cCount].nLimitLevel = (*ii).vOwnedChannelList[j].nLimitLevel;
			packet.Info[packet.cCount].bShow = (*ii).vOwnedChannelList[j].bShow;
			packet.Info[packet.cCount].nServerID = (*ii).vOwnedChannelList[j].nServerID;
			packet.Info[packet.cCount].cThreadID = (*ii).vOwnedChannelList[j].cThreadID;
			packet.cCount++;
		}
		AddSendData(SERVICE_SERVER_CHANNELINFO, 0, (char*)&packet, sizeof(TServiceChannelInfo) - sizeof(packet.Info) + (sizeof(sChannelInfo) * packet.cCount));
	}
}
#endif

void CDNServiceConnection::SendRportDelayedProcess(int nManagedID, ULONG nDelayedTick)
{
	TServiceDelayedReport packet;		//SERVICE_SERVER_DELAYEDREPORT
	memset(&packet, 0, sizeof(TServiceDelayedReport));

	packet.nManagedID = nManagedID;
	packet.nDelayedTick = nDelayedTick;

	AddSendData(SERVICE_SERVER_DELAYEDREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendQueryDelayedReport(int nManagedID, const char* szQueryName, DWORD dwThreadID, DWORD dwElapsed)
{
	TQueryDelayedReport packet;
	memset(&packet, 0, sizeof(TQueryDelayedReport));

	packet.nMID = nManagedID;
	packet.dwThreadID = dwThreadID;
	packet.dwElasped = dwElapsed;
	strncpy (packet.szQueryName, szQueryName, QUERYNAMESIZE-1);

	AddSendData(SERVICE_SERVER_QUERYDELAYREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendGameDelayedReport(WCHAR * wszLog)
{
	TGameDelayedReport packet;
	memset(&packet, 0, sizeof(TGameDelayedReport));
	_wcscpy(packet.wszLog, _countof(packet.wszLog), wszLog, (int)wcslen(wszLog));

	AddSendData(SERVICE_SERVER_GAMEDELAYREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendVillageDelayedReport(WCHAR * wszLog)
{
	TVillageDelayedReport packet;
	memset(&packet, 0, sizeof(TVillageDelayedReport));
	_wcscpy(packet.wszLog, _countof(packet.wszLog), wszLog, (int)wcslen(wszLog));

	AddSendData(SERVICE_SERVER_VILLAGEDELAYREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendHeartBeat()
{
	AddSendData(SERVICE_SERVER_HEARTBEAT, 0, NULL, 0);
}

void CDNServiceConnection::SendReportUnrecoverBlock(bool bBlock, bool bPermannently)
{
	TServiceUnrecover packet;
	memset(&packet, 0, sizeof(TServiceUnrecover));

	packet.bBlock = bBlock;
	packet.bPermannently = bPermannently;

	AddSendData(SERVICE_SERVER_UNRECOVERY, 0, (char*)&packet, sizeof(TServiceUnrecover));
}

void CDNServiceConnection::SendReloadResult(bool bResult)
{
	TServerExtReloadResult packet;
	memset(&packet, 0, sizeof(TServerExtReloadResult));

	packet.bResult = bResult;

	AddSendData(SERVICE_SERVER_EXT_RELOADRESULT, 0, (char*)&packet, sizeof(TServerExtReloadResult));
}

void CDNServiceConnection::SendLoginWorldDBResult(int nResult, int nWorldID, char* szIp, int nPort)
{
	TServerLoginWorldDResult packet;
	memset(&packet, 0, sizeof(packet));

	packet.nResult = nResult;
	packet.nWorldID = nWorldID;
	if (szIp)
		_strcpy(packet.szIp, _countof(packet.szIp), szIp, (int)strlen(szIp));
	packet.nPort = nPort;

	AddSendData(SERVICE_SERVER_LOGIN_WORDDBRESULT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendDetectException(int nExceptionType)
{
	TServerException packet;
	memset(&packet, 0, sizeof(packet));

	packet.nMID = m_nManagedID;
	packet.nType = nExceptionType;

	AddSendData(SERVICE_SERVER_EXCEPTION_REPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::ReportProcess()
{
#if defined (_LOGINSERVER)
	if (g_pUserConnectionManager && g_pMasterConnectionManager) {
#if defined(PRE_ADD_LOGIN_USERCOUNT)
		SendReportLogin(g_pUserConnectionManager->GetUserCount() - g_pMasterConnectionManager->GetWaitUserAmount());
#else
		SendReportLogin(g_pUserConnectionManager->GetCount() - g_pMasterConnectionManager->GetWaitUserAmount());
#endif
	}
	if (g_pSQLWorldManager)
		g_pSQLWorldManager->ReportErrorWorldDB (this);

#elif defined (_GAMESERVER)
	if (g_pGameServerManager)
	{
		UINT nUserCount, nRoomCount, nTotalRoomCount;
		g_pGameServerManager->GetRoomUserCount(nUserCount, nRoomCount, nTotalRoomCount);
		bool bZero = false;
		bZero = g_pGameServerManager->GetZeroPopulation();
		SendReportGame(bZero, nUserCount, nRoomCount, nTotalRoomCount);

	}
#elif defined (_VILLAGESERVER)
	if (g_pUserSessionManager)
	{
		std::map <int, int> Map;
		UINT nCount = g_pUserSessionManager->GetUserCount(Map);
		SendReportVillage(nCount, Map);
	}
#elif defined (_MASTERSERVER)
	if (g_pWaitUserManager)
	{
		SendReportMaster(g_Config.nWorldSetID, g_pWaitUserManager->GetWaitUserCount());
	}
#endif
}

void CDNServiceConnection::SendReqeustServiceInfo()
{
	if (m_bIsInit)
		return;

	AddSendData(SERVICE_SERVER_REQUESTSERVICE, 0, NULL, 0);
}

void CDNServiceConnection::SendRegistManager(int nManagedID, int nType, const char * pVersion, const char * pResVersion, int nWorldID, BYTE cWorldCount/* = 0*/, BYTE * pWorldID/* = NULL*/)
{
	TServerManagedID packet;
	memset(&packet, 0, sizeof(packet));

	packet.nMID = nManagedID;
	packet.nManagedType = nType;

	if (cWorldCount > 0 && pWorldID != NULL)
	{
		for (int i = 0; i < cWorldCount; i++)
			packet.cWorldID[i] = pWorldID[i];
		packet.cWorldCount = cWorldCount;
	}
	else if (nWorldID > 0)
	{
		packet.cWorldCount = 1;
		packet.cWorldID[0] = (BYTE)nWorldID;
	}

	_strcpy(packet.szVersion, NAMELENMAX, pVersion, (int)strlen(pVersion));
	_strcpy(packet.szResVersion, NAMELENMAX, pResVersion, (int)strlen(pResVersion));

#if defined (_FINAL_BUILD)
	packet.bFinalBuild = true;
#else
	packet.bFinalBuild = false;
#endif

	AddSendData(SERVICE_SERVER_REGIST_MANAGEDID, 0, (char*)&packet, sizeof(packet) - sizeof(packet.cWorldID) + (packet.cWorldCount * sizeof(BYTE)));
}

void CDNServiceConnection::SendReportLogin(UINT nUserCount)
{
	TServiceReportLogin packet;
	memset(&packet, 0, sizeof(packet));

	packet.nUserCount = nUserCount;
	AddSendData(SERVICE_SERVER_USUAL_LOGINREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendReportVillage(UINT nUserCount, std::map <int, int> & Map)
{
	TServiceReportVillage packet;
	memset(&packet, 0, sizeof(packet));

	packet.nUserCount = nUserCount;
	std::map <int, int>::iterator ii;
	for (ii = Map.begin(); ii != Map.end(); ii++)
	{
		packet.Info[packet.nCount].nChannelID = (*ii).first;
		packet.Info[packet.nCount].nChannelUserCount = (*ii).second;
		packet.nCount++;
	}

	AddSendData(SERVICE_SERVER_USUAL_VILLAGEREPORT, 0, (char*)&packet, sizeof(packet) - sizeof(packet.Info) + (packet.nCount * sizeof(TVillageUserReport)));
}

void CDNServiceConnection::SendReportMaster(int nWorldID, UINT nWaitUserCount)
{
	TServiceReportMaster packet;
	memset(&packet, 0, sizeof(TServiceReportMaster));

	packet.nWorldID = nWorldID;
	packet.nWaitserCount = nWaitUserCount;

#if defined (_MASTERSERVER)
	g_pDivisionManager->GetFarmInfo (&packet);
#endif

	AddSendData(SERVICE_SERVER_USUAL_MASTERREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendReportGame(bool bZeropopulation, UINT nUserCount, UINT nRoomCount, UINT nTotalRoomCount)
{
	TServiceReportGame packet;
	memset(&packet, 0, sizeof(packet));

	packet.cIsZeroPopulation = bZeropopulation == true ? 1 : 0;
	packet.nUserCount = nUserCount;
	packet.nRoomCount = nRoomCount;
	packet.nTotalRoomCount = nTotalRoomCount;
	
	AddSendData(SERVICE_SERVER_USUAL_GAMEREPORT, 0, (char*)&packet, sizeof(packet));
}

void CDNServiceConnection::SendPong()
{
	AddSendData(SERVICE_SERVER_PONG, 0, NULL, 0);
}

void CDNServiceConnection::SendGPKLoadFail()
{
	AddSendData(SERVICE_SERVER_GPKRELOADFAIL, 0, NULL, 0);
}

#if defined( _VILLAGESERVER )
void CDNServiceConnection::SendUpdateWorldPvPRoom(int nRetCode)
{
	TApplyWorldPvPRoom packet;
	memset(&packet, 0, sizeof(TApplyWorldPvPRoom));
	packet.nRetCode = nRetCode;

	AddSendData(SERVICE_SERVER_UPDATEWORLDPVPROOM, 0, (char*)&packet, sizeof(packet));
}
#endif