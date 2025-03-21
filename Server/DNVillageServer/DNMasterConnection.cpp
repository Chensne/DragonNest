#include "StdAfx.h"
#include "DNMasterConnection.h"
#include "DNPartyManager.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNIocpManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNFieldManager.h"
#include "DNField.h"
#include "DNWorldUserState.h"
#include "DNGuildSystem.h"
#include "DNGuildVillage.h"
#include "Log.h"
#include "DNPvPRoomManager.h"
#include "DNPvPRoom.h"
#include "DNIsolate.h"
#include "DNEvent.h"
#include "DNServiceConnection.h"
#include "NoticeSystem.h"
#include "DNAuthManager.h"
#include "DNLogConnection.h"
#include "./EtStringManager/EtUIXML.h"
#ifdef _USE_VOICECHAT
#include "dnvoicechat.h"
#endif
#include "MasterSystemCacheRepository.h"
#include "TimeSet.h"
#include "DNGameDataManager.h"
#include "DNFarm.h"
#include "DNPeriodQuestSystem.h"
#include "DNLadderSystemManager.h"
#if defined(_WORK)
#include "PsUpdater.h"
#endif	// #if defined(_WORK)
#include "DNLadderRoom.h"
#include "DNGuildWarManager.h"
#include "DNGuildRecruitCacheRepository.h"
#if defined (PRE_ADD_DONATION)
#include "DNDonationScheduler.h"
#endif // #if defined (PRE_ADD_DONATION)
#if defined (PRE_ADD_BESTFRIEND)
#include "DNBestFriend.h"
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
#include "DnPrivateChatManager.h"
#include "DNPrivateChatChannel.h"
#endif
#include "DNMissionSystem.h"
#if defined(PRE_ADD_DWC)
#include "DNDWCSystem.h"
#include "DNDWCTeam.h"
#endif

extern TVillageConfig g_Config;
CDNMasterConnection* g_pMasterConnection = NULL;

CDNMasterConnection::CDNMasterConnection(void)
{
	m_bConnectCompleted = false;
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)
}

CDNMasterConnection::~CDNMasterConnection(void)
{
}

void CDNMasterConnection::Reconnect()
{
	if (!CConnection::GetActive() && !GetConnecting() && g_pIocpManager)
	{
		SetConnecting(true);
		if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_MASTER, g_Config.MasterInfo.szIP, g_Config.MasterInfo.nPort) < 0)
		{
			SetConnecting(false);
			g_Log.Log(LogType::_ERROR, L"MasterServer Fail(%S, %d)\r\n", g_Config.MasterInfo.szIP, g_Config.MasterInfo.nPort);
		}
		else {
			g_Log.Log(LogType::_NORMAL, L"MasterServer (%S, %d) Connecting..\r\n",g_Config.MasterInfo.szIP, g_Config.MasterInfo.nPort);
		}
	}
}

int CDNMasterConnection::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	switch (iMainCmd)
	{
#ifdef _WORK
		case MAVI_RELOADEXT:
			{
				if (g_pDataManager->AllLoad() == false)
				{
					_DANGER_POINT_MSG(L"ext reload에 실패하였습니다. 서버를 재기동해주세요");
				}
				break;
			}
#endif		//

	case MAVI_REGISTWORLDINFO:
		{
								 /*
#if defined(_WORK)
			if (!g_PsUpdater)
				g_PsUpdater = new(std::nothrow) CPsUpdater;

			g_PsUpdater->AsyncUpdate ();
#endif	// #if defined(_WORK)
			*/
			MARegist *pRegist = (MARegist*)pData;
			g_Config.nWorldSetID = pRegist->cWorldSetID;

			//내 월드가 먼지 알아야 로드 할 수 있다.
			if (!g_pFieldManager->Init())
			{
#if defined (_FINAL_BUILD)
				::ExitProcess(0);
#endif
			}

			if (g_pEvent && g_pEvent->IsInitWorld(g_Config.nWorldSetID) == false)
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
				{
					pDBCon->QueryEventList( cThreadID, g_Config.nWorldSetID);
//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
//					pDBCon->QuerySimpleConfig(cThreadID);
//#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
				}
			}

			if (g_pFarm && g_pFarm->GetInitFlag() == false)
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
						pDBCon->QueryFarmList(cThreadID, g_Config.nWorldSetID);
#if defined( PRE_WORLDCOMBINE_PVP )
				if (pDBCon)
					pDBCon->QueryUpdateWorldPvPRoom(0, g_Config.nWorldSetID);
#endif
			}
			if (g_pPeriodQuestSystem)
				g_pPeriodQuestSystem->LoadWorldQuestInfo(g_Config.nWorldSetID);
#if defined (PRE_ADD_DONATION)
			if (CDNDonationScheduler::IsActive())
				CDNDonationScheduler::GetInstance().AddWorldID(g_Config.nWorldSetID);
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PRIVATECHAT_CHANNEL )
			if( g_pPrivateChatChannelManager && g_pPrivateChatChannelManager->GetFirst() == false )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if ( pDBCon && pDBCon->GetActive() )
				{					
					pDBCon->QueryGetPrivateChatChannelInfo(0, g_Config.nWorldSetID);
					pDBCon->QueryDelPrivateMemberServerID(0, g_Config.nWorldSetID, g_Config.nManagedID );
					g_pPrivateChatChannelManager->SetFirst(true);
				}				
			}
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
			if( !g_pDataManager->bIsAlteiaWorldEventTime() )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if ( pDBCon && pDBCon->GetActive() )
					pDBCon->QueryResetAlteiaWorldEvent(0, g_Config.nWorldSetID);
#if defined( _WORK )
				g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"Reset AlteiaWorldEvent WorldID:%d\r\n", g_Config.nWorldSetID);
#endif
			}	
			time_t tAlteiaEventStartTime = g_pDataManager->GetAlteiaWorldEventBeginTime();
			time_t tAlteiaEventEndTime = g_pDataManager->GetAlteiaWorldEventEndTime();
			if( tAlteiaEventStartTime > 0 && tAlteiaEventEndTime > 0 )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if ( pDBCon && pDBCon->GetActive() )
					pDBCon->QueryAddAlteiaWorldEventTime(0, g_Config.nWorldSetID, tAlteiaEventStartTime, tAlteiaEventEndTime);
			}
#endif
#if defined(PRE_ADD_DWC)
			if (g_pDWCTeamManager && g_pDWCTeamManager->GetInitFlag() == false)
			{
				if(g_pFieldManager->bIsIncludeDWCVillage())
					g_pDWCTeamManager->SetIncludeDWCVillage(true);

				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryGetDWCChannelInfo(cThreadID, g_Config.nWorldSetID);
			}
#endif
			//Master Connect Flow
			std::string strPublicIP;
			g_pIocpManager->GetPublicIP(strPublicIP);
			SendVIllageRegistInfo(g_Config.nManagedID, strPublicIP.c_str(), g_Config.nClientAcceptPort);
			g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[MAVI_REGIST] WorldID:%d\r\n", g_Config.nWorldSetID);
			return ERROR_NONE;
		}
		break;

	case MAVI_REQUSERLIST:
		{
			MAReqUserList * pPacket = (MAReqUserList*)pData;

			if (pPacket->nRetCode == ERROR_NONE)
			{
				BYTE cCount = 0;
				TFarmItemFromDB	Farms[Farm::Max::FARMCOUNT];
				memset(Farms, 0, sizeof(Farms));
				g_pFarm->GetFarmList(Farms, cCount);
				SendFarmListGot(Farms, cCount);					//팜정보도 한바탕 보내줍니다.
				g_pUserSessionManager->SendAddUserList();	//항상 최하단에 위치해야함
				g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[MAVI_REQUSERLIST] WorldID:%d\r\n", g_Config.nWorldSetID);
			}
			else
				g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, 0, 0, 0, L"[MAVI_REQUSERLIST] Master Registration Failed WorldID:%d\r\n", g_Config.nWorldSetID);
			return ERROR_NONE;
		}
		break;

	case MAVI_REGISTCOMPLETE:
		{
			m_bConnectCompleted = true;
			g_pIocpManager->VerifyAccept(ACCEPTOPEN_VERIFY_TYPE_MASTERCONNECTION);
			g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[MAVI_REGISTCOMPLETE] WorldID:%d\r\n", g_Config.nWorldSetID);
			return ERROR_NONE;
		}
		break;

	case MAVI_VILLAGEINFO:
		{
			MAVIVillageInfo * pPacket = (MAVIVillageInfo*)pData;

			bool bFlag = false;
			for (int i = 0; i < pPacket->cCount; i++)
			{
				std::vector < std::pair <int, sChannelInfo> >::iterator ii;
				for (ii = m_vChannelList.begin(); ii != m_vChannelList.end(); ii++)
				{
					if ((*ii).second.nChannelID == pPacket->Info[i].nChannelID)
					{
						memcpy(&(*ii).second, &pPacket->Info[i], sizeof(sChannelInfo));
						bFlag = true;
					}
				}
			}

			if (m_vChannelList.size() <= 0 || bFlag == false)
			{
				for (int i = 0; i < pPacket->cCount; i++)
					m_vChannelList.push_back(std::make_pair(pPacket->nVillageID, pPacket->Info[i]));
			}
		}
		break;

	case MAVI_VILLAGEINFODEL:
		{
			MAVIVillageInfoDelete * pPacket = (MAVIVillageInfoDelete*)pData;

			std::vector < std::pair <int, sChannelInfo> >::iterator ii;
			for (ii = m_vChannelList.begin(); ii != m_vChannelList.end();)
			{
				if ((*ii).first == pPacket->nVillageID)
					ii = m_vChannelList.erase(ii);
				else
					ii++;
			}
		}
		break;

	case MAVI_LOGINSTATE:
		{
			MAVILoginState *pState = (MAVILoginState*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSession(pState->nSessionID);
			if (pUserObj){
				pUserObj->m_eUserState = STATE_READYTOLOGIN;

				pUserObj->m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();
				DN_ASSERT(0 != pUserObj->m_biCertifyingKey,	"Invalid!");	// 인증키가 0 이 생성되면 않됨 !!! (없음 의미)

				g_pAuthManager->QueryStoreAuth(SERVERTYPE_VILLAGE, pUserObj);
			}
		}
		break;

	case MAVI_SETGAMEID:
		{
			MAVISetGameID* pPacket = reinterpret_cast<MAVISetGameID*>(pData);
			if( pPacket->cReqGameIDType == REQINFO_TYPE_SINGLE || pPacket->cReqGameIDType == REQINFO_TYPE_PVP_BREAKINTO || 
				pPacket->cReqGameIDType == REQINFO_TYPE_PARTY_BREAKINTO || pPacket->cReqGameIDType == REQINFO_TYPE_FARM ||
				pPacket->cReqGameIDType == REQINFO_TYPE_LADDER_OBSERVER )
			{
				CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( static_cast<UINT>(pPacket->InstanceID) );
				if( !pUserObj ) 
				{
					_DANGER_POINT();
					return ERROR_GENERIC_INVALIDREQUEST;
				}

				// 게임섭에 문제가 있다.
				if( pPacket->nRet < 0)
				{
					pUserObj->m_eUserState = STATE_NONE;
					pUserObj->m_bIsStartGame = false;

					pUserObj->SendCancelStage(ERROR_PARTY_STARTSTAGE_FAIL);
					if( pPacket->cReqGameIDType == REQINFO_TYPE_LADDER_OBSERVER )
					{
						LadderSystem::CRoom* pRoom = LadderSystem::CManager::GetInstance().GetRoomPtr( pUserObj->GetCharacterName() );
						if( pRoom && pRoom->GetRoomState() == LadderSystem::RoomState::Observer )
							pRoom->ChangeRoomState( LadderSystem::RoomState::WaitUser, LadderSystem::RoomStateReason::MAX_OBSERVER );
					}
					return ERROR_NONE;
				}

				//이동설정
				pUserObj->m_bIsMove = true;
				pUserObj->m_GameTaskType = pPacket->GameTaskType;

				if (pPacket->cReqGameIDType == REQINFO_TYPE_PARTY_BREAKINTO)
					pUserObj->m_cReqGameIDType	= REQINFO_TYPE_PARTY_BREAKINTO;
				else
					pUserObj->m_cReqGameIDType	= (pPacket->cReqGameIDType == REQINFO_TYPE_SINGLE) ? REQINFO_TYPE_SINGLE : REQINFO_TYPE_PVP;
				pUserObj->SetGameID(pPacket->wGameID);
				pUserObj->SetRoomID(pPacket->iRoomID);
				if( pPacket->cReqGameIDType == REQINFO_TYPE_SINGLE )
					pUserObj->SetTargetMapIndexByStartStage();

				//이동해야할 게임서버의 아이피와 포트를 저장한다.
				pUserObj->m_nGameServerIP		= pPacket->nIP;
				pUserObj->m_nGameServerPort		= pPacket->nPort;
				pUserObj->m_nGameServerTcpPort	= pPacket->nTcpPort;
				pUserObj->m_eUserState			= STATE_READYTOGAME;

				pUserObj->m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();
				DN_ASSERT(0 != pUserObj->m_biCertifyingKey,	"Invalid!");	// 인증키가 0 이 생성되면 않됨 !!! (없음 의미)

				g_pAuthManager->QueryStoreAuth(SERVERTYPE_VILLAGE, pUserObj);

				g_Log.Log(LogType::_NORMAL, pUserObj, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] MAVI_SETGAMEID GameID:%d, RoomID:%d (%u.%u.%u.%u)\r\n", pUserObj->GetAccountDBID(), pUserObj->GetCharacterDBID(), pUserObj->GetSessionID(), pUserObj->GetObjectID(), pPacket->wGameID, pPacket->iRoomID, ((BYTE*)&pUserObj->m_nGameServerIP)[0], ((BYTE*)&pUserObj->m_nGameServerIP)[1], ((BYTE*)&pUserObj->m_nGameServerIP)[2], ((BYTE*)&pUserObj->m_nGameServerIP)[3]);
			}
			else if( pPacket->cReqGameIDType == REQINFO_TYPE_PARTY )
			{
				CDNParty* pParty = g_pPartyManager->GetParty( pPacket->InstanceID );
#if defined( PRE_WORLDCOMBINE_PARTY )
				if( Party::bIsWorldCombineParty(pPacket->Type) )
				{
					if( pPacket->nRet != ERROR_NONE )
					{
						g_pPartyManager->DelCombinePartyData( (int)pPacket->InstanceID );
					}
					return ERROR_NONE;
				}
#endif
				if( !pParty ) 
				{
					_DANGER_POINT();
					return ERROR_NONE;
				}

				// 게임섭에 문제가 있다.
				if( pPacket->nRet < 0 )
				{
					pParty->SetStartGame(false);
					pParty->SetPartyUserStartFlag(false);

					pParty->SendAllCancelStage( ERROR_PARTY_STARTSTAGE_FAIL );
					return ERROR_NONE;
				}

#if defined( PRE_PARTY_DB )
#else
				g_pPartyManager->EraseWaitPartyList(pParty);	// wait list에서 party를 빼버린다.
#endif // #if defined( PRE_PARTY_DB )
				pParty->SetMove(true);
				pParty->SetPartyUserMoveFlag(true);

				if (pPacket->cVillageID == g_Config.nVillageID)
				{
					pParty->SetMemberTargetMapIndexByStartStage();
					pParty->SendAllRoomID( pPacket->GameTaskType, REQINFO_TYPE_PARTY, pPacket->wGameID, pPacket->iRoomID);
					//pParty->SendAllLocalSymbolData();	// 게임서버에 db저장 안되는 symbol data 날려줘야한다
					pParty->SendAllReadyToGame(pPacket->nIP, pPacket->nPort, pPacket->nTcpPort);	// 유저데이터 저장

					g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[PartyIndex:%I64d] MAVI_SETGAMEID GameID:%d, RoomID:%d\r\n", pPacket->InstanceID, pPacket->wGameID, pPacket->iRoomID);
				}
			}
			else if( pPacket->cReqGameIDType == REQINFO_TYPE_PVP )
			{
#if defined( PRE_WORLDCOMBINE_PVP )
				if( pPacket->eWorldReqType > WorldPvPMissionRoom::Common::NoneType )
				{
					if( pPacket->nRet < 0 )
					{		
						CDNPvPRoomManager::GetInstance().DelWorldPvPRoom((int)(pPacket->InstanceID));
					}
					return ERROR_NONE;
				}
#endif
				CDNPvPRoom* pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, static_cast<UINT>(pPacket->InstanceID) );
				if( !pPvPRoom ) 
				{
					_DANGER_POINT();
					return ERROR_NONE;
				}

				// 게임섭에 문제가 있다.
				if( pPacket->nRet < 0 )
				{		
					pPvPRoom->SendAllCancelStage( ERROR_PVP_STARTPVP_FAILED );
					return ERROR_NONE;
				}

				pPvPRoom->SendAllRoomID( pPacket->GameTaskType, REQINFO_TYPE_PVP, pPacket->wGameID, pPacket->iRoomID );
				//pPvPRoom->SendAllLocalSymbolData();	// 게임서버에 db저장 안되는 symbol data 날려줘야한다
				pPvPRoom->SendAllReadyToGame(pPacket->nIP, pPacket->nPort, pPacket->nTcpPort);	// 유저데이터 저장

				g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[PvPIndex:%d] MAVI_SETGAMEID GameID:%d, RoomID:%d\r\n", static_cast<UINT>(pPacket->InstanceID), pPacket->wGameID, pPacket->iRoomID );
			}
			else
				_DANGER_POINT();
			return ERROR_NONE;
		}
		break;
	case MAVI_LADDERSYSTEM_SETGAMEID:
		{
			MAVILadderSetGameID* pPacket = reinterpret_cast<MAVILadderSetGameID*>(pData);

			// ValidCheck
			int iValidUserCount		= 0;
			int iNeedVaildUserCount = 0;

			std::map<INT64,LadderSystem::CRoom*> mRoom;

			for( int i=0 ; i<pPacket->cUserCount ; ++i )
			{
				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBIDArr[i] );
				if( pSession == NULL )
					continue;

				LadderSystem::CRoom* pLadderRoom = LadderSystem::CManager::GetInstance().GetRoomPtr( pSession->GetCharacterName() );
				if( pLadderRoom == NULL || pLadderRoom->GetRoomState() != LadderSystem::RoomState::Syncing )
					continue;

				if( pPacket->nRet != ERROR_NONE )
				{
					pLadderRoom->ChangeRoomState( LadderSystem::RoomState::WaitUser, LadderSystem::RoomStateReason::CANT_CREATE_GAMEROOM );
				}
				else
				{
					mRoom.insert( std::make_pair(pLadderRoom->GetRoomIndex(),pLadderRoom) );
				}
			}

			if( mRoom.size() != 2 )
				return ERROR_NONE;

			std::map<INT64,LadderSystem::CRoom*>::iterator itor = mRoom.begin();
			LadderSystem::CRoom* pLadderRoom  = (*itor++).second;
			LadderSystem::CRoom* pLadderRoom2 = (*itor).second;

			if( pLadderRoom->GetOpponentRoomIndex() != pLadderRoom2->GetRoomIndex() || pLadderRoom->GetRoomIndex() != pLadderRoom2->GetOpponentRoomIndex() )
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}

			pLadderRoom->SendPvPGameModeTableID( pPacket->nGameModeTableID );
			pLadderRoom->SendAllRoomID( pPacket->wGameID, pPacket->iRoomID );
			pLadderRoom->SendAllReadyToGame( pPacket->nIP, pPacket->nPort, pPacket->nTcpPort);	// 유저데이터 저장

			pLadderRoom2->SendPvPGameModeTableID( pPacket->nGameModeTableID );
			pLadderRoom2->SendAllRoomID(  pPacket->wGameID, pPacket->iRoomID );
			pLadderRoom2->SendAllReadyToGame(pPacket->nIP, pPacket->nPort, pPacket->nTcpPort);	// 유저데이터 저장

			/*
			g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[PvPIndex:%d] MAVI_SETGAMEID GameID:%d, RoomID:%d\r\n", pPacket->uiIndex, pPacket->wGameID, pPacket->iRoomID );
			*/
			break;
		}
	case MAVI_LADDERSYSTEM_ROOMSYNC:
		{
			LadderSystem::MAVIRoomSync* pPacket = reinterpret_cast<LadderSystem::MAVIRoomSync*>(pData);

			bool bRet = LadderSystem::CManager::GetInstance().bIsValidPairRoom( pPacket->biRoomIndexArr[0], pPacket->biRoomIndexArr[1] );
			if( bRet == true )
			{
				LadderSystem::CManager::GetInstance().ChangeRoomState( pPacket->biRoomIndexArr[0], pPacket->State );
				LadderSystem::CManager::GetInstance().ChangeRoomState( pPacket->biRoomIndexArr[1], pPacket->State );
			}

			break;
		}
	case MAVI_LADDERSYSTEM_ENTERCHANNEL:
		{
			VIMALadderEnterChannel* pPacket = reinterpret_cast<VIMALadderEnterChannel*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession == NULL )
				break;
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
			if( pSession->GetField() && !pSession->GetField()->bIsPvPLobby()) // 요건 마을에서 방리스트 보고 들간거..					
			{
				const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
				if( pChannelInfo )												
					pSession->CmdSelectChannel( pChannelInfo->nChannelID, true );
				break;
			}
#endif
			LadderSystem::CManager::GetInstance().OnEnterChannel( pSession, pPacket->MatchType );
			break;
		}
	case MAVI_LADDERSYSTEM_INVITEUSER:
		{
			VIMALadderInviteUser* pPacket = reinterpret_cast<VIMALadderInviteUser*>(pData);

			CDNUserSession* pInviteSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pInviteSession == NULL )
				break;
			int iRet = pPacket->nRetCode;
			if (iRet == ERROR_NONE)
				iRet = LadderSystem::CManager::GetInstance().OnInviteUser( pInviteSession, pPacket->wszCharName );
			pInviteSession->SendPvPLadderInviteUser( iRet, pPacket->wszCharName );
			break;
		}
		case MAVI_LADDERSYSTEM_INVITECONFIRM:
		{
			VIMALadderInviteConfirm* pPacket = reinterpret_cast<VIMALadderInviteConfirm*>(pData);
			LadderSystem::CManager::GetInstance().OnInviteUserConfirm( pPacket );
			break;
		}
	case MAVI_CHECKUSER:
		{
			MAVICheckUser *pCheckUser = (MAVICheckUser*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSession(pCheckUser->nSessionID);
			if (!pUserObj) return ERROR_NONE;

			if (pCheckUser->nRet != ERROR_NONE){
				pUserObj->DetachConnection(L"Connect|Check Fail");
				return ERROR_GENERIC_INVALIDREQUEST;
			}
			// 여기서 혹시 모르니 전체 DB에 ErrorCheck 풀어달라고 요청
			g_pDBConnectionManager->SendResetErrorCheck(pCheckUser->nAccountDBID);

			pUserObj->InitAccount(pCheckUser);
			pUserObj->SetPvPVillageInfo( pCheckUser->cPvPVillageID, pCheckUser->unPvPVillageChannelID );

			return ERROR_NONE;
		}
		break;

	case MAVI_TARGETVILLAGEINFO:
		{
			MAVITargetVillageInfo * pPacket = (MAVITargetVillageInfo*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			if (pPacket->nRet == ERROR_NONE)
			{
				const sChannelInfo* pChannelInfo			= g_pMasterConnection->GetChannelInfo( pUserObj->GetChannelID() );
				const sChannelInfo* pTargetCurChannelInfo	= g_pMasterConnection->GetChannelInfo( pPacket->nTargetChannelID );
				if( pChannelInfo && pTargetCurChannelInfo )
				{
#if defined(PRE_ADD_DWC)
					if( pChannelInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_DWC) && pTargetCurChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
#else
					if( pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP && pTargetCurChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
#endif
					{
						pUserObj->SetPvPVillageInfo( pChannelInfo->cVillageID, pChannelInfo->nChannelID );
					}
				}

				if (pUserObj->GetPartyID() > 0 && pPacket->cPartyInto <= 0)
				{
					//파티라면
					if (pPacket->nTargetChannelID != pUserObj->GetChannelID())	//서로 다른 채널로 이동이면
					{
						CDNParty* pParty = g_pPartyManager->GetParty(pUserObj->GetPartyID());
						if (pParty)
						{
#if defined( PRE_PARTY_DB )
#else
							g_pPartyManager->EraseWaitPartyList(pParty);
#endif // #if defined( PRE_PARTY_DB )
						}
						else
						{
							g_Log.Log(LogType::_ERROR, L"MAVI_TARGETVILLAGEINFO pParty == NULL\n");
							return ERROR_NONE;
						}

						UINT nMemberAID[PARTYMAX];
						memset(nMemberAID, 0, sizeof(nMemberAID));
						pParty->GetMembersByAccountDBID(nMemberAID);

						CDNUserSession * pPartyUser = NULL;
						for (int i = 0; i < PARTYCOUNTMAX; i++)
						{
							if (nMemberAID[i] <= 0) continue;
							pPartyUser = g_pUserSessionManager->FindUserSessionByAccountDBID(nMemberAID[i]);
							if (pPartyUser)
							{
								if (pPacket->cTargetGateNo > 0)	// 정상
									pPartyUser->ChangeMap(pPacket->cVillageID, pPacket->cTargetGateNo, pPacket->nTargetChannelID, pPacket->nTargetMapIdx, pPacket->szIP, pPacket->nPort);
								else 	// cheat용 채널만 이동일경우도 나올 수 있어효~
									pPartyUser->ChangeMap(pPacket->cVillageID, pPacket->nTargetChannelID, pPacket->nTargetMapIdx, pPacket->szIP, pPacket->nPort);
							}
							else
								_DANGER_POINT();		//이건 멍미?
						}
#if defined( PRE_PARTY_DB )
						if (pPacket->cTargetGateNo <= 0)	// 치트이동시 
						{							
							if ( pPartyUser &&  pPartyUser->GetDBConnection() && pPartyUser->GetDBConnection()->GetActive() )
								pPartyUser->GetDBConnection()->QueryModParty( pPartyUser, pParty, g_Config.nManagedID, Party::LocationType::Village, pPacket->nTargetChannelID );
						}						
#endif  // #if defined( PRE_PARTY_DB )
					}
					else
						_DANGER_POINT();
				}
				else
				{
					if (pPacket->cTargetGateNo > 0){	// 정상
						pUserObj->ChangeMap(pPacket->cVillageID, pPacket->cTargetGateNo, pPacket->nTargetChannelID, pPacket->nTargetMapIdx, pPacket->szIP, pPacket->nPort);
						return ERROR_NONE;
					}
					else {	// cheat용 채널만 이동일경우도 나올 수 있어효~ 파티조인시 이동도 추가
						pUserObj->WarpVillage(pPacket);
					}
				}
			}
			else 
			{
				pUserObj->m_eUserState = STATE_NONE;
				pUserObj->SendVillageInfo(pPacket->szIP, pPacket->nPort, pPacket->nRet, 0, 0);
			}
		}
		break;

	case MAVI_DETACHUSER:
		{
			MADetachUser *pDetach = (MADetachUser*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pDetach->nAccountDBID);
			if (!pUserObj)
			{
				if( pDetach->bIsDuplicate )		
				{
					SendDuplicateLogin(pDetach->nAccountDBID, false, pDetach->nSessionID);
					g_Log.Log(LogType::_NORMAL,0, pDetach->nAccountDBID, 0, pDetach->nSessionID, L"[MAVI_DETACHUSER] Duplicate User Not Village ID:%d, SID:%d !!\r\n", g_Config.nVillageID, g_Config.nManagedID);
				}
				return ERROR_NONE;
			}

#if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
			// 중복 로그인으로 끊는 경우, 처음에는 클라이언트에게 접속 종료 요청.
			// DetachFlag가 On인 경우에만 서버에서 접속을 종료시킴(클라가 조작되거나 패킷을 받지 않는 경우, 중복 로그인으로 연결 해제 요청이 다시 들어올수 있음)
			if( pDetach->bIsDuplicate && !pUserObj->GetDetachFlag() )
			{
				pUserObj->SendCompleteDetachMsg( ERROR_DUPLICATEUSER_INGAME, L"ERROR_DUPLICATEUSER_INGAME", true );
				SendDuplicateLogin(pDetach->nAccountDBID, true, pDetach->nSessionID);
				g_Log.Log(LogType::_NORMAL,0, pDetach->nAccountDBID, 0, pDetach->nSessionID, L"[MAVI_DETACHUSER] Duplicate User Village!! ID:%d, SID:%d\r\n", g_Config.nVillageID, g_Config.nManagedID);
			}
			else
			{
				pUserObj->DetachConnection(L"Connect|MAVI_DETACHUSER");
				g_Log.Log(LogType::_NORMAL,0, pDetach->nAccountDBID, 0, pDetach->nSessionID, L"[MAVI_DETACHUSER] User Village!! ID:%d, SID:%d\r\n", g_Config.nVillageID, g_Config.nManagedID);
			}
#else	// #if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
			pUserObj->DetachConnection(L"Connect|MAVI_DETACHUSER");
			g_Log.Log(LogType::_NORMAL,0, pDetach->nAccountDBID, 0, pDetach->nSessionID, L"[MAVI_DETACHUSER] User Village!! ID:%d, SID:%d\r\n", g_Config.nVillageID, g_Config.nManagedID);
			if( pDetach->bIsDuplicate )
			{
				SendDuplicateLogin(pDetach->nAccountDBID, true, pDetach->nSessionID);
				g_Log.Log(LogType::_NORMAL,0, pDetach->nAccountDBID, 0, pDetach->nSessionID, L"[MAVI_DETACHUSER] Duplicate User Village!! ID:%d, SID:%d\r\n", g_Config.nVillageID, g_Config.nManagedID);
			}
#endif	// #if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
		}
		break;

	case MAVI_USERSTATE:
		{
			MAUserState * pPacket = (MAUserState*)pData;

			if (g_pWorldUserState == NULL)
				break;

			if (pPacket->cNameLen <= 0 || pPacket->cNameLen > NAMELENMAX-1)
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}

			WCHAR wszName[NAMELENMAX];
			_wcscpy(wszName, NAMELENMAX, pPacket->wszBuf, pPacket->cNameLen);

			if (pPacket->Type == WorldUserState::Add )
			{
				g_pWorldUserState->AddUserState(wszName, pPacket->biCharacterDBID, pPacket->cLocationState, pPacket->cCommunityState, pPacket->nChannelID, pPacket->nMapIdx);
			}
			else if (pPacket->Type == WorldUserState::Delete )
			{
				g_pWorldUserState->DelUserState(wszName, pPacket->biCharacterDBID);
				LadderSystem::CManager::GetInstance().OnDisconnectUser( pPacket->biCharacterDBID, wszName );
			}
			else if( pPacket->Type == WorldUserState::Modify )
			{
				g_pWorldUserState->UpdateUserState(wszName, pPacket->biCharacterDBID, pPacket->cLocationState, pPacket->cCommunityState, pPacket->nChannelID, pPacket->nMapIdx);
			}
			else
				_DANGER_POINT();

#if defined(PRE_ADD_DWC)
			if(g_pDWCTeamManager && g_pDWCTeamManager->bIsIncludeDWCVillage())
				g_pDWCTeamManager->ChangeDWCTeamMemberState(pPacket->biCharacterDBID, pPacket->Type == WorldUserState::Delete, 
				pPacket->cLocationState, pPacket->nChannelID, pPacket->nMapIdx);
#endif
			return ERROR_NONE;
		}
		break;

#if defined(_CH)
	case MAVI_FCMSTATE:	// 피로도
		{
			MAFCMState *pState = (MAFCMState*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pState->nAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			pUserObj->SetFCMOnlineMin(pState->nOnlineMin, pState->bSend);
		}
		break;
#endif	// _CH

		// 파티관련
#if defined( PRE_PARTY_DB )	
	case MAVI_RESPARTYINVITE:
		{
			MAVIResPartyInvite* pPacket= reinterpret_cast<MAVIResPartyInvite*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiReqAccountDBID );
			if( pSession == NULL )
				return ERROR_NONE;

			switch( pPacket->iRet )
			{
				case ERROR_PARTY_INVITEFAIL:
				case ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND:
				case ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY:
				case ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED:
				case ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME:
				case ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY:
				case ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE:
				case ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER:
				case ERROR_PARTY_INVITEFAIL_ALREADY_INVITED:
				{
					pSession->SendPartyInviteFail( pPacket->iRet );
					break;
				}
				case ERROR_ISOLATE_REQUESTFAIL:
				{
					pSession->SendIsolateResult( pPacket->iRet );
					break;
				}
			}
			
			return ERROR_NONE;
		}
	case MAVI_REQPARTYINVITE:
		{
			MAVIReqPartyInvite* pPacket = reinterpret_cast<MAVIReqPartyInvite*>(pData);

			CDNUserSession* pInviteSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszInviteCharName );
			if( pInviteSession == NULL )
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
#if defined(PRE_ADD_DWC)
			if(pInviteSession->IsDWCCharacter())
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
#endif

#if defined(PRE_FIX_62281)
			int nRet = pInviteSession->CheckPartyInvitableState(pPacket->wszReqCharName, pPacket->iPartyMinLevel);
			if(nRet != ERROR_NONE)
			{
				g_pMasterConnection->SendResPartyInvite( nRet, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
#else
			if( pInviteSession->m_eUserState != STATE_NONE )
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( pInviteSession->IsAcceptAbleOption( pPacket->biReqCharacterDBID, pPacket->uiReqAccountDBID, _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false )
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( pInviteSession->GetIsolate()->IsIsolateItem( pPacket->wszReqCharName) )
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( (pInviteSession->GetAccountLevel() >= AccountLevel_New) && (pInviteSession->GetAccountLevel() <= AccountLevel_Master))
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			const sChannelInfo* pUserChInfo = g_pMasterConnection->GetChannelInfo(pInviteSession->GetChannelID());
			if( pUserChInfo == NULL )
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY)
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( !pInviteSession->IsNoneWindowState())
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( pInviteSession->GetPartyID() > 0)
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			if( pInviteSession->GetLevel() < pPacket->iPartyMinLevel )
			{
				g_pMasterConnection->SendResPartyInvite( ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
#endif
			pInviteSession->SetWindowState(WINDOW_ISACCEPT);
			pInviteSession->SendPartyInviteMsg( pPacket->PartyID, pPacket->wszPartyName, pPacket->iPartyPassword, pPacket->wszReqCharName, (BYTE)pPacket->iPartyMemberMax, (BYTE)pPacket->iPartyMemberCount, (USHORT)pPacket->iPartyAvrLV, -1, pPacket->iReqMapIndex );
			return ERROR_NONE;
		}
	case MAVI_PARTYINVITEDENIED:
		{
			MAVIPartyInviteDenided* pPacket = reinterpret_cast<MAVIPartyInviteDenided*>(pData);

			CDNParty* pParty = g_pPartyManager->GetParty( pPacket->PartyID );
			if( pParty == NULL )	
				return ERROR_NONE;

			if( pParty->InviteDenied( pPacket->wszInviteCharName ) == true )
			{
				CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pParty->GetLeaderAccountDBID());
				if (pUser)
					pUser->SendPartyInviteDenied( pPacket->wszInviteCharName, pPacket->cIsOpenBlind > 0 ? ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED : ERROR_NONE);
			}

			return ERROR_NONE;
		}
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	case MAVI_REQPARTYASKJOIN:
		{
			MAVIReqPartyAskJoin* pPacket = reinterpret_cast<MAVIReqPartyAskJoin*>(pData);

			CDNUserSession* pTargetSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszTargetChracterName );
			if( pTargetSession == NULL )
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_FAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
			if( pTargetSession->GetPartyID() <= 0 )
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			CDNParty* pParty = g_pPartyManager->GetParty(pTargetSession->GetPartyID());
			if( pParty == NULL )
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_PARTY_NOTFOUND, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			CDNUserSession* pLeader = g_pUserSessionManager->FindUserSessionByAccountDBID(pParty->GetLeaderAccountDBID());
			if( pLeader == NULL )
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_FAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
			if( pLeader->GetIsolate()->IsIsolateItem(pPacket->wszReqChracterName) )
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_FAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
			if( pLeader->IsAcceptAbleOption(pPacket->biReqCharacterDBID, pPacket->uiReqAccountDBID, _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false)
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_FAIL, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}
			if( !pLeader->IsNoneWindowState() )
			{
				g_pMasterConnection->SendResPartyAskJoin( ERROR_PARTY_ASKJOIN_DESTUSER_SITUATION_NOTALLOWED, pPacket->uiReqAccountDBID );
				return ERROR_NONE;
			}

			pLeader->SetWindowState(WINDOW_ISACCEPT);
			pLeader->SendAskJoinToLeader(pPacket->wszReqChracterName, pPacket->cReqUserJob, pPacket->cReqUserLevel);
			return ERROR_NONE;
		}
	case MAVI_RESPARTYASKJOIN:
		{
			MAVIResPartyAskJoin* pPacket = reinterpret_cast<MAVIResPartyAskJoin*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiReqAccountDBID );
			if( pSession )
				pSession->SendAskJoinResult( pPacket->iRet );

			return ERROR_NONE;
		}
	case MAVI_ASKJOINAGREEINFO:
		{
			MAVIAskJoinAgreeInfo* pPacket = reinterpret_cast<MAVIAskJoinAgreeInfo*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAskerAccountDBID );
			if( pSession )
				pSession->SendAskJoinAgreeInfo( pPacket->PartyID, pPacket->iPassword );	//패스워드는 빼도 된다

			return ERROR_NONE;
		}
#endif
#else
	case MAVI_REQPARTYID:
		{
			MAVIReqPartyID * pPacket = (MAVIReqPartyID*)pData;

			CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
			if(pUser)
			{
				if (pUser->GetPartyID() > 0)		//파티는 만드신다고 해놓고 고 사이에 다른파티에 엔터 할 수 있다. 그럼 조낸 꼬임
					return ERROR_NONE;

				CDNParty * pParty = g_pPartyManager->CreateParty(pUser, pPacket->PartyID,  pPacket->wszPartyName, pPacket->wszPartyPass, pPacket->cMemberMax, pPacket->ItemLootRule, pPacket->ItemRank, pPacket->cUserLvLimitMin, pPacket->cUserLvLimitMax, pPacket->nTargetMapIdx, pPacket->Difficulty, pPacket->cPartyJobDice);
				if (pParty)
				{
					int nMemberIdx = -1;
					if (pParty->AddPartyMember(pUser->GetAccountDBID(), pUser->GetSessionID(), pUser->GetCharacterName(), nMemberIdx, true))
					{
						pUser->SendCreateParty(ERROR_NONE, pParty->GetPartyID(), pPacket->nAfterInviteAccountDBID > 0 ? true : false, pParty->m_bRaidParty ? _RAID_PARTY_8 : _NORMAL_PARTY);

#ifdef _USE_VOICECHAT
						if (pPacket->cUseVoice > 0 && g_pVoiceChat)		//사용이 켜저있으면 생성
							SendReqVoiceChannelID(pParty->GetPartyID(), _VOICECHANNEL_REQTYPE_PARTY);
#endif

						pUser->SetPartyID( pParty->GetPartyID() );
						pUser->m_nPartyMemberIndex = nMemberIdx;
						pUser->m_boPartyLeader = true;
							
						pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
						pParty->SendPartyMemberMoveEachOther(pUser->GetSessionID(), pUser->GetTargetPos().nX, pUser->GetTargetPos().nY, pUser->GetTargetPos().nZ);

						if (pPacket->nAfterInviteAccountDBID > 0)
						{
							CDNUserSession * pInviteUser = (CDNUserSession*)g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAfterInviteAccountDBID);
							if (pInviteUser != NULL)
							{
								//초대할넘 상태 검사
								if( !pInviteUser->IsNoneWindowState() )
								{
									pUser->SendPartyInviteFail(ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED);
									return ERROR_NONE;
								}
								if (pParty->InvitePartyMember(pInviteUser->GetCharacterName()) == true)
								{
									pInviteUser->SendPartyInviteMsg(pParty->GetPartyID(), pParty->GetPartyName(), pParty->m_wszPartyPassword, pUser->GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv());
									pInviteUser->SetWindowState(WINDOW_ISACCEPT);
								}
								else
									pUser->SendPartyInviteFail(ERROR_PARTY_INVITEFAIL_ALREADY_INVITED);
							}
							else
								_DANGER_POINT();
						}
						return ERROR_NONE;
					}
					else
					{
						_DANGER_POINT();
						g_pPartyManager->DestroyParty(pParty->GetPartyID());
					}
				}
				else
					_DANGER_POINT();
				pUser->SendCreateParty(ERROR_PARTY_CREATE_FAIL, 0);
			}
			else
				_DANGER_POINT();
			return ERROR_NONE;
		}
	case MAVI_REQPARTYIDFAIL:
#endif // #if defined( PRE_PARTY_DB )
	case MAVI_INVITEPARTYMEMBER_RETMSG:
		{
			MAVIResult *pResult = (MAVIResult*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pResult->nAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			switch (pResult->nMainCmd)
			{
#if defined( PRE_PARTY_DB )
#else
				case MAVI_REQPARTYIDFAIL:
				{
					pUserObj->SendCreateParty(pResult->nRet, 0);
					break;
				}
#endif // #if defined( PRE_PARTY_DB )

				case MAVI_INVITEPARTYMEMBER_RETMSG:
				{
					pUserObj->SendPartyInviteFail(pResult->nRet);
					break;
				}
			}
			return ERROR_NONE;
		}

	case MAVI_PUSHPARTY:
		{
			MAVIPushParty * pPacket = (MAVIPushParty*)pData;

#if defined( PRE_PARTY_DB )
			g_pPartyManager->PushParty(pPacket);
#else
			if (g_pPartyManager->PushParty(pPacket) == NULL )
				_DANGER_POINT();
#endif
		}
		break;

	case MAVI_INVITEPARTYMEMBER:
		{
			MAVIInvitePartyMember * pPacket = (MAVIInvitePartyMember*)pData;

			CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nInvitedAccountDBID);
			if (pUser)
			{
#if defined(PRE_FIX_62281)
				bool bPermitCheck = true;
				for (int j = 0; j < PERMITMAPPASSCLASSMAX; j++)
				{
					if (pPacket->cPassClassIds[j] <= 0) continue;
					if (pUser->GetClassID() == pPacket->cPassClassIds[j])
					{
						bPermitCheck = false;
						break;
					}
				}

				if (bPermitCheck)
				{
					if (pPacket->cPermitLevel > pUser->GetLevel())
					{
						SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_ZONLEVEL_SHORTAGE);
						break;
					}
				}

#if defined(PRE_PARTY_DB)
				int nRet = pUser->CheckPartyInvitableState(pPacket->wszInviterName, pPacket->cLimitLevelMin);
#else
				int nRet = pUser->CheckPartyInvitableState(pPacket->wszInviterName, pPacket->cLimitLevelMin, pPacket->cLimitLevelMax);
#endif
				if(nRet != ERROR_NONE)
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, nRet);
				else
				{
					pUser->SetWindowState(WINDOW_ISACCEPT);
#if defined( PRE_PARTY_DB )
					pUser->SendPartyInviteMsg(pPacket->PartyID, pPacket->wszPartyName, -1, pPacket->wszInviterName, pPacket->cPartyMemberMax, pPacket->cPartyMemberCount, pPacket->cPartyAvrLevel, pPacket->nGameServerID, pPacket->nMapIdx);
#else
					pUser->SendPartyInviteMsg(pPacket->PartyID, pPacket->wszPartyName, NULL, pPacket->wszInviterName, pPacket->cPartyMemberMax, pPacket->cPartyMemberCount, pPacket->cPartyAvrLevel, pPacket->nGameServerID, pPacket->nMapIdx);
#endif // #if defined( PRE_PARTY_DB )
				}
			}

#else
				if (pUser->m_eUserState != STATE_NONE || !pUser->IsNoneWindowState())		//대상이 이동중이거나 레디 등등의 상태이면 초대 불가하다
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED);
					break;
				}

				bool bPermitCheck = true;
				for (int j = 0; j < PERMITMAPPASSCLASSMAX; j++)
				{
					if (pPacket->cPassClassIds[j] <= 0) continue;
					if (pUser->GetClassID() == pPacket->cPassClassIds[j])
					{
						bPermitCheck = false;
						break;
					}
				}

				if (bPermitCheck)
				{
					if (pPacket->cPermitLevel > pUser->GetLevel())
					{
						SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_ZONLEVEL_SHORTAGE);
						break;
					}
				}

				//상대방의 게임옵션을 확인합니다.
				if (pUser->IsAcceptAbleOption(pPacket->biInviterCharacterDBID, pPacket->nInviterAccountDBID, _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL);
					break;
				}

				//상대방이 나를 차단했는지 알아 봅니다
				if (pUser->GetIsolate()->IsIsolateItem(pPacket->wszInviterName))
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL);
					break;
				}

				// GM이면 없는 사람처럼 해야함
				if ((pUser->GetAccountLevel() >= AccountLevel_New) && (pUser->GetAccountLevel() <= AccountLevel_Master))
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND);
					break;
				}

				//유저가 있습니다. 파티 초대를 받을 수 있는 상황인지 체킹후에 초대 메세지를 보냅니다.
				const sChannelInfo * pChInfo = g_pMasterConnection->GetChannelInfo(pUser->GetChannelID());
				if (pChInfo == NULL)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL);
					break;
				}

				if (pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED);
					break;
				}
				
#if defined( PRE_PARTY_DB )
#else
				if (pUser->GetMapIndex() != pPacket->nVillageMapIdx)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED);
					break;
				}
#endif // #if defined( PRE_PARTY_DB )

				if (pUser->GetPartyID() > 0)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY);
					break;
				}

				if (pUser->GetLevel() < pPacket->cLimitLevelMin)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE);
					break;
				}

#if defined( PRE_PARTY_DB )
#else
				if (pUser->GetLevel() > pPacket->cLimitLevelMax)
				{
					SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER);
					break;
				}
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
				pUser->SendPartyInviteMsg(pPacket->PartyID, pPacket->wszPartyName, -1, pPacket->wszInviterName, pPacket->cPartyMemberMax, pPacket->cPartyMemberCount, pPacket->cPartyAvrLevel, pPacket->nGameServerID, pPacket->nMapIdx);
#else
				pUser->SendPartyInviteMsg(pPacket->PartyID, pPacket->wszPartyName, NULL, pPacket->wszInviterName, pPacket->cPartyMemberMax, pPacket->cPartyMemberCount, pPacket->cPartyAvrLevel, pPacket->nGameServerID, pPacket->nMapIdx);
#endif // #if defined( PRE_PARTY_DB )
			}
#endif // #if defined(PRE_FIX_62281)
			else
				SendInvitePartyResult(pPacket->nGameServerID, pPacket->wszInviterName, pPacket->wszInvitedName, ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND);
		}
		break;

	case MAVI_INVITEGUILDMEMB:
		{
			MAInviteGuildMember * pPacket = (MAInviteGuildMember*)pData;
			CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToCharacterName);

			if (pUser)
			{
#if defined(PRE_ADD_DWC)
				if(pUser->IsDWCCharacter())
				{
					SendInviteGuildResult(pPacket->nAccountDBID, ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE);
					return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;
				}
#endif
				// 상대방 캐릭터 로딩상태 확인
				if (!pUser->GetLoadUserData())
				{
					SendInviteGuildResult(pPacket->nAccountDBID, ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE);
					return ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE;
				}

				// 상대방 길드가입 여부
				if (pUser->GetGuildUID().IsSet()) 
				{
					SendInviteGuildResult(pPacket->nAccountDBID, ERROR_GUILD_ALREADYINGUILD_OPPOSITEUSER);
					return ERROR_GUILD_ALREADYINGUILD_OPPOSITEUSER;
				}

				// 자기자신에게 요청여부 
				if (pPacket->nAccountDBID == pUser->GetAccountDBID())
				{
					SendInviteGuildResult(pPacket->nAccountDBID, ERROR_GUILD_CANTINVITE_YOURSELF);
					return ERROR_GUILD_CANTINVITE_YOURSELF;
				}

				// 유저간 관계옵션 검사
				if (pUser->IsAcceptAbleOption(pPacket->nCharacterDBID, pPacket->nAccountDBID, _ACCEPTABLE_CHECKTYPE_GUILDINVITE) == false)
				{
					SendInviteGuildResult(pPacket->nAccountDBID, ERROR_GUILD_REFUESED_GUILDINVITATION);
					return ERROR_GUILD_REFUESED_GUILDINVITATION;
				}
				// 유저 상태 검사 및 변경
				if( pUser->IsNoneWindowState() )
				{
					SendInviteGuildResult(pPacket->nAccountDBID, ERROR_GUILD_CANTACCEPT_GUILDINVITATION);
					return ERROR_GUILD_CANTACCEPT_GUILDINVITATION;
				}
				pUser->SetWindowState(WINDOW_ISACCEPT);
				pUser->SendInviteGuildMemberReq(pPacket->GuildUID, pPacket->nAccountDBID, pPacket->nSessionID, pPacket->wszFromCharacterName, 
					ERROR_NONE, pUser->GetAccountDBID(), pUser->GetSessionID(), pPacket->wszGuildName);
				return ERROR_NONE;
				
			}
			else
			{
				// 마스터서버에 해당유저가 존재하지 않음을 알려준다.
				SendInviteGuildResult(pPacket->nAccountDBID, ERROR_GUILD_NOTEXIST_OPPOSITEUSER);
			}
			
		}
		break;


	case MAVI_INVITEGUILDMEMBER_RETMSG:
		{
			MAVIGuildMemberInvitedResult * pPacket = (MAVIGuildMemberInvitedResult*)pData;
			CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
			if (!pUser)
				break;

			// CS_INVITEGUILDMEMBACK의 결과값
			if (pPacket->bAck)
			{
				pUser->SendInviteGuildMemberAck(	NULL, NULL, pPacket->wszInvitedName,
												ERROR_GUILD_REFUESED_GUILDINVITATION, NULL, NULL,
												0, 0, NULL, NULL);
			}
			else
			{
				// CS_INVITEGUILDMEMBREQ의 결과값
				pUser->SendInviteGuildMemberReq(pUser->GetGuildUID(), pUser->GetAccountDBID(), pUser->GetSessionID(), pPacket->wszInvitedName, pPacket->nRet);
			}
			
		}	
		break;

	case MAVI_GUILDWAREINFO:			// 길드창고 정보
		{
			MAGuildWareInfo * pPacket = (MAGuildWareInfo*)pData;

			if (pPacket->nManagedID == g_Config.nManagedID)
				break;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			//if (FALSE == pGuild->IsEnable()) break;
#endif


			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = NULL;
				pGuildWare = pGuildVillage->GetGuildWare ();

			
				if (pGuildWare)
					pGuildWare->OnRecvGuildWareInfo (pPacket);
			}

		}
		break;

	case MAVI_RETGUILDWAREINFO:			// 길드창고 정보 결과
		{
			MAGuildWareInfoResult * pPacket = (MAGuildWareInfoResult*)pData;

			// 최초 전송한 서버였는지 확인한다.
			if (pPacket->nFromManagedID != g_Config.nManagedID)
				break;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = NULL;
				pGuildWare = pGuildVillage->GetGuildWare ();


				if (pGuildWare)
					pGuildWare->OnRecvGuildWareInfoResult (pPacket);
			}

		}
		break;

	case MAVI_GUILDMEMBER_LEVELUP:			// 길드원 레벨업 정보
		{
			MAGuildMemberLevelUp * pPacket = (MAGuildMemberLevelUp*)pData;

			if (pPacket->nManagedID == g_Config.nManagedID)
				break;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			pGuild->UpdateMemberLevel (pPacket->nCharacterDBID, pPacket->cLevel);

		}
		break;

	case MAVI_REFRESH_GUILDITEM:			// 길드아이템 업데이트
		{
			
			MARefreshGuildItem * pPacket = (MARefreshGuildItem*)pData;

			if (pPacket->nManagedID == g_Config.nManagedID)
				break;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
				pGuildVillage->RecvMaRefreshGuildItem (pPacket);
			
		}
		break;

	case MAVI_REFRESH_GUILDCOIN:			// 길드코인 업데이트
		{

			MARefreshGuildCoin * pPacket = (MARefreshGuildCoin*)pData;

			if (pPacket->nManagedID == g_Config.nManagedID)
				break;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
				pGuildVillage->RecvMaRefreshGuildCoin (pPacket);

		}
		break;

	case MAVI_EXTEND_GUILDWARE:				// 길드창고 사이즈 확장
		{

			MAExtendGuildWare * pPacket = (MAExtendGuildWare*)pData;

			if (pPacket->nManagedID == g_Config.nManagedID)
				break;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			pGuild->SetInfo()->wGuildWareSize = pPacket->wWareSize;

			pGuild->SendExtendGuildWare((short)pPacket->wWareSize);

		}
		break;

	case MAVI_DISMISSGUILD:				// 길드 해체 알림
		{
			g_pGuildManager->OnRecvMaDismissGuild(reinterpret_cast<MADismissGuild*>(pData));
		}
		break;

	case MAVI_ADDGUILDMEMB:				// 길드원 추가 알림
		{
			g_pGuildManager->OnRecvMaAddGuildMember(reinterpret_cast<MAAddGuildMember*>(pData));
		}
		break;

	case MAVI_DELGUILDMEMB:				// 길드원 제거 (탈퇴/추방) 알림
		{
			g_pGuildManager->OnRecvMaDelGuildMember(reinterpret_cast<MADelGuildMember*>(pData));
		}
		break;

	case MAVI_CHANGEGUILDINFO:			// 길드 정보 변경 알림
		{
			g_pGuildManager->OnRecvMaChangeGuildInfo(reinterpret_cast<MAChangeGuildInfo*>(pData));
		}
		break;

	case MAVI_CHANGEGUILDMEMBINFO:		// 길드원 정보 변경 알림
		{
			g_pGuildManager->OnRecvMaChangeGuildMemberInfo(reinterpret_cast<MAChangeGuildMemberInfo*>(pData));
		}
		break;

	case MAVI_GUILDCHAT:				// 길드 채팅 알림
		{
			g_pGuildManager->OnRecvMaGuildChat(reinterpret_cast<MAGuildChat*>(pData));
		}
		break;
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	case MAVI_GUILDCHAT_MOBILE:
		{
			g_pGuildManager->OnRecvMaDoorsGuildChat(reinterpret_cast<MADoorsGuildChat*>(pData));
		}
		break;
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	// 길드명 변경
	case MAVI_GUILDCHANGE_NAME:
		{
			g_pGuildManager->OnRecvMaGuildChangeName(reinterpret_cast<MAGuildChangeName*>(pData));
			g_pGuildWarManager->OnRecvMaGuildChangeName(reinterpret_cast<MAGuildChangeName*>(pData));
		}
		break;

	// 길드마크 변경
	case MAVI_GUILDCHANGEMARK:
		{
			g_pGuildManager->OnRecvMaGuildChangeMark(reinterpret_cast<MAGuildChangeMark*>(pData));
			g_pGuildWarManager->OnRecvMaGuildChangeMark(reinterpret_cast<MAGuildChangeMark*>(pData));
		}
		break;

	case MAVI_UPDATEGUILDEXP:
		{
			g_pGuildManager->OnRecvMaGuildUpdateExp(reinterpret_cast<MAUpdateGuildExp*>(pData));
		}
		break;

		// 길드전 신청 알림
	case MAVI_ENROLL_GUILDWAR:
		{
			g_pGuildManager->OnRecvMaEnrollGuildWar(reinterpret_cast<MAEnrollGuildWar*>(pData));
		}
		break;
	case MAVI_CHANGE_GUILDWAR_STEP :
		{
			MAChangeGuildWarStep * pPacket = (MAChangeGuildWarStep*)pData;

			if (pPacket->bCheatFlag)
				g_pGuildWarManager->SetStepIndex(GUILDWAR_STEP_NONE);

			bool bChangeEvent = false;
			// 스케쥴ID가 같으면 업데이트
			if (g_pGuildWarManager->GetScheduleID() == pPacket->wScheduleID)
			{
				if (pPacket->cEventType == GUILDWAR_EVENT_START && g_pGuildWarManager->GetStepIndex() < pPacket->cEventStep)
					bChangeEvent = true;
				else if (pPacket->cEventType == GUILDWAR_EVENT_END && g_pGuildWarManager->GetStepIndex() == pPacket->cEventStep)
					bChangeEvent = true;
			}
			else // 스케쥴ID가 다르면 새로운 등록
				bChangeEvent = true;


			if (bChangeEvent)
			{
				g_pGuildWarManager->SetGuildWarStep(pPacket->cEventStep, pPacket->wScheduleID, pPacket->wWinersWeightRate);
				g_pUserSessionManager->SendGuildWarEvent(pPacket->wScheduleID, pPacket->cEventStep, pPacket->cEventType);
			}
			// 마지막이면 초기화
			if( pPacket->cEventStep == GUILDWAR_STEP_REWARD && pPacket->cEventType == GUILDWAR_EVENT_END )
				g_pGuildWarManager->Reset();
		}
		break;
	case MAVI_GETGUILDWAR_SCHEDULE : // 스케쥴 정보 요청
		{
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryGetGuildWarSchedule(rand()%THREADMAX, g_Config.nWorldSetID);
			}
		}		
		break;
	case MAVI_GETGUILDWAR_FINALSCHEDULE : // 길드전 본선 스케줄 조회 요청
		{
			MAGetGuildWarFinalSchedule * pPacket = (MAGetGuildWarFinalSchedule*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryGetGuildWarFinalSchedule(rand()%THREADMAX, g_Config.nWorldSetID, pPacket->wScheduleID);
			}
		}
		break;
	case MAVI_SETGUILDWAR_FINALPROCESS : // 길드전 본선 진행 현황
		{
			MASetGuildWarFinalProcess* pPacket = (MASetGuildWarFinalProcess*)pData;
			if( g_pGuildWarManager )
			{
				g_pGuildWarManager->SetGuildWarFinalPart(pPacket->cGuildFinalPart, pPacket->tBeginTime);

				if( pPacket->cGuildFinalPart != GUILDWAR_FINALPART_NONE && g_pGuildWarManager->GetPreWinGuildUID().IsSet() ) // 우승길드 셋팅되어 있으면 초기화
				{					
					CDNGuildBase* pGuild = g_pGuildManager->At (g_pGuildWarManager->GetPreWinGuildUID());
					g_pGuildWarManager->ResetPreWinGuildUID();
					if (!pGuild)						
						break;
#if !defined( PRE_ADD_NODELETEGUILD )
					CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
					if (FALSE == pGuild->IsEnable()) break;
#endif
					CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
					pGuildVillage->SendGuildWarPreWinGuild(false);
				}
			}
		}
		break;
	case MAVI_GETGUILDWAR_POINT :			// 길드전 포인트 조회 요청(청팀, 홍팀)
		{
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryGetGuildWarPoint(rand()%THREADMAX, g_Config.nWorldSetID, 0, 'T', 0);
			}
		}
		break;
	case MAVI_SETGUILDWAR_POINT :			// 길드전 포인트 셋팅.
		{
			MASetGuildWarPoint * pPacket = (MASetGuildWarPoint*)pData;
			if( g_pGuildWarManager )			
				g_pGuildWarManager->SetTeamPoint(pPacket->nBlueTeamPoint, pPacket->nRedTeamPoint);
		}
		break;
	case MAVI_ADDGUILDWAR_POINT :			// 길드전 포인트 획득
		{			
			MAAddGuildWarPoint * pPacket = (MAAddGuildWarPoint*)pData;
			if( g_pGuildWarManager )
			{
				if( pPacket->cTeamType == GUILDWAR_TEAM_BLUE )
					g_pGuildWarManager->AddBlueTeamPoint(pPacket->nAddPoint);
				else if( pPacket->cTeamType == GUILDWAR_TEAM_RED)
					g_pGuildWarManager->AddRedTeamPoint(pPacket->nAddPoint);

				//길드에 적립
				CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
				if (!pGuild)
					break;

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
				if (FALSE == pGuild->IsEnable()) break;
#endif

				CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
				if( pGuildVillage->GetGuildWarScheduleID() != g_pGuildWarManager->GetScheduleID() )
				{
					pGuildVillage->GuildWarReset();
					pGuildVillage->SetGuildWarScheduleID(g_pGuildWarManager->GetScheduleID());
				}
				pGuildVillage->AddGuildWarPoint(pPacket->nAddPoint);
			}
		}
		break;
	case MAVI_GETGUILDWAR_FINAL_TEAM :	// 본선 진출 길드 정보(본선 시작전)
		{
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
				{
					pDBCon->QueryGetGuildWarPointGuildTotal(rand()%THREADMAX, g_Config.nWorldSetID, 0, true, 'A', g_pGuildWarManager->GetScheduleID(), 0 );
					g_Log.Log(LogType::_GUILDWAR,g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] QueryGetGuildWarPointGuildTotal ScheduleID:%d !!\r\n", g_pGuildWarManager->GetScheduleID() );
				}
				else
				{
					g_Log.Log(LogType::_GUILDWAR,g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] QueryGetGuildWarPointGuildTotal Not DB Connection!!\r\n" );
				}
			}
		}
		break;
	case MAVI_SETGUILDWAR_FINAL_TEAM :	// 본선 진출 길드 정보 저장(대진표 저장)	
		{
			MAVISetGuildWarFinalTeam * pPacket = (MAVISetGuildWarFinalTeam*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryAddGuildWarFinalMatchList(rand()%THREADMAX, g_Config.nWorldSetID, g_pGuildWarManager->GetScheduleID(), pPacket->GuildDBID);
			}
		}
		break;
	case MAVI_GETGUILDWAR_TOURNAMENTINFO :
		{
			MAGetGuildWarTournamentInfo* pPacket = (MAGetGuildWarTournamentInfo*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryGetGuildWarFinalResults(rand()%THREADMAX, g_Config.nWorldSetID, pPacket->wScheduleID);
			}
		}
		break;
	case MAVI_SETGUILDWAR_TOURNAMENTINFO :
		{
			MAGuildWarTournamentInfo* pPacket = (MAGuildWarTournamentInfo*)pData;
			if( g_pGuildWarManager )			
				g_pGuildWarManager->SetGuildTournamentInfo(pPacket->sGuildWarFinalInfo);
		}
		break;
	case MAVI_GETGUILDWAR_TRIAL_STATS :	// 예선 통계 정보
		{
			// 예선 통계 정보 가져오기..
			// 순서, QueryGetGuildWarPointPartTotal, QueryGetGuildWarPointGuildPartTotal, QueryGetGuildWarPointDaily, QueryGetGuildWarPointGuildTotal
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if( pDBCon )
				{
					pDBCon->QueryGetGuildWarPointPartTotal(cThreadID, g_Config.nWorldSetID, 0, 'A', 0);
					g_Log.Log(LogType::_GUILDWAR,g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] MAVI_GETGUILDWAR_TRIAL_STATS !!\r\n");
				}
			}
		}
		break;

	case MAVI_SETGUILDWAR_SECRETMISSION:	// 길드전 시크릿 미션
		{
			MASetGuildWarSecretMission* pPacket = (MASetGuildWarSecretMission*)pData;
			if( g_pGuildWarManager )			
				g_pGuildWarManager->SetGuildSecretMission(pPacket);
		}
		break;
	case MAVI_SETGUILDWAR_FINAL_RESULT :
		{
			MAVISetGuildWarFinalResult* pPacket = (MAVISetGuildWarFinalResult*)pData;
			if( g_pGuildWarManager )
				g_pGuildWarManager->SetGuildTournamentResult(pPacket);
		}
		break;
	case MAVI_SETGUILDWAR_FINAL_RESULT_DB :
		{
			MAVISetGuildWarFinalDB* pPacket = (MAVISetGuildWarFinalDB*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if( pDBCon )
				{
					pDBCon->QueryAddGuildWarFinalResults(cThreadID, g_Config.nWorldSetID, pPacket->GuildDBID,
						pPacket->wScheduleID, pPacket->cMatchSequence, pPacket->cMatchTypeCode, pPacket->bWin ? 1:2 );
				}
			}			
		}
		break;
	case MAVI_GETGUILDWAR_PREWIN_GUILD :
		{
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if (pDBCon)
					pDBCon->QueryGetGuildWarPreWinGuild(rand()%THREADMAX, g_Config.nWorldSetID);
			}
		}
		break;
	case MAVI_SETGUILDWAR_PREWIN_GUILD :
		{
			MAGuildWarPreWinGuild* pPacket = (MAGuildWarPreWinGuild*)pData;
			if( g_pGuildWarManager )
			{
				if( pPacket->bPreWin )				
					g_pGuildWarManager->SetPreWinGuildUID(pPacket->GuildUID);
				else
					g_pGuildWarManager->ResetPreWinGuildUID();

				g_Log.Log(LogType::_GUILDWAR, L"[GUILDWAR] VillageServer Set PreWinGuildReward! GuildID:%d\n", pPacket->GuildUID.nDBID);
				
				CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
				if (!pGuild)
					break;

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
				if (FALSE == pGuild->IsEnable()) break;
#endif
				CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
				pGuildVillage->SendGuildWarPreWinGuild(pPacket->bPreWin);
			}
		}
		break;
	case MAVI_SETGUILDWAR_PREWIN_SKILLCOOLTIME :
		{
			MAGuildWarPreWinSkillCoolTime* pPacket = (MAGuildWarPreWinSkillCoolTime*)pData;
			if( g_pGuildWarManager )
				g_pGuildWarManager->SetPreWinSKillCoolTime(pPacket->dwSkillCoolTime);
		}
		break;
	case MAVI_SETGUILDWAR_EVENT_TIME :
		{
			MASetGuildWarEventTime* pPacket = (MASetGuildWarEventTime*)pData;
			if( g_pGuildWarManager )
			{
				g_pGuildWarManager->SetGuildWarSechdule(pPacket);
				g_pUserSessionManager->SendGuildWarEventTime();
			}
		}
		break;
	case MAVI_SETGUILDWAR_TOURNAMENTWIN :
		{
			MASetGuildWarTournamentWin* pPacket = (MASetGuildWarTournamentWin*)pData;
			if( g_pUserSessionManager )
			{
				// 전체유저에게 보내는 거기때문에 Copy줄이자.
				SCGuildWarTournamentWin GuildWarTournamentWin;
				memset(&GuildWarTournamentWin, 0, sizeof(GuildWarTournamentWin));
				GuildWarTournamentWin.cMatchTypeCode = pPacket->cMatchTypeCode;
				memcpy(GuildWarTournamentWin.wszGuildName, pPacket->wszGuildName, sizeof(GuildWarTournamentWin.wszGuildName));
				g_pUserSessionManager->SendGuildWarTournamentWin(&GuildWarTournamentWin);
			}
		}
		break;
	case MAVI_ADD_DBJOBSYSTEM_RESERVE :
		{
			MAVIAddDBJobSystemReserve* pPacket = (MAVIAddDBJobSystemReserve*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if( pDBCon )				
					pDBCon->QueryAddDBJobSystemReserve(cThreadID, g_Config.nWorldSetID, pPacket->cJobType);
			}
		}
		break;
	case MAVI_GET_DBJOBSYSTEM_RESERVE :
		{
			MAVIGetDBJobSystemReserve* pPacket = (MAVIGetDBJobSystemReserve*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if( pDBCon )				
					pDBCon->QueryGetDBJobSystemReserve(cThreadID, g_Config.nWorldSetID, pPacket->nJobSeq);
			}
		}
		break;
	case MAVI_GETGUILDWAR_POINT_RUNNING :
		{
			if( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if( pDBCon )				
					pDBCon->QueryGetGuildWarPointRunningTotal(cThreadID, g_Config.nWorldSetID);
			}
		}
		break;
	case MAVI_SETGUILDWAR_POINT_RUNNING :
		{
			MASetGuildWarPointRunningTotal* pPacket = (MASetGuildWarPointRunningTotal*)pData;
			if( g_pGuildWarManager )
			{
				g_pGuildWarManager->SetGuildWarTrialRankingInfo(pPacket);
			}
		}
		break;
	case MAVI_GUILDWAR_REFRESH_GUILD_POINT :
		{
			MAGuildWarRefreshGuildPoint* pPacket = (MAGuildWarRefreshGuildPoint*)pData;
			MAChangeGuildInfo GuildInfo;
			memset(&GuildInfo, 0, sizeof(MAChangeGuildInfo));

			for (int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i )
			{
				if( !pPacket->GuildUID[i].IsSet() )
					break;

				//길드에 갱신
				CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID[i]);
				if (!pGuild)
					continue;

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
				if (FALSE == pGuild->IsEnable()) break;
#endif
				if( g_pGuildManager )
				{
					GuildInfo.Int1 = pPacket->nGuildPoint[i];
					GuildInfo.Int2 = pPacket->nGuildPoint[i];
					g_pGuildManager->MaChangePoint(pGuild, &GuildInfo);
				}
			}
		}
		break;
	case MAVI_GETGUILDWAR_PREWIN_REWARD :
		{
			MAVIGetGuildWarPrewinReward* pPacket = (MAVIGetGuildWarPrewinReward*)pData;
			if ( g_pDBConnectionManager )
			{
				BYTE cThreadID;
				CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
				if( pDBCon )				
					pDBCon->QueryGetGuildWarRewarForGuild(cThreadID, g_Config.nWorldSetID, pPacket->wScheduleID, pPacket->nGuildDBID, 'M');
			}
		}
		break;
	case MAVI_GUILDWAR_RESET_BUYED_ITEM_COUNT:
		{
			g_pUserSessionManager->ResetGuildWarBuyedItems();
		}
		break;

	case MAVI_GUILDRECRUIT_MEMBER:
		{
			MAGuildRecruitMember* pPacket = (MAGuildRecruitMember*)pData;
			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiAccountDBID);
			if (!pUserObj) return ERROR_NONE;
			
			if( pPacket->cAcceptType == GuildRecruitSystem::AcceptType::AcceptOn )
			{
				// 유저가 있으면 유저 길드정보 셋팅
				pUserObj->SetGuildSelfView(TGuildSelfView(pPacket->GuildView, GUILDROLE_TYPE_JUNIOR));
				// 길드 정보가 서버에 존재하는지 체크 후 없으면 길드 정보 불러오기
				CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
				if (!pGuild)
				{
					if (pUserObj->GetDBConnection() && pUserObj->GetDBConnection()->GetActive())
					{
						pUserObj->GetDBConnection()->QueryGetGuildInfo(pUserObj, pPacket->GuildUID.nDBID, true);
						// 길드 적용 효과 얻어오기						
						pUserObj->GetDBConnection()->QueryGetGuildRewardItem( pUserObj->GetDBThreadID(), pUserObj->GetWorldSetID(), pUserObj->GetAccountDBID(), pPacket->GuildUID.nDBID );
					}
					break;
				}
#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
				if (FALSE == pGuild->IsEnable()) break;
#endif
				TGuildRewardItem *RewardItemInfo;
				RewardItemInfo = pGuild->GetGuildRewardItem();
				pUserObj->SendGuildRewardItem(RewardItemInfo);
				pUserObj->GetEventSystem()->OnEvent( EventSystem::OnGuildJoin );
			}
			pUserObj->SendGuildRecruitMemberResult( ERROR_NONE, pPacket->GuildView.wszGuildName, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
		}
		break;
	case MAVI_GUILD_ADD_REWARDITEM:
		{
			MAGuildRewardItem* pPacket = (MAGuildRewardItem*)pData;
			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (!pGuild)
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif
			if( g_pGuildManager )
			{
				TGuildRewardItem RewardItemInfo;
				memset( &RewardItemInfo, 0, sizeof(RewardItemInfo) );
				pGuild->AddGuildRewardItem(pPacket->nItemID, pPacket->m_tExpireDate, RewardItemInfo);
				pGuild->SendAddGuildRewardItem( RewardItemInfo );
			}
		}
		break;
	case MAVI_GUILD_EXTEND_GUILDSIZE:
		{
			MAExtendGuildSize *pPacket = (MAExtendGuildSize*)pData;
			
			const TGuildUID GuildUID = pPacket->GuildUID;

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif
			
			pGuild->SetInfo()->wGuildSize = pPacket->nGuildSize;
			
			// 길드원에게 알려준다
			pGuild->SendExtendGuildSize((short)pPacket->nGuildSize);
		}
		break;
	case MAVI_UPDATEGUILDWARE:
		{
			MAUpdateGuildWare * pPacket = (MAUpdateGuildWare*)pData;

			const TGuildUID GuildUID = pPacket->GUildID;
			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			pGuild->ReqGetGuildWareInfo(true);
		}
		break;


	case MAVI_FRIENDADDNOTICE:
		{
			MAVIFriendAddNotice * pPacket = (MAVIFriendAddNotice*)pData;

			CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAddedAccountDBID);
			if (pUser)
				pUser->SendFriendAddNotice(pPacket->wszAddName);
		}
		break;

	case MAVI_PRIVATECHAT:
		{
			MAPrivateChat *pChat = (MAPrivateChat*)pData;
			WCHAR wszChatMsg[CHATLENMAX] = { 0, };

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pChat->nAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			//채팅 받았다
			if (pChat->nRet == ERROR_NONE)
			{
				//에러라면 이하 검사하지 않는다.
				if (pUserObj->GetIsolate()->IsIsolateItem(pChat->wszFromCharacterName))
					return ERROR_NONE;

				// GM이면 없는 사람처럼 해야함
				if ((pUserObj->GetAccountLevel() >= AccountLevel_New) && (pUserObj->GetAccountLevel() <= AccountLevel_QA)){
					if (!pUserObj->GetGMCommand()->IsWhisperName(pChat->wszFromCharacterName)){
						return ERROR_NONE;
					}
				}

				_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->wChatLen);
			}

			pUserObj->SendChat(pChat->cType, pChat->wChatLen, pChat->wszFromCharacterName, wszChatMsg, NULL, pChat->nRet);
			return ERROR_NONE;
		}
		break;

	case MAVI_CHAT:
		{
			MAChat *pChat = (MAChat*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->wChatLen);

			//여기서 채트 타입에 따라 변신해야겠다아~
#if defined(PRE_ADD_WORLD_MSG_RED)
			if (pChat->cType == CHATTYPE_WORLD || pChat->cType == CHATTYPE_WORLD_POPMSG)
#else	// #if defined(PRE_ADD_WORLD_MSG_RED)
			if (pChat->cType == CHATTYPE_WORLD)
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
			{
				//붙어 있는 유저에게 다 쏴주신다.
				g_pUserSessionManager->SendChat(static_cast<eChatType>(pChat->cType), pChat->wChatLen, pChat->wszFromCharacterName, wszChatMsg);
			}
			else
			{
				CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pChat->nAccountDBID);
				if (!pUserObj) return ERROR_NONE;

				pUserObj->SendChat(static_cast<eChatType>(pChat->cType), pChat->wChatLen, pChat->wszFromCharacterName, wszChatMsg);
			}
			return ERROR_NONE;
		}

	case MAVI_ZONECHAT:
		{
			MAZoneChat * pPacket = (MAZoneChat*)pData;

			WCHAR wszChatMsg[CHATLENMAX];
			memset(wszChatMsg, 0, sizeof(wszChatMsg));
			_wcscpy(wszChatMsg, CHATLENMAX, pPacket->wszChatMsg, pPacket->wChatLen);
			
			g_pUserSessionManager->SendZoneChat(pPacket->nMapIdx, pPacket->wszFromCharacterName, wszChatMsg, pPacket->wChatLen);

			return ERROR_NONE;
		}
		break;

	case MAVI_WORLDSYSTEMMSG:
		{
			MAWorldSystemMsg *pMsg = (MAWorldSystemMsg*)pData;
			g_pUserSessionManager->SendWorldSystemMsg(pMsg->wszFromCharacterName, pMsg->cType, pMsg->nID, pMsg->nValue, pMsg->wszToCharacterName);
		}
		break;
#if defined( PRE_PRIVATECHAT_CHANNEL )
	case MAVI_PRIVATECHNNELCHAT:
		{
			MAPrivateChannelChat *pMsg = (MAPrivateChannelChat*)pData;
			if(g_pPrivateChatChannelManager)
			{
				CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo(pMsg->cWorldSetID, pMsg->nChannelID);
				if(pPrivateChatChannel)
				{	
					std::list<TPrivateChatChannelMember> MemberList;
					MemberList.clear();
					pPrivateChatChannel->GetPrivateChannelMember(MemberList);

					CDNUserSession* pSession = NULL;

					for(std::list<TPrivateChatChannelMember>::iterator itor = MemberList.begin(); itor != MemberList.end();itor++)
					{
						pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( itor->nAccountDBID );
						if(pSession)
						{
							pSession->SendChat( CHATTYPE_PRIVATECHANNEL, pMsg->wChatLen, pMsg->wszFromCharacterName, pMsg->wszChatMsg );
						}
					}					
				}
			}
			
		}
		break;
#endif

		// PvP
	case MAVI_PVP_CREATEROOM:
		{
			if( !g_pUserSessionManager )
				return ERROR_NONE;
			
			MAVIPVP_CREATEROOM* pPacket		= reinterpret_cast<MAVIPVP_CREATEROOM*>(pData);
			CDNUserSession*		pUserObj	= g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiCreateAccountDBID );

			if( pPacket->nRetCode != ERROR_NONE )
			{
				if( pUserObj )
					pUserObj->SendPvPCreateRoom( pPacket->nRetCode );
			}
			else
			{				
				// ToDo:MaterServer에서는PvP룸을 생성하였는데 Village에서는 실패하면 예외처리 해줘야한다.(정상적인 경우에는 발생하면 안되므로 해당 Connection 끊어줘도 무방할듯)
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				if (pPacket->sCSPVP_CREATEROOM.cRoomType >= PvPCommon::RoomType::max)
				{
					_DANGER_POINT();
					break;
				}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				CDNPvPRoomManager::GetInstance().CreatePvPRoom( pUserObj, pPacket );
			}
			break;
		}
	case MAVI_PVP_MODIFYROOM:
		{
			MAVIPVP_MODIFYROOM* pPacket = reinterpret_cast<MAVIPVP_MODIFYROOM*>(pData);
			CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
			if( pPvPRoom == NULL )
				break;
			
			if( pPacket->nRetCode == ERROR_NONE )
			{
				pPvPRoom->ModifyRoom( pPacket );
			}
			else
			{
				// Event방은 수정/편집 할 수 없다.
				if( pPvPRoom->GetEventRoomIndex() > 0)
					break;

				if( g_pUserSessionManager )
				{
					CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
					if( pUserObj )
						pUserObj->SendPvPModifyRoom( pPacket->nRetCode );
				}
			}
			break;
		}
	case MAVI_PVP_DESTROYROOM:
		{
			MAVIPVP_DESTROYROOM* pPacket = reinterpret_cast<MAVIPVP_DESTROYROOM*>(pData);
			if( CDNPvPRoomManager::GetInstance().bDestroyPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex ) )
			{
#if 0
				if( g_pFieldManager )
				{
					WCHAR wszBuffer[MAX_PATH];
					wsprintf( wszBuffer, L"%d번 PvP방이 파괴되었습니다.", pPacket->uiPvPIndex );
					g_pFieldManager->SendDebugChat(pPacket->cVillageChannelID, wszBuffer);
				}
#endif
			}
			break;
		}

	case MAVI_PVP_LEAVEROOM:
		{
			MAVIPVP_LEAVEROOM* pPacket = reinterpret_cast<MAVIPVP_LEAVEROOM*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
				if( pPvPRoom )
					pPvPRoom->LeaveUser( pPacket->uiLeaveAccountDBID, pPacket->eType );
				else
					_DANGER_POINT();
			}
			else
			{
				if( g_pUserSessionManager )
				{
					CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiLeaveAccountDBID );
					if( pUserObj )
						pUserObj->SendPvPLeaveRoom( pPacket->nRetCode, pPacket->eType, 0 );
				}
				else
					_DANGER_POINT();
			}
			break;
		}

	case MAVI_PVP_ROOMLIST:
		{
			MAVIPVP_ROOMLIST* pPacket = reinterpret_cast<MAVIPVP_ROOMLIST*>(pData);
			if( g_pUserSessionManager )
			{
				CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
#if defined(PRE_ADD_DWC)				
				if(pUserObj && pUserObj->IsDWCCharacter() && pPacket->sCSPVP_ROOMLIST.cChannelType == 0 )	//처음 로비 접속시 전송되는 패킷
					pPacket->sCSPVP_ROOMLIST.cChannelType = PvPCommon::RoomType::dwc;
#endif
				if( pUserObj )
					CDNPvPRoomManager::GetInstance().SendPvPRoomList( pUserObj, pPacket );
			}
			else
				_DANGER_POINT();
			break;
		}
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	case MAVI_PVP_ROOMLIST_RELAY :
		{
			MAVIPVP_ROOMLIST_RELAY* pPacket = reinterpret_cast<MAVIPVP_ROOMLIST_RELAY*>(pData);
			CDNPvPRoomManager::GetInstance().SendPvPRoomList(NULL, &pPacket->sRoomList, pPacket->cVillageID, pPacket->uiAccountDBID);
		}
		break;
	case MAVI_PVP_ROOMLIST_RELAY_ACK :
		{
			PVP_ROOMLIST_RELAY* pPacket = reinterpret_cast<PVP_ROOMLIST_RELAY*>(pData);
			if( g_pUserSessionManager )
			{
				CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
				if( pUserObj )				
					pUserObj->SendPvPRoomList(&pPacket->sSCPVP_ROOMLIST, pPacket->nSize);
			}
		}
		break;
#endif

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	case MAVI_PVP_CHANGECHANNEL:
		{
			MAVIPVP_CHANGECHANNEL * pPacket = (MAVIPVP_CHANGECHANNEL*)pData;
			if (g_pUserSessionManager)
			{
				CDNUserSession * pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
				if (pSession)
				{
					if (pPacket->nRetCode == ERROR_NONE)
						pSession->SetPvPChannelType(pPacket->cType);
					pSession->SendPvPChangeChannelResult(pPacket->cType, pPacket->nRetCode);
				}
				else
					_DANGER_POINT();
			}
		}
		break;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	case MAVI_PVP_WAITUSERLIST:
		{
			MAVIPVP_WAITUSERLIST* pPacket = reinterpret_cast<MAVIPVP_WAITUSERLIST*>(pData);
			if( g_pUserSessionManager )
			{
				CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
				if( pUserSession )
					CDNPvPRoomManager::GetInstance().SendPvPWaitUserList( pUserSession, pPacket );
			}
			break;
		}

	case MAVI_PVP_JOINROOM:
		{
			MAVIPVP_JOINROOM* pPacket = reinterpret_cast<MAVIPVP_JOINROOM*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
				CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
				if( pUserObj )
				{
					if( pUserObj->GetField() && !pUserObj->GetField()->bIsPvPLobby()) // 요건 마을에서 방리스트 보고 들간거..					
					{
						const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
						if( pChannelInfo )												
							pUserObj->CmdSelectChannel( pChannelInfo->nChannelID, true );
						break;
					}
					else
					{
						// 이게 아니면 인벤체크를 여기서 한다.
						if (pUserObj->_CheckPvPGameModeInventory(pPacket->uiPvPIndex) == false)
						{
							if( g_pMasterConnection && g_pMasterConnection->GetActive() )
							{
								// 조건 안맞으면 나가라..
								g_pMasterConnection->SendPvPLeaveRoom( pUserObj->GetAccountDBID() );
								break;
							}
						}
#if defined( PRE_PVP_GAMBLEROOM )				
						CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
						if( pPvPRoom )
						{
							int nRetCode = pPvPRoom->CheckGambleRoomJoin(pUserObj);
							if( nRetCode != ERROR_NONE )
							{
								pUserObj->SendPvPJoinRoom( nRetCode );
								if( g_pMasterConnection && g_pMasterConnection->GetActive() )
								{
									// 조건 안맞으면 나가라..
									g_pMasterConnection->SendPvPLeaveRoom( pUserObj->GetAccountDBID() );
									break;
								}
								break;
							}							
						}
#endif
					}
				}
#endif // 
#if defined( PRE_WORLDCOMBINE_PVP )
				if( pPacket->uiPvPIndex > PvPCommon::Common::MaxRoomCount && pUserObj )
				{
					TWorldPvPRoomDBData* pWorldPvPRoomData = CDNPvPRoomManager::GetInstance().GetWorldPvPRoom( pPacket->uiPvPIndex );
					int nWorldPvPRoomChannelID = CDNPvPRoomManager::GetInstance().GetWorldPvPRoomChannelID();
					if( pWorldPvPRoomData )
					{
						CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( nWorldPvPRoomChannelID, pPacket->uiPvPIndex );
						if( g_pMasterConnection && pPvPRoom )
						{
							if( pUserObj->GetField() && !pUserObj->GetField()->bIsPvPLobby())
							{
								const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
								if( pChannelInfo )												
									pUserObj->CmdSelectChannel( pChannelInfo->nChannelID, true );
								break;
							}
							pUserObj->SetPvPIndex(pPacket->uiPvPIndex);
							pUserObj->SendPvPJoinRoom(ERROR_NONE, pPvPRoom);
							g_pMasterConnection->SendWorldPvPJoinRoom( pUserObj->GetAccountDBID(), pUserObj->GetCharacterDBID(), pPacket->uiPvPIndex, pWorldPvPRoomData, pPacket->unPvPTeam, true );
						}
					}
					break;
				}
#endif
				CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
				if( pPvPRoom )
					pPvPRoom->JoinUser( pPacket );
				else
					_DANGER_POINT();
			}
			else
			{
				if( g_pUserSessionManager )
				{
					CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
					if( pUserObj )
					{
						pUserObj->SendPvPJoinRoom( pPacket->nRetCode );
						pUserObj->SetPvPRoomListRefreshTime( 0 );	// 리프레쉬타임 초기화
					}
				}
				else
					_DANGER_POINT();
			}
			break;
		}


	case MAVI_PVP_ROOMSTATE:
		{
			MAVIPVP_ROOMSTATE* pPacket = reinterpret_cast<MAVIPVP_ROOMSTATE*>(pData);
			CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
			if( pPvPRoom )
				pPvPRoom->SetRoomState( pPacket->uiRoomState );
			else
				_DANGER_POINT();
			break;
		}
	case MAVI_PVP_CHANGETEAM:
		{
			MAVIPVP_CHANGETEAM* pPacket = reinterpret_cast<MAVIPVP_CHANGETEAM*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
				if( pPvPRoom )
					pPvPRoom->ChangeTeam( pPacket );
				else
					_DANGER_POINT();
			}
			else
			{
				if( g_pUserSessionManager )
				{
					CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->sVIMAPVP_CHANGETEAM.uiAccountDBID );
					if( pUserObj )
						pUserObj->SendPvPChangeTeam( pPacket->nRetCode );
				}
				else
					_DANGER_POINT();
			}
			break;
		}
	case MAVI_PVP_CHANGEUSERSTATE:
		{
			MAVIPVP_CHANGEUSERSTATE* pPacket = reinterpret_cast<MAVIPVP_CHANGEUSERSTATE*>(pData);
			CDNPvPRoom* pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
			if( pPvPRoom )
				pPvPRoom->ChangeUserState( pPacket );
			else
				_DANGER_POINT();
			break;
		}
	case MAVI_PVP_START:
		{
			MAVIPVP_START*	pPacket		= reinterpret_cast<MAVIPVP_START*>(pData);
			CDNPvPRoom*		pPvPRoom	= CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
			if( pPvPRoom )
			{
				pPvPRoom->StartPvP( pPacket );
			}
			else
			{
				if( g_pUserSessionManager )
				{
					CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
					if( pUserObj )
						pUserObj->SendPvPStart( pPacket->nRetCode );
				}
			}
			break;
		}

		case MAVI_PVP_STARTMSG:
		{
			MAVIPVP_STARTMSG* pPacket = reinterpret_cast<MAVIPVP_STARTMSG*>(pData);
			CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( pPacket->unVillageChannelID, pPacket->uiPvPIndex );
			if( pPvPRoom )
				pPvPRoom->StartPvPMsg( pPacket );
			break;
		}

		case MAVI_PVP_ROOMINFO:
		{
			MAVIPVP_ROOMINFO* pPacket = reinterpret_cast<MAVIPVP_ROOMINFO*>(pData);
			if( g_pUserSessionManager )
			{
				CDNUserSession* pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
				if( pUserObj )
					pUserObj->SendPvPRoomInfo( pPacket );
			}
			else
				_DANGER_POINT();
			break;
		}

		case MAVI_PVP_MEMBERINDEX:
			{
				MAVIPvPMemberIndex * pPacket = (MAVIPvPMemberIndex*)pData;

				CDNUserSession * pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
				if (pSession)
				{
					if (pPacket->nRetCode == ERROR_NONE)
					{
						CDNPvPRoom * pPvP = CDNPvPRoomManager::GetInstance().GetPvPRoom(pSession->GetChannelID(), pSession->GetPvPIndex());
						if (pPvP)
						{
							pPvP->SendMemberIndex(pPacket->nTeam, pPacket->cCount, pPacket->Index, pPacket->nRetCode);
						}
						else _DANGER_POINT();
					}
					else
					{
						pSession->SendPvPMemberIndex(pPacket->nTeam, 0, NULL, pPacket->nRetCode);
					}
				}
				else _DANGER_POINT();
				break;
			}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		case MAVI_PVP_SWAP_TOURNAMENT_INDEX :
			{
				MAVIPVPSwapTournamentIndex* pPacket = (MAVIPVPSwapTournamentIndex*)pData;
				CDNUserSession * pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
				if (pSession)
				{
					if (pPacket->nRetCode == ERROR_NONE)
					{
						CDNPvPRoom * pPvP = CDNPvPRoomManager::GetInstance().GetPvPRoom(pSession->GetChannelID(), pSession->GetPvPIndex());
						if (pPvP)						
							pPvP->SendPvPSwapTounamentIndex(pPacket->cSourceIndex, pPacket->cDestIndex);
					}
					else					
						pSession->SendPvPSwapTournamentIndex(pPacket->nRetCode, pPacket->cSourceIndex, pPacket->cDestIndex);
				}
				else _DANGER_POINT();
				break;
			}
			break;
		case MAVI_PVP_SHUFFLE_TOURNAMENT_INDEX :
			{
				MAVIPVPShuffleTournamentIndex* pPacket = (MAVIPVPShuffleTournamentIndex*)pData;
				CDNPvPRoom * pPvP = CDNPvPRoomManager::GetInstance().GetPvPRoom(pPacket->unVillageChannelID, pPacket->uiPvPIndex);
				if( pPvP )				
					pPvP->SetPvPShuffleIndex(pPacket);
			}
			break;
#endif

		case MAVI_MASTERSYSTEM_CHECK_MASTERAPPLICATION:
		{
			MasterSystem::MAVICheckMasterApplication* pPacket = reinterpret_cast<MasterSystem::MAVICheckMasterApplication*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiPupilAccountDBID );
			if( pSession )
			{
				pSession->SendMasterSystemMasterApplication( pPacket->nRet );
				
				// PupilList 요청
				if( pPacket->nRet == ERROR_NONE )
				{
					MasterSystem::CCacheRepository::GetInstance().GetPupilList( pSession, pPacket->biMasterCharacterDBID, true );
				}
			}
			break;
		}
		case MAVI_MASTERSYSTEM_SYNC_SIMPLEINFO:
		{
			MasterSystem::MAVISyncSimpleInfo* pPacket = reinterpret_cast<MasterSystem::MAVISyncSimpleInfo*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
			{
				if( pSession->GetCharacterDBID() == pPacket->biCharacterDBID )
				{
					if( pSession->CheckDBConnection() == false )
						break;

					pSession->GetDBConnection()->QueryGetMasterSystemSimpleInfo( pSession->GetDBThreadID(), pSession, true, pPacket->EventCode );

					if( pSession->GetPartyID() > 0 )
					{
						CDNParty* pParty = g_pPartyManager->GetParty( pSession->GetPartyID() );
						if( pParty )
							pParty->UpdateMasterSystemCountInfo();
					}
				}
			}
			break;
		}
		case MAVI_MASTERSYSTEM_SYNC_JOIN:
		{
			MasterSystem::MAVISyncJoin* pPacket = reinterpret_cast<MasterSystem::MAVISyncJoin*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
			{
				if( pSession->GetCharacterDBID() == pPacket->biCharacterDBID )
					pSession->SendMasterSystemJoin( ERROR_NONE, true, pPacket->bIsAddPupil, false );
			}
			break;
		}
		case MAVI_MASTERSYSTEM_SYNC_LEAVE:
		{
			MasterSystem::MAVISyncLeave* pPacket = reinterpret_cast<MasterSystem::MAVISyncLeave*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
			{
				if( pSession->GetCharacterDBID() == pPacket->biCharacterDBID )
					pSession->SendMasterSystemLeave( ERROR_NONE, pPacket->bIsDelPupil );
			}
			break;
		}
		case MAVI_MASTERSYSTEM_SYNC_GRADUATE:
		{
			MasterSystem::MAVISyncGraduate* pPacket = reinterpret_cast<MasterSystem::MAVISyncGraduate*>(pData);
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
				pSession->SendMasterSystemGraduate( pPacket->wszCharName );
			break;
		}
		case MAVI_MASTERSYSTEM_SYNC_CONNECT:
		{
			MasterSystem::MAVISyncConnect* pPacket = reinterpret_cast<MasterSystem::MAVISyncConnect*>(pData);
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
				pSession->SendMasterSystemConnect( pPacket->bIsConnect, pPacket->wszCharName );
			break;
		}
		case MAVI_MASTERSYSTEM_CHECK_LEAVE:
		{
			MasterSystem::MAVICheckLeave* pPacket = reinterpret_cast<MasterSystem::MAVICheckLeave*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
			{
				if( pPacket->iRet != ERROR_NONE && pPacket->iRet != ERROR_MASTERSYSTEM_LEAVE_DESTUSER_OFFLINE )
				{
					pSession->SendMasterSystemLeave( pPacket->iRet );
				}
				else
				{
					if( pSession->CheckDBConnection() == false )
					{
						pSession->SendMasterSystemLeave( ERROR_GENERIC_DBCON_NOT_FOUND );
						return ERROR_NONE;
					}

					INT64 biMasterCharacterDBID = (pPacket->bIsMaster) ? pSession->GetCharacterDBID() : pPacket->biDestCharacterDBID;
					INT64 biPupilCharacterDBID	= (pPacket->bIsMaster) ? pPacket->biDestCharacterDBID : pSession->GetCharacterDBID();
					DBDNWorldDef::TransactorCode::eCode Code = (pPacket->bIsMaster) ? DBDNWorldDef::TransactorCode::Master : DBDNWorldDef::TransactorCode::Pupil;

					int iPenaltyDay				= 0;
					int iPenaltyRespectPoint	= 0;

					if( Code == DBDNWorldDef::TransactorCode::Master )
					{
						const TPupilInfo* pPupilInfo = MasterSystem::CCacheRepository::GetInstance().GetPupilInfo( biMasterCharacterDBID, biPupilCharacterDBID );
						if( pPupilInfo == NULL )
						{
							pSession->SendMasterSystemLeave( ERROR_MASTERSYSTEM_LEAVE_FAILED );
							return ERROR_NONE;
						}						
					}
					else if( Code == DBDNWorldDef::TransactorCode::Pupil )
					{
						iPenaltyDay	= MasterSystem::Penalty::PupilLeavePenaltyDay;
					}
					else
					{
						_DANGER_POINT();
						return ERROR_NONE;
					}

#if defined( _FINAL_BUILD )
					pSession->GetDBConnection()->QueryLeaveMasterSystem( pSession, biMasterCharacterDBID, biPupilCharacterDBID, Code, iPenaltyDay, iPenaltyRespectPoint );
#else
					pSession->GetDBConnection()->QueryLeaveMasterSystem( pSession, biMasterCharacterDBID, biPupilCharacterDBID, Code, iPenaltyDay, iPenaltyRespectPoint, pSession->m_bIsMasterSystemSkipDate );
#endif // #if defined( _FINAL_BUILD )
				}
			}
			break;
		}
		case MAVI_MASTERSYSTEM_RECALL_MASTER:
		{
			MasterSystem::MAVIRecallMaster* pPacket = reinterpret_cast<MasterSystem::MAVIRecallMaster*>(pData);
			
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
			{
				if( pPacket->iRet == ERROR_NONE )
				{
					if( pSession->m_eUserState != STATE_NONE || pSession->GetPartyID() > 0 || !pSession->IsNoneWindowState() )
					{
						SendMasterSystemBreakInto( ERROR_MASTERSYSTEM_RECALL_CANTSTATUS, pSession->GetAccountDBID(), pPacket->wszCharName, pSession->GetMasterSystemData() );
						break;
					}
					if( pPacket->bIsConfirm ) // 스승일 경우 상태변경
						pSession->SetWindowState(WINDOW_ISACCEPT);
				}
				pSession->SendMasterSystemRecallMaster( pPacket->iRet, pPacket->wszCharName, pPacket->bIsConfirm );
			}
			break;
		}
		case MAVI_MASTERSYSTEM_JOIN_CONFIRM:
		{
			MasterSystem::MAVIJoinConfirm* pPacket = reinterpret_cast<MasterSystem::MAVIJoinConfirm*>(pData);
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiMasterAccountDBID);
			if( pSession )
			{
				// 수락 요청 보내주기..
				pSession->SendMasterSystemJoinComfirm(pPacket->cLevel, pPacket->cJob, pPacket->wszPupilCharName);
			}
			else
			{
				SendMasterSystemJoinConfirmResult(ERROR_MASTERSYSTEM_CANT_JOINSTATE, false, 0, pPacket->wszPupilCharName);
			}
			break;
		}
		case MAVI_MASTERSYSTEM_JOIN_CONFIRM_RESULT:
		{
			MasterSystem::MAVIJoinResult* pPacket = reinterpret_cast<MasterSystem::MAVIJoinResult*>(pData);
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszPupilCharName);
			if( pSession )
			{
				if( pPacket->iRet == ERROR_NONE )
				{
					if( pPacket->bIsAccept )
					{
#if defined( _FINAL_BUILD )
						pSession->GetDBConnection()->QueryJoinMasterSystem( pSession, pPacket->biMasterCharacterDBID, pSession->GetCharacterDBID(), false, DBDNWorldDef::TransactorCode::Pupil );
#else
						pSession->GetDBConnection()->QueryJoinMasterSystem( pSession, pPacket->biMasterCharacterDBID, pSession->GetCharacterDBID(), false, DBDNWorldDef::TransactorCode::Pupil, pSession->m_bIsMasterSystemSkipDate );
#endif // #if defined( _FINAL_BUILD )						
					}
					else
						pSession->SendMasterSystemJoin(ERROR_MASTERSYSTEM_JOIN_DENY, false, false, true);
				}
				else								
					pSession->SendMasterSystemJoin(pPacket->iRet, false, false, true);
			}
			break;
		}
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		case MAVI_SYNC_SYSTEMMAIL:
		{
			MAVISyncSystemMail* pPacket = reinterpret_cast<MAVISyncSystemMail*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession )
				pSession->SendNotifyMail( pPacket->iTotalMailCount, pPacket->iNotReadMailCount, pPacket->i7DaysLeftMailCount, true );
			return ERROR_NONE;
		}
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

		case MAVI_RESRECALL:
		{
			MAVIResRecall* pPacket = reinterpret_cast<MAVIResRecall*>(pData);
			if( _CmdRecall( pPacket ) == false )
			{
				//std::wstring wString = boost::io::str( boost::wformat( L"소환에 실패하였습니다." ) );
#if defined(PRE_ADD_MULTILANGUAGE)				
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4413, pPacket->cRecallerSelectedLang) ) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4413) ) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
				SendChat( CHATTYPE_GM, pPacket->uiAccountDBID, wString.c_str(), static_cast<short>(wString.size()), -1 );
			}

			return ERROR_NONE;
		}

		case MAVI_VILLAGETRACE:
		{
			MAVIVillageTrace* pPacket = reinterpret_cast<MAVIVillageTrace*>(pData);
			if( _CmdVillageTrace( pPacket ) == false )
			{
				if( pPacket->bIsGMCall )
				{
#if defined(PRE_ADD_MULTILANGUAGE)
					std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4427, pPacket->cSelectedLang) ) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
					std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4427) ) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
					SendChat( CHATTYPE_GM, pPacket->uiAccountDBID, wString.c_str(), static_cast<short>(wString.size()), -1 );
				}
			}

			return ERROR_NONE;
		}

	case MAVI_VOICECHANNELID:
		{
#ifdef _USE_VOICECHAT
			MAVIVoiceChannelID * pPacket = (MAVIVoiceChannelID*)pData;
			
			switch (pPacket->cType)
			{
			case _VOICECHANNEL_REQTYPE_PARTY:
				{
					CDNParty * pParty = g_pPartyManager->GetParty(pPacket->InstanceID);
					if (pParty)
					{
						if (pParty->CreateVoiceChannel(pPacket->nVoiceChannelD[0]))
						{
							//todo
						}
					}
					else
						_DANGER_POINT();
				}
				break;

			case _VOICECHANNEL_REQTYPE_PVP:
				{
					/*CDNPvPRoom * pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom(pPacket->nPvPLobbyChannelID, pPacket->nID);
					if (pPvPRoom)
						pPvPRoom->CreateVoiceChannel(pPacket->nVoiceChannelD);
					else
						_DANGER_POINT();*/
				}
				break;
			}
#endif
		}
		break;

	case MAVI_NOTICE:
		{
			MANotice * pPacket = (MANotice*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pPacket->wszNoticeMsg, pPacket->nLen);

			if (pPacket->Info.nChannelID > 0)
				pPacket->Info.nMapIdx = GetChannelMapIndex(pPacket->Info.nChannelID);

			if (pPacket->Info.nSlideShowSec > 0 && g_pNoticeSystem->AddNotice(&pPacket->Info, wszChatMsg) == false)
				break;
			
			switch (pPacket->Info.nNoticeType)
			{
			case _NOTICETYPE_WORLD: g_pUserSessionManager->SendNotice(wszChatMsg, pPacket->nLen, pPacket->Info.nSlideShowSec); break;
			case _NOTICETYPE_EACHSERVER:
				{
					if (g_pServiceConnection && pPacket->Info.nManagedID != 0)
						if (pPacket->Info.nManagedID == g_pServiceConnection->GetManagedID())
							g_pUserSessionManager->SendNotice(wszChatMsg, pPacket->nLen, pPacket->Info.nSlideShowSec);
				}
				break;
			case _NOTICETYPE_CHANNEL: g_pFieldManager->SendChannelNotice(pPacket->Info.nChannelID, pPacket->nLen, pPacket->wszNoticeMsg, pPacket->Info.nSlideShowSec); break;
			case _NOTICETYPE_ZONE: g_pFieldManager->SendFieldNotice(pPacket->Info.nMapIdx, pPacket->nLen, pPacket->wszNoticeMsg, pPacket->Info.nSlideShowSec); break;
			}
		}
		break;

	case MAVI_CANCELNOTICE:
		{
			if (g_pUserSessionManager)
				g_pUserSessionManager->SendNoticeCancel();
			else _DANGER_POINT();
		}
		break;

	/*case MAVI_ZONENOTICE:
		{
			MAZoneNotice * pPacket = (MAZoneNotice*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pPacket->wszNoticeMsg, pPacket->nLen);

			g_pFieldManager->SendZoneNotice(pPacket->nMapIndex, pPacket->nLen, pPacket->wszNoticeMsg);
		}
		break;*/

	case MAVI_NOTIFYMAIL:
		{
			MANotifyMail *pMail = (MANotifyMail*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pMail->nToAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			if (pMail->biToCharacterDBID == pUserObj->GetCharacterDBID())
				pUserObj->SendNotifyMail(pMail->wTotalMailCount, pMail->wNotReadMailCount, pMail->w7DaysLeftCount, pMail->bNewMail);	// 메일왔다고 통보
		}
		break;

	case MAVI_NOTIFYMARKET:
		{
			MANotifyMarket *pMarket = (MANotifyMarket*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pMarket->nSellerAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			if (pMarket->biSellerCharacterDBID == pUserObj->GetCharacterDBID())
				pUserObj->SendNotifyMarket(pMarket->nItemID, pMarket->wCalculationCount);
		}
		break;

	case MAVI_NOTIFYGIFT:
		{
			MANotifyGift *pGift = (MANotifyGift*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pGift->nToAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			if (pGift->biToCharacterDBID == pUserObj->GetCharacterDBID())
				pUserObj->SendCashshopNotifyGift(pGift->bNew, pGift->nGiftCount);		// 선물왔다고 통보
		}
		break;

	case MAVI_RESTRAINT:
		{
			MARestraint * pPacket = (MARestraint*)pData;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
			if (!pUserObj) return ERROR_NONE;

			pUserObj->GetDBConnection()->QueryGetRestraint(pUserObj);
		}
		break;

	case MAVI_PCBANGRESULT:
		{
			MAPCBangResult *pResult = (MAPCBangResult*)pData;

			CDNUserSession *pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pResult->nAccountDBID);
			if( !pUser ) break;

			pUser->PCBangResult(pResult);
		}
		break;

	case MAVI_UPDATECHANNELSHOWINFO:
		{
			MAVIUpdateChannelShowInfo* pPacket = reinterpret_cast<MAVIUpdateChannelShowInfo*>(pData);
			
			for( UINT i=0 ; i<m_vChannelList.size() ; ++i )
			{
				if( m_vChannelList[i].second.nChannelID == pPacket->unChannelID )
				{
					m_vChannelList[i].second.bShow = pPacket->bShow;
					return ERROR_NONE;
				}
			}

			return ERROR_NONE;
		}
		break;

	case MAVI_UPPDATE_WORLDEVENTCOUNTER:
		{
			MAUpdateWorldEventCounter* pPacket = reinterpret_cast<MAUpdateWorldEventCounter*>(pData);

			if (g_pPeriodQuestSystem)
				g_pPeriodQuestSystem->UpdateWorldEvent(pPacket->cWorldSetID, pPacket->nScheduleID, pPacket->nCount);

			return ERROR_NONE;
		}
		break;

		case MAVI_FARMINFOUPDATE:
		{
			MAVIFarmInfoUpdate * pPacket = (MAVIFarmInfoUpdate*)pData;
			g_pFarm->UpdateFarmInfo(pPacket->FarmInfo, pPacket->cFarmCount);
			return ERROR_NONE;
		}
		case MAVI_FARM_SYNC:
		{
			MAVIFarmSync* pPacket = reinterpret_cast<MAVIFarmSync*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession && pSession->CheckDBConnection() )
			{
				pSession->GetDBConnection()->QueryGetCountHarvestDepotItem( pSession );
			}
			return ERROR_NONE;
		}
	case MAVI_CHANGE_CHARACTERNAME:
		{
			MAChangeCharacterName *pPacket = (MAChangeCharacterName*)pData;

			if (g_pUserSessionManager)
				g_pUserSessionManager->SwapUserKeyName(pPacket->wszOriginName, pPacket->wszCharacterName);

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
#endif
			if (pGuild)
				pGuild->UpdateMemberName (pPacket->nCharacterDBID, pPacket->wszCharacterName);

#if defined (PRE_ADD_BESTFRIEND)
			CDNUserSession* pBFSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiBFAccountDBID );
			if (pBFSession)
				pBFSession->GetBestFriend()->ChangeName(pPacket->wszCharacterName);
#endif
		}
		break;
	case MAVI_LADDERSYSTEM_DELUSER:
		{
			LadderSystem::MAVIDelUser* pPacket = reinterpret_cast<LadderSystem::MAVIDelUser*>(pData);
			LadderSystem::CManager::GetInstance().OnDisconnectUser( pPacket->biCharDBID, pPacket->wszCharName );
			break;
		}
	case MAVI_USERTEMPDATA_RESULT:
		{
			MAVIUserTempDataResult *pResult = (MAVIUserTempDataResult*)pData;

			CDNUserSession *pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pResult->uiAccountDBID);
			if( !pUser ) break;
			pUser->SetDungeonClearCount (pResult->nDungeonClearCount);
#if defined(PRE_ADD_REMOTE_QUEST)
			pUser->GetQuest()->RefreshRemoteQuest(pResult->nAcceptWaitRemoteQuestCount, pResult->AcceptWaitRemoteQuestList);
#endif
#if defined(PRE_ADD_GAMEQUIT_REWARD)
			pUser->SetUserGameQuitRewardType(pResult->eUserGameQuitRewardType);
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
			break;
		}
		case MAVI_CHECK_LASTDUNGEONINFO:
		{
			MAVICheckLastDungeonInfo* pPacket= reinterpret_cast<MAVICheckLastDungeonInfo*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession == NULL )
				break;

#if defined( _WORK )
			std::cout << "MAVI_CHECK_LASTDUNGEONINFO CharDBID=" << pSession->GetCharacterDBID() << " Check=" << (pPacket->bIsCheck ? "true" : "false") << std::endl;
#endif // #if defined( _WORK )
			if( pPacket->bIsCheck == true )
			{
				pSession->SetCheckLastDungeonInfo(true);
				pSession->SendCheckLastDungeonInfo( pPacket->wszPartyName );
			}
			else
			{
				pSession->ModCommonVariableData( CommonVariable::Type::LastPartyDungeonInfo, 0 );
			}
			break;
		}
		case MAVI_CONFIRM_LASTDUNGEONINFO:
		{
			MAVIConfirmLastDungeonInfo* pPacket = reinterpret_cast<MAVIConfirmLastDungeonInfo*>(pData);

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
			if( pSession == NULL )
				break;

#if defined( _WORK )
			std::cout << "MAVI_CONFIRM_LASTDUNGEONINFO CharDBID=" << pSession->GetCharacterDBID() << " Ret=" << pPacket->iRet << std::endl;
#endif // #if defined( _WORK )

#if defined( PRE_PARTY_DB )
			switch( pPacket->BreakIntoType )
			{
				case BreakInto::Type::PartyRestore:
				{
					switch( pPacket->iRet )
					{
						case ERROR_GENERIC_GAMECON_NOT_FOUND:
						case ERROR_CANT_EXSIT_BACKUPPARTYINFO:
						{
							pSession->ModCommonVariableData( CommonVariable::Type::LastPartyDungeonInfo, 0 );
							break;
						}
					}
					pSession->SendConfirmLastDungeonInfo( pPacket->iRet );
					break;
				}
				case BreakInto::Type::PartyJoin:
				{
					if( pPacket->iRet != ERROR_NONE )
						pSession->SendJoinParty( pPacket->iRet, 0, NULL );
					break;
				}
			}
#else
			switch( pPacket->iRet )
			{
				case ERROR_GENERIC_GAMECON_NOT_FOUND:
				case ERROR_CANT_EXSIT_BACKUPPARTYINFO:
				{
					pSession->ModCommonVariableData CommonVariable::Type::LastPartyDungeonInfo, 0 );
					break;
				}
			}

			pSession->SendConfirmLastDungeonInfo( pPacket->iRet );
#endif // #if defined( PRE_PARTY_DB )
			break;
		}

#if defined (PRE_ADD_BESTFRIEND)
		case MAVI_SEARCH_BESTFRIEND:
			{
				MASearchBestFriend *pPacket = (MASearchBestFriend*)pData;

				if (!pPacket->bAck)
				{
					CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszName);
					if (pUser)
					{
						if (pUser->GetBestFriend()->IsRegistered())
						{
							SendSearchBestFriend(pPacket->nAccountDBID, ERROR_BESTFRIEND_REGISTFAIL_DESTUSER_SITUATION_NOTALLOWED, true, pPacket->wszName);
							break;
						}

						SendSearchBestFriend(pPacket->nAccountDBID, ERROR_NONE, true, pPacket->wszName, pUser->GetLevel(), pUser->GetStatusData()->cJob);
					}
					else
					{
						SendSearchBestFriend(pPacket->nAccountDBID, ERROR_BESTFRIEND_NOTSEARCH, true, pPacket->wszName);
					}
				}
				else
				{
					CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
					if (pUser)
						pUser->SendSearchBestFriendResult(pPacket->nRet, pPacket->cLevel, pPacket->cJob, pPacket->wszName);
				}	


			}
			break;
		case MAVI_REGIST_BESTFRIEND:
			{
				MARegistBestFriend *pPacket = (MARegistBestFriend*)pData;
				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToName);

				if (pUser)
				{
					// 조건 체크
					if (pUser->GetBestFriend()->IsRegistered())
					{
						SendRegistBestFriendResult(pPacket->nAccountDBID, ERROR_BESTFRIEND_REGISTFAIL_DESTUSER_SITUATION_NOTALLOWED, false, 0, NULL, 0, 0, NULL);
						break;
					}

					// 절친 요청
					pUser->SendRegistBestFriendReq(ERROR_NONE, pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->wszFromName, pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetCharacterName());
				}
				else
					SendRegistBestFriendResult(pPacket->nAccountDBID, ERROR_BESTFRIEND_NOTEXIST_OPPOSITEUSER, false, 0, NULL, 0, 0, NULL);
			}
			break;

		case MAVI_REGIST_BESTFRIEND_RETMSG:
			{
				MARegistBestFriendResult *pPacket = (MARegistBestFriendResult*)pData;
				
				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
				if (!pUser)
					break;

				if (pPacket->nRet == ERROR_NONE)
				{
					INT64 biRegistSerial = pUser->GetBestFriend()->GetRegistSerial();
					const TItem* pRegistItem = pUser->GetItem()->GetCashInventory(biRegistSerial);
					if (!pRegistItem)
						break;

					TItemData* pItemData = g_pDataManager->GetItemData(pRegistItem->nItemID);
					if (!pItemData)
						break;

					// 승낙: DB 처리
					pUser->GetDBConnection()->QueryRegistBestFriend(pUser, pPacket->nToAccountDBID, pPacket->biToCharacterDBID, pPacket->wszToName, biRegistSerial, pRegistItem->nItemID);
				}
				else
				{
					if (pPacket->bAck)
						pUser->SendRegistBestFriendAck(ERROR_BESTFRIEND_REFUESED_REGISTRATION, 0, 0, 0, pPacket->wszToName);
					else
						pUser->SendRegistBestFriendReq(pPacket->nRet, pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetCharacterName(), 0, 0, pPacket->wszToName);
				}
			}
			break;

		case MAVI_COMPLETE_BESTFRIEND:
			{
				MACompleteBestFriend *pPacket = (MACompleteBestFriend*)pData;
				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nFromAccountDBID);
				if (!pUser)
					break;

				pUser->GetBestFriend()->ExecuteReward(pPacket->nItemID);
				pUser->GetDBConnection()->QueryGetBestFriend(pUser->GetDBThreadID(), pUser, true);
				pUser->SendCompleteBestFriend(ERROR_NONE, pPacket->wszName);
			}
			break;

		case MAVI_EDIT_BESTFRIENDMEMO:
			{
				MAEditBestFriendMemo *pPacket = (MAEditBestFriendMemo*)pData;

				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nToAccountDBID);
				if (!pUser)
					break;

				pUser->GetBestFriend()->ChangeMemo(false, pPacket->wszMemo);
				pUser->SendEditBestFriendMemo(ERROR_NONE, false, pPacket->wszMemo);
			}
			break;

		case MAVI_CANCEL_BESTFRIEND:
			{
				MACancelBestFriend *pPacket = (MACancelBestFriend*)pData;
				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToName);
				if (!pUser)
					break;

				pUser->GetDBConnection()->QueryGetBestFriend(pUser->GetDBThreadID(), pUser, true);

				pUser->SendCancelBestFriend(ERROR_NONE, pPacket->wszFromName, pPacket->bCancel);
			}
			break;

		case MAVI_CLOSE_BESTFRIEND:
			{
				MACloseBestFriend *pPacket = (MACloseBestFriend*)pData;
				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToName);
				if (!pUser)
					break;

				pUser->SendCloseBestFriend(ERROR_NONE, pPacket->wszFromName);

				pUser->CloseBestFirend();
			}
			break;
		case MAVI_LEVELUP_BESTFRIEND:
			{
				MALevelUpBestFriend *pPacket = (MALevelUpBestFriend*)pData;
				CDNUserSession * pUser = g_pUserSessionManager->FindUserSessionByName(pPacket->wszName);
				if (!pUser)
					break;
				pUser->GetBestFriend()->ChangeLevel(pPacket->cLevel);
			}
			break;
#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined(PRE_ADD_QUICK_PVP)
		case MAVI_MAKEQUICKPVP_ROOM :
			{
				MAVIMakeQuickPvPRoom* pPacket = (MAVIMakeQuickPvPRoom*)pData;
				CDNUserSession *pMasterSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiMasterAccountDBID);
				if( pMasterSession == NULL )
					break;
				CDNUserSession *pSlaveSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiSlaveAccountDBID);
				if( pSlaveSession == NULL )
					break;
				if( pPacket->nRet == ERROR_NONE)
				{
					const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
					if( pChannelInfo )
					{						
						pMasterSession->CmdSelectChannel( pChannelInfo->nChannelID, true );
						pSlaveSession->CmdSelectChannel( pChannelInfo->nChannelID, true );
						g_Log.Log(LogType::_NORMAL, pMasterSession, L"MakeQuickPvP %s vs %s\r\n", pMasterSession->GetCharacterName(), pSlaveSession->GetCharacterName());
					}
				}
				else
				{
					//에러처리..
					pMasterSession->SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
					pSlaveSession->SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
				}
			}
			break;
#endif
#if defined( PRE_WORLDCOMBINE_PARTY )
		case MAVI_DELWORLDPARTY:
			{
				MADelWorldParty* pPacket = (MADelWorldParty*)pData;
				g_pPartyManager->DelCombinePartyData( (int)pPacket->nPrimaryIndex );
			}
			break;
		case MAVI_WORLDPARTYMEMBER:
			{
				GAMASendWorldPartyMember* pPacket = (GAMASendWorldPartyMember*)pData;

				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
				if (!pSession) break;

				if( pPacket->nRetCode == ERROR_NONE )
				{
					SCPartyMemberInfo TxPacket;
					memset( &TxPacket, 0, sizeof(TxPacket) );

					TxPacket.PartyID = pPacket->PartyID;
					TxPacket.PartyType = _WORLDCOMBINE_PARTY;
					TxPacket.nCount = pPacket->nCount;
					for( int i=0 ; i<pPacket->nCount ; ++i )
						TxPacket.MemberData[i] = pPacket->MemberData[i];

					pSession->SendPartyMemberInfo( &TxPacket );
				}
				else
				{
					
					pSession->SendPartyInfoErr( ERROR_PARTY_NOTFOUND_MEMBERINFO );
				}				
			}
			break;
#endif
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
		case MAVI_GETPARTYID:
			{
				MAVIGetPartyID* pPacket = (MAVIGetPartyID*)pData;

				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nAccountDBID);
				if (!pSession) break;

				g_pMasterConnection->SendGetPartyIDResult(pPacket->nSenderAccountDBID, pSession->GetPartyID());
			}
			break;

		case MAVI_GETPARTYID_RESULT:
			{
				MAVIGetPartyIDResult* pPacket = (MAVIGetPartyIDResult*)pData;

				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nSenderAccountDBID);
				if (!pSession) break;

				CDNParty* pParty = g_pPartyManager->GetParty(pPacket->PartyID);
				pSession->SendPartyInfo(ERROR_NONE, pParty);
			}
			break;
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
		case MAVI_PRIVATECHATCHANNEL_ADD:
			{
				MAAddPrivateChannel* pPacket = (MAAddPrivateChannel*)pData;
				if( g_pPrivateChatChannelManager  && pPacket->nManagedID != g_Config.nManagedID )
				{
					g_pPrivateChatChannelManager->AddPrivateChatChannel( pPacket->cWorldSetID, pPacket->tPrivateChatChannel);
					CDNPrivateChaChannel* pPrivateChatChannel =  g_pPrivateChatChannelManager->GetPrivateChannelInfo( pPacket->cWorldSetID, pPacket->tPrivateChatChannel.nPrivateChatChannelID);
					if( pPrivateChatChannel )
						pPrivateChatChannel->SetLoaded(true);
					
				}
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MEMBERADD:
			{
				MAAddPrivateChannelMember* pPacket = (MAAddPrivateChannelMember*)pData;
				if( g_pPrivateChatChannelManager && pPacket->nManagedID != g_Config.nManagedID )
				{
					CDNPrivateChaChannel* pPrivateChatChannel =  g_pPrivateChatChannelManager->GetPrivateChannelInfo(pPacket->cWorldSetID, pPacket->nPrivateChatChannelID);
					if( pPrivateChatChannel )
					{
						pPrivateChatChannel->AddPrivateChannelMember(pPacket->Member);
					}
				}
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MEMBERINVITE:
			{
				MAInvitePrivateChannelMember* pPacket = (MAInvitePrivateChannelMember*)pData;
				
				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszInviteName);
				if(pSession)
				{
					if(pSession->GetPrivateChannelID())
					{
						if(g_pMasterConnection && g_pMasterConnection->GetActive() )
						{
							g_pMasterConnection->SendInvitePrivateChatChannelResult(ERROR_ALREADY_JOINCHANNEL, pPacket->nMasterAccountDBID);
						}						
						return ERROR_ALREADY_JOINCHANNEL;
					}
					else
					{
						pSession->SetPrivateChannelID(pPacket->nPrivateChatChannelID);
						pSession->GetDBConnection()->QueryInvitePrivateChatChannelMember(pSession, pPacket->nPrivateChatChannelID, pPacket->nMasterAccountDBID);
					}
					return ERROR_NONE;
				}
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MEMBERINVITERESULT:
			{
				MAInvitePrivateChannelMemberResult* pPacket = (MAInvitePrivateChannelMemberResult*)pData;

				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nMasterAccountDBID);
				if(pSession)
				{					
					pSession->SendPrivateChatChannelInviteResult(pPacket->nRet);
					return ERROR_NONE;
				}
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MEMBERDEL:
			{
				MADelPrivateChannelMember* pPacket = (MADelPrivateChannelMember*)pData;
				if( g_pPrivateChatChannelManager && pPacket->nManagedID != g_Config.nManagedID )
				{
					CDNPrivateChaChannel* pPrivateChatChannel =  g_pPrivateChatChannelManager->GetPrivateChannelInfo( pPacket->cWorldSetID, pPacket->nPrivateChatChannelID);
					if( pPrivateChatChannel )
					{
						pPrivateChatChannel->DelPrivateChannelMember( pPacket->eType, pPacket->biCharacterDBID);
					}
				}
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MEMBERKICKRESULT:
			{
				MAKickPrivateChannelMemberResult* pPacket = (MAKickPrivateChannelMemberResult*)pData;
				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->nAccountDBID );
				if( pSession  )
				{
					pSession->SendPrivateChatChannelKickResult(ERROR_NONE, true);
					pSession->SetPrivateChannelID(0);
				}
				return ERROR_NONE;
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MOD:
			{
				MAModPrivateChannel* pPacket = (MAModPrivateChannel*)pData;
				if( g_pPrivateChatChannelManager && pPacket->nManagedID != g_Config.nManagedID )
				{
					CDNPrivateChaChannel* pPrivateChatChannel =  g_pPrivateChatChannelManager->GetPrivateChannelInfo(pPacket->cWorldSetID, pPacket->nPrivateChatChannelID);
					if( pPrivateChatChannel )
					{
						pPrivateChatChannel->ModPrivateChannelInfo( pPacket->eType, pPacket->nPassWord, pPacket->biCharacterDBID);
					}
				}
			}
			break;
		case MAVI_PRIVATECHATCHANNEL_MODMEMBERNAME:
			{
				MAModPrivateChannelMemberName* pPacket = (MAModPrivateChannelMemberName*)pData;
				if( g_pPrivateChatChannelManager && pPacket->nManagedID != g_Config.nManagedID )
				{
					CDNPrivateChaChannel* pPrivateChatChannel =  g_pPrivateChatChannelManager->GetPrivateChannelInfo(pPacket->cWorldSetID, pPacket->nPrivateChatChannelID);
					if( pPrivateChatChannel )
					{
						pPrivateChatChannel->ModPrivateChannelMemberName( pPacket->biCharacterDBID, pPacket->wszName );
					}
				}
			}
			break;
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
		case MAVI_DEL_WORLDPVPROOM:
			{
				MADelWorldPvPRoom* pPacket = (MADelWorldPvPRoom*)pData;
				int nRoomIndex = (UINT)(pPacket->nRoomIndex + PvPCommon::Common::MaxRoomCount);
				CDNPvPRoomManager::GetInstance().DelWorldPvPRoom(nRoomIndex);
			}
			break;
		case MAVI_WORLDPVPROOM_JOINRESULT:
			{
				MAWorldPvPRoomJoinResult* pPacket = (MAWorldPvPRoomJoinResult*)pData;

				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiAccountDBID);
				if (!pSession) break;
				if( pPacket->nRet != ERROR_NONE )
				{	
					pSession->SendWorldPvPRoomJoinResult( pPacket->nRet );
					pSession->SetPvPIndex(0);
				}
			}
			break;
		case MAVI_WORLDPVPROOM_GMCRETERESULT:
			{
				MAWorldPvPRoomGMCreateResult* pPacket = (MAWorldPvPRoomGMCreateResult*)pData;

				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiAccountDBID);
				if (!pSession) break;
				if( pPacket->nRet != ERROR_NONE )
				{	
					pSession->SendWorldPvPRoomJoinResult( pPacket->nRet );
					pSession->SetPvPIndex(0);
				}
				else
				{	
					int nWorldPvPRoomChannelID = CDNPvPRoomManager::GetInstance().GetWorldPvPRoomChannelID();
					if( pPacket->Data.nDBRoomID > 0 )
					{
						MAVIPVP_CREATEROOM CreateRoomData;
						memset(&CreateRoomData, 0, sizeof(CreateRoomData));
						CreateRoomData.unVillageChannelID = nWorldPvPRoomChannelID;
						CreateRoomData.uiPvPIndex = pPacket->uiPvPIndex;
						CreateRoomData.cGameMode = pPacket->Data.cModeID;
						CreateRoomData.sCSPVP_CREATEROOM.cMaxUser = pPacket->Data.cMaxPlayers;
						CreateRoomData.sCSPVP_CREATEROOM.uiMapIndex = pPacket->Data.nMapID;
						CreateRoomData.sCSPVP_CREATEROOM.uiGameModeTableID = pPacket->Data.cModeID;
						CreateRoomData.sCSPVP_CREATEROOM.uiSelectWinCondition = pPacket->Data.nWinCondition;
						CreateRoomData.sCSPVP_CREATEROOM.uiSelectPlayTimeSec = pPacket->Data.nPlayTimeSec;
						CreateRoomData.sCSPVP_CREATEROOM.uiGameModeTableID = pPacket->Data.cModeID;
						CreateRoomData.sCSPVP_CREATEROOM.cMinLevel = pPacket->Data.cMinLevel;
						CreateRoomData.sCSPVP_CREATEROOM.cMaxLevel = pPacket->Data.cMaxLevel;
						CreateRoomData.sCSPVP_CREATEROOM.cRoomNameLen = (BYTE)wcslen(pPacket->Data.wszRoomName);
						CreateRoomData.sCSPVP_CREATEROOM.nWorldPvPRoomType = WorldPvPMissionRoom::Common::GMRoom;
						_wcscpy( CreateRoomData.sCSPVP_CREATEROOM.wszBuf, _countof(CreateRoomData.sCSPVP_CREATEROOM.wszBuf), pPacket->Data.wszRoomName, CreateRoomData.sCSPVP_CREATEROOM.cRoomNameLen );

						CDNPvPRoom* pRoom = new (std::nothrow) CDNPvPRoom;
						if( !pRoom )
							return ERROR_NONE;

						short nRet = pRoom->Create( &CreateRoomData );
						
						if( g_pMasterConnection )
						{								
							pSession->SetPvPIndex(pPacket->uiPvPIndex);
							pSession->SendPvPJoinRoom(ERROR_NONE, pRoom);
							UINT uiTeam = PvPCommon::Team::Observer;

							g_pMasterConnection->SendWorldPvPJoinRoom( pSession->GetAccountDBID(), pSession->GetCharacterDBID(), pPacket->uiPvPIndex, &pPacket->Data, uiTeam, true );
						}
						delete pRoom;
					}
					break;
				}
			}
			break;
#endif
#if defined(PRE_ADD_MUTE_USERCHATTING)
		case MAVI_MUTE_USERCHAT:
			{
				MAMuteUserChat* pPacket = (MAMuteUserChat*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->uiMuteUserAccountID);

				if(pSession)
				{
					pSession->ModCommonVariableData(CommonVariable::Type::MuteChatting, pPacket->nMuteMinute);
					return ERROR_NONE;
				}
				break;
			}
#endif	// #if defined(PRE_ADD_MUTE_USERCHATTING)
#if defined( PRE_ALTEIAWORLD_EXPLORE )
		case MAVI_ALTEIAWORLD_SENDTICKET:
			{
				MAAlteiaWorldSendTicket* pPacket = (MAAlteiaWorldSendTicket*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszRecvCharacterName);

				if(pSession)
				{	
					int nMaxSendTicketCount = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldSendTicketCount ));
					pSession->GetDBConnection()->QueryAddAlteiaWorldSendTicketList(pSession, pPacket->biSendCharacterDBID, pPacket->wszSendCharacterName, nMaxSendTicketCount );
					return ERROR_NONE;
				}
				else
				{
					if( g_pMasterConnection )
						g_pMasterConnection->SendAddAlteiaWorldSendTicketResult(ERROR_ALTEIAWORLD_NOTEXIST_RECVCHARACTER, pPacket->biSendCharacterDBID);
				}
			}
			break;
		case MAVI_ALTEIAWORLD_SENDTICKET_RESULT:
			{
				MAAlteiaWorldSendTicketResult* pPacket = (MAAlteiaWorldSendTicketResult*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nSendAccountDBID);

				if(pSession)
				{
					if( pPacket->nRetCode == ERROR_NONE )
						pSession->DelAlteiaSendTicket();
					pSession->SendAlteiaWorldSendTicketResult( pPacket->nRetCode, pSession->GetAlteiaSendTicketCount() );
					return ERROR_NONE;
				}
			}
			break;
#endif
#if defined(PRE_ADD_CHNC2C)
		case MAVI_C2C_CHAR_COINBALANCE :
			{
				MAC2CGetCoinBalance* pPacket = (MAC2CGetCoinBalance*)pData;
				if ( g_pDBConnectionManager )
				{
					BYTE cThreadID;
					CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
					if (pDBCon)
						pDBCon->QueryGetGameMoney(rand()%THREADMAX, g_Config.nWorldSetID, pPacket->biCharacterDBID, pPacket->szSeqID);
				}
			}
			break;
		case MAVI_C2C_CHAR_ADDCOIN :
			{
				MAC2CAddCoin* pPacket = (MAC2CAddCoin*)pData;
				if ( g_pDBConnectionManager )
				{
					BYTE cThreadID;
					CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
					if (pDBCon)
						pDBCon->QueryTransferGameMoney(rand()%THREADMAX, g_Config.nWorldSetID, pPacket->uiAccountDBID, pPacket->biCharacterDBID, pPacket->biAddCoin, pPacket->szSeqID, pPacket->szBookID);
				}
			}
			break;
		case MAVI_C2C_CHAR_REDUCECOIN :
			{
				MAC2CReduceCoin* pPacket = (MAC2CReduceCoin*)pData;
				if ( g_pDBConnectionManager )
				{
					BYTE cThreadID;
					CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
					if (pDBCon)
						pDBCon->QueryKeepGameMoney(rand()%THREADMAX, g_Config.nWorldSetID, pPacket->uiAccountDBID, pPacket->biCharacterDBID, pPacket->biReduceCoin, pPacket->szSeqID, pPacket->szBookID);
				}
			}
			break;
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined( PRE_DRAGONBUFF )
		case MAVI_APPLYWORLDBUFF:
			{
				MAApplyWorldBuff* pPacket = (MAApplyWorldBuff*)pData;
				if( g_pDataManager )
				{
					g_pDataManager->AddWorldBuffData(pPacket->nItemID);
				}
				if( g_pUserSessionManager )
				{
					g_pUserSessionManager->ApplyWorldBuff( pPacket->wszCharacterName, pPacket->nItemID, pPacket->nMapIdx );
				}
			}
			break;
#endif
#if defined( PRE_PVP_GAMBLEROOM )
		case MAVI_GAMBLEROOM_CREATE:
			{				
				MACreateGambleRoom* pPacket = (MACreateGambleRoom*)pData;
				CDNPvPRoomManager::GetInstance().CreateGambleRoom(pPacket->nTotalGambleRoomCount);
			}
			break;
#endif
#if defined(PRE_ADD_DWC)
		case MAVI_INVITE_DWCTEAMMEMB:
			{
				MAInviteDWCTeamMember* pPacket = (MAInviteDWCTeamMember*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToCharacterName);
				if(pSession)
					pSession->SendInviteDWCTeamMemberReq(pPacket->nTeamID, pPacket->nAccountDBID, pPacket->wszFromCharacterName, pPacket->wszTeamName );
				else					
					SendInviteDWCTeamMemberACK(pPacket->nAccountDBID, ERROR_DWC_NOT_DWC_CHARACTER, false, NULL);
			}
			break;
		case MAVI_INVITE_DWCTEAMMEMB_ACK:
			{				
				MAInviteDWCTeamMemberAck* pPacket = (MAInviteDWCTeamMemberAck*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nInviterDBID);
				if(pSession)
				{
					pSession->SendInviteDWCTeamMemberAck(pPacket->nRetCode);
				}
			}			
			break;
		case MAVI_ADD_DWCTEAMMEMBER:
			{
				MAAddDWCTeamMember* pPacket = (MAAddDWCTeamMember*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nTartgetUserAccountDBID);
				if(pSession)
				{
					pSession->SendInviteDWCTeamMemberAck( pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->wszCharacterName, ERROR_NONE, 0 , 0, pPacket->nJob, &pPacket->Location, pPacket->nTeamID );
					if(g_pDWCTeamManager)
					{
						CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
						if(pDWCTeam)
						{
							TDWCTeamMember DWCTeamMember(pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->wszCharacterName, pPacket->nJob, false);
							pDWCTeam->AddMember(&DWCTeamMember, &pPacket->Location);
						}
					}
				}
			}
			break;
		case MAVI_DEL_DWCTEAMMEMB:
			{
				MALeaveDWCTeamMember* pPacket = (MALeaveDWCTeamMember*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nTartgetUserAccountDBID);
				if( pSession )
				{
					pSession->SendLeaveDWCTeamMember( pPacket->nTeamID, pPacket->biLeaveUserCharacterDBID, pPacket->wszCharacterName, pPacket->nRet);
				}
				if(g_pDWCTeamManager)
				{
					CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
					if(pDWCTeam)
					{
						pDWCTeam->DelMember(pPacket->biLeaveUserCharacterDBID);
						pDWCTeam->ResetTeamScore();
					}
				}
			}
			break;
		case MAVI_DISMISS_DWCTEAM:
			{
				MADismissDWCTeam* pPacket = (MADismissDWCTeam*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nTartgetUserAccountDBID);
				if( pSession )
				{
					pSession->SendDismissDWCTeam( pPacket->nTeamID, pPacket->nRet );
					pSession->SetDWCTeamID(0);
				}
				if(g_pDWCTeamManager)
					g_pDWCTeamManager->DelDWCTeamResource(pPacket->nTeamID);
			}
			break;
		case MAVI_CHANGE_DWCTEAMMEMB_STATE:
			{
				MAChangeDWCTeamMemberState* pPacket = (MAChangeDWCTeamMemberState*)pData;
				CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nTartgetUserAccountDBID);
				if( pSession )
				{
					pSession->SendChangeDWCTeamMemberState( pPacket->nTeamID, pPacket->biCharacterDBID, &pPacket->Location );
				}
				if(g_pDWCTeamManager)
				{
					CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
					if(pDWCTeam)
					{	//변경이 일어난 빌리지에서 이미 브로드캐스트 했으므로 유저 상태 업데이트만 하고 전송은 안함
						pDWCTeam->ChangeDWCTeamMemberState(pPacket->biCharacterDBID, &pPacket->Location, false);
						if(pPacket->bLogin)
							g_pDWCTeamManager->RegistDWCCharacter(pPacket->biCharacterDBID, pPacket->nTeamID);
					}
				}
			}
			break;
		case MAVI_DWC_UPDATE_SCORE:
			{
				MADWCUpdateScore* pPacket = (MADWCUpdateScore*)pData;
				if(g_pDWCTeamManager && g_pDWCTeamManager->bIsIncludeDWCVillage())
				{
					CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
					if(pDWCTeam)
					{
						pDWCTeam->UpdateDWCScore(pPacket);
					}
				}
			}
#endif
	}
	return ERROR_UNKNOWN_HEADER;
}

void CDNMasterConnection::SendProcessDelayed(UINT nDelayedTick)
{
	VIMAProcessDelayed Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nDelayedTick = nDelayedTick;

	AddSendData(VIMA_PROCESSDELAYED, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendVIllageRegistInfo(int nManagedID, const char * pszIP, short nPort)
{
	VIMARegist Regist;
	memset(&Regist, 0, sizeof(VIMARegist));

	Regist.cVillageID = g_Config.nVillageID;
	Regist.nManagedID = nManagedID;
	_strcpy(Regist.szIP, _countof(Regist.szIP), pszIP, (int)strlen(pszIP));
	Regist.nPort = nPort;

	AddSendData(VIMA_REGIST, 0, (char*)&Regist, sizeof(Regist));
}

void CDNMasterConnection::SendEndofVillageInfo()
{
	AddSendData(VIMA_ENDOFREGIST, 0, NULL, 0);

	if( g_pFieldManager )
		g_pFieldManager->UpdateChannelShowInfo();
}

void CDNMasterConnection::SendAddUserList(CDNUserSession *pUserObj)
{
	VIMAAddUserList AddUserList;
	memset(&AddUserList, 0, sizeof(VIMAAddUserList));

	AddUserList.nAccountDBID = pUserObj->GetAccountDBID();
	AddUserList.nSessionID = pUserObj->GetSessionID();
	AddUserList.biCharacterDBID = pUserObj->GetCharacterDBID();
	AddUserList.wChannelID = pUserObj->GetChannelID();
	_wcscpy(AddUserList.wszAccountName, _countof(AddUserList.wszAccountName), pUserObj->GetAccountName(), (int)wcslen(pUserObj->GetAccountName()));
	_wcscpy(AddUserList.wszCharacterName, _countof(AddUserList.wszCharacterName), pUserObj->wszName(), (int)wcslen(pUserObj->wszName()));
	_strcpy(AddUserList.szIP, _countof(AddUserList.szIP), pUserObj->GetIp(), (int)strlen(pUserObj->GetIp()));
	_strcpy(AddUserList.szVirtualIp, _countof(AddUserList.szVirtualIp), pUserObj->m_szVirtualIp, (int)strlen(pUserObj->m_szVirtualIp));
	AddUserList.bAdult					= pUserObj->m_bAdult;
	AddUserList.cPvPVillageID			= pUserObj->GetPvPVillageID();
	AddUserList.unPvPVillageChannelID	= pUserObj->GetPvPVillageChannelID();
#if defined (_KR)
	memcpy(AddUserList.szMID, pUserObj->m_szMID, sizeof(AddUserList.szMID));
	AddUserList.dwGRC = pUserObj->m_dwGRC;
#endif
	AddUserList.cPCBangGrade = pUserObj->GetPCBangGrade();
#ifdef PRE_ADD_MULTILANGUAGE
	AddUserList.cSelectedLanguage = pUserObj->m_eSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#if defined(_ID)
	_strcpy(AddUserList.szMacAddress, _countof(AddUserList.szMacAddress), pUserObj->m_szMacAddress, (int)strlen(pUserObj->m_szMacAddress));
	_strcpy(AddUserList.szKey, _countof(AddUserList.szKey), pUserObj->m_szKey, (int)strlen(pUserObj->m_szKey));
	AddUserList.dwKreonCN = pUserObj->m_dwKreonCN;
#endif
#if defined(_KRAZ)
	AddUserList.ShutdownData = pUserObj->m_ShutdownData;
#endif	// #if defined(_KRAZ)
#if defined(PRE_ADD_DWC)
	AddUserList.cAccountLevel = pUserObj->GetAccountLevel();
#endif

	AddSendData(VIMA_ADDUSERLIST, 0, (char*)&AddUserList, sizeof(AddUserList));
}

void CDNMasterConnection::SendDelUser(UINT nAccountDBID, UINT nSessionID)
{
	VIMADelUser DelUser;
	memset(&DelUser, 0, sizeof(VIMADelUser));
	DelUser.nAccountDBID = nAccountDBID;
	DelUser.nSessionID = nSessionID;
	AddSendData(VIMA_DELUSER, 0, (char*)&DelUser, sizeof(VIMADelUser));
}

void CDNMasterConnection::SendCheckUser(UINT nSessionID)
{
	VIMACheckUser CheckUser;
	memset(&CheckUser, 0, sizeof(VIMACheckUser));
	CheckUser.nSessionID = nSessionID;
	AddSendData(VIMA_CHECKUSER, 0, (char*)&CheckUser, sizeof(VIMACheckUser));
}

void CDNMasterConnection::SendEnterVillage(UINT nAccountDBID, int nChannelID)
{
	VIMAEnterVillage Packet;
	memset(&Packet, 0, sizeof(VIMAEnterVillage));

	Packet.nAccountDBID = nAccountDBID;
	Packet.nChannelID = nChannelID;

	AddSendData(VIMA_ENTERVILLAGE, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendCheckReconnectLogin(UINT nAccountDBID)
{
	VIMAReconnectLogin Packet;
	memset(&Packet, 0, sizeof(VIMAReconnectLogin));

	Packet.nAccountDBID = nAccountDBID;

	AddSendData(VIMA_RECONNECTLOGIN, 0, (char*)&Packet, sizeof(Packet));
}


void CDNMasterConnection::SendReqGameID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, USHORT unVillageChannelID, int nRandomSeed, int nMapIndex, char cGateNo, TDUNGEONDIFFICULTY Difficulty, bool bDirectConnect, CDNParty * pParty, UINT nFarmDBID, char cGateSelect )
{
	VIMAReqGameID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.GameTaskType = GameTaskType;
	TxPacket.cReqGameIDType = cReqGameIDType;
	TxPacket.InstanceID = InstanceID;
	TxPacket.unVillageChannelID	= unVillageChannelID;
	TxPacket.nRandomSeed = nRandomSeed;
	TxPacket.nMapIndex = nMapIndex;
	TxPacket.cGateNo = cGateNo;
	TxPacket.cGateSelect = cGateSelect;
	TxPacket.StageDifficulty = Difficulty;
	TxPacket.bDirectConnect = bDirectConnect;
	TxPacket.nFarmDBID = nFarmDBID;

	if (pParty)
	{
#if defined( PRE_PARTY_DB )
		TxPacket.PartyData.PartyData = pParty->GetPartyStructData();
#else
		TxPacket.PartyData.cMemberMax = pParty->GetMemberMax();
		TxPacket.PartyData.ItemLootRule = pParty->GetItemLootRule();
		TxPacket.PartyData.ItemLootRank = pParty->GetItemLootRank();
		TxPacket.PartyData.cUserLvLimitMin = pParty->GetMinLevel();
#endif
		TxPacket.PartyData.nLeaderAccountDBID = pParty->GetLeaderAccountDBID();
		TxPacket.PartyData.nLeaderSessionID = pParty->GetLeaderSessionID();
		TxPacket.PartyData.nChannelID = pParty->GetChannelID();
		
#if defined( PRE_PARTY_DB )
#else
		TxPacket.PartyData.cUserLvLimitMax = pParty->GetMaxLevel();
		TxPacket.PartyData.nTargetMapIdx = pParty->GetTargetMapIndex();
		TxPacket.PartyData.Difficulty = pParty->GetDifficulty();
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
#else
		TxPacket.PartyData.cJobDice = pParty->m_cIsJobDice;
		_wcscpy(TxPacket.PartyData.wszPartyPass, _countof(TxPacket.PartyData.wszPartyPass), pParty->m_wszPartyPassword, (int)wcslen(pParty->m_wszPartyPassword));
		_wcscpy(TxPacket.PartyData.wszPartyName, _countof(TxPacket.PartyData.wszPartyName), pParty->GetPartyName(), (int)wcslen(pParty->GetPartyName()));
#endif // #if defined( PRE_PARTY_DB )		

		int nCnt = 0;
		const std::vector<UINT>& KickList = pParty->GetKickList();
		for (int i = (int)KickList.size()-1; i >= 0; i--)
		{
			TxPacket.PartyData.nKickedMemberDBID[nCnt] = KickList[i];
			nCnt++;
			if (nCnt >= PARTYKICKMAX) 
				break;
		}

#ifdef _USE_VOICECHAT
		TxPacket.PartyData.nVoiceChannelID = pParty->m_nVoiceChannelID;
#endif
#if defined( PRE_PARTY_DB )
#else
		TxPacket.PartyData.cUpkeepCount = (BYTE)pParty->GetUpkeepCount();
#endif
		UINT MemberArr[PARTYMAX];
		pParty->GetMembersByAccountDBID(MemberArr);

		bool bMemberArray[PARTYMAX];
		pParty->GetMemberCheckFlag(bMemberArray);
		
#if defined( PRE_PARTY_DB )
		TxPacket.PartyData.PartyData.iCurMemberCount = 0;
		for (int i = 0; i < PARTYMAX; i++)
		{
			if (MemberArr[i] <= 0) continue;
			TxPacket.PartyData.MemberInfo[TxPacket.PartyData.PartyData.iCurMemberCount].nAccountDBID = MemberArr[i];
			TxPacket.PartyData.MemberInfo[TxPacket.PartyData.PartyData.iCurMemberCount].cMemberIndex = i;

			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(MemberArr[i]);
#ifdef _USE_VOICECHAT			
			if (!pSession) continue;
			TxPacket.PartyData.MemberInfo[TxPacket.PartyData.PartyData.iCurMemberCount].VoiceInfo.cVoiceAvailable = pSession->m_bVoiceAvailable == true ? 1 : 0;
			memcpy(TxPacket.PartyData.MemberInfo[TxPacket.PartyData.PartyData.iCurMemberCount].VoiceInfo.nMutedList, pSession->m_nVoiceMutedList, sizeof(UINT[PARTYCOUNTMAX]));
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
			if( !pSession ) continue;
			if( pSession->GetComebackAppellation() > 0 )
				TxPacket.PartyData.MemberInfo[TxPacket.PartyData.PartyData.iCurMemberCount].bCheckComebackAppellation = true;
#endif
			TxPacket.PartyData.PartyData.iCurMemberCount++;
		}
#else	//	#if defined( PRE_PARTY_DB )
		for (int i = 0; i < PARTYMAX; i++)
		{
			if (MemberArr[i] <= 0) continue;
			TxPacket.PartyData.MemberInfo[TxPacket.PartyData.cMemberCount].nAccountDBID = MemberArr[i];
			TxPacket.PartyData.MemberInfo[TxPacket.PartyData.cMemberCount].cMemberIndex = i;
			
#ifdef _USE_VOICECHAT
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(MemberArr[i]);
			if (!pSession) continue;
			TxPacket.PartyData.MemberInfo[TxPacket.PartyData.cMemberCount].VoiceInfo.cVoiceAvailable = pSession->m_bVoiceAvailable == true ? 1 : 0;
			memcpy(TxPacket.PartyData.MemberInfo[TxPacket.PartyData.cMemberCount].VoiceInfo.nMutedList, pSession->m_nVoiceMutedList, sizeof(UINT[PARTYCOUNTMAX]));
#endif
			TxPacket.PartyData.cMemberCount++;
		}
#endif	//	#if defined( PRE_PARTY_DB )
	}

#if defined( PRE_PARTY_DB )
	AddSendData( VIMA_REQGAMEID, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) - sizeof(TxPacket.PartyData.MemberInfo) + (sizeof(TMemberInfo) * TxPacket.PartyData.PartyData.iCurMemberCount) );
#else
	AddSendData( VIMA_REQGAMEID, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) - sizeof(TxPacket.PartyData.MemberInfo) + (sizeof(TMemberInfo) * TxPacket.PartyData.cMemberCount) );
#endif
}

void CDNMasterConnection::SendPvPLadderEnterChannel( UINT uiAccountDBID, LadderSystem::MatchType::eCode MatchType )
{
	VIMALadderEnterChannel TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.MatchType		= MatchType;

	AddSendData( VIMA_LADDER_ENTERCHANNEL, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendReqLadderGameID( LadderSystem::CRoom* pLadderRoom, LadderSystem::CRoom* pLadderRoom2, USHORT unVillageChannelID, int iRandomSeed, int iMapIndex )
{
	VIMALadderReqGameID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	_ASSERT( pLadderRoom->GetMatchType() == pLadderRoom2->GetMatchType() );

	TxPacket.MatchType			= pLadderRoom->GetMatchType();
	TxPacket.biRoomIndexArr[0]	= pLadderRoom->GetRoomIndex();
	TxPacket.biRoomIndexArr[1]	= pLadderRoom2->GetRoomIndex();
	TxPacket.unVillageChannelID	= unVillageChannelID;
	TxPacket.iRandomSeed		= iRandomSeed;
	TxPacket.iMapIndex			= iMapIndex;

	const std::vector<LadderSystem::CRoom::SUserInfo>& UserInfo	 = pLadderRoom->GetUserInfo();
	const std::vector<LadderSystem::CRoom::SUserInfo>& UserInfo2 = pLadderRoom2->GetUserInfo();

	if( UserInfo.empty() || UserInfo2.empty() )
	{
		_ASSERT(0);
		return;
	}

	switch( pLadderRoom->GetMatchType() )
	{
		case LadderSystem::MatchType::_1vs1:
		{
			TxPacket.LadderData.cMemberCount = 2;
			TxPacket.LadderData.MemberInfoArr[0].uiAccountDBID	= UserInfo[0].uiAccountDBID;
			TxPacket.LadderData.MemberInfoArr[0].uiTeam			= PvPCommon::Team::A;
			TxPacket.LadderData.MemberInfoArr[1].uiAccountDBID	= UserInfo2[0].uiAccountDBID;
			TxPacket.LadderData.MemberInfoArr[1].uiTeam			= PvPCommon::Team::B;
			break;
		}
		case LadderSystem::MatchType::_2vs2:
		case LadderSystem::MatchType::_3vs3:
		case LadderSystem::MatchType::_4vs4:
#if defined(PRE_ADD_DWC)
		case LadderSystem::MatchType::_3vs3_DWC:
		case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
#endif
		{
			int iCount = 0;
			for( UINT i=0 ; i<UserInfo.size() ; ++i )
			{
				TxPacket.LadderData.MemberInfoArr[iCount].uiAccountDBID	= UserInfo[i].uiAccountDBID;
				TxPacket.LadderData.MemberInfoArr[iCount].uiTeam		= PvPCommon::Team::A;
				++iCount;
			}
			for( UINT i=0 ; i<UserInfo2.size() ; ++i )
			{
				TxPacket.LadderData.MemberInfoArr[iCount].uiAccountDBID	= UserInfo2[i].uiAccountDBID;
				TxPacket.LadderData.MemberInfoArr[iCount].uiTeam		= PvPCommon::Team::B;
				++iCount;
			}
			TxPacket.LadderData.cMemberCount = static_cast<BYTE>(iCount);
			break;
		}
	}

	pLadderRoom->ChangeRoomState( LadderSystem::RoomState::Syncing );
	pLadderRoom2->ChangeRoomState( LadderSystem::RoomState::Syncing );

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.LadderData.MemberInfoArr)+TxPacket.LadderData.cMemberCount*sizeof(TxPacket.LadderData.MemberInfoArr[0]);
	AddSendData( VIMA_LADDER_REQGAMEID, 0, reinterpret_cast<char*>(&TxPacket), iSize );
}

void CDNMasterConnection::SendLadderObserver( INT64 biCharacterDBID, int iGameServerID, int iGameRoomID )
{
	VIMALadderObserver TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.iGameServerID		= iGameServerID;
	TxPacket.iGameRoomID		= iGameRoomID;
	
	AddSendData( VIMA_LADDER_OBSERVER, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendVillageUserReport()
{
	std::map <int, int> Map;
	UINT nCount = g_pUserSessionManager->GetUserCount(Map);

	VIMAUserReport Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nUserCount = nCount;
	std::map <int, int>::iterator ii;
	for (ii = Map.begin(); ii != Map.end(); ii++)
	{
		Packet.Info[Packet.nCount].nChannelID = (*ii).first;
		Packet.Info[Packet.nCount].nChannelUserCount = (*ii).second;
		Packet.nCount++;
		if (Packet.nCount >= VILLAGECHANNELMAX )
		{
			_DANGER_POINT();
			break;
		}
	}
	
	AddSendData(VIMA_VILLAGEUSERREPORT, 0, (char*)&Packet, sizeof(Packet) - sizeof(Packet.Info) + (Packet.nCount * sizeof(TVillageUserReport)));
}

#if defined( PRE_PARTY_DB )
void CDNMasterConnection::SendVillageToVillage(BYTE cReqType, INT64 biID, int nMapIndex, int nEnteredGateIndex, int nMoveChannel, CDNParty * pParty, INT64 nItemSerial)
#else
void CDNMasterConnection::SendVillageToVillage(BYTE cReqType, UINT nID, int nMapIndex, int nEnteredGateIndex, int nMoveChannel, CDNParty * pParty, INT64 nItemSerial)
#endif // #if defined( PRE_PARTY_DB )
{
	VIMAVillageToVillage Packet;
	memset(&Packet, 0, sizeof(VIMAVillageToVillage));

	Packet.cReqType = cReqType;
#if defined( PRE_PARTY_DB )
	Packet.biID = biID;
#else
	Packet.nID = nID;
#endif // #if defined( PRE_PARTY_DB )
	Packet.nMapIndex = nMapIndex;
	Packet.nEnteredGateIndex = nEnteredGateIndex;
	Packet.nMoveChannel = nMoveChannel;
	Packet.nItemSerial = nItemSerial;

	if (pParty)
	{
		Packet.PartyData.nLeaderAccountDBID = pParty->GetLeaderAccountDBID();
		Packet.PartyData.nLeaderSessionID = pParty->GetLeaderSessionID();
#if defined( PRE_PARTY_DB )
		Packet.PartyData.PartyData = pParty->GetPartyStructData();
		Packet.PartyData.PartyData.iCurMemberCount = 0;
#else		
		Packet.PartyData.nChannelID = pParty->GetChannelID();
		Packet.PartyData.cMemberMax = pParty->GetMemberMax();
		Packet.PartyData.ItemLootRank = pParty->GetItemLootRank();
		Packet.PartyData.ItemLootRule = pParty->GetItemLootRule();
		Packet.PartyData.cUserLvLimitMin = pParty->GetMinLevel();
		Packet.PartyData.cUserLvLimitMax = pParty->GetMaxLevel();
		Packet.PartyData.nTargetMapIdx = pParty->GetTargetMapIndex();
		Packet.PartyData.Difficulty = pParty->GetDifficulty();
#endif // #if defined( PRE_PARTY_DB )

		
		
		
#if defined( PRE_PARTY_DB )
#else
		Packet.PartyData.cJobDice = pParty->m_cIsJobDice;
		_wcscpy(Packet.PartyData.wszPartyPass, _countof(Packet.PartyData.wszPartyPass), pParty->m_wszPartyPassword, (int)wcslen(pParty->m_wszPartyPassword) );
		_wcscpy(Packet.PartyData.wszPartyName, _countof(Packet.PartyData.wszPartyName), pParty->GetPartyName(), (int)wcslen(pParty->GetPartyName()));
		Packet.PartyData.cUpkeepCount = (BYTE)pParty->GetUpkeepCount();
#endif // #if defined( PRE_PARTY_DB )

		int nCnt = 0;
		const std::vector <UINT>& KickList = pParty->GetKickList();
		for (int i = (int)KickList.size()-1; i >= 0; i--)
		{
			Packet.PartyData.nKickedMemberDBID[nCnt] = KickList[i];
			nCnt++;
			if (nCnt >= PARTYKICKMAX) break;
		}

#ifdef _USE_VOICECHAT
		Packet.PartyData.nVoiceChannelID = pParty->m_nVoiceChannelID;
#endif		
		UINT MemberArr[PARTYMAX];
		pParty->GetMembersByAccountDBID(MemberArr);
#if defined( PRE_PARTY_DB )
		for (int i = 0; i < PARTYMAX; i++)
		{
			if (MemberArr[i] <= 0) continue;
			Packet.PartyData.MemberInfo[Packet.PartyData.PartyData.iCurMemberCount].nAccountDBID = MemberArr[i];
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(MemberArr[i]);
#ifdef _USE_VOICECHAT			
			if (!pSession) continue;
			Packet.PartyData.MemberInfo[Packet.PartyData.PartyData.iCurMemberCount].VoiceInfo.cVoiceAvailable = pSession->m_bVoiceAvailable == true ? 1 : 0;
			memcpy(Packet.PartyData.MemberInfo[Packet.PartyData.PartyData.iCurMemberCount].VoiceInfo.nMutedList, pSession->m_nVoiceMutedList, sizeof(UINT[PARTYCOUNTMAX]));
#endif
			Packet.PartyData.MemberInfo[Packet.PartyData.PartyData.iCurMemberCount].biCharacterDBID = pSession->GetCharacterDBID();
#if defined( PRE_ADD_NEWCOMEBACK )
			if (!pSession) continue;
			if( pSession->GetComebackAppellation() > 0 )
				Packet.PartyData.MemberInfo[Packet.PartyData.PartyData.iCurMemberCount].bCheckComebackAppellation = true;
#endif
			Packet.PartyData.PartyData.iCurMemberCount++;
		}
#else	//	#if defined( PRE_PARTY_DB )
		for (int i = 0; i < PARTYMAX; i++)
		{
			if (MemberArr[i] <= 0) continue;
			Packet.PartyData.MemberInfo[Packet.PartyData.cMemberCount].nAccountDBID = MemberArr[i];
#ifdef _USE_VOICECHAT
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(MemberArr[i]);
			if (!pSession) continue;
			Packet.PartyData.MemberInfo[Packet.PartyData.cMemberCount].VoiceInfo.cVoiceAvailable = pSession->m_bVoiceAvailable == true ? 1 : 0;
			memcpy(Packet.PartyData.MemberInfo[Packet.PartyData.cMemberCount].VoiceInfo.nMutedList, pSession->m_nVoiceMutedList, sizeof(UINT[PARTYCOUNTMAX]));
#endif
			Packet.PartyData.cMemberCount++;
		}		
#endif	//	#if defined( PRE_PARTY_DB )
	}

	AddSendData(VIMA_VILLAGETOVILLAGE, 0, (char*)&Packet, sizeof(Packet));
}

#if defined( PRE_PARTY_DB )

void CDNMasterConnection::SendReqPartyInvite( CDNUserSession* pReqSession, const WCHAR* pwszInviteCharName, CDNParty* pParty )
{
	VIMAReqPartyInvite TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.PartyID = pReqSession->GetPartyID();
	TxPacket.iPartyMinLevel = pParty->GetMinLevel();
	_wcscpy( TxPacket.wszPartyName, _countof(TxPacket.wszPartyName), pParty->GetPartyName(), static_cast<int>(wcslen(pParty->GetPartyName())) );
	TxPacket.iPartyPassword = pParty->GetPassword();
	TxPacket.iPartyMemberMax = pParty->GetMemberMax();
	TxPacket.iPartyMemberCount = pParty->GetMemberCount();
	TxPacket.iPartyAvrLV = pParty->GetAvrUserLv();
	TxPacket.uiReqAccountDBID = pReqSession->GetAccountDBID();
	TxPacket.biReqCharacterDBID = pReqSession->GetCharacterDBID();
	TxPacket.iReqMapIndex = pReqSession->GetMapIndex();
	_wcscpy( TxPacket.wszReqCharName, _countof(TxPacket.wszReqCharName), pReqSession->GetCharacterName(), static_cast<int>(wcslen(pReqSession->GetCharacterName())) );
	_wcscpy(TxPacket.wszInviteCharName, _countof(TxPacket.wszInviteCharName), pwszInviteCharName, static_cast<int>(wcslen(pwszInviteCharName)) );

	AddSendData( VIMA_REQPARTYINVITE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendResPartyInvite( int iRet, UINT uiReqAccountDBID )
{
	VIMAResPartyInvite TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet = iRet;
	TxPacket.uiReqAccountDBID = uiReqAccountDBID;

	AddSendData( VIMA_RESPARTYINVITE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPartyInviteDenied( const WCHAR* pwszReqCharName, TPARTYID PartyID, const WCHAR* pwszInviteCharName, BYTE cIsOpenBlind )
{
	VIMAPartyInviteDenied TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.PartyID = PartyID;
	TxPacket.cIsOpenBlind = cIsOpenBlind;
	_wcscpy( TxPacket.wszReqCharName, _countof(TxPacket.wszReqCharName), pwszReqCharName, static_cast<int>(wcslen(pwszReqCharName)) );
	_wcscpy( TxPacket.wszInviteCharName, _countof(TxPacket.wszInviteCharName), pwszInviteCharName, static_cast<int>(wcslen(pwszInviteCharName)) );

	AddSendData( VIMA_PARTYINVITEDENIED, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
void CDNMasterConnection::SendGetPartyID( UINT nSenderAccountDBID , const WCHAR* pwszCharName)
{
	VIMAGetPartyID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nSenderAccountDBID = nSenderAccountDBID;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, static_cast<int>(wcslen(pwszCharName)) );

	AddSendData( VIMA_GETPARTYID, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendGetPartyIDResult( UINT nSenderAccountDBID , TPARTYID PartyID )
{
	VIMAGetPartyIDResult TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nSenderAccountDBID = nSenderAccountDBID;
	TxPacket.PartyID = PartyID;

	AddSendData( VIMA_GETPARTYID_RESULT, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendReqPartyAskJoin(CDNUserSession* pReqSession, const WCHAR* pwszTargetCharName)
{
	VIMAReqPartyAskJoin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiReqAccountDBID = pReqSession->GetAccountDBID();
	TxPacket.biReqCharacterDBID = pReqSession->GetCharacterDBID();
	TxPacket.cReqUserJob = pReqSession->GetUserJob();
	TxPacket.cReqUserLevel = pReqSession->GetLevel();
	_wcscpy( TxPacket.wszReqChracterName, _countof(TxPacket.wszReqChracterName), pReqSession->GetCharacterName(), static_cast<int>(wcslen(pReqSession->GetCharacterName())) );
	_wcscpy(TxPacket.wszTargetChracterName, _countof(TxPacket.wszTargetChracterName), pwszTargetCharName, static_cast<int>(wcslen(pwszTargetCharName)) );

	AddSendData( VIMA_REQPARTYASKJOIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendResPartyAskJoin(int iRet, UINT uiReqAccountDBID, const WCHAR* pwszReqChracterName )
{
	VIMAResPartyAskJoin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	
	TxPacket.iRet = iRet;
	TxPacket.uiReqAccountDBID = uiReqAccountDBID;
	if(pwszReqChracterName)
		_wcscpy(TxPacket.wszReqChracterName, _countof(TxPacket.wszReqChracterName), pwszReqChracterName, static_cast<int>(wcslen(pwszReqChracterName)) );

	AddSendData( VIMA_RESPARTYASKJOIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendAskJoinAgreeInfo( const WCHAR* pwszAskerCharName, TPARTYID PartyID, int iPassword)
{
	VIMAAskJoinAgreeInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	TxPacket.PartyID = PartyID;
	TxPacket.iPassword = iPassword;
	_wcscpy(TxPacket.wszAskerCharName, _countof(TxPacket.wszAskerCharName), pwszAskerCharName, static_cast<int>(wcslen(pwszAskerCharName)) );

	AddSendData( VIMA_ASKJOINAGREEINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#endif	//#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)

#else

void CDNMasterConnection::SendReqPartyID(UINT nLeaderAccountDBID, int nChannelID, BYTE cMemberMax, TPARTYITEMLOOTRULE LootRule, TITEMRANK LootItemRank, int nUserLvLimitMin, \
		int nUserLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cIsPartyJobDice, const WCHAR * pwszPartyName, const WCHAR * pPass, UINT nAfterInviteDBID, bool bUseVoice)
{
	VIMAReqPartyID Packet;
	memset(&Packet, 0, sizeof(VIMAReqPartyID));

	Packet.nAccountDBID = nLeaderAccountDBID;
	Packet.nChannelID = nChannelID;
	Packet.cMemberMax = cMemberMax;

	Packet.ItemLootRule = LootRule;
	Packet.ItemLootRank = LootItemRank;
	Packet.cUserLvLimitMin = (BYTE)nUserLvLimitMin;
	Packet.cUserLvLimitMax = (BYTE)nUserLvLimitMax;

	Packet.nCreateAfterInvite = nAfterInviteDBID;
	Packet.nTargetMapIdx = nTargetMapIdx;
	Packet.Difficulty = Difficulty;
	Packet.cPartyJobDice = cIsPartyJobDice;
#ifdef _USE_VOICECHAT
	Packet.cUseVoice = bUseVoice == true ? 1 : 0;
#endif

	_wcscpy(Packet.wszBuf, _countof(Packet.wszBuf), pwszPartyName, (int)wcslen(pwszPartyName));
	Packet.cNameLen = (BYTE)wcslen(pwszPartyName);

	if (pPass != NULL && pPass[0] != '\0')
	{
		_wcscpy(Packet.wszBuf + Packet.cNameLen, _countof(Packet.wszBuf) - Packet.cNameLen, pPass, (int)wcslen(pPass));
		Packet.cPassWordLen = (BYTE)wcslen(pPass);
	}

	AddSendData(VIMA_REQPARTYID, 0, (char*)&Packet, sizeof(Packet) - sizeof(Packet.wszBuf) + \
		((Packet.cNameLen + Packet.cPassWordLen) * sizeof(WCHAR)));
}

#endif // #if defined( PRE_PARTY_DB )

void CDNMasterConnection::SendInvitePartyResult(int nGameServerID, const WCHAR * pwszInviterName, const WCHAR * pwszInvitedName, int nRetCode)
{
	VIMAInvitePartyMemberResult Packet;
	memset(&Packet, 0, sizeof(VIMAInvitePartyMemberResult));

	Packet.nGameServerID = nGameServerID;
	if (pwszInviterName) _wcscpy(Packet.wszInviterName, NAMELENMAX, pwszInviterName, (int)wcslen(pwszInviterName));
	if (pwszInvitedName) _wcscpy(Packet.wszInvitedName, NAMELENMAX, pwszInvitedName, (int)wcslen(pwszInvitedName));
	Packet.nRetCode = nRetCode;

	AddSendData(VIMA_RETINVITEPARTYMEMBER, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendLoginState(UINT nSessionID)
{
	VIMALoginState State = { 0, };
	State.nSessionID = nSessionID;
	AddSendData(VIMA_LOGINSTATE, 0, (char*)&State, sizeof(VIMALoginState));
}

void CDNMasterConnection::SendInviteGuildMember(TGuildUID GuildUID, UINT nAccountDBID, UINT nSessionID, INT64 nCharacterDBID, LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, LPCWSTR lpwszGuildName)
{
	MAInviteGuildMember Packet;
	Packet.GuildUID		= GuildUID;
	Packet.nAccountDBID		= nAccountDBID;
	Packet.nSessionID		= nSessionID;
	Packet.nCharacterDBID	= nCharacterDBID;
	_wcscpy(Packet.wszFromCharacterName, _countof(Packet.wszFromCharacterName), lpwszFromCharacterName, (int)wcslen(lpwszFromCharacterName));
	_wcscpy(Packet.wszToCharacterName, _countof(Packet.wszToCharacterName), lpwszToCharacterName, (int)wcslen(lpwszToCharacterName));
	_wcscpy(Packet.wszGuildName, _countof(Packet.wszGuildName), lpwszGuildName, (int)wcslen(lpwszGuildName));
	
	AddSendData(VIMA_INVAITEGUILDMEMB, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendInviteGuildResult(UINT nAccountDBID, int nRetCode, bool bAck, LPCWSTR pwszToCharacterName)
{
	VIMAInviteGuildMemberResult Packet;
	memset(&Packet, 0, sizeof(VIMAInviteGuildMemberResult));

	Packet.nRetCode		= nRetCode;
	Packet.nInviterDBID = nAccountDBID;
	Packet.bAck			= bAck;

	if (pwszToCharacterName)
		_wcscpy(Packet.wszInvitedName, NAMELENMAX, pwszToCharacterName, (int)wcslen(pwszToCharacterName));
	
	AddSendData(VIMA_RETINVITEGUILDMEMBER, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendGuildWareInfo(TGuildUID GuildUID)
{
	MAGuildWareInfo Packet;

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = GuildUID;

	AddSendData (VIMA_GUILDWAREINFO, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendGuildWareInfoResult(MAGuildWareInfoResult& result)
{
	int nSize = sizeof(MAGuildWareInfoResult) - sizeof(result.WareSlotList) + (sizeof(TGuildWareSlot)*result.cSlotListCount);

	AddSendData (VIMA_RETGUILDWAREINFO, 0, (char*)&result, nSize);
}

void CDNMasterConnection::SendGuildMemberLevelUp(TGuildUID GuildUID, INT64 nCharacterDBID, char cLevel)
{
	MAGuildMemberLevelUp Packet;

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = GuildUID;
	Packet.nCharacterDBID = nCharacterDBID;
	Packet.cLevel = cLevel;

	AddSendData(VIMA_GUILDMEMBER_LEVELUP, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendDismissGuild(const TGuildUID pGuildUID)
{
	MADismissGuild Packet;

	Packet.GuildUID = pGuildUID;
	Packet.nManagedID = g_Config.nManagedID;

	AddSendData(VIMA_DISMISSGUILD, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendAddGuildMember(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszFromCharacterName, TP_JOB nJob, char cLevel, __time64_t pJoinDate)
{
	MAAddGuildMember Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;
	Packet.nJob = nJob;
	Packet.cLevel = cLevel;
	Packet.JoinDate = pJoinDate;
	_wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), lpwszFromCharacterName, (int)wcslen(lpwszFromCharacterName));

	AddSendData(VIMA_ADDGUILDMEMB, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendDelGuildMember(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, bool bIsExiled)
{
	MADelGuildMember Packet;

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;
	Packet.bIsExiled = bIsExiled;

	AddSendData(VIMA_DELGUILDMEMB, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendChangeGuildInfo(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, BYTE btGuildUpdate, int iInt1, int iInt2, int iInt3, int iInt4, INT64 biInt64, LPCWSTR pText)
{
	MAChangeGuildInfo Packet;

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;
	Packet.btGuildUpdate = btGuildUpdate;
	Packet.Int1 = iInt1;
	Packet.Int2 = iInt2;
	Packet.Int64 = biInt64;
	Packet.Int3 = iInt3;
	Packet.Int4 = iInt4;
	if (pText) {
		_wcscpy(Packet.Text, _countof(Packet.Text), pText, (int)wcslen(pText));
	}

	AddSendData(VIMA_CHANGEGUILDINFO, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendRefreshGuildItem (const TGuildUID pGuildUID, char cType, INT64 biSrcSerial, INT64 biDestSerial, const TItemInfo *pSrcInfo, const TItemInfo *pDestInfo, __time64_t VersionDate)
{
	MARefreshGuildItem Refresh;
	memset (&Refresh, 0, sizeof(Refresh));

	Refresh.nManagedID = g_Config.nManagedID;
	Refresh.GuildUID = pGuildUID;

	Refresh.cMoveType = cType;
	Refresh.biSrcSerial = biSrcSerial;
	Refresh.biDestSerial = biDestSerial;
	Refresh.VersionDate = VersionDate;

	if (pSrcInfo) 
		Refresh.SrcInfo = *pSrcInfo;

	if (pDestInfo) 
		Refresh.DestInfo = *pDestInfo;

	AddSendData(VIMA_REFRESH_GUILDITEM, 0, reinterpret_cast<char*>(&Refresh), sizeof(Refresh));
}

void CDNMasterConnection::SendRefreshGuildCoin (const TGuildUID pGuildUID, INT64 biTotalCoin)
{
	MARefreshGuildCoin Refresh;
	memset (&Refresh, 0, sizeof(Refresh));

	Refresh.nManagedID	= g_Config.nManagedID;
	Refresh.GuildUID	= pGuildUID;
	Refresh.biTotalCoin = biTotalCoin;
	
	AddSendData(VIMA_REFRESH_GUILDCOIN, 0, reinterpret_cast<char*>(&Refresh), sizeof(Refresh));
}

void CDNMasterConnection::SendExtendGuildWareSize (const TGuildUID pGuildUID, short wSize)
{
	MAExtendGuildWare Packet;
	memset (&Packet, 0, sizeof(Packet));

	Packet.nManagedID	= g_Config.nManagedID;
	Packet.GuildUID		= pGuildUID;
	Packet.wWareSize	= wSize;

	AddSendData(VIMA_EXTEND_GUILDWARE, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendChangeGuildMemberInfo(const TGuildUID pGuildUID, UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText, bool bReturn)
{
	MAChangeGuildMemberInfo Packet;
	memset (&Packet, 0, sizeof(Packet));

	if(bReturn)	//자기 자신이 패킷을 다시 받아야 할 경우
		Packet.nManagedID = 0;
	else
		Packet.nManagedID = g_Config.nManagedID;

	Packet.GuildUID = pGuildUID;
	Packet.nReqAccountDBID = nReqAccountDBID;
	Packet.nReqCharacterDBID = nReqCharacterDBID;
	Packet.nChgAccountDBID = nChgAccountDBID;
	Packet.nChgCharacterDBID = nChgCharacterDBID;
	Packet.btGuildMemberUpdate = btGuildMemberUpdate;
	Packet.Int1 = iInt1;
	Packet.Int2 = iInt2;
	Packet.Int64 = biInt64;
	if (pText) {
		_wcscpy(Packet.Text, _countof(Packet.Text), pText, (int)wcslen(pText));
	}

	AddSendData(VIMA_CHANGEGUILDMEMBINFO, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendGuildChat(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen)
{
	MAGuildChat Packet;
	memset (&Packet, 0, sizeof(Packet));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;

	_wcscpy( Packet.wszChatMsg, _countof(Packet.wszChatMsg), lpwszChatMsg, nLen );
	Packet.nLen = nLen;

	int iSize = static_cast<int>(sizeof(Packet) - sizeof(Packet.wszChatMsg) + sizeof(Packet.wszChatMsg[0]) * Packet.nLen);

	AddSendData(VIMA_GUILDCHAT, 0, reinterpret_cast<char*>(&Packet), iSize);
}

void CDNMasterConnection::SendGuildChangeName(const TGuildUID pGuildUID, LPCWSTR lpwszGuildName)
{
	MAGuildChangeName Packet;
	memset(&Packet, 0, sizeof(MAGuildChangeName));

	Packet.GuildUID = pGuildUID;
	_wcscpy( Packet.wszGuildName, _countof(Packet.wszGuildName), lpwszGuildName, (int)wcslen(lpwszGuildName) );

	AddSendData(VIMA_GUILDCHANGENAME, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildChangeName));
}

void CDNMasterConnection::SendGuildChangeMark(const TGuildUID pGuildUID, short wMark, short wMarkBG, short wMarkBorder)
{
	MAGuildChangeMark Packet;
	memset(&Packet, 0, sizeof(MAGuildChangeMark));

	Packet.GuildUID = pGuildUID;
	Packet.wMark = wMark;
	Packet.wMarkBG = wMarkBG;
	Packet.wMarkBorder = wMarkBorder;
	
	AddSendData(VIMA_GUILDCHANGEMARK, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildChangeMark));
}

void CDNMasterConnection::SendUpdateGuildExp(const TGuildUID pGuildUID, const TAModGuildExp* pPacket)
{
	MAUpdateGuildExp Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.cPointType = pPacket->cPointType;
	Packet.nPointValue = pPacket->nPointValue;
	Packet.nTotalGuildExp = pPacket->nTotalGuildExp;
	Packet.nDailyPointValue = pPacket->nDailyPointValue;
	Packet.nLevel = pPacket->nLevel;
	Packet.biCharacterDBID = pPacket->biCharacterDBID;
	Packet.nMissionID = pPacket->nMissionID;

	AddSendData(VIMA_UPDATEGUILDEXP, 0, reinterpret_cast<char*>(&Packet), sizeof(MAUpdateGuildExp));
}

void CDNMasterConnection::SendEnrollGuildWar (const TGuildUID pGuildUID, short wScheduleID, BYTE cTeamColorCode)
{
	MAEnrollGuildWar Packet;
	memset(&Packet, 0, sizeof(MAEnrollGuildWar));

	Packet.nManagedID	= g_Config.nManagedID;
	Packet.GuildUID		= pGuildUID;
	Packet.wScheduleID	= wScheduleID;
	Packet.cTeamColorCode = cTeamColorCode;

	AddSendData(VIMA_ENROLLGUILDWAR, 0, reinterpret_cast<char*>(&Packet), sizeof(MAEnrollGuildWar));
}

void CDNMasterConnection::SendSetGuildWarSchedule(int nResultCode, short wSchdeduleID, short wWinersWeightRate, TGuildWarEventInfo* pEventInfo, bool bForce, bool bFinalProgress, __time64_t tRewardExpireDate)
{
	VIMASetGuildWarSchedule Packet;
	memset(&Packet, 0, sizeof(VIMASetGuildWarSchedule));

	Packet.nResultCode = nResultCode;
	Packet.wScheduleID = wSchdeduleID;
	Packet.wWinersWeightRate = wWinersWeightRate;
	Packet.bForce = bForce;
	Packet.bFinalProgress = bFinalProgress;
	Packet.tRewardExpireDate = tRewardExpireDate;
	memcpy(Packet.EventInfo, pEventInfo, sizeof(Packet.EventInfo));
	AddSendData(VIMA_SETGUILDWAR_SCHEDULE, 0, reinterpret_cast<char*>(&Packet), sizeof(VIMASetGuildWarSchedule));
}

void CDNMasterConnection::SendSetGuildWarFinalSchedule(TGuildWarEventInfo* pFinalSchedule)
{
	MASetGuildWarFinalSchedule Packet;
	memset(&Packet, 0, sizeof(MASetGuildWarFinalSchedule));

	memcpy(Packet.GuildWarFinalSchedule, pFinalSchedule, sizeof(Packet.GuildWarFinalSchedule));

	AddSendData(VIMA_SETGUILDWAR_FINALSCHEDULE, 0, reinterpret_cast<char*>(&Packet), sizeof(MASetGuildWarFinalSchedule));
}

void CDNMasterConnection::SendSetGuildWarPoint(int nResultCode, int nBlueTeamPoint, int nRedTeamPoint)
{
	MASetGuildWarPoint Packet;
	memset(&Packet, 0, sizeof(MASetGuildWarPoint));

	Packet.nResultCode = nResultCode;
	Packet.nBlueTeamPoint = nBlueTeamPoint;
	Packet.nRedTeamPoint = nRedTeamPoint;

	AddSendData(VIMA_SETGUILDWAR_POINT, 0, reinterpret_cast<char*>(&Packet), sizeof(MASetGuildWarPoint));
}

void CDNMasterConnection::SendAddGuildWarPoint(char cTeamType, TGuildUID GuildUID, int nAddPoint)
{
	MAAddGuildWarPoint Packet;
	memset(&Packet, 0, sizeof(MAAddGuildWarPoint));

	Packet.cTeamType = cTeamType;
	Packet.GuildUID = GuildUID;
	Packet.nAddPoint = nAddPoint;

	AddSendData(VIMA_ADDGUILDWAR_POINT, 0, reinterpret_cast<char*>(&Packet), sizeof(MAAddGuildWarPoint));
}

void CDNMasterConnection::SendSetGuildWarTournamentInfo(TAGetGuildWarFinalResults* pGuildWarFinalResults)
{
	MAGuildWarTournamentInfo Packet;
	memset(&Packet, 0, sizeof(MAGuildWarTournamentInfo));

	char cMatchSequence = 0;
	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{
		if( pGuildWarFinalResults->GuildFinalResultInfo[i].nGuildDBID == 0 )
			break;
		cMatchSequence = pGuildWarFinalResults->GuildFinalResultInfo[i].cMatchSequence -1;	//
		if( cMatchSequence < 0 || cMatchSequence >= GUILDWAR_FINALS_TEAM_MAX)
			break;

		TGuildUID GuidUID(g_Config.nWorldSetID, pGuildWarFinalResults->GuildFinalResultInfo[i].nGuildDBID);
		Packet.sGuildWarFinalInfo[cMatchSequence].GuildUID = GuidUID;
		Packet.sGuildWarFinalInfo[cMatchSequence].cMatchTypeCode = pGuildWarFinalResults->GuildFinalResultInfo[i].cMatchTypeCode;
		memcpy(Packet.sGuildWarFinalInfo[cMatchSequence].wszGuildName, pGuildWarFinalResults->GuildFinalResultInfo[i].wszGuildName, sizeof(Packet.sGuildWarFinalInfo[cMatchSequence].wszGuildName) );

		Packet.sGuildWarFinalInfo[cMatchSequence].wGuildMark			= pGuildWarFinalResults->GuildFinalResultInfo[i].wGuildMark;
		Packet.sGuildWarFinalInfo[cMatchSequence].wGuildMarkBG			= pGuildWarFinalResults->GuildFinalResultInfo[i].wGuildMarkBG;
		Packet.sGuildWarFinalInfo[cMatchSequence].wGuildMarkBorder		= pGuildWarFinalResults->GuildFinalResultInfo[i].wGuildMarkBorder;

		if(pGuildWarFinalResults->GuildFinalResultInfo[i].cMatchResultCode == 1 
			|| pGuildWarFinalResults->GuildFinalResultInfo[i].cMatchResultCode == 4) //1=승리, 2=패배, 3=기권, 4=부전승
		{
			Packet.sGuildWarFinalInfo[cMatchSequence].bWin = true;
		}
		else
			Packet.sGuildWarFinalInfo[cMatchSequence].bWin = false;
	}
	AddSendData(VIMA_SETGUILDWAR_TOURNAMENTINFO, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildWarTournamentInfo));
}

void CDNMasterConnection::SendSetGuildWarPreWinGuild(TGuildUID GuildUID, short wScheduleID)
{
	MAGuildWarPreWinGuild Packet;
	memset(&Packet, 0, sizeof(MAGuildWarPreWinGuild));

	Packet.GuildUID = GuildUID;
	Packet.wScheduleID = wScheduleID;
	AddSendData(VIMA_SETGUILDWAR_PRE_WIN_GUILD, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildWarPreWinGuild));
}

void CDNMasterConnection::SendSetGuildWarFinalTeam(TAGetGuildWarPointGuildTotal* pGuildWarPointGuildTotal)
{
	VIMASetGuildWarFinalTeam Packet;
	memset(&Packet, 0, sizeof(VIMASetGuildWarFinalTeam));
	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{
		if( pGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].nGuildDBID <= 0 || pGuildWarPointGuildTotal->nTotalCount == Packet.nCount)
			break;
		TGuildUID GuildUID(g_Config.nWorldSetID, pGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].nGuildDBID);
		Packet.GuidUID[i] = GuildUID;
		memcpy(Packet.wszGuildName[i], pGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].wszGuildName, sizeof(pGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].wszGuildName));
		Packet.nTotalPoint[i] = pGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].nOpeningPointSum;
		++Packet.nCount;
	}

	AddSendData(VIMA_SETGUILDWAR_FINAL, 0,reinterpret_cast<char*>(&Packet), sizeof(VIMASetGuildWarFinalTeam));
}

void CDNMasterConnection::SendSetGuildWarPointRunning(TAGetGuildWarPointRunningTotal* pGuildWarPointRunningTotal )
{
	MASetGuildWarPointRunningTotal Packet;
	memset(&Packet, 0, sizeof(MASetGuildWarPointRunningTotal));
	int nTotal = 0;
	for( int i=0; i<GUILDWAR_TRIAL_POINT_TEAM_MAX; ++i)
	{
		if( pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].nGuildDBID <=0 || pGuildWarPointRunningTotal->nTotalCount == nTotal )
			break;
		Packet.sGuildWarPointRunningTotal[i].wRanking = pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].wRanking;
		memcpy(Packet.sGuildWarPointRunningTotal[i].wszGuildName, pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].wszGuildName, sizeof(Packet.sGuildWarPointRunningTotal[i].wszGuildName));
		memcpy(Packet.sGuildWarPointRunningTotal[i].wszGuildMasterName, pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].wszMasterName,sizeof(Packet.sGuildWarPointRunningTotal[i].wszGuildMasterName));
		Packet.sGuildWarPointRunningTotal[i].wCurGuildUserCount = pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].wGuildMemberCount;
		Packet.sGuildWarPointRunningTotal[i].nTotalPoint = pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].nOpeningPointSum;

		Packet.sGuildWarPointRunningTotal[i].wMaxGuildUserCount = pGuildWarPointRunningTotal->sGuildWarPointGuildTotal[i].nMaxMemberCount;
		++nTotal;
	}
	AddSendData(VIMA_SETGUILDWAR_POINT_RUNNING_TOTAL, 0, reinterpret_cast<char*>(&Packet), sizeof(MASetGuildWarPointRunningTotal));
}
void CDNMasterConnection::SendSetGuildWarPreWinReward()
{
	AddSendData(VIMA_SETGUILDWAR_PRE_WIN_REWARD, 0,NULL, 0);
}

void CDNMasterConnection::SendGuildWarRefreshGuildPoint(TAGetGuildWarPointFinalRewards* pPointFinalRewards )
{
	MAGuildWarRefreshGuildPoint Packet;
	memset(&Packet, 0, sizeof(MAGuildWarRefreshGuildPoint));

	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{
		if( pPointFinalRewards->nGuildDBID[i] == 0 )
			break;
		TGuildUID GuildUID(g_Config.nWorldSetID, pPointFinalRewards->nGuildDBID[i] );
		Packet.GuildUID[i] = GuildUID;
		Packet.nGuildPoint[i] = pPointFinalRewards->nGuildPoint[i];
	}

	AddSendData(VIMA_GUILDWAR_REFRESH_GUILD_POINT, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildWarRefreshGuildPoint));
}
void CDNMasterConnection::SendGuildWarScheduleReload()
{
	AddSendData(VIMA_GUILDWAR_SCHEDULE_RELOAD, 0,NULL, 0);
}

void CDNMasterConnection::SendSetGuilldWarPreWinSkillCoolTime(int nManagedID, DWORD dwSkillCoolTime)
{
	MAGuildWarPreWinSkillCoolTime Packet;

	memset(&Packet, 0, sizeof(MAGuildWarPreWinSkillCoolTime));
	Packet.nManagedID = nManagedID;
	Packet.dwSkillCoolTime = dwSkillCoolTime;

	AddSendData(VIMA_SETGUILDWAR_PREWIN_SKILLCOOLTIME, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildWarPreWinSkillCoolTime));
}

void CDNMasterConnection::SendAddJobReserve(int nResultCode, char cJobType, int nJobSeq)
{
	VIMAAddDBJobSystemReserve Packet;

	memset(&Packet, 0, sizeof(VIMAAddDBJobSystemReserve));
	Packet.nResultCode = nResultCode;
	Packet.cJobType = cJobType;
	Packet.nJobSeq = nJobSeq;

	AddSendData(VIMA_ADD_DBJOBSYSTEM_RESERVE, 0, reinterpret_cast<char*>(&Packet), sizeof(VIMAAddDBJobSystemReserve));
}

void CDNMasterConnection::SendGetJobReserve(int nResultCode, int nJobSeq, char cJobStatus)
{
	VIMAGetDBJobSystemReserve Packet;

	memset(&Packet, 0, sizeof(VIMAGetDBJobSystemReserve));
	Packet.nResultCode = nResultCode;
	Packet.nJobSeq = nJobSeq;
	Packet.cJobStatus = cJobStatus;

	AddSendData(VIMA_GET_DBJOBSYSTEM_RESERVE, 0, reinterpret_cast<char*>(&Packet), sizeof(VIMAGetDBJobSystemReserve));
}
void CDNMasterConnection::SendMatchListSaveResult(int nResultCode)
{
	VIMAMatchListSaveResult Packet;
	memset(&Packet, 0, sizeof(Packet));
	Packet.nResultCode = nResultCode;

	AddSendData(VIMA_MATCHLIST_SAVE_RESULT, 0, reinterpret_cast<char*>(&Packet), sizeof(VIMAMatchListSaveResult));
}

void CDNMasterConnection::SendGuildRecruitMemberResult( const TGuildUID pGuildUID, WCHAR* wszCharacterName, INT64 nCharacterDBID, TGuildView GuildView, GuildRecruitSystem::AcceptType::eType type )
{
	MAGuildRecruitMember Packet;
	memset(&Packet, 0, sizeof(Packet));
	Packet.nManagedID	= g_Config.nManagedID;
	Packet.cAcceptType = type;
	if (wszCharacterName) 
		_wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), wszCharacterName, (int)wcslen(wszCharacterName));
	Packet.biCharacterDBID = nCharacterDBID;
	Packet.GuildUID = pGuildUID;
	Packet.GuildView = GuildView;

	AddSendData(VIMA_GUILDRECRUIT_MEBMER, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildRecruitMember));
}
void CDNMasterConnection::SendAddGuildRewardItem( const TGuildUID pGuildUID, TAAddGuildRewardItem* GuildRewardItem )
{
	MAGuildRewardItem Packet;
	memset(&Packet, 0, sizeof(Packet));
	Packet.nManagedID	= g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nItemID = GuildRewardItem->nItemID;
	Packet.cItemBuyType = GuildRewardItem->cItemBuyType;
	Packet.m_tExpireDate = GuildRewardItem->m_tExpireDate;

	AddSendData(VIMA_GUILD_ADD_REWARDITEM, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildRewardItem));
}
void CDNMasterConnection::SendExtendGuildSize( const TGuildUID pGuildUID, short wSize )
{
	MAExtendGuildSize Packet;
	memset(&Packet, 0, sizeof(Packet));
	Packet.nManagedID	= g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nGuildSize = wSize;

	AddSendData(VIMA_GUILD_EXTEND_GUILDSIZE, 0, reinterpret_cast<char*>(&Packet), sizeof(MAExtendGuildSize));
}

void CDNMasterConnection::SendChangeCharacterName(const TGuildUID pGuildUID, char cWorldSetID, UINT uiAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszOriginName, LPCWSTR lpwszCharacterName, INT64 biBFCharacterDBID)
{
	MAChangeCharacterName Packet;
	memset(&Packet, 0, sizeof(MAChangeCharacterName));

	Packet.nManagedID	= g_Config.nManagedID;
	Packet.GuildUID		= pGuildUID;
	Packet.cWorldSetID	= cWorldSetID;
	Packet.uiAccountDBID  = uiAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;
#if defined (PRE_ADD_BESTFRIEND)
	Packet.biBFCharacterDBID = biBFCharacterDBID;
#endif

	if (lpwszOriginName) _wcscpy(Packet.wszOriginName, _countof(Packet.wszOriginName), lpwszOriginName, (int)wcslen(lpwszOriginName));
	if (lpwszCharacterName) _wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), lpwszCharacterName, (int)wcslen(lpwszCharacterName));

	AddSendData(VIMA_CHANGECHARACTERNAME, 0, reinterpret_cast<char*>(&Packet), sizeof(MAChangeCharacterName));
}

// MasterSystem
void CDNMasterConnection::SendCheckMasterApplication( const INT64 biMasterCharacterDBID, const INT64 biPupilCharacterDBID )
{
	MasterSystem::VIMACheckMasterApplication TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biMasterCharacterDBID	= biMasterCharacterDBID;
	TxPacket.biPupilCharacterDBID	= biPupilCharacterDBID;

	AddSendData( VIMA_MASTERSYSTEM_CHECKMASTERAPPLICATION, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncSimpleInfo( const INT64 biCharacterDBID, MasterSystem::EventType::eCode Type )
{
	MasterSystem::VIMASyncSimpleInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.EventCode			= Type;

	AddSendData( VIMA_MASTERSYSTEM_SYNC_SIMPLEINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncJoin( const INT64 biCharacterDBID, bool bIsAddPupil )
{
	MasterSystem::VIMASyncJoin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.bIsAddPupil		= bIsAddPupil;

	AddSendData( VIMA_MASTERSYSTEM_SYNC_JOIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncLeave( const INT64 biCharacterDBID, bool bIsDelPupil )
{
	MasterSystem::VIMASyncLeave TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.bIsDelPupil		= bIsDelPupil;

	AddSendData( VIMA_MASTERSYSTEM_SYNC_LEAVE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncGraduate( const INT64 biCharacterDBID, WCHAR* pwszCharName )
{
	MasterSystem::VIMASyncGraduate TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, (int)wcslen(pwszCharName) );

	AddSendData( VIMA_MASTERSYSTEM_SYNC_GRADUATE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncConnect( const bool bIsConnect, WCHAR* pwszCharName, const TMasterSystemData* pMasterSystemData )
{
	// 사제 정보가 없으면 동기 맞출 필요가 없다.
	if( pMasterSystemData->SimpleInfo.cCharacterDBIDCount == 0 )
		return;

	MasterSystem::VIMASyncConnect TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.bIsConnect				= bIsConnect;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, (int)wcslen(pwszCharName) );
	TxPacket.cCharacterDBIDCount	= pMasterSystemData->SimpleInfo.cCharacterDBIDCount;
	for( UINT i=0 ; i<TxPacket.cCharacterDBIDCount ; ++i )
		TxPacket.CharacterDBIDList[i] = pMasterSystemData->SimpleInfo.OppositeInfo[i].CharacterDBID;

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.CharacterDBIDList)+TxPacket.cCharacterDBIDCount*sizeof(TxPacket.CharacterDBIDList[0]);
	AddSendData( VIMA_MASTERSYSTEM_SYNC_CONNECT, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemCheckLeave( UINT uiAccountDBID, const INT64 biDestCharacterDBID, bool bIsMaster )
{
	MasterSystem::VIMACheckLeave TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID			= uiAccountDBID;
	TxPacket.biDestCharacterDBID	= biDestCharacterDBID;
	TxPacket.bIsMaster				= bIsMaster;

	AddSendData( VIMA_MASTERSYSTEM_CHECK_LEAVE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemRecallMaster( UINT uiPupilAccountDBID, WCHAR* pwszPupilCharName, WCHAR* pwszMasterCharName, const TMasterSystemData* pMasterSystemData  )
{
	MasterSystem::VIMARecallMaster TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiPupilAccountDBID	= uiPupilAccountDBID;
	_wcscpy( TxPacket.wszPupilCharName, _countof(TxPacket.wszPupilCharName), pwszPupilCharName, (int)wcslen(pwszPupilCharName) );
	_wcscpy( TxPacket.wszMasterCharName, _countof(TxPacket.wszMasterCharName), pwszMasterCharName, (int)wcslen(pwszMasterCharName) );
	TxPacket.cCharacterDBIDCount = pMasterSystemData->SimpleInfo.cCharacterDBIDCount;
	for( UINT i=0 ; i<pMasterSystemData->SimpleInfo.cCharacterDBIDCount ; ++i )
		TxPacket.CharacterDBIDList[i] = pMasterSystemData->SimpleInfo.OppositeInfo[i].CharacterDBID;

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.CharacterDBIDList)+TxPacket.cCharacterDBIDCount*sizeof(TxPacket.CharacterDBIDList[0]);

	AddSendData( VIMA_MASTERSYSTEM_RECALL_MASTER, 0, reinterpret_cast<char*>(&TxPacket), iSize );
}

void CDNMasterConnection::SendMasterSystemBreakInto( int iRet, UINT uiMasterAccountDBID, WCHAR* pwszPupilCharName, const TMasterSystemData* pMasterSystemData )
{
	MasterSystem::VIMABreakInto TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet					= iRet;
	TxPacket.uiMasterAccountDBID	= uiMasterAccountDBID;
	_wcscpy( TxPacket.wszPupilCharName, _countof(TxPacket.wszPupilCharName), pwszPupilCharName, (int)wcslen(pwszPupilCharName) );
	TxPacket.cCharacterDBIDCount	= pMasterSystemData->SimpleInfo.cCharacterDBIDCount;
	for( UINT i=0 ; i<TxPacket.cCharacterDBIDCount ; ++i )
		TxPacket.CharacterDBIDList[i] = pMasterSystemData->SimpleInfo.OppositeInfo[i].CharacterDBID;

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.CharacterDBIDList)+TxPacket.cCharacterDBIDCount*sizeof(TxPacket.CharacterDBIDList[0]);
	
	AddSendData( VIMA_MASTERSYSTEM_BREAKINTO, 0, reinterpret_cast<char*>(&TxPacket), iSize );
}

void CDNMasterConnection::SendMasterSystemJoinConfirm( const INT64 biMasterCharacterDBID, const BYTE cLevel, const BYTE cJob, WCHAR* pwszPupilCharName)
{
	MasterSystem::VIMAJoinConfirm TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biMasterCharacterDBID = biMasterCharacterDBID;
	TxPacket.cLevel				= cLevel;
	TxPacket.cJob				= cJob;
	_wcscpy( TxPacket.wszPupilCharName, _countof(TxPacket.wszPupilCharName), pwszPupilCharName, (int)wcslen(pwszPupilCharName) );

	AddSendData( VIMA_MASTERSYSTEM_JOIN_CONFIRM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemJoinConfirmResult( const int iRet, const bool bIsAccept, const INT64 biMasterCharacterDBID,  WCHAR* pwszPupilCharName)
{
	MasterSystem::VIMAJoinConfirmResult TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	
	TxPacket.iRet = iRet;
	TxPacket.bIsAccept = bIsAccept;
	TxPacket.biMasterCharacterDBID = biMasterCharacterDBID;
	_wcscpy( TxPacket.wszPupilCharName, _countof(TxPacket.wszPupilCharName), pwszPupilCharName, (int)wcslen(pwszPupilCharName) );

	AddSendData( VIMA_MASTERSYSTEM_JOIN_CONFIRM_RESULT, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDNMasterConnection::SendSyncSystemMail( TASendSystemMail* pMail )
{
	VIMASyncSystemMail TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biReceiverCharacterDBID	= pMail->biReceiverCharacterDBID;
	TxPacket.nMailDBID					= pMail->nMailDBID;
	TxPacket.iTotalMailCount			= pMail->iTotalMailCount;
	TxPacket.iNotReadMailCount			= pMail->iNotReadMailCount;
	TxPacket.i7DaysLeftMailCount		= pMail->i7DaysLeftMailCount;

	AddSendData( VIMA_SYNC_SYSTEMMAIL, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDNMasterConnection::SendFriendAddNotify(UINT nAddedAccountDBID, const WCHAR * pAddName)
{
	VIMAFriendAddNotice Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nAddedAccountDBID = nAddedAccountDBID;
	_wcscpy(Packet.wszAddName, _countof(Packet.wszAddName), pAddName, (int)wcslen(pAddName));

	AddSendData(VIMA_FRIENDADD, 0, (char*)&Packet, sizeof(Packet));
}

#ifdef PRE_ADD_DOORS
void CDNMasterConnection::SendPrivateChat(UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen, INT64 biDestCharacterDBID/* = 0*/)
#else		//#ifdef PRE_ADD_DOORS
void CDNMasterConnection::SendPrivateChat(UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen)
#endif		//#ifdef PRE_ADD_DOORS
{
	VIMAPrivateChat Private = { 0, };

	Private.nAccountDBID = nFromAccountDBID;
	memcpy(Private.wszToCharacterName, pwszToCharacterName, sizeof(Private.wszToCharacterName));
	Private.cType = cType;
	Private.wChatLen = wChatLen;
	_wcscpy(Private.wszChatMsg, _countof(Private.wszChatMsg), pwszChatMsg, (int)wcslen(pwszChatMsg));
#ifdef PRE_ADD_DOORS
	Private.biDestCharacterDBID = biDestCharacterDBID;
#endif		//#ifdef PRE_ADD_DOORS
	
	AddSendData(VIMA_PRIVATECHAT, 0, (char*)&Private, sizeof(Private) - sizeof(Private.wszChatMsg) + (wChatLen * 2));
}

void CDNMasterConnection::SendChat(char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen, int nMapIdx)
{
	VIMAChat Chat = { 0, };

	Chat.nAccountDBID = nFromAccountDBID;
	Chat.cType = cType;
	Chat.wChatLen = wChatLen;
	Chat.nMapIdx = nMapIdx;
	_wcscpy(Chat.wszChatMsg, _countof(Chat.wszChatMsg), pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(VIMA_CHAT, 0, (char*)&Chat, sizeof(Chat) - sizeof(Chat.wszChatMsg) + (wChatLen * 2));
}

void CDNMasterConnection::SendWorldSystemMsg(UINT nFromAccountDBID, char cType, int nID, int nValue, const WCHAR* pwszToCharacterName)
{
	VIMAWorldSystemMsg Msg = { 0, };

	Msg.nAccountDBID = nFromAccountDBID;
	Msg.cType = cType;
	Msg.nID = nID;
	Msg.nValue = nValue;
	if (pwszToCharacterName) 
		_wcscpy(Msg.wszToCharacterName, _countof(Msg.wszToCharacterName), pwszToCharacterName, (int)wcslen(pwszToCharacterName));

	AddSendData(VIMA_WORLDSYSTEMMSG, 0, (char*)&Msg, sizeof(VIMAWorldSystemMsg));
}

void CDNMasterConnection::SendWorldSystemMsg888(UINT nFromAccountDBID, char cType, int nID, int nValue, const WCHAR* pwszToCharacterName)
{
	VIMAWorldSystemMsg Msg = { 0, };

	Msg.nAccountDBID = nFromAccountDBID;
	Msg.cType = cType;
	Msg.nID = nID;
	Msg.nValue = nValue;
	if (pwszToCharacterName)
		_wcscpy(Msg.wszToCharacterName, _countof(Msg.wszToCharacterName), pwszToCharacterName, (int)wcslen(pwszToCharacterName));

	AddSendData(VIMA_WORLDSYSTEMMSG, 0, (char*)&Msg, sizeof(VIMAWorldSystemMsg));
}

#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNMasterConnection::SendPrivateChannelChat(char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen, INT64 nChannelID)
{
	VIMAPrivateChannelChatMsg Chat = { 0, };

	Chat.nAccountDBID = nFromAccountDBID;
	Chat.cType = cType;
	Chat.wChatLen = wChatLen;
	Chat.nChannelID = nChannelID;
	if(pwszChatMsg)
		_wcscpy(Chat.wszChatMsg, _countof(Chat.wszChatMsg), pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(VIMA_PRIVATECHANNELCHAT, 0, (char*)&Chat, sizeof(VIMAPrivateChannelChatMsg));
}
#endif

// VoiceChat
void CDNMasterConnection::SendReqVoiceChannelID( TINSTANCEID InstanceID, int nChannelType, int nPvPLobbyChannelID)
{
	VIMAReqVoiceChannelID Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.InstanceID = InstanceID;
	Packet.nPvPLobbyChannelID = nPvPLobbyChannelID;
	Packet.cVoiceChannelType = (BYTE)nChannelType;

	AddSendData(VIMA_REQVOICECHANNELID, 0, (char*)&Packet, sizeof(VIMAReqVoiceChannelID));
}

// PvP
void CDNMasterConnection::SendMovePvPLobbyToPvPVillage( const UINT uiAccountDBID )
{
	VIMAPVP_MOVELOBBYTOVILLAGE TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;

	AddSendData( VIMA_PVP_MOVELOBBYTOVILLAGE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(VIMAPVP_MOVELOBBYTOVILLAGE) );
}

#if defined(PRE_ADD_PVP_TOURNAMENT)
#if defined( PRE_PVP_GAMBLEROOM )
void CDNMasterConnection::SendPvPCreateRoom( const UINT uiCreateAccountDBID, const BYTE cGameMode, const USHORT unVillageChannelID, const UINT uiVillageMapIndex, const CSPVP_CREATEROOM* pPacket, TEvent* pEvent, BYTE cJob, TPvPGambleRoomData* GambleData )
#else
void CDNMasterConnection::SendPvPCreateRoom( const UINT uiCreateAccountDBID, const BYTE cGameMode, const USHORT unVillageChannelID, const UINT uiVillageMapIndex, const CSPVP_CREATEROOM* pPacket, TEvent* pEvent, BYTE cJob )
#endif
#else
void CDNMasterConnection::SendPvPCreateRoom( const UINT uiCreateAccountDBID, const BYTE cGameMode, const USHORT unVillageChannelID, const UINT uiVillageMapIndex, const CSPVP_CREATEROOM* pPacket, TEvent* pEvent/*=NULL*/ )
#endif
{
	VIMAPVP_CREATEROOM TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiCreateAccountDBID	= uiCreateAccountDBID;
	TxPacket.unVillageChannelID		= unVillageChannelID;
	TxPacket.uiVillageMapIndex		= uiVillageMapIndex;
	TxPacket.cGameMode				= cGameMode;
	if( pEvent )
	{
		TxPacket.nEventID = pEvent->nEventID;
		memcpy( &TxPacket.EventData, pEvent, sizeof(TEvent) );
	}
#if defined(PRE_ADD_PVP_TOURNAMENT)
	TxPacket.cCreateUserJob = cJob;
#endif
#if defined( PRE_PVP_GAMBLEROOM )
	if( GambleData )
	{
		TxPacket.cGambleType = GambleData->cGambleType;
		TxPacket.nPrice = GambleData->nPrice;
	}	
#endif
	memcpy( &TxPacket.sCSPVP_CREATEROOM, pPacket, sizeof(CSPVP_CREATEROOM)-sizeof(pPacket->wszBuf)+sizeof(WCHAR)*(pPacket->cRoomNameLen+pPacket->cRoomPWLen) );

	AddSendData( VIMA_PVP_CREATEROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(VIMAPVP_CREATEROOM)-sizeof(pPacket->wszBuf)+sizeof(WCHAR)*(pPacket->cRoomNameLen+pPacket->cRoomPWLen) );
}

void CDNMasterConnection::SendPvPModifyRoom( const UINT uiAccountDBID, const BYTE cGameMode, const CSPVP_MODIFYROOM* pPacket )
{
	VIMAPVP_MODIFYROOM TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.cGameMode		= cGameMode;
	memcpy( &TxPacket.sCSPVP_MODIFYROOM, pPacket, sizeof(CSPVP_MODIFYROOM)-sizeof(pPacket->sCSPVP_CREATEROOM.wszBuf)+sizeof(WCHAR)*(pPacket->sCSPVP_CREATEROOM.cRoomNameLen+pPacket->sCSPVP_CREATEROOM.cRoomPWLen) );

	AddSendData( VIMA_PVP_MODIFYROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(VIMAPVP_MODIFYROOM)-sizeof(pPacket->sCSPVP_CREATEROOM.wszBuf)+sizeof(WCHAR)*(pPacket->sCSPVP_CREATEROOM.cRoomNameLen+pPacket->sCSPVP_CREATEROOM.cRoomPWLen) );
}

void CDNMasterConnection::SendPvPLeaveRoom( const UINT uiLeaveAccountDBID )
{
	VIMAPVP_LEAVEROOM TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiLeaveAccountDBID = uiLeaveAccountDBID;

	AddSendData( VIMA_PVP_LEAVEROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPChangeCaptain( const UINT uiAccountDBID, const CSPVP_CHANGECAPTAIN* pPacket )
{
	VIMAPVP_CHANGECAPTAIN TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	memcpy( &TxPacket.sCSPVP_CHANGECAPTAIN, pPacket, sizeof(CSPVP_CHANGECAPTAIN) );

	AddSendData( VIMA_PVP_CHANGECAPTAIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPRoomList( const UINT uiAccountDBID, const USHORT unVillageChannelID, const CSPVP_ROOMLIST* pPacket )
{
	VIMAPVP_ROOMLIST TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.unVillageChannelID	= unVillageChannelID;
	memcpy( &TxPacket.sCSPVP_ROOMLIST, pPacket, sizeof(CSPVP_ROOMLIST) );

	AddSendData( VIMA_PVP_ROOMLIST, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPWaitUserList( const UINT uiAccountDBID, const USHORT unVillageChannelID, const CSPVP_WAITUSERLIST* pPacket )
{
	VIMAPVP_WAITUSERLIST TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.unVillageChannelID	= unVillageChannelID;
	memcpy( &TxPacket.sCSPVP_WAITUSERLIST, pPacket, sizeof(CSPVP_WAITUSERLIST) );

	AddSendData( VIMA_PVP_WAITUSERLIST, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
void CDNMasterConnection::SendPvPRoomListRelay( const BYTE cVIllageID, const UINT uiAccountDBID, const short nRetCode, std::vector<CDNPvPRoom*>& vPvPRoom, const UINT uiMaxPage)
{
	PVP_ROOMLIST_RELAY TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket));

	TxPacket.cVillageID = cVIllageID;
	TxPacket.uiAccountDBID = uiAccountDBID;

	TxPacket.sSCPVP_ROOMLIST.nRetCode		= nRetCode;
	TxPacket.sSCPVP_ROOMLIST.cRoomCount		= static_cast<BYTE>( vPvPRoom.size() );
	TxPacket.sSCPVP_ROOMLIST.uiMaxPage		= uiMaxPage;
	WCHAR* pBufferOffset	= TxPacket.sSCPVP_ROOMLIST.RoomInfoList.wszBuffer;
	for( UINT i=0 ; i<vPvPRoom.size() ; ++i )
		pBufferOffset = vPvPRoom[i]->MakeRoomInfoList( NULL, &TxPacket.sSCPVP_ROOMLIST.RoomInfoList, i, pBufferOffset );

	int nSize = static_cast<int>(sizeof(TxPacket.sSCPVP_ROOMLIST)-sizeof(TxPacket.sSCPVP_ROOMLIST.RoomInfoList.wszBuffer)+(sizeof(WCHAR)*(pBufferOffset-TxPacket.sSCPVP_ROOMLIST.RoomInfoList.wszBuffer)) );

	TxPacket.nSize = nSize;

	AddSendData( VIMA_PVP_ROOMLIST_RELAY, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket)-sizeof(TxPacket.sSCPVP_ROOMLIST)+nSize );
}
#endif

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
void CDNMasterConnection::SendPvPChangeChannel(UINT nAccountDBID, BYTE cType, BYTE cPvPLevel, BYTE cSeperateLevel, bool bSend)
{
	PVP_CHANGECHANNEL packet;
	memset(&packet, 0, sizeof(PVP_CHANGECHANNEL));

	packet.nAccountDBID = nAccountDBID;
	packet.cType = cType;
	packet.cPvPLevel;
	packet.cSeperateLevel = cSeperateLevel;
	packet.bSend = bSend;

	AddSendData(VIMA_PVP_CHANGECHANNEL, 0, reinterpret_cast<char*>(&packet), sizeof(packet));
}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

void CDNMasterConnection::SendPvPJoinRoom( const UINT uiAccountDBID, BYTE cUserLevel, const CSPVP_JOINROOM* pPacket, CDNUserSession * pSession )
{
	// 패스워드 길이 검사
	if( pPacket->cRoomPWLen > PvPCommon::TxtMax::RoomPW )
		return;

	VIMAPVP_JOINROOM TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.cUserLevel		= cUserLevel;
	memcpy( &TxPacket.sCSPVP_JOINROOM, pPacket, sizeof(CSPVP_JOINROOM)-sizeof(TxPacket.sCSPVP_JOINROOM.wszRoomPW)+pPacket->cRoomPWLen*sizeof(WCHAR) );
	if (pSession)
		TxPacket.sCSPVP_JOINROOM.nGuildDBID = pSession->GetGuildUID().nDBID;
#if defined(PRE_ADD_PVP_TOURNAMENT)
	if( pSession )
		TxPacket.cUserJob = pSession->GetUserJob();
#endif
	AddSendData( VIMA_PVP_JOINROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(VIMAPVP_JOINROOM)-sizeof(TxPacket.sCSPVP_JOINROOM.wszRoomPW)+pPacket->cRoomPWLen*sizeof(WCHAR) );
}

void CDNMasterConnection::SendPvPReady( const UINT uiAccountDBID, const CSPVP_READY* pPacket )
{
	VIMAPVP_READY TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	memcpy( &TxPacket.sCSPVP_READY, pPacket, sizeof(CSPVP_READY) );

	AddSendData( VIMA_PVP_READY, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPStart( const UINT uiAccountDBID, const CSPVP_START* pPacket )
{
	VIMAPVP_START TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	memcpy( &TxPacket.sCSPVP_START, pPacket, sizeof(CSPVP_START) );

#if defined( _FINAL_BUILD )
	TxPacket.sCSPVP_START.unCheck = PvPCommon::Check::AllCheck;
#endif

	AddSendData( VIMA_PVP_START, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPRandomJoinRoom( const UINT uiAccountDBID, BYTE cUserLevel )
{
	VIMAPVP_RANDOMJOINROOM TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.cUserLevel		= cUserLevel;

	AddSendData( VIMA_PVP_RANDOMJOINROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(VIMAPVP_RANDOMJOINROOM) );
}

void CDNMasterConnection::SendPvPChangeTeam( const UINT uiAccountDBID, BYTE cLevel, const CSPVP_CHANGETEAM* pPacket )
{
	VIMAPVP_CHANGETEAM TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.cUserLevel		= cLevel;
	memcpy( &TxPacket.sCSPVP_CHANGETEAM, pPacket, sizeof(CSPVP_CHANGETEAM) );

	AddSendData( VIMA_PVP_CHANGETEAM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPBan( const UINT uiAccountDBID, const CSPVP_BAN* pPacket )
{
	VIMAPVP_BAN	TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	memcpy( &TxPacket.sCSPvPBan, pPacket, sizeof(CSPVP_BAN) );

	AddSendData( VIMA_PVP_BAN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPEnterLobby( const UINT uiAccountDBID, const USHORT unVillageChannelID )
{
	VIMAPVP_ENTERLOBBY TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.unVillageChannelID	= unVillageChannelID;

	AddSendData( VIMA_PVP_ENTERLOBBY, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPLadderInviteUser( UINT uiAccountDBID, const WCHAR* pwszCharName )
{
	VIMALadderInviteUser TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, static_cast<int>(wcslen(pwszCharName)) );

	AddSendData( VIMA_LADDER_INVITEUSER, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPLadderInviteConfirm( UINT uiAccountDBID, const WCHAR* pwszConfirmCharName, const LadderSystem::CS_INVITE_CONFIRM* pPacket )
{
	VIMALadderInviteConfirm TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	_wcscpy( TxPacket.wszConfirmCharName, _countof(TxPacket.wszConfirmCharName), pwszConfirmCharName, static_cast<int>(wcslen(pwszConfirmCharName)) );
	memcpy( &TxPacket.sInviteComfirm, pPacket, sizeof(TxPacket.sInviteComfirm) );

	AddSendData( VIMA_LADDER_INVITECONFIRM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPFatigueOption( const UINT uiAccountDBID, bool bFatigueOption)
{
	VIMAPVP_FATIGUE_OPTION TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket));

	TxPacket.uiAccountDBID = uiAccountDBID;
	TxPacket.bFatigueOption = bFatigueOption;

	AddSendData( VIMA_PVP_FATIGUE_OPTION, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPSwapMemberIndex(UINT nAccountDBID, BYTE cCount, const TSwapMemberIndex * pIndex)
{
	if (pIndex == NULL)	return;

	VIMAPVPSwapMemberIndex Packet;
	memset(&Packet, 0, sizeof(VIMAPVPSwapMemberIndex));

	Packet.nAccountDBID = nAccountDBID;
	Packet.cCount = cCount;
	memcpy(Packet.Index, pIndex, sizeof(Packet.Index));

	AddSendData(VIMA_PVP_SWAPTMEMBER_INDEX, 0, (char*)(&Packet), sizeof(Packet) - sizeof(Packet.Index) + (sizeof(TSwapMemberIndex) * Packet.cCount));
}

void CDNMasterConnection::SendPvPChangeMemberGrade(UINT nAccountDBID, USHORT nType, UINT nSessionID, bool bAsign)
{
	VIMAPVPChangeMemberGrade Packet;
	memset(&Packet, 0, sizeof(VIMAPVPChangeMemberGrade));

	Packet.nAccountDBID = nAccountDBID;
	Packet.nType = nType;
	Packet.bAsign = bAsign;
	Packet.nTargetSessionID = nSessionID;

	AddSendData(VIMA_PVP_CHANGEMEMBER_GRADE, 0, (char*)(&Packet), sizeof(Packet));
}

#if defined(PRE_ADD_PVP_TOURNAMENT)
void CDNMasterConnection::SendPvPSwapTournamentIndex(UINT nAccountDBID, char cSourceIndex, char cDestIndex)
{
	VIMAPVPSwapTournamentIndex Packet;
	memset(&Packet, 0, sizeof(VIMAPVPSwapTournamentIndex));

	Packet.nAccountDBID = nAccountDBID;
	Packet.cSourceIndex = cSourceIndex;
	Packet.cDestIndex = cDestIndex;

	AddSendData(VIMA_PVP_SWAP_TOURNAMENT_INDEX, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
}
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)

#if defined( PRE_WORLDCOMBINE_PVP )

void CDNMasterConnection::SendWorldPvPCreateRoom( int nRoomIndex, TWorldPvPMissionRoom Data, UINT nAccountDBID/*=0*/ )
{
	VIMACreateWorldPvPRoom Packet;
	memset(&Packet, 0, sizeof(VIMACreateWorldPvPRoom));

	Packet.nRoomIndex = nRoomIndex;
	Packet.Data = Data;
	Packet.nGMAccountDBID = nAccountDBID;
	AddSendData(VIMA_WORLDPVP_REQGAMEID, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
}

void CDNMasterConnection::SendWorldPvPJoinRoom( UINT nAccountDBID, INT64 biCharacterDBID, int nRoomIndex, TWorldPvPRoomDBData *Data, UINT uiTeam, bool bPvPLobby )
{
	VIMAWorldPvPRoomBreakInto Packet;
	memset(&Packet, 0, sizeof(VIMAWorldPvPRoomBreakInto));
	

	Packet.uiAccountDBID = nAccountDBID;
	Packet.biCharacterDBID = biCharacterDBID;
	Packet.nManagedID = Data->nServerID;
	Packet.nRoomIndex = nRoomIndex;
	Packet.nRoomID = Data->nRoomID;
	Packet.uiTeam = uiTeam;
	Packet.bPvPLobby = bPvPLobby;
	Packet.BreakIntoType = BreakInto::Type::WorldPvPRoom;
	
	AddSendData(VIMA_WORLDPVP_BREAKINTO, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
}

#endif

void CDNMasterConnection::SendNotifyMail(UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail)
{
	VIMANotifyMail Mail = { 0, };
	Mail.nToAccountDBID = nToAccountDBID;
	Mail.biToCharacterDBID = biToCharacterDBID;
	Mail.wTotalMailCount = wTotalMailCount;
	Mail.wNotReadMailCount = wNotReadMailCount;
	Mail.w7DaysLeftMailCount = w7DaysLeftCount;
	Mail.bNewMail = bNewMail;
	AddSendData(VIMA_NOTIFYMAIL, 0, (char*)&Mail, sizeof(VIMANotifyMail));
}

void CDNMasterConnection::SendNotifyMarket(UINT nSellerAccountDBID, INT64 biSellerCharacterDBID, int nItemID, short wCalculationCount)
{
	VIMANotifyMarket Market = {0, };
	Market.nSellerAccountDBID = nSellerAccountDBID;
	Market.biSellerCharacterDBID = biSellerCharacterDBID;
	Market.nItemID = nItemID;
	Market.wCalculationCount = wCalculationCount;
	AddSendData(VIMA_NOTIFYMARKET, 0, (char*)&Market, sizeof(VIMANotifyMarket));
}

void CDNMasterConnection::SendNotifyGift(UINT nToAccountDBID, INT64 biToCharacterDBID, bool bNew, int nGiftCount)
{
	VIMANotifyGift Result;
	Result.nToAccountDBID = nToAccountDBID;
	Result.biToCharacterDBID = biToCharacterDBID;
	Result.bNew = bNew;
	Result.nGiftCount = nGiftCount;
	AddSendData(VIMA_NOTIFYGIFT, 0, (char*)&Result, sizeof(VIMANotifyGift));
}

void CDNMasterConnection::SendFarmListGot(TFarmItemFromDB * pFarm, BYTE cCount)
{
	if( cCount >= Farm::Max::FARMCOUNT )
	{
		_DANGER_POINT();
		return;			//이러시면 아니데옵니다.
	}

	VIMAFarmList Packet;
	memset(&Packet, 0, sizeof(VIMAFarmList));

	for (int i = 0; i < cCount; i++)
		memcpy_s(&Packet.Farms[i], sizeof(Packet.Farms[i]), &pFarm[i], sizeof(TFarmItemFromDB));
	Packet.cCount = cCount;

	AddSendData(VIMA_FARMLIST, 0, (char*)&Packet, sizeof(VIMAFarmList) - sizeof(Packet.Farms) + (sizeof(TFarmItem) * cCount));
}

void CDNMasterConnection::SendNoticeFromClient(const WCHAR * pMsg, int nLen)
{
	VIMANotice Packet;
	memset(&Packet, 0, sizeof(Packet));
	Packet.nLen = nLen;
	_wcscpy(Packet.szMsg, _countof(Packet.szMsg), pMsg, (int)wcslen(pMsg));
	AddSendData(VIMA_NOTICE, 0, (char*)&Packet, sizeof(Packet) - sizeof(Packet.szMsg) + (nLen * sizeof(WCHAR)));
}

void CDNMasterConnection::SendZoneNoticeFromClient(int nMapIndex, const WCHAR * pMsg, int nLen)
{
	VIMAZoneNotice Packet;
	memset(&Packet, 0, sizeof(Packet));
	Packet.nMapIndex = nMapIndex;
	Packet.nLen = nLen;
	_wcscpy(Packet.szMsg, _countof(Packet.szMsg), pMsg, (int)wcslen(pMsg));
	AddSendData(VIMA_ZONENOTICE, 0, (char*)&Packet, sizeof(Packet) - sizeof(Packet.szMsg) + (nLen * sizeof(WCHAR)));
}

#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendTraceBreakInto(int nType,  const UINT uiAccountDBID, const WCHAR* pwszCharacterName, char cSelectedLang )
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendTraceBreakInto(int nType,  const UINT uiAccountDBID, const WCHAR* pwszCharacterName )
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
{
	VIMATraceBreakInto TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	
	TxPacket.nType = nType;
	TxPacket.uiAccountDBID = uiAccountDBID;
	if (pwszCharacterName) _wcscpy( TxPacket.wszParam, SCHAR_MAX, pwszCharacterName, (int)wcslen(pwszCharacterName) );
#if defined(PRE_ADD_MULTILANGUAGE)
	TxPacket.cCallerSelectedLang = cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	
	AddSendData( VIMA_TRACEBREAKINTO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendReqRecall( const UINT uiAccountDBID, const UINT uiRecallChannelID, const WCHAR* pwszCharacterName, char cSelectedLang )
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendReqRecall( const UINT uiAccountDBID, const UINT uiRecallChannelID, const WCHAR* pwszCharacterName )
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
{
	VIMAReqRecall TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.uiRecallChannelID	= uiRecallChannelID;
#if defined(PRE_ADD_MULTILANGUAGE)
	TxPacket.cCallerSelectedLang = cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	if (pwszCharacterName) _wcscpy( TxPacket.wszCharacterName, NAMELENMAX, pwszCharacterName, (int)wcslen(pwszCharacterName) );

	AddSendData( VIMA_REQRECALL, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendBanUser(UINT nAccountDBID, const WCHAR * pCharacterName, char cSelectedLang)
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendBanUser(UINT nAccountDBID, const WCHAR * pCharacterName)
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
{
	VIMABanUser	Packet;
	memset(&Packet, 0, sizeof(VIMABanUser));

	Packet.nAccountDBID = nAccountDBID;
	_wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), pCharacterName, (int)wcslen(pCharacterName));
#if defined(PRE_ADD_MULTILANGUAGE)
	Packet.cSelectedLang = cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	AddSendData(VIMA_BANUSER, 0, reinterpret_cast<char*>(&Packet), sizeof(VIMABanUser));
}

void CDNMasterConnection::SendForceDestroyPvP(UINT nAccountDBID, int nPvPIndex, const WCHAR * pForceWinGuildName)
{
	VIMADestroyPvP packet;
	memset(&packet, 0, sizeof(VIMADestroyPvP));

	packet.nAccountDBID = nAccountDBID;
	packet.nPvPIndex = nPvPIndex;
	if (pForceWinGuildName)
		_wcscpy(packet.wszGuildName, GUILDWAR_FINALS_TEAM_MAX, pForceWinGuildName, static_cast<int>(wcslen(pForceWinGuildName)));

	AddSendData(VIMA_DESTROYPVP, 0, reinterpret_cast<char*>(&packet), sizeof(VIMADestroyPvP));
}

#if defined(PRE_ADD_QUICK_PVP)
void CDNMasterConnection::SendMakeQuickPvPRoom(UINT nMasterAccountDBID, UINT nSlaveAccountDBID)
{
	VIMAMakeQuickPvPRoom packet;
	memset(&packet, 0, sizeof(VIMAMakeQuickPvPRoom));

	packet.uiMasterAccountDBID = nMasterAccountDBID;
	packet.uiSlaveAccountDBID = nSlaveAccountDBID;

	AddSendData(VIMA_MAKEQUICKPVP_ROOM, 0, reinterpret_cast<char*>(&packet), sizeof(VIMAMakeQuickPvPRoom));
}
#endif //#if defined(PRE_ADD_QUICK_PVP)

bool CDNMasterConnection::VerifyChannelID(int nChannelID, int nMapIdx)
{
	std::vector < std::pair < int, sChannelInfo > >::iterator ii;
	for (ii = m_vChannelList.begin(); ii != m_vChannelList.end(); ii++)
	{
		if ((*ii).second.nChannelID == nChannelID && (*ii).second.nMapIdx == nMapIdx)
			return true;
	}
	return false;
}

int CDNMasterConnection::GetChannelIndex(int nChannelID)
{
	std::vector < std::pair < int, sChannelInfo > >::iterator ii;
	for (ii = m_vChannelList.begin(); ii != m_vChannelList.end(); ii++)
	{
		if ((*ii).second.nChannelID == nChannelID)
			return (*ii).second.nChannelIdx;
	}
	return -1;
}

const sChannelInfo*	CDNMasterConnection::GetChannelInfo( const int nChannelID )
{	
	for( std::vector<std::pair<int,sChannelInfo>>::iterator itor=m_vChannelList.begin() ; itor!=m_vChannelList.end() ; ++itor )	
	{		
		if( (*itor).second.nChannelID == nChannelID )
			return &(*itor).second;
	}
	return NULL;
}

const sChannelInfo*	CDNMasterConnection::GetChannelInfoFromMapIndex( const int nMapIndex )
{
	for( std::vector<std::pair<int,sChannelInfo>>::iterator itor=m_vChannelList.begin() ; itor!=m_vChannelList.end() ; ++itor )	
	{		
		if( (*itor).second.nMapIdx == nMapIndex )
			return &(*itor).second;
	}
	return NULL;
}

const sChannelInfo* CDNMasterConnection::GetChannelInfoFromAttr( const int iAttr )
{
	for( std::vector<std::pair<int,sChannelInfo>>::iterator itor=m_vChannelList.begin() ; itor!=m_vChannelList.end() ; ++itor )	
	{		
		if( (*itor).second.nChannelAttribute&iAttr )
			return &(*itor).second;
	}
	return NULL;
}

int CDNMasterConnection::GetChannelMapIndex(int nChannelID)
{
	std::vector<std::pair<int,sChannelInfo>>::iterator ii;
	for(ii = m_vChannelList.begin(); ii != m_vChannelList.end(); ii++)
	{
		if ((*ii).second.nChannelID == nChannelID)
			return (*ii).second.nMapIdx;
	}
	return 0;
}

int CDNMasterConnection::GetMeritBonusID(int nChannelID)
{
	std::vector<std::pair<int,sChannelInfo>>::iterator ii;
	for(ii = m_vChannelList.begin(); ii != m_vChannelList.end(); ii++)
	{
		if ((*ii).second.nChannelID == nChannelID)
			return (*ii).second.nMeritBonusID;
	}
	return 0;
}

bool CDNMasterConnection::GetActive()
{
	if (m_bConnectCompleted == false)
		return false;
	return CConnection::GetActive();
}

bool CDNMasterConnection::_CmdRecall( MAVIResRecall* pPacket )
{
	CDNUserSession* pRecallUser = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiRecallAccountDBID );
	if( !pRecallUser )
		return false;

	if( pRecallUser->m_eUserState != STATE_NONE )
	{
		//std::wstring wString = boost::io::str( boost::wformat( L"%s 님은 소환을 할 수 없는 State(%d) 입니다.") % pRecallUser->GetCharacterName() % static_cast<int>(pRecallUser->m_eUserState) );
#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4417, pPacket->cRecallerSelectedLang) ) % pRecallUser->GetCharacterName() % static_cast<int>(pRecallUser->m_eUserState) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4417) ) % pRecallUser->GetCharacterName() % static_cast<int>(pRecallUser->m_eUserState) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		SendChat( CHATTYPE_GM, pPacket->uiAccountDBID, wString.c_str(), static_cast<short>(wString.size()), -1 );
		return true;
	}

	if( pRecallUser->GetField() && pRecallUser->GetField()->GetChnnelID() == pPacket->uiRecallChannelID )
		return true;

	return ( pRecallUser->CmdSelectChannel( pPacket->uiRecallChannelID ) == ERROR_NONE ) ? true : false;
}

bool CDNMasterConnection::_CmdVillageTrace( MAVIVillageTrace* pPacket )
{
	CDNUserSession* pUser = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiAccountDBID );
	if( !pUser )
		return false;

	if( pUser->m_eUserState != STATE_NONE )
	{
		if( pPacket->bIsGMCall )
		{
#if defined(PRE_ADD_MULTILANGUAGE)
			std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4428, pPacket->cSelectedLang) ) % pUser->GetCharacterName() % static_cast<int>(pUser->m_eUserState) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4428) ) % pUser->GetCharacterName() % static_cast<int>(pUser->m_eUserState) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			SendChat( CHATTYPE_GM, pPacket->uiAccountDBID, wString.c_str(), static_cast<short>(wString.size()), -1 );
		}
		return true;
	}

	if( pUser->GetChannelID() == pPacket->iTargetChannelID )
	{
		CDNUserSession* pTargetUser = g_pUserSessionManager->FindUserSessionByAccountDBID( pPacket->uiTargetAccountDBID );
		if( !pTargetUser )
			return false;

		if( pTargetUser->m_eUserState != STATE_NONE )
		{
			if( pPacket->bIsGMCall )
			{
#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4428, pPacket->cSelectedLang) ) % pUser->GetCharacterName() % static_cast<int>(pUser->m_eUserState) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4428) ) % pUser->GetCharacterName() % static_cast<int>(pUser->m_eUserState) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
				SendChat( CHATTYPE_GM, pPacket->uiAccountDBID, wString.c_str(), static_cast<short>(wString.size()), -1 );
			}
			return true;
		}

		// GM 호출일 때만 바로 옆으로 이동한다.
		if( pPacket->bIsGMCall == true )
		{
			BYTE pBuffer[128];
			CPacketCompressStream Stream( pBuffer, 128 );

			EtVector3 vPos( pTargetUser->GetCurrentPos().nX/1000.f, pTargetUser->GetCurrentPos().nY/1000.f, pTargetUser->GetCurrentPos().nZ/1000.f );
			EtVector2 vLook( 0.f, 0.f );

			Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Write( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

			pUser->SendActorMsg( pUser->GetSessionID(), eActor::SC_CMDWARP, Stream.Tell(), Stream.GetBuffer() );
		}
		return true;
	}
	
	return (pUser->CmdSelectChannel( pPacket->iTargetChannelID, true ) == ERROR_NONE ) ? true : false;
}

void CDNMasterConnection::SendUpdateChannelShowInfo( USHORT unChannelID, bool bShow, int nServerID, BYTE cThreadID )
{
	VIMAUpdateChannelShowInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.unChannelID = unChannelID;
	TxPacket.bShow = bShow;
	TxPacket.nServerID = nServerID;
	TxPacket.cThreadID = cThreadID;

	AddSendData( VIMA_UPDATECHANNELSHOWINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined(_KR) || defined(_TH) || defined(_ID)
void CDNMasterConnection::SendPCBangResult(UINT nAccountDBID)
{
	VIMAPCBangResult Result;
	Result.nAccountDBID = nAccountDBID;
	AddSendData(VIMA_PCBANGRESULT, 0, (char*)&Result, sizeof(VIMAPCBangResult) );
}
#endif	// #if defined(_KR) || defined(_TH)

void CDNMasterConnection::SendSaveUserTempData(CDNUserSession* pSession)
{
	VIMASaveUserTempData TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = pSession->GetAccountDBID();
	TxPacket.nDungeonClearCount = pSession->GetDungeonClearCount();
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	TxPacket.eUserGameQuitRewardType = pSession->GetUserGameQuitRewardType();
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(PRE_ADD_REMOTE_QUEST)
	pSession->GetQuest()->GetAcceptWaitRemoteQuest(&TxPacket.nAcceptWaitRemoteQuestCount, TxPacket.AcceptWaitRemoteQuestList);
	AddSendData (VIMA_SAVE_USERTEMPDATA, 0, (char*)&TxPacket, sizeof(VIMASaveUserTempData) - sizeof(TxPacket.AcceptWaitRemoteQuestList) + sizeof(int) * TxPacket.nAcceptWaitRemoteQuestCount );
#else
	AddSendData (VIMA_SAVE_USERTEMPDATA, 0, (char*)&TxPacket, sizeof(VIMASaveUserTempData) );
#endif
}

void CDNMasterConnection::SendLoadUserTempData(UINT uiAccountDBID)
{
	VIMALoadUserTempData TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	AddSendData (VIMA_LOAD_USERTEMPDATA, 0, (char*)&TxPacket, sizeof(VIMALoadUserTempData) );
}

void CDNMasterConnection::SendDuplicateLogin(UINT nAccountDBID, bool bIsDetach, UINT nSessionID)
{
	VIMADuplicateLogin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nAccountDBID = nAccountDBID;
	TxPacket.bIsDetach = bIsDetach;
	TxPacket.nSessionID = nSessionID;

	AddSendData (VIMA_DUPLICATE_LOGIN, 0, (char*)&TxPacket, sizeof(VIMADuplicateLogin) );
}

void CDNMasterConnection::SendWorldEventCounter(char cWorldSetID, int nScheduleID, int nCount)
{
	MAUpdateWorldEventCounter TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.cWorldSetID = cWorldSetID;
	TxPacket.nScheduleID = nScheduleID;
	TxPacket.nCount = nCount;

	AddSendData (VIMA_UPPDATE_WORLDEVENTCOUNTER, 0, (char*)&TxPacket, sizeof(MAUpdateWorldEventCounter) );
}

void CDNMasterConnection::SendCheckLastDungeonInfo( CDNUserSession* pSession, INT64 biValue )
{
	VIMACheckLastDungeonInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= pSession->GetAccountDBID();
	TxPacket.biCharacterDBID	= pSession->GetCharacterDBID();
	TxPacket.iManagedID			= LODWORD( biValue );
	TxPacket.iRoomID			= HIDWORD( biValue );

	AddSendData( VIMA_CHECK_LASTDUNGEONINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );

#if defined( _WORK)
	std::cout << "SendCheckLastDungeonInfo MasterServerReq CharDBID=" << pSession->GetCharacterDBID() << std::endl;
#endif // #if defined( _WORK)
}

#if defined( PRE_PARTY_DB )
void CDNMasterConnection::SendConfirmLastDungeonInfo( CDNUserSession* pSession, INT64 biValue, BreakInto::Type::eCode BreakIntoType/*=BreakInto::Type::PartyRestore*/, bool bIsOK/*=true*/ )
#else
void CDNMasterConnection::SendConfirmLastDungeonInfo( CDNUserSession* pSession, INT64 biValue, bool bIsOK/*=true*/ )
#endif // #if defined( PRE_PARTY_DB )
{
	VIMAConfirmLastDungeonInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= pSession->GetAccountDBID();
	TxPacket.biCharacterDBID	= pSession->GetCharacterDBID();
	TxPacket.iManagedID			= LODWORD( biValue );
	TxPacket.iRoomID			= HIDWORD( biValue );
	TxPacket.bIsOK				= bIsOK;
#if defined( PRE_PARTY_DB )
	TxPacket.BreakIntoType		= BreakIntoType;
#endif // #if defined( PRE_PARTY_DB )

	AddSendData( VIMA_CONFIRM_LASTDUNGEONINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );

#if defined( _WORK)
	std::cout << "SendConfirmLastDungeonInfo MasterServerReq CharDBID=" << pSession->GetCharacterDBID() << " OK=" << (bIsOK?"true":"false") << std::endl;
#endif // #if defined( _WORK)
}

#if defined (PRE_ADD_BESTFRIEND)
void CDNMasterConnection::SendSearchBestFriend(UINT nAccountDBID, int nRet, bool bAck, LPCWSTR lpwszCharacterName, char cLevel, char cJob)
{
	MASearchBestFriend packet;
	memset(&packet, 0, sizeof(packet));

	packet.nRet = nRet;
	packet.nAccountDBID = nAccountDBID;
	packet.bAck = bAck;
	packet.cLevel = cLevel;
	packet.cJob = cJob;

	if (lpwszCharacterName)
		_wcscpy(packet.wszName, _countof(packet.wszName), lpwszCharacterName, _countof(packet.wszName));


	AddSendData(VIMA_SEARCH_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}	

void CDNMasterConnection::SendRegistBestFriend(UINT nAccountDBID, INT64 biCharacterDBID, LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, int nRegistItemID)
{
	MARegistBestFriend packet;
	memset(&packet, 0, sizeof(packet));

	packet.nAccountDBID = nAccountDBID;
	packet.biCharacterDBID = biCharacterDBID;
	packet.nRegistItemID = nRegistItemID;

	if (lpwszFromCharacterName)
		_wcscpy(packet.wszFromName, _countof(packet.wszFromName), lpwszFromCharacterName, _countof(packet.wszFromName));
	if (lpwszToCharacterName)
		_wcscpy(packet.wszToName, _countof(packet.wszToName), lpwszToCharacterName, _countof(packet.wszToName));

	AddSendData(VIMA_REGIST_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}	

void CDNMasterConnection::SendRegistBestFriendResult(UINT nAccountDBID, int nRetCode, bool bAck, INT64 biFromCharacterDBID, LPCWSTR lpwszFromCharacterName, UINT nToAccountDBID, INT64 biToCharacterDBID, LPCWSTR lpwszToCharacterName)
{
	MARegistBestFriendResult packet;
	memset(&packet, 0, sizeof(packet));

	packet.nRet	= nRetCode;
	packet.nAccountDBID = nAccountDBID;
	packet.bAck	= bAck;

	packet.biFromCharacterDBID = biFromCharacterDBID;
	if (lpwszFromCharacterName)
		_wcscpy(packet.wszFromName, _countof(packet.wszFromName), lpwszFromCharacterName, _countof(packet.wszFromName));

	packet.nToAccountDBID = nToAccountDBID;
	packet.biToCharacterDBID = biToCharacterDBID;
	if (lpwszToCharacterName)
		_wcscpy(packet.wszToName, _countof(packet.wszToName), lpwszToCharacterName, _countof(packet.wszToName));

	AddSendData(VIMA_RETREGIST_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}

void CDNMasterConnection::SendCompleteBestFriend(UINT nFromAccountDBID, UINT nToAccountDBID, INT64 biCharacterDBID, LPCWSTR lpwszCharacterName, int nItemID)
{
	MACompleteBestFriend packet;
	memset(&packet, 0, sizeof(packet));

	packet.nFromAccountDBID = nFromAccountDBID;

	packet.nToAccountDBID = nToAccountDBID;
	packet.biCharacterDBID = biCharacterDBID;
	packet.nItemID = nItemID;

	if (lpwszCharacterName)
		_wcscpy(packet.wszName, _countof(packet.wszName), lpwszCharacterName, _countof(packet.wszName));

	AddSendData(VIMA_COMPLETE_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}


void CDNMasterConnection::SendEditBestFriendMemo(UINT nFromAccountDBID, INT64 biToCharacterDBID, LPCWSTR lpwszToMemo)
{
	MAEditBestFriendMemo packet;
	memset(&packet, 0, sizeof(packet));

	packet.nFromAccountDBID = nFromAccountDBID;
	packet.biToCharacterDBID = biToCharacterDBID;
	if (lpwszToMemo)
		_wcscpy(packet.wszMemo, _countof(packet.wszMemo), lpwszToMemo, (int)wcslen(lpwszToMemo));

	AddSendData(VIMA_EDIT_BESTFRIENDMEMO, 0, (char*)&packet, sizeof(packet));
}

void CDNMasterConnection::SendCancelBestFriend(LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, bool bCancel)
{
	MACancelBestFriend packet;
	memset(&packet, 0, sizeof(packet));

	packet.bCancel = bCancel;

	if (lpwszFromCharacterName)
		_wcscpy(packet.wszFromName, _countof(packet.wszFromName), lpwszFromCharacterName, _countof(packet.wszFromName));

	if (lpwszToCharacterName)
		_wcscpy(packet.wszToName, _countof(packet.wszToName), lpwszToCharacterName, _countof(packet.wszToName));

	AddSendData(VIMA_CANCEL_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}

void CDNMasterConnection::SendCloseBestFriend(LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName)
{
	MACloseBestFriend packet;
	memset(&packet, 0, sizeof(packet));

	if (lpwszFromCharacterName)
	_wcscpy(packet.wszFromName, _countof(packet.wszFromName), lpwszFromCharacterName, _countof(packet.wszFromName));

	if (lpwszToCharacterName)
		_wcscpy(packet.wszToName, _countof(packet.wszToName), lpwszToCharacterName, _countof(packet.wszToName));

	AddSendData(VIMA_CLOSE_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}

void CDNMasterConnection::SendLevelUpBestFriend(BYTE cLevel, LPCWSTR lpwszCharacterName)
{
	MALevelUpBestFriend packet;
	memset(&packet, 0, sizeof(packet));

	packet.cLevel = cLevel;
	if (lpwszCharacterName)
		_wcscpy(packet.wszName, _countof(packet.wszName), lpwszCharacterName, _countof(packet.wszName));

	AddSendData(VIMA_LEVELUP_BESTFRIEND, 0, (char*)&packet, sizeof(packet));
}


#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined( PRE_WORLDCOMBINE_PARTY )
void CDNMasterConnection::SendGetWorldPartyMember(UINT nAccountDBID, INT64 biCharacterDBID, const Party::Data &PartyData)
{
	MAGetWorldPartyMember Packet;
	memset(&Packet, 0, sizeof(MAGetWorldPartyMember));

	Packet.nAccountDBID = nAccountDBID;
	Packet.biCharacterDBID = biCharacterDBID;
	Packet.iServerID = PartyData.iServerID;
	Packet.PartyID = PartyData.PartyID;
	Packet.iRoomID = PartyData.iRoomID;

	AddSendData(VIMA_GET_WORLDPARTYMEMBER, 0, (char*)&Packet, sizeof(MAGetWorldPartyMember));
}
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNMasterConnection::SendAddPrivateChatChannel(TPrivateChatChannelInfo tPrivateChatChannel)
{
	MAAddPrivateChannel Packet;
	memset(&Packet, 0, sizeof(MAAddPrivateChannel));
	
	Packet.nManagedID = g_Config.nManagedID;
	Packet.tPrivateChatChannel = tPrivateChatChannel;
	
	AddSendData(VIMA_PRIVATECHATCHANNEL_ADD, 0, (char*)&Packet, sizeof(MAAddPrivateChannel));
}

void CDNMasterConnection::SendAddPrivateChatChannelMember(INT64 nChannelID, TPrivateChatChannelMember Member)
{
	MAAddPrivateChannelMember Packet;
	memset(&Packet, 0, sizeof(MAAddPrivateChannelMember));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.Member = Member;

	AddSendData(VIMA_PRIVATECHATCHANNEL_MEMBERADD, 0, (char*)&Packet, sizeof(MAAddPrivateChannelMember));
}

void CDNMasterConnection::SendInvitePrivateChatChannel(INT64 nChannelID, UINT nAccountDBID, WCHAR* wszInviteCharacterName)
{
	MAInvitePrivateChannelMember Packet;
	memset(&Packet, 0, sizeof(MAInvitePrivateChannelMember));
	
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.nMasterAccountDBID = nAccountDBID;
	if(wszInviteCharacterName)
		_wcscpy(Packet.wszInviteName, _countof(Packet.wszInviteName), wszInviteCharacterName, (int)wcslen(wszInviteCharacterName));

	AddSendData(VIMA_PRIVATECHATCHANNEL_MEMBERINVITE, 0, (char*)&Packet, sizeof(MAInvitePrivateChannelMember));
}

void CDNMasterConnection::SendInvitePrivateChatChannelResult(int nRet, UINT nAccountDBID)
{
	MAInvitePrivateChannelMemberResult Packet;
	memset(&Packet, 0, sizeof(MAInvitePrivateChannelMemberResult));
	
	Packet.nRet = nRet;
	Packet.nMasterAccountDBID = nAccountDBID;

	AddSendData(VIMA_PRIVATECHATCHANNEL_MEMBERINVITERESULT, 0, (char*)&Packet, sizeof(MAInvitePrivateChannelMemberResult));
}

void CDNMasterConnection::SendDelPrivateChatChannelMember(PrivateChatChannel::Common::eModType eType, INT64 nChannelID, INT64 biCharacterDBID, const WCHAR* wszName)
{
	MADelPrivateChannelMember Packet;
	memset(&Packet, 0, sizeof(MADelPrivateChannelMember));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.eType = eType;
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.biCharacterDBID = biCharacterDBID;
	if(wszName)
		_wcscpy(Packet.wszName, _countof(Packet.wszName), wszName, (int)wcslen(wszName));

	AddSendData(VIMA_PRIVATECHATCHANNEL_MEMBERDEL, 0, (char*)&Packet, sizeof(MADelPrivateChannelMember));
}

void CDNMasterConnection::SendKickPrivateChatChannelMemberResult(INT64 biCharacterDBID)
{
	MAKickPrivateChannelMemberResult Packet;
	memset(&Packet, 0, sizeof(MAKickPrivateChannelMemberResult));

	Packet.biCharacterDBID = biCharacterDBID;

	AddSendData(VIMA_PRIVATECHATCHANNEL_MEMBERKICKRESULT, 0, (char*)&Packet, sizeof(MAKickPrivateChannelMemberResult));
}

void CDNMasterConnection::SendModPrivateChatChannelInfo(INT64 nChannelID, PrivateChatChannel::Common::eModType eModType, int nPassWord, INT64 biCharacterDBID)
{
	MAModPrivateChannel Packet;
	memset(&Packet, 0, sizeof(MAModPrivateChannel));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.eType = eModType;
	Packet.nPassWord = nPassWord;
	Packet.biCharacterDBID = biCharacterDBID;

	AddSendData(VIMA_PRIVATECHATCHANNEL_MOD, 0, (char*)&Packet, sizeof(MAModPrivateChannel));
}

void CDNMasterConnection::SendModPrivateChatChannelMemberName(INT64 nChannelID, INT64 biCharacterDBID, const WCHAR* wszName )
{
	MAModPrivateChannelMemberName Packet;
	memset(&Packet, 0, sizeof(MAModPrivateChannel));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.biCharacterDBID = biCharacterDBID;

	if(wszName)
		_wcscpy(Packet.wszName, _countof(Packet.wszName), wszName, (int)wcslen(wszName));

	AddSendData(VIMA_PRIVATECHATCHANNEL_MODMEMBERNAME, 0, (char*)&Packet, sizeof(MAModPrivateChannelMemberName));
}
#endif
#if defined( PRE_ADD_MUTE_USERCHATTING )
#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendMuteUserFind(UINT uiGmAccountDBID, char cWorldSetID, const WCHAR * pMuteCharacterName, int nMuteMinute, char cSelectLanguage )
#else	// #if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendMuteUserFind(UINT uiGmAccountDBID, char cWorldSetID, const WCHAR * pMuteCharacterName, int nMuteMinute )
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
{
	MAMuteUserFind Packet;
	memset(&Packet, 0, sizeof(MAMuteUserFind));

	Packet.uiGmAccountID = uiGmAccountDBID;
	Packet.nMuteMinute = nMuteMinute;
#if defined(PRE_ADD_MULTILANGUAGE)
	Packet.cSelectedLanguage = cSelectLanguage;
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
	_wcscpy(Packet.wszMuteUserName, _countof(Packet.wszMuteUserName), pMuteCharacterName, (int)wcslen(pMuteCharacterName));

	AddSendData(VIMA_MUTE_USERFIND, 0, (char*)&Packet, sizeof(Packet));
}
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
void CDNMasterConnection::SendAddAlteiaWorldSendTicket( char cWorldSetID, const WCHAR* wszRecvCharacterName, INT64 biSendCharacterDBID, WCHAR* wszSendCharacterName)
{
	MAAlteiaWorldSendTicket Packet;
	memset(&Packet, 0, sizeof(MAAlteiaWorldSendTicket));

	_wcscpy(Packet.wszRecvCharacterName, _countof(Packet.wszRecvCharacterName), wszRecvCharacterName, (int)wcslen(wszRecvCharacterName));
	_wcscpy(Packet.wszSendCharacterName, _countof(Packet.wszSendCharacterName), wszSendCharacterName, (int)wcslen(wszSendCharacterName));
	Packet.biSendCharacterDBID = biSendCharacterDBID;
	Packet.cWorldSetID = cWorldSetID;

	AddSendData(VIMA_ALTEIAWORLD_SENDTICKET, 0, (char*)&Packet, sizeof(MAAlteiaWorldSendTicket));
}

void CDNMasterConnection::SendAddAlteiaWorldSendTicketResult( int nRetCode, INT64 biSendCharacterDBID )
{
	MAAlteiaWorldSendTicketResult Packet;
	memset(&Packet, 0, sizeof(MAAlteiaWorldSendTicketResult));

	Packet.nRetCode = nRetCode;
	Packet.biSendCharacterDBID = biSendCharacterDBID;

	AddSendData(VIMA_ALTEIAWORLD_SENDTICKET_RESULT, 0, (char*)&Packet, sizeof(MAAlteiaWorldSendTicketResult));
}
#endif
#if defined(PRE_ADD_CHNC2C)
void CDNMasterConnection::SendC2CCoinBalanceResult( int nRetCode, Int64 biCoinBalance, const char* szSeqID)
{
	MAC2CGetCoinBalanceResult Packet;
	memset(&Packet, 0, sizeof(MAC2CGetCoinBalanceResult));

	Packet.nRetCode = nRetCode;
	Packet.biCoinBalance = biCoinBalance;
	memcpy(&Packet.szSeqID, szSeqID, sizeof(Packet.szSeqID));

	AddSendData(VIMA_C2C_CHAR_COINBALANCE, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendC2CAddCoinResult( int nRetCode, const char* szSeqID)
{
	MAC2CAddCoinResult Packet;
	memset(&Packet, 0, sizeof(MAC2CAddCoinResult));

	Packet.nRetCode = nRetCode;
	memcpy(&Packet.szSeqID, szSeqID, sizeof(Packet.szSeqID));

	AddSendData(VIMA_C2C_CHAR_ADDCOIN, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendC2CReduceCoinResult( int nRetCode, const char* szSeqID)
{
	MAC2CReduceCoinResult Packet;
	memset(&Packet, 0, sizeof(MAC2CReduceCoinResult));

	Packet.nRetCode = nRetCode;
	memcpy(&Packet.szSeqID, szSeqID, sizeof(Packet.szSeqID));

	AddSendData(VIMA_C2C_CHAR_REDUCECOIN, 0, (char*)&Packet, sizeof(Packet));
}
#endif //#if defined(PRE_ADD_CHNC2C)

#if defined( PRE_DRAGONBUFF )
void CDNMasterConnection::SendApplyWorldBuff( WCHAR *wszCharacterName, int nItemID, int nMapIdx )
{
	MAApplyWorldBuff Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.nItemID = nItemID;
	Packet.nMapIdx = nMapIdx;

	if(wszCharacterName)
		_wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), wszCharacterName, (int)wcslen(wszCharacterName));

	AddSendData(VIMA_APPLY_WORLDBUFF, 0, (char*)&Packet, sizeof(Packet));
}
#endif

#if defined(PRE_ADD_DWC)
void CDNMasterConnection::SendInviteDWCTeamMember(UINT nTeamID, UINT nAccountDBID, LPCWSTR lpwszFromCharacterName, LPCWSTR lpwszToCharacterName, LPCWSTR lpwszTeamName)
{
	MAInviteDWCTeamMember Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nTeamID = nTeamID;
	Packet.nAccountDBID	= nAccountDBID;
	_wcscpy(Packet.wszFromCharacterName, _countof(Packet.wszFromCharacterName), lpwszFromCharacterName, (int)wcslen(lpwszFromCharacterName));
	_wcscpy(Packet.wszToCharacterName, _countof(Packet.wszToCharacterName), lpwszToCharacterName, (int)wcslen(lpwszToCharacterName));
	_wcscpy(Packet.wszTeamName, _countof(Packet.wszTeamName), lpwszTeamName, (int)wcslen(lpwszTeamName));

	AddSendData(VIMA_INVITE_DWCTEAMMEMB, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendInviteDWCTeamMemberACK(UINT nAccountDBID, int nRetCode, bool bAck, LPCWSTR pwszToCharacterName)
{
	MAInviteDWCTeamMemberAck Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nRetCode		= nRetCode;
	Packet.nInviterDBID = nAccountDBID;
	Packet.bAck			= bAck;

	if (pwszToCharacterName)
		_wcscpy(Packet.wszInvitedName, NAMELENMAX, pwszToCharacterName, (int)wcslen(pwszToCharacterName));

	AddSendData(VIMA_INVITE_DWCTEAMMEMB_ACK, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendAddDWCTeamMember(UINT nTargetUserAccountDBID, UINT nTeamID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszFromCharacterName, TP_JOB nJob, TCommunityLocation* pLocation, bool bAlredySentByVillage)
{
	MAAddDWCTeamMember Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nTartgetUserAccountDBID = nTargetUserAccountDBID;
	Packet.nTeamID = nTeamID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.biCharacterDBID = nCharacterDBID;
	Packet.nJob = nJob;
	if(pLocation)
		Packet.Location = (*pLocation);
	Packet.bAlredySentByVillage = bAlredySentByVillage;
	_wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), lpwszFromCharacterName, (int)wcslen(lpwszFromCharacterName));

	AddSendData(VIMA_ADD_DWCTEAMMEMB, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendDismissDWCTeam(UINT nTargetUserAccountDBID, UINT nTeamID, int nRet, bool bAlredySentByVillage)
{
	MADismissDWCTeam Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nTartgetUserAccountDBID = nTargetUserAccountDBID;
	Packet.nRet = nRet;
	Packet.nTeamID = nTeamID;
	Packet.bAlredySentByVillage = bAlredySentByVillage;

	AddSendData(VIMA_DISMISS_DWCTEAM, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendLeaveDWCTeamMember(UINT nTargetUserAccountDBID, UINT nTeamID, INT64 biLeaveUserCharacterDBID, LPCWSTR pwszCharacterName, int nRet, bool bAlredySentByVillage)
{
	MALeaveDWCTeamMember Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nTartgetUserAccountDBID = nTargetUserAccountDBID;
	Packet.nRet = nRet;
	Packet.biLeaveUserCharacterDBID = biLeaveUserCharacterDBID;
	Packet.nTeamID = nTeamID;
	if (pwszCharacterName)
		_wcscpy(Packet.wszCharacterName, NAMELENMAX, pwszCharacterName, (int)wcslen(pwszCharacterName));
	Packet.bAlredySentByVillage = bAlredySentByVillage;

	AddSendData(VIMA_DEL_DWCTEAMMEMB, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendChangeDWCTeamMemberState(UINT nTargetUserAccountDBID, UINT nTeamID, INT64 biCharacterDBID, TCommunityLocation* pLocation, bool bLogin)
{
	MAChangeDWCTeamMemberState Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nTartgetUserAccountDBID = nTargetUserAccountDBID;
	Packet.nTeamID = nTeamID;
	Packet.biCharacterDBID = biCharacterDBID;
	if(pLocation)
		Packet.Location = (*pLocation);
	Packet.bLogin = bLogin;

	AddSendData(VIMA_CHANGE_DWCTEAMMEMB_STATE, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendDWCTeamMemberList(UINT nAccountDBID, UINT nTeamID, std::vector<INT64> &VecMemberList)
{
	VIMADWCTeamMemberList Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nAccountDBID = nAccountDBID;
	Packet.nTeamID = nTeamID;
	for(int i = 0 ; i < MIN(VecMemberList.size(), DWC::DWC_MAX_MEMBERISZE) ; i++ )
		Packet.MemberListCharacterDBID[Packet.cCount++] = VecMemberList[i];

	int nLen = sizeof(Packet) - sizeof(Packet.MemberListCharacterDBID) + sizeof(INT64)*Packet.cCount;
	AddSendData(VIMA_DWC_TEAMMEMBERLIST, 0, reinterpret_cast<char*>(&Packet), nLen);
}

void CDNMasterConnection::SendDWCTeamChat(UINT nTeamID, UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszChatMsg, short nLen)
{
	MADWCTeamChat Packet;
	memset (&Packet, 0, sizeof(Packet));

	Packet.nTeamID = nTeamID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.biCharacterDBID = nCharacterDBID;

	_wcscpy( Packet.wszChatMsg, _countof(Packet.wszChatMsg), lpwszChatMsg, nLen );
	Packet.nLen = nLen;

	int iSize = static_cast<int>(sizeof(Packet) - sizeof(Packet.wszChatMsg) + sizeof(Packet.wszChatMsg[0]) * Packet.nLen);

	AddSendData(VIMA_DWC_TEAMCHAT, 0, reinterpret_cast<char*>(&Packet), iSize);
}
#endif

#if defined( PRE_PVP_GAMBLEROOM )
void CDNMasterConnection::SendCreateGambleRoomFlag(bool bFlag)
{
	MACreateGambleRoom Packet;
	memset(&Packet, 0, sizeof(Packet));
	
	Packet.bFlag = bFlag;

	AddSendData(VIMA_GAMBLE_CREATEROOMFLAG, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendDelGambleRoom()
{
	MADelGambleRoom Packet;
	memset(&Packet, 0, sizeof(Packet));

	AddSendData(VIMA_GAMBLE_DELETEROOM, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}
#endif