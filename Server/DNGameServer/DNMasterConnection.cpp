#include "StdAfx.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "DNUserSession.h"
#include "DNGameServerManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNRUDPGameServer.h"
#include "DNIocpManager.h"
#include "Log.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DNWorldUserState.h"
#include "DNUserTcpconnection.h"
#include "DNGameRoom.h"
#include "DNServiceConnection.h"
#include "NoticeSystem.h"
#include "DNFarmGameRoom.h"
#include "DNGuildSystem.h"
#include "DNPeriodQuestSystem.h"
#if defined(_WORK)
#include "EtActionCoreMng.h"
#include "PsUpdater.h"
#endif	// #if defined(_WORK)
#include "DNGameDataManager.h"

#if defined (PRE_ADD_DONATION)
#include "DNDonationScheduler.h"
#endif // #if defined (PRE_ADD_DONATION)

#if defined( PRE_PRIVATECHAT_CHANNEL )
#include "DnPrivateChatManager.h"
#include "DNPrivateChatChannel.h"
#endif

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
#include "DNPvPGameRoom.h"
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

extern TGameConfig g_Config;

CDNMasterConnection::CDNMasterConnection(void): CConnection()
{
	m_bConnectCompleted = false;
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)
	m_PreWinGuildUID.Reset();
	GuildWarReset();
}

CDNMasterConnection::~CDNMasterConnection(void)
{
}

bool CDNMasterConnection::GetActive()
{
	if (m_bConnectCompleted == false)
		return false;
	return CConnection::GetActive();
}

void CDNMasterConnection::Reconnect()
{
	//이쪽은 동기가 틀려서 DNMasterConnection및 게임 리소스 억세스 하면 안데요~
	if (!CConnection::GetActive() && !GetConnecting())
	{
		SetConnecting(true);
		if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_MASTER, m_szIP, m_wPort) < 0)
		{
			SetConnecting(false);
			g_Log.Log(LogType::_ERROR, L"MasterServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
		}
		else 
		{
#if defined( STRESS_TEST )
#else
			g_Log.Log(LogType::_NORMAL, L"MasterServer (%S, %d) Connecting..\r\n", m_szIP, m_wPort);			
#endif // #if defined( STRESS_TEST )
		}
	}
}

bool CDNMasterConnection::PreMessageProcess(int nMainCmd, int nSubCmd, char * pData, int nLen)
{
	switch (nMainCmd)
	{
#ifdef _WORK
		case MAGA_RELOADEXT:
			{
				if (g_pDataManager->AllLoad() == false)
				{
					_DANGER_POINT_MSG(L"ext reload에 실패하였습니다. 서버를 재기동해주세요");
				}
				break;
			}

		case MAGA_RELOADACT:
			{
				g_ActionCoreMng.ReleaseAllContainer();
				break;
			}
#endif		//

	case MAGA_REGISTWORLDINFO:
		{
								 /*
#if defined(_WORK)
			if (!g_PsUpdater)
				g_PsUpdater = new(std::nothrow) CPsUpdater;

			g_PsUpdater->AsyncUpdate ();
#endif	// #if defined(_WORK)
			*/
			MARegist *pRegist = (MARegist*)pData;
			if (pRegist->cWorldSetID <= 0) 
				return ERROR_NONE;		// 0이면 걍 무시. 0이상 값으로 어차피 한번 더 온다.

			m_cWorldSetID = pRegist->cWorldSetID;
			m_cGameID = pRegist->cGameID;		//이 마스터에서의 나(게임서버의 식별자)
			g_pMasterConnectionManager->AddWorldIDConnection(m_nSessionID, m_cWorldSetID);

			//GameServerInfo Notice to MasterServer
			unsigned char cServerIdx[64];
			unsigned long iIP[64];
			unsigned short iPort[64];
			bool bMargin[64];
			memset(cServerIdx, 0, sizeof(cServerIdx));
			memset(iIP, 0, sizeof(iIP));
			memset(iPort, 0, sizeof(iPort));
			memset(bMargin, 0, sizeof(bMargin));

			g_pGameServerManager->GetGameServerInfo(iIP, iPort, cServerIdx, bMargin);
			if (SendGameServerInfoList(iIP, iPort, cServerIdx, bMargin, g_Config.nManagedID) == false)
			{
				DetachConnection(L"MAGA_REGISTWORLDINFO");
				break;
			}

			g_Log.Log(LogType::_NORMAL, m_cWorldSetID, 0, 0, 0, L"[MAGA_REGIST] WorldID:%d\r\n", m_cWorldSetID);

			BYTE cThreadID;
			CDNDBConnection *pDBCon = g_pDBConnectionManager->GetDBConnection(cThreadID);
			if (pDBCon)
			{
				//시작하면서 얻어야 하는 것 모음
				if (g_pEvent->IsInitWorld(m_cWorldSetID) == false)
					pDBCon->QueryEventList(cThreadID, m_cWorldSetID);

				if (g_pPeriodQuestSystem)
					g_pPeriodQuestSystem->LoadWorldQuestInfo(m_cWorldSetID);

#if defined (PRE_ADD_DONATION)
				if (CDNDonationScheduler::IsActive())
					CDNDonationScheduler::GetInstance().AddWorldID(m_cWorldSetID);
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PRIVATECHAT_CHANNEL )
				if( g_pPrivateChatChannelManager && pDBCon->GetActive() )
				{
					pDBCon->QueryGetPrivateChatChannelInfo(0, m_cWorldSetID);	
					pDBCon->QueryDelPrivateMemberServerID(0, m_cWorldSetID, g_Config.nManagedID );
				}
#endif
			}

#if defined( PRE_FIX_WORLDCOMBINEPARTY )
			if( g_Config.bWorldCombineGameServer )
			{
				SendWorldCombineGameServer();
			}
#endif
		}
		break;

	case MAGA_REQUSERLIST:
		{
			MAReqUserList * pPacket = (MAReqUserList*)pData;
			if (pPacket->nRetCode == ERROR_NONE)
			{
				g_pGameServerManager->ConnectedWorld(m_cWorldSetID);
				g_Log.Log(LogType::_NORMAL, m_cWorldSetID, 0, 0, 0, L"[MAGA_REQUSERLIST] WorldID:%d\r\n", m_cWorldSetID);
			}
			else
				g_Log.Log(LogType::_ERROR, m_cWorldSetID, 0, 0, 0, L"[MAGA_REQUSERLIST] Master Registration Failed WorldID:%d\r\n", m_cWorldSetID);
		}
		break;

	case MAGA_REGISTCOMPLETE:
		{
			m_bConnectCompleted = true;
			//check
#if defined( PRE_WORLDCOMBINE_PARTY )
			if (g_pMasterConnectionManager->IsMasterConnected(m_cWorldSetID))
				g_pIocpManager->VerifyAccept(ACCEPTOPEN_VERIFY_TYPE_MASTERCONNECTION, m_cWorldSetID );
#else
			if (g_pMasterConnectionManager->IsAllMasterConnected())
				g_pIocpManager->VerifyAccept(ACCEPTOPEN_VERIFY_TYPE_MASTERCONNECTION);
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
			g_Log.Log(LogType::_NORMAL, m_cWorldSetID, 0, 0, 0, L"[MAGA_REGISTCOMPLETE] WorldID:%d\r\n", m_cWorldSetID);
		}
		break;

	case MAGA_REQROOMID:
		{
			//여기가 게임이 진행이 되는 엔트리다. 
			CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServer();

			if (pServer)
			{
				pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
			else
			{
				MAGAReqRoomID *pReqRoomID = (MAGAReqRoomID*)pData;
				SendSetRoomID( pReqRoomID->GameTaskType, pReqRoomID->cReqGameIDType, pReqRoomID->InstanceID, 0, 0, pReqRoomID->cVillageID, NULL, 0 );
				g_Log.Log( LogType::_GAMECONNECTLOG, L"GetGameServer Failed!!\n" );
			}
		}
		break;

	case MAGA_REQTUTORIALROOMID:
		{
			//여기가 게임이 진행이 되는 엔트리다. 
			CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServer();
			if (pServer)
				pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			else
			{
				MAGAReqTutorialRoomID *pReqRoomID = (MAGAReqTutorialRoomID*)pData;
				SendSetTutorialRoomID(pReqRoomID->nAccountDBID, 0, 0, pReqRoomID->nLoginServerID);
				_DANGER_POINT();
			}
		}
		break;

	case MAGA_USERSTATE:
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
			memset( wszName, 0, sizeof(wszName) );
			_wcscpy(wszName, NAMELENMAX, pPacket->wszBuf, pPacket->cNameLen);

			if (pPacket->Type == WorldUserState::Add )
			{
				g_pWorldUserState->AddUserState(wszName, pPacket->biCharacterDBID, pPacket->cLocationState, pPacket->cCommunityState, pPacket->nChannelID, pPacket->nMapIdx);
			}
			else if (pPacket->Type == WorldUserState::Delete )
			{
				g_pWorldUserState->DelUserState(wszName, pPacket->biCharacterDBID);
			}
			else if (pPacket->Type == WorldUserState::Modify )
			{
				g_pWorldUserState->UpdateUserState(wszName, pPacket->biCharacterDBID, pPacket->cLocationState, pPacket->cCommunityState, pPacket->nChannelID, pPacket->nMapIdx);
			}
			else
				_DANGER_POINT();
		}
		break;

	case MAGA_NOTICE:
		{
			MANotice * pPacket = (MANotice*)pData;

			if (pPacket->nLen > CHATLENMAX-1)	break;
			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pPacket->wszNoticeMsg, pPacket->nLen);

			if (g_pNoticeSystem && pPacket->Info.nSlideShowSec > 0 && g_pNoticeSystem->AddNotice(&pPacket->Info, wszChatMsg) == false)
				break;

			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;

	case MAGA_CANCELNOTICE:
		{
			if (g_pNoticeSystem->CancelNotice())
			{
				for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
				{
					CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
					if (pServer)
						pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
				}
			}
		}
		break;

	case MAGA_CHAT:
		{
			MAChat * pPacket = (MAChat*)pData;
#if defined(PRE_ADD_WORLD_MSG_RED)
			if (pPacket->cType == CHATTYPE_WORLD || pPacket->cType == CHATTYPE_WORLD_POPMSG)
#else	// #if defined(PRE_ADD_WORLD_MSG_RED)
			if (pPacket->cType == CHATTYPE_WORLD)
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
			{
				for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
				{
					CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
					if (pServer)
						pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
				}
			}
			else
				return false;		//월드메세지가 아니라면 세션처리로 넘긴다.
			break;
		}

	case MAGA_WORLDSYSTEMMSG:
		{
			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
#if defined( PRE_PRIVATECHAT_CHANNEL )
	case MAGA_PRIVATECHANNELCHAT:
		{
			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
#endif

	case MAGA_RESRECALL:
	{
		MAGAResRecall* pPacket = reinterpret_cast<MAGAResRecall*>(pData);

		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiRecallAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			else
				_DANGER_POINT();
		}
		else
			_DANGER_POINT();			
		break;
	}		

	//=============================================================================================================
	// PvP Start
	//=============================================================================================================

		case MAGA_PVP_GAMEMODE:
		{
			MAGAPVP_GAMEMODE* pPacket = reinterpret_cast<MAGAPVP_GAMEMODE*>(pData);

			if( g_pGameServerManager )
			{
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->iGameRoomID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
				else
					_DANGER_POINT();
			}
			else
				_DANGER_POINT();			
		}
		break;

		case MAGA_BREAKINTOROOM:
		{
			MAGABreakIntoRoom* pPacket = reinterpret_cast<MAGABreakIntoRoom*>(pData);

			if( g_pGameServerManager )
			{
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->iGameRoomID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
				else
				{
					if( pPacket->BreakIntoType == BreakInto::Type::WorldZoneParty )
						SendInvitedMemberReturnMsg(pPacket->uiAccountDBID, ERROR_PARTY_JOINFAIL);
#if defined( PRE_PARTY_DB )
					else if( pPacket->BreakIntoType == BreakInto::Type::PartyRestore )
						g_pMasterConnectionManager->SendConfirmLastDungeonInfo( pPacket->cWorldSetID, ERROR_CANT_EXSIT_BACKUPPARTYINFO, pPacket->uiAccountDBID, pPacket->BreakIntoType );
					else if( pPacket->BreakIntoType == BreakInto::Type::PartyJoin )
						g_pMasterConnectionManager->SendConfirmLastDungeonInfo( pPacket->cWorldSetID, ERROR_PARTY_JOINFAIL, pPacket->uiAccountDBID, pPacket->BreakIntoType );
#else
					else if( pPacket->BreakIntoType == BreakInto::Type::PartyRestore )
						g_pMasterConnectionManager->SendConfirmLastDungeonInfo( pPacket->cWorldSetID, ERROR_CANT_EXSIT_BACKUPPARTYINFO, pPacket->uiAccountDBID );
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_WORLDCOMBINE_PVP )
					else if( pPacket->BreakIntoType == BreakInto::Type::WorldPvPRoom )
						g_pMasterConnectionManager->SendWorldPvPRoomJoinResult( pPacket->cWorldSetID, ERROR_PVP_JOINROOM_NOTFOUNDROOM, pPacket->uiAccountDBID );
#endif
				}
			}
			else
				_DANGER_POINT();
		}
		break;
		case MAGA_PVP_MEMBERUSERSTATE:
			{
				MAGAPVPChangeUserState * pPacket = (MAGAPVPChangeUserState*)pData;

				if (g_pGameServerManager)
				{
					CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->nPvPRoomID );
					if( pServer )
						pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
				}
				break;
			}
		case MAGA_PVP_FORCESTOP:
			{
				MAGAForceStopPvP * pPacket = (MAGAForceStopPvP*)pData;

				if (g_pGameServerManager)
				{
					CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByRoomID(pPacket->nGameRoomID);
					if (pServer)
						pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
					else
						_DANGER_POINT();
				}
				break;
			}
		case MAGA_PVP_ALLSTOPGUILDWAR :
			{
				if (g_pGameServerManager)
				{
					for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
					{
						CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
						if (pServer)
							pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, NULL, 0, EXTERNALTYPE_MASTER, m_cWorldSetID);
					}
				}
				break;
			}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		case MAGA_PVP_TOURNAMENT_INFO :
			{
				MAGAPVP_TOURNAMENT_INFO* pPacket = reinterpret_cast<MAGAPVP_TOURNAMENT_INFO*>(pData);

				if( g_pGameServerManager )
				{
					CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->nGameRoomID );
					if( pServer )
						pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
					else
						_DANGER_POINT();
				}
				else
					_DANGER_POINT();
			}
#endif

	//=============================================================================================================
	// PvP End
	//=============================================================================================================

	// Auth
	case MAGA_RESETAUTHLIST:
		{
			MAResetAuthList *pQ = reinterpret_cast<MAResetAuthList*>(pData);
			DN_ASSERT(NULL != pQ,									"Invalid!");
			DN_ASSERT(CHECK_RANGE(pQ->cWorldSetID, 1, WORLDCOUNTMAX),	"Invalid!");
			DN_ASSERT(!pQ->IsEmpty(),								"Invalid!");

			MAResetAuthOnly aA;
			aA.cWorldSetID = pQ->cWorldSetID;

			for (int iIndex = 0 ; pQ->nCount > iIndex ; ++iIndex) {
				const MAResetAuthNode* pResetAuthNode = pQ->At(iIndex);
				if (!pResetAuthNode) {
					DN_CONTINUE;
				}

				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByAID(pResetAuthNode->uiAccountDBID);		//account id 여야합니다.
				if (pServer) {
					aA.nSessionID = pResetAuthNode->nSessionID;
					aA.nAccountDBID = pResetAuthNode->uiAccountDBID;
					aA.cWorldSetID = pQ->cWorldSetID;
					pServer->StoreExternalBuffer(pResetAuthNode->uiAccountDBID, MAGA_RESETAUTHONLY, 0, reinterpret_cast<char*>(&aA), sizeof(aA), EXTERNALTYPE_MASTER, m_cWorldSetID);
				}
			}
		}
		break;

	// Guild
	case MAGA_DISMISSGUILD:				// 길드 해체 알림
		{
			g_pGuildManager->OnRecvMaDismissGuild(reinterpret_cast<MADismissGuild*>(pData));
		}
		break;


	case MAGA_ADDGUILDMEMBER:				// 길드원 추가 알림
		{
			g_pGuildManager->OnRecvMaAddGuildMember(reinterpret_cast<MAAddGuildMember*>(pData));
		}
		break;

	case MAGA_DELGUILDMEMBER:				// 길드원 제거 (탈퇴/추방) 알림
		{
			g_pGuildManager->OnRecvMaDelGuildMember(reinterpret_cast<MADelGuildMember*>(pData));
		}
		break;

	case MAGA_CHANGEGUILDINFO:			// 길드 정보 변경 알림
		{
			g_pGuildManager->OnRecvMaChangeGuildInfo(reinterpret_cast<MAChangeGuildInfo*>(pData));
		}
		break;

	case MAGA_CHANGEGUILDMEMBERINFO:		// 길드원 정보 변경 알림
		{
			g_pGuildManager->OnRecvMaChangeGuildMemberInfo(reinterpret_cast<MAChangeGuildMemberInfo*>(pData));
		}
		break;

	case MAGA_GUILDCHAT:				// 길드 채팅 알림
		{
			g_pGuildManager->OnRecvMaGuildChat(reinterpret_cast<MAGuildChat*>(pData));
		}
		break;

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	case MAGA_GUILDCHAT_MOBILE:
		{
			g_pGuildManager->OnRecvMaDoorsGuildChat(reinterpret_cast<MADoorsGuildChat*>(pData));
		}
		break;
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

	case MAGA_GUILDMEMBER_LEVELUP:
		{
			g_pGuildManager->OnRecvMaMemberLevelUp(reinterpret_cast<MAGuildMemberLevelUp*>(pData));
		}
		break;

	case  MAGA_CHANGEGUILDNAME :
		{
			g_pGuildManager->OnRecvMaGuildChangeName(reinterpret_cast<MAGuildChangeName*>(pData));
		}
		break;

	case  MAGA_CHANGEGUILDMARK :
		{
			g_pGuildManager->OnRecvMaGuildChangeMark(reinterpret_cast<MAGuildChangeMark*>(pData));
		}
		break;
	case MAGA_UPDATEGUILDEXP:
		{
			g_pGuildManager->OnRecvMaGuildUpdateExp(reinterpret_cast<MAUpdateGuildExp*>(pData));
		}
		break;
	case MAGA_ENROLLGUILDWAR:
		{
			g_pGuildManager->OnRecvMaEnrollGuildWar(reinterpret_cast<MAEnrollGuildWar*>(pData));
		}
		break;
	case MAGA_CHANGE_GUILDWAR_STEP :
		{
			MAChangeGuildWarStep* pPacket = (MAChangeGuildWarStep*)pData;

			if (pPacket->bCheatFlag)
				SetGuildWarStepIndex(GUILDWAR_STEP_NONE);

			bool bChangeEvent = false;
			// 스케쥴ID가 같으면 업데이트
			if (GetGuildWarScheduleID() == pPacket->wScheduleID)
			{
				if (pPacket->cEventType == GUILDWAR_EVENT_START && GetGuildWarStepIndex() < pPacket->cEventStep)
					bChangeEvent = true;
				else if (pPacket->cEventType == GUILDWAR_EVENT_END && GetGuildWarStepIndex() == pPacket->cEventStep)
					bChangeEvent = true;
			}
			else // 스케쥴ID가 다르면 새로운 등록
				bChangeEvent = true;

			if (bChangeEvent)
			{
				SetGuildWarScheduleID(pPacket->wScheduleID);
				SetGuildWarStepIndex(pPacket->cEventStep);
			}
			
			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
			// 마지막이면 초기화
			if( pPacket->cEventStep == GUILDWAR_STEP_REWARD && pPacket->cEventType == GUILDWAR_EVENT_END )
				GuildWarReset();
		}
		break;
	case MAGA_SETGUILDWAR_POINT :
		{
			MASetGuildWarPoint * pPacket = (MASetGuildWarPoint*)pData;
			SetTeamPoint(pPacket->nBlueTeamPoint, pPacket->nRedTeamPoint);
		}
		break;
	case MAGA_ADDGUILDWAR_POINT :
		{
			MAAddGuildWarPoint * pPacket = (MAAddGuildWarPoint*)pData;
			if( pPacket->cTeamType == GUILDWAR_TEAM_BLUE )
				AddBlueTeamPoint(pPacket->nAddPoint);			
			else if( pPacket->cTeamType == GUILDWAR_TEAM_RED)
				AddRedTeamPoint(pPacket->nAddPoint);
		}
		break;
	case MAGA_SETGUILDWAR_SECRETMISSION:
		{
			MASetGuildWarSecretMission * pPacket = (MASetGuildWarSecretMission*)pData;
			SetGuildSecretMission(pPacket);
			break;
		}
	case MAGA_SETGUILDWAR_FINALPROCESS :
		{
			MASetGuildWarFinalProcess* pPacket = (MASetGuildWarFinalProcess*)pData;
			if( pPacket->tBeginTime == 0)
				SetGuildWarFinalPart(pPacket->cGuildFinalPart);
			else
				SetGuildWarFinalPart(pPacket->cGuildFinalPart+1);

			// 우승길드 셋팅되어 있으면 초기화
			if( pPacket->cGuildFinalPart != GUILDWAR_FINALPART_NONE && m_PreWinGuildUID.IsSet() ) 
			{
				MAGuildWarPreWinGuild aA;
				aA.GuildUID	= m_PreWinGuildUID;
				aA.bPreWin = false;	

				for (int i = 0 ; i < g_pGameServerManager->GetGameServerSize(); ++i) 
				{
					CDNRUDPGameServer *pServer = g_pGameServerManager->GetGameServerIndex (i);
					if (pServer)
						pServer->StoreExternalBuffer (0, MAGA_SETGUILDWAR_PRE_WIN_GUILD, 0, reinterpret_cast<char*>(&aA), sizeof(aA), EXTERNALTYPE_MASTER, m_cWorldSetID);
				}
				m_PreWinGuildUID.Reset();
			}
		}
		break;
	case MAGA_SETGUILDWAR_PRE_WIN_GUILD :
		{
			MAGuildWarPreWinGuild* pPacket = (MAGuildWarPreWinGuild*)pData;	
			if( pPacket->bPreWin )			
				SetPreWinGuildUID(pPacket->GuildUID);
			else
				m_PreWinGuildUID.Reset();

			g_Log.Log(LogType::_GUILDWAR, L"[GUILDWAR] GameServer Set PreWinGuildReward! GuildID:%d\n", pPacket->GuildUID.nDBID);
			
			for (int i = 0 ; i < g_pGameServerManager->GetGameServerSize(); ++i) 
			{
				CDNRUDPGameServer *pServer = g_pGameServerManager->GetGameServerIndex (i);
				if (pServer)
					pServer->StoreExternalBuffer (0, MAGA_SETGUILDWAR_PRE_WIN_GUILD, 0, reinterpret_cast<char*>(pPacket), sizeof(MAGuildWarPreWinGuild), EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
	case MAGA_SETGUILDWAR_TOURNAMENTWIN :
		{
			MASetGuildWarTournamentWin* pPacket = (MASetGuildWarTournamentWin*)pData;
			for (int i = 0 ; i < g_pGameServerManager->GetGameServerSize(); ++i) 
			{
				CDNRUDPGameServer *pServer = g_pGameServerManager->GetGameServerIndex (i);
				if (pServer)
					pServer->StoreExternalBuffer (0, MAGA_SETGUILDWAR_TOURNAMENTWIN, 0, reinterpret_cast<char*>(pData), sizeof(MASetGuildWarTournamentWin), EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
	case MAGA_SETGUILDWAR_EVENT_TIME :
		{
			MASetGuildWarEventTime* pPacket = (MASetGuildWarEventTime*)pData;
			SetGuildWarSechdule(pPacket);			
		}
		break;
	case MAGA_GUILDRECRUIT_MEMBER:
		{		
			MAGuildRecruitMember* pPacket = reinterpret_cast<MAGuildRecruitMember*>(pData);
			if( g_pGameServerManager )
			{
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
			}
			break;
		}
		break;
	case MAGA_GUILD_ADD_REWARDITEM:
		{
			MAGuildRewardItem* pPacket = (MAGuildRewardItem*)pData;							

			for (int i = 0 ; i < g_pGameServerManager->GetGameServerSize(); ++i) 
			{
				CDNRUDPGameServer *pServer = g_pGameServerManager->GetGameServerIndex (i);
				if (pServer)
					pServer->StoreExternalBuffer (0, nMainCmd, 0, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
	case MAGA_GUILD_EXTEND_GUILDSIZE:
		{
			MAExtendGuildSize* pPacket = (MAExtendGuildSize*)pData;							

			for (int i = 0 ; i < g_pGameServerManager->GetGameServerSize(); ++i) 
			{
				CDNRUDPGameServer *pServer = g_pGameServerManager->GetGameServerIndex (i);
				if (pServer)
					pServer->StoreExternalBuffer (0, nMainCmd, 0, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
	case MAGA_CHANGE_CHARACTERNAME:
		{
			MAChangeCharacterName *pPacket = (MAChangeCharacterName*)pData;

			CDNGuildBase* pGuild = g_pGuildManager->At (pPacket->GuildUID);
			if (pGuild)
				pGuild->UpdateMemberName (pPacket->nCharacterDBID, pPacket->wszCharacterName);

			for (int i = 0 ; i < g_pGameServerManager->GetGameServerSize(); ++i) 
			{
				CDNRUDPGameServer *pServer = g_pGameServerManager->GetGameServerIndex (i);
				if (pServer)
					pServer->StoreExternalBuffer (0, MAGA_CHANGE_CHARACTERNAME, 0, reinterpret_cast<char*>(pPacket), sizeof(MAChangeCharacterName), EXTERNALTYPE_MASTER);
			}
		}
		break;

	case MAGA_ZEROPOPULATION:
		{
			MAGAZeroPopulation * pPacket = (MAGAZeroPopulation*)pData;

			g_pGameServerManager->SetZeroPopulation(pPacket->bZeroPopulation);
			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}

			// 실시간 보고
			if (g_pServiceConnection)
			{
				if( g_pServiceConnection->GetSocketContextPtr() == NULL )
					break;

				CScopeInterlocked Scope( &g_pServiceConnection->GetSocketContextPtr()->m_lActiveCount );
				if( Scope.bIsDelete() )
					break;

				g_pServiceConnection->RealTimeReport();
			}
		}
		break;
	case MAGA_MASTERSYSTEM_SYNC_SIMPLEINFO:
	{
		MasterSystem::MAGASyncSimpleInfo* pPacket = reinterpret_cast<MasterSystem::MAGASyncSimpleInfo*>(pData);
		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		}
		break;
	}
	case MAGA_MASTERSYSTEM_SYNC_JOIN:
	{
		MasterSystem::MAGASyncJoin* pPacket = reinterpret_cast<MasterSystem::MAGASyncJoin*>(pData);
		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		}
		break;
	}
	case MAGA_MASTERSYSTEM_SYNC_LEAVE:
	{
		MasterSystem::MAGASyncLeave* pPacket = reinterpret_cast<MasterSystem::MAGASyncLeave*>(pData);
		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		}
		break;
	}
	case MAGA_MASTERSYSTEM_SYNC_GRADUATE:
	{
		MasterSystem::MAGASyncGraduate* pPacket = reinterpret_cast<MasterSystem::MAGASyncGraduate*>(pData);
		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		}
		break;
	}
	case MAGA_MASTERSYSTEM_SYNC_CONNECT:
		{
			MasterSystem::MAGASyncConnect* pPacket = reinterpret_cast<MasterSystem::MAGASyncConnect*>(pData);
			if( g_pGameServerManager )
			{
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
			}
			break;
		}
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	case MAGA_SYNC_SYSTEMMAIL:
	{
		MAGASyncSystemMail* pPacket = reinterpret_cast<MAGASyncSystemMail*>(pData);
		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->uiAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		}
		break;
	}
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	case MAGA_FARM_SYNC:
	case MAGA_FARM_SYNC_ADDWATER:
	{
		UINT uiAccountDBID = 0;
		memcpy( &uiAccountDBID, pData, sizeof(UINT) );
		if( g_pGameServerManager )
		{
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( uiAccountDBID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		}
		break;
	}

	case MAGA_UPPDATE_WORLDEVENTCOUNTER:
		{
			MAUpdateWorldEventCounter* pPacket = reinterpret_cast<MAUpdateWorldEventCounter*>(pData);

			if (g_pPeriodQuestSystem)
				g_pPeriodQuestSystem->UpdateWorldEvent(pPacket->cWorldSetID, pPacket->nScheduleID, pPacket->nCount);

			break;
		}
	case MAGA_CHECK_LASTDUNGEONINFO:
	{
		MAGACheckLastDungeonInfo* pPacket = reinterpret_cast<MAGACheckLastDungeonInfo*>(pData);

		CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->iRoomID );
		if( pServer )
			pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		else
			g_pMasterConnectionManager->SendCheckLastDungeonInfo( pPacket->cWorldSetID, pPacket->uiAccountDBID, pPacket->biCharacterDBID, false );
		break;
	}
	case MAGA_DELETE_BACKUPDUNGEONINFO:
	{
		MAGADeleteBackupDungeonInfo* pPacket = reinterpret_cast<MAGADeleteBackupDungeonInfo*>(pData);
		CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->iRoomID );
		if( pServer )
			pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
		break;
	}

#if defined( PRE_WORLDCOMBINE_PARTY )
	case MAGA_GET_WORLDPARTYMEMBER:
		{
			MAGetWorldPartyMember* pPacket = reinterpret_cast<MAGetWorldPartyMember*>(pData);
			CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->iRoomID );
			if( pServer )
				pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
			else
			{
				Party::MemberData MemberData[PARTYCOUNTMAX];
				memset(MemberData, 0, sizeof(Party::MemberData)*PARTYCOUNTMAX);
				g_pMasterConnectionManager->SendWorldPartyMember(m_cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->PartyID, 0, NULL, ERROR_PARTY_JOINFAIL);
			}
			break;
		}
#endif
#if defined( PRE_ADD_BESTFRIEND )
	case MAGA_CANCEL_BESTFRIEND:
		{	
			MACancelBestFriend* pPacket = reinterpret_cast<MACancelBestFriend*>(pData);
			if( g_pGameServerManager )
			{				
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->nAccountDBID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
			}
		}
		break;
	case MAGA_CLOSE_BESTFRIEND:
		{	
			MACloseBestFriend* pPacket = reinterpret_cast<MACloseBestFriend*>(pData);
			if( g_pGameServerManager )
			{
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->nAccountDBID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
			}
		}
		break;
	case  MAGA_LEVELUP_BESTFRIEND:
		{
			MALevelUpBestFriend* pPacket = reinterpret_cast<MALevelUpBestFriend*>(pData);
			if( g_pGameServerManager )
			{
				CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID( pPacket->nAccountDBID );
				if( pServer )
					pServer->StoreExternalBuffer( 0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID );
			}
		}
		break;
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	case  MAGA_PRIVATECHATCHANNEL_ADD:
		{
			MAAddPrivateChannel* pPacket = reinterpret_cast<MAAddPrivateChannel*>(pData);
			
			if( g_pPrivateChatChannelManager && pPacket->nManagedID != g_Config.nManagedID )
			{
				g_pPrivateChatChannelManager->AddPrivateChatChannel(pPacket->cWorldSetID, pPacket->tPrivateChatChannel);
				CDNPrivateChaChannel* pPrivateChatChannel =  g_pPrivateChatChannelManager->GetPrivateChannelInfo( pPacket->cWorldSetID, pPacket->tPrivateChatChannel.nPrivateChatChannelID);
				if( pPrivateChatChannel )
					pPrivateChatChannel->SetLoaded(true);
			}			
		}
		break;
	case  MAGA_PRIVATECHATCHANNEL_MEMBERADD:
		{
			MAAddPrivateChannelMember* pPacket = reinterpret_cast<MAAddPrivateChannelMember*>(pData);	
			
			CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo(m_cWorldSetID, pPacket->nPrivateChatChannelID);
			if( pPrivateChatChannel && pPacket->nManagedID != g_Config.nManagedID )
			{
				pPrivateChatChannel->AddPrivateChannelMember(pPacket->Member);
			}			

			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;
	case  MAGA_PRIVATECHATCHANNEL_MEMBERINVITE:
		{
			MAInvitePrivateChannelMember* pPacket = reinterpret_cast<MAInvitePrivateChannelMember*>(pData);	
			
			CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByAID(pPacket->nInviteAccountDBID);
			if (pServer)
				pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
		}
		break;
	case  MAGA_PRIVATECHATCHANNEL_MEMBERINVITERESULT:
		{
			MAInvitePrivateChannelMemberResult* pPacket = reinterpret_cast<MAInvitePrivateChannelMemberResult*>(pData);	
			
			CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByAID(pPacket->nMasterAccountDBID);
			if (pServer)
				pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			
		}
		break;
	case  MAGA_PRIVATECHATCHANNEL_MEMBERDEL:
		{
			MADelPrivateChannelMember* pPacket = reinterpret_cast<MADelPrivateChannelMember*>(pData);
			
			CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo(m_cWorldSetID, pPacket->nPrivateChatChannelID);
			if( pPrivateChatChannel && pPacket->nManagedID != g_Config.nManagedID )
			{
				pPrivateChatChannel->DelPrivateChannelMember(pPacket->eType, pPacket->biCharacterDBID);
			}				

			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
		}
		break;	
	case  MAGA_PRIVATECHATCHANNEL_MEMBERKICKRESULT:
		{
			MAKickPrivateChannelMemberResult* pPacket = reinterpret_cast<MAKickPrivateChannelMemberResult*>(pData);
			
			CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByAID(pPacket->nAccountDBID);
			if (pServer)
				pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			
		}
		break;
	case  MAGA_PRIVATECHATCHANNEL_MOD:
		{
			MAModPrivateChannel* pPacket = reinterpret_cast<MAModPrivateChannel*>(pData);	

			CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo(m_cWorldSetID, pPacket->nPrivateChatChannelID);
			if( pPrivateChatChannel && pPacket->nManagedID != g_Config.nManagedID )
			{
				pPrivateChatChannel->ModPrivateChannelInfo(pPacket->eType, pPacket->nPassWord, pPacket->biCharacterDBID);
			}	

			
			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}
			
		}
		break;
	case  MAGA_PRIVATECHATCHANNEL_MODMEMBERNAME:
		{
			MAModPrivateChannelMemberName* pPacket = reinterpret_cast<MAModPrivateChannelMemberName*>(pData);	

			CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo(m_cWorldSetID, pPacket->nPrivateChatChannelID);
			if( pPrivateChatChannel && pPacket->nManagedID != g_Config.nManagedID )
			{
				pPrivateChatChannel->ModPrivateChannelMemberName( pPacket->biCharacterDBID, pPacket->wszName );
			}	


			for (int i = 0; i < g_pGameServerManager->GetGameServerSize(); i++)
			{
				CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerIndex(i);
				if (pServer)
					pServer->StoreExternalBuffer(0, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			}

		}
		break;
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	case  MAGA_PVP_CHANGEROOMMASTER:
		{
			MAGAChangeRoomMaster* pPacket = reinterpret_cast<MAGAChangeRoomMaster*>(pData);

			CDNRUDPGameServer* pGameServer = g_pGameServerManager->GetGameServerByRoomID( pPacket->uiRoomID );
			if( pGameServer )
			{
				CDNGameRoom* pRoom = pGameServer->GetRoom( pPacket->uiRoomID );
				if( pRoom && pRoom->bIsPvPRoom() )
				{
					CDNPvPGameRoom * pPvPGameRoom = static_cast<CDNPvPGameRoom*>( pRoom );
					pPvPGameRoom->ChangeRoomMaster( pPacket->biRoomMasterCharacterDBID, pPacket->uiRoomMasterSessionID );
				}
			}
		}
		break;
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
	default:
		return false;
	}
	return true;
}

int CDNMasterConnection::MessageProcess(int nMainCmd, int nSubCmd, char * pData, int nLen)
{
	if (PreMessageProcess(nMainCmd, nSubCmd, pData, nLen) == false)
	{
		UINT nAccountDBID;
		memcpy(&nAccountDBID, pData, sizeof(UINT));

		CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByAID(nAccountDBID);		//account id 여야합니다.
		if (pServer)	//외부메세지 파싱
		{
			pServer->StoreExternalBuffer(nAccountDBID, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_MASTER, m_cWorldSetID);
			return ERROR_NONE;
		}
		// 여기서 만약 유저 자르라는 거면..
		if( nMainCmd == MAGA_DETACHUSER )
		{
			MADetachUser* pDetach = (MADetachUser*)pData;
			if( pDetach->bIsDuplicate )
			{
				SendDuplicateLogin(nAccountDBID, false, pDetach->nSessionID);
				g_Log.Log(LogType::_NORMAL, m_cWorldSetID, pDetach->nAccountDBID, 0, pDetach->nSessionID, L"[MAGA_DETACHUSER] Duplicate User Not InGame!!\r\n");
			}
			return ERROR_NONE;
		}		
		g_Log.Log(LogType::_ERROR, m_cWorldSetID, 0, 0, 0, L"Unknown Command Check [MCD:%d] [SCD:%d]\n", nMainCmd, nSubCmd);
	}

	return ERROR_UNKNOWN_HEADER;
}

bool CDNMasterConnection::SendGameServerInfoList(unsigned long * iIP, unsigned short * iPort, unsigned char * cIdx, bool * margin, int nManageID)
{
	GARegist Regist;
	memset(&Regist, 0, sizeof(GARegist));

	int i;
	for (i = 0; i < sizeof(Regist.ServerInfo)/sizeof(*Regist.ServerInfo); i++)
	{
		if (iIP[i] == 0)	break;
		Regist.ServerInfo[i].cServerIdx = cIdx[i];
		Regist.ServerInfo[i].nIP = iIP[i];
		Regist.ServerInfo[i].nPort = iPort[i];
		Regist.ServerInfo[i].cMargin = margin[i] == true ? 1 : 0;
	}
	Regist.cInfoCount = i;
	Regist.nTcpOpenPort = g_Config.nClientAcceptPort;
	Regist.cAffinityType = g_Config.cAffinityType;
	Regist.nManagedID = nManageID;

	int nRet = AddSendData(GAMA_REGIST, 0, (char*)&Regist, sizeof(Regist) - sizeof(Regist.ServerInfo) + (Regist.cInfoCount * sizeof(GameServerInfo)));
	return nRet == 0 ? true : false;
}

void CDNMasterConnection::SendGameFrame(int nServerIdx, int nWholeRoomCnt, int nRoomCnt, int nUserCnt, int nMinFrame, int nMaxFrame, int nAvrFrame, bool bHasMargin)
{
	GAMADelayReport Packet;

	Packet.nServerIdx = nServerIdx;
	Packet.nWholeRoomCnt = nWholeRoomCnt;
	Packet.nRoomCnt = nRoomCnt;
	Packet.nUserCnt = nUserCnt;
	Packet.nMinFrame = nMinFrame;
	Packet.nMaxFrame = nMaxFrame;
	Packet.nAvrFrame = nAvrFrame;
	Packet.cHasMargin = bHasMargin == true ? 1 : 0;

	AddSendData(GAMA_DELAYREPORT, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendAddUserList(CDNUserSession *pSession)
{
	GAMAAddUserList AddUserList;
	memset(&AddUserList, 0, sizeof(GAMAAddUserList));

	AddUserList.nThreadIdx = pSession->GetServerID();
	AddUserList.nSessionID = pSession->GetSessionID();
	AddUserList.nAccountDBID = pSession->GetAccountDBID();
	AddUserList.biCharacterDBID = pSession->GetCharacterDBID();
	AddUserList.PartyID = pSession->GetGameRoom()->GetPartyIndex();
	AddUserList.wChannelID = pSession->GetGameRoom()->GetPartyChannelID();
	_wcscpy(AddUserList.wszAccountName, IDLENMAX, pSession->GetAccountName(), IDLENMAX);
	_wcscpy(AddUserList.wszCharacterName, NAMELENMAX, pSession->GetCharacterName(), NAMELENMAX);
	AddUserList.bAdult = pSession->m_bAdult;
	_strcpy(AddUserList.szVirtualIp, _countof(AddUserList.szVirtualIp), pSession->m_szVirtualIp, (int)strlen(pSession->m_szVirtualIp));
	AddUserList.nRoomID = pSession->GetRoomID();
	CDNTcpConnection * pCon = pSession->GetTcpConnection();
	if (pCon)
		_strcpy(AddUserList.szIP, _countof(AddUserList.szIP), pCon->GetIp(), (int)strlen(pCon->GetIp()));
	else _DANGER_POINT();
#if defined (_KR)
	memcpy(AddUserList.szMID, pSession->m_szMID, sizeof(AddUserList.szMID));
	AddUserList.dwGRC = pSession->m_dwGRC;
#endif
	AddUserList.cPCBangGrade = pSession->GetPCBangGrade();
#ifdef PRE_ADD_MULTILANGUAGE
	AddUserList.cSelectedLanguage = pSession->m_eSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#if defined(_ID)
	_strcpy(AddUserList.szMacAddress, _countof(AddUserList.szMacAddress), pSession->GetMacAddress(), (int)strlen(pSession->GetMacAddress()));
	_strcpy(AddUserList.szKey, _countof(AddUserList.szKey), pSession->GetIDNKey(), (int)strlen(pSession->GetIDNKey()));	
	AddUserList.dwKreonCN = pSession->m_dwKreonCN;
#endif // #if defined(_ID)
#if defined(_KRAZ)
	AddUserList.ShutdownData = pSession->m_ShutdownData;
#endif	// #if defined(_KRAZ)
#if defined(PRE_ADD_DWC)
	AddUserList.cAccountLevel = pSession->GetAccountLevel();
#endif

	AddSendData(GAMA_ADDUSERLIST, 0, (char*)&AddUserList, sizeof(GAMAAddUserList));
}

void CDNMasterConnection::SendEndofVillageInfo(UINT nServerID)
{
	GAMAEndofRegist Packet;
	Packet.nServerID = nServerID;
	AddSendData(GAMA_ENDOFREGIST, 0, (char*)&Packet, sizeof(GAMAEndofRegist));
}

void CDNMasterConnection::SendConnectComplete()
{
	AddSendData(GAMA_CONNECTCOMPLETE, 0, NULL, 0);
}

void CDNMasterConnection::SendDelUser(UINT nAccountDBID, bool bIsIntended, UINT nSessionID)
{
	GAMADelUser DelUser;
	memset(&DelUser, 0, sizeof(GAMADelUser));
	DelUser.nAccountDBID = nAccountDBID;
	DelUser.cIsUnIntended = bIsIntended == false ? 1 : 0;
	DelUser.nSessionID = nSessionID;
	AddSendData(GAMA_DELUSER, 0, (char*)&DelUser, sizeof(GAMADelUser));
}

void CDNMasterConnection::SendCheckUser(UINT nSessionID)
{
	GAMACheckUser Packet;			// GAMA_CHECKUSER
	Packet.nSessionID = nSessionID;
	AddSendData(GAMA_CHECKUSER, 0, (char*)&Packet, sizeof(GAMACheckUser));
}

void CDNMasterConnection::SendEnterGame(UINT nAccountDBID, int nRoomID, int nServerIdx)
{
	GAMAEnterGame Packet;
	Packet.nAccountDBID = nAccountDBID;
	Packet.nRoomID = nRoomID;
	Packet.nServerIdx = nServerIdx;
	AddSendData(GAMA_ENTERGAME, 0, (char*)&Packet, sizeof(GAMAEnterGame));
}

void CDNMasterConnection::SendReconnectLogin(UINT nAccountDBID)
{
	GAMAReconnectLogin Packet;
	memset(&Packet, 0, sizeof(GAMAReconnectLogin));

	Packet.nAccountDBID = nAccountDBID;

	AddSendData(GAMA_RECONNECTLOGIN, 0, (char*)&Packet, sizeof(GAMAReconnectLogin));
}

void CDNMasterConnection::SendUpdateWorldUserState(const WCHAR * pName, int nMapIdx)
{
	GAMAUpdateWorldUserState Packet;
	memset(&Packet, 0, sizeof(Packet));

	_wcscpy(Packet.szName, _countof(Packet.szName), pName, (int)wcslen(pName));
	Packet.nMapIdx = nMapIdx;

	AddSendData(GAMA_UPDATEWORLDUSERSTATE, 0, (char*)&Packet, sizeof(GAMAUpdateWorldUserState));
}

#if defined( PRE_WORLDCOMBINE_PARTY )
#if defined( PRE_WORLDCOMBINE_PVP )
int CDNMasterConnection::SendSetRoomID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, int iServerIdx, int iRoomID, BYTE cVillageID, UINT * pPartyMember, TPARTYID PartyIDForBreakInto, ePartyType Type, WorldPvPMissionRoom::Common::eReqType eWorldReqType )
#else // #if defined( PRE_WORLDCOMBINE_PVP )
int CDNMasterConnection::SendSetRoomID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, int iServerIdx, int iRoomID, BYTE cVillageID, UINT * pPartyMember, TPARTYID PartyIDForBreakInto, ePartyType Type )
#endif // #if defined( PRE_WORLDCOMBINE_PVP )
#else // #if defined( PRE_WORLDCOMBINE_PARTY )
int CDNMasterConnection::SendSetRoomID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, TINSTANCEID InstanceID, int iServerIdx, int iRoomID, BYTE cVillageID, UINT * pPartyMember, TPARTYID PartyIDForBreakInto )
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
{
	GAMASetRoomID TxPacket;
	memset(&TxPacket, 0, sizeof(TxPacket));

	TxPacket.GameTaskType	= GameTaskType;
	TxPacket.cReqGameIDType	= cReqGameIDType;
	TxPacket.InstanceID		= InstanceID;
	TxPacket.cGameID		= m_cGameID;
	TxPacket.cServerIdx		= iServerIdx;
	TxPacket.cVillageID		= cVillageID;
	TxPacket.iRoomID		= iRoomID;
	TxPacket.PartyIDForBreakInto = PartyIDForBreakInto;
#if defined( PRE_WORLDCOMBINE_PARTY )
	TxPacket.Type			= Type;
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
	TxPacket.eWorldReqType = eWorldReqType;
#endif

	if (pPartyMember != NULL)
	{
		for (int i = 0; i < PARTYMAX; i++)
		{
			if (pPartyMember[i] <= 0) continue;
			TxPacket.nRoomMember[i] = pPartyMember[i];
		}
	}

	return AddSendData( GAMA_SETROOMID, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendSetTutorialRoomID(UINT nAccountDBID, int iServerIdx, int nRoomID, int nLoginServerID)
{
	GAMASetTutorialRoomID RoomID = { 0, };

	RoomID.nAccountDBID = nAccountDBID;
	RoomID.cGameID = m_cGameID;
	RoomID.cServerIdx = (BYTE)iServerIdx;
	RoomID.nRoomID = nRoomID;
	RoomID.nLoginServerID = nLoginServerID;

	AddSendData(GAMA_SETTUTORIALROOMID, 0, (char*)&RoomID, sizeof(GAMASetTutorialRoomID));
}

void CDNMasterConnection::SendRequestNextVillageInfo( UINT uiAccountDBID, int nMapIndex, int nGateIndex, bool bReturnVillage, INT64 nItemSerial)
{
	GAMARequestNextVillageInfo Packet;			//GAMA_REQNEXTVILLAGEINFO
	memset(&Packet, 0, sizeof(GAMARequestNextVillageInfo));

	Packet.cReqGameIDType		= REQINFO_TYPE_SINGLE;
	Packet.InstanceID			= uiAccountDBID;
	Packet.cReturnVillage		= bReturnVillage ? 1 : 0;
	Packet.nMapIndex			= nMapIndex;
	Packet.nEnteredGateIndex	= nGateIndex;
	Packet.nItemSerial = nItemSerial;

	AddSendData(GAMA_REQNEXTVILLAGEINFO, 0, (char*)&Packet, sizeof(GAMARequestNextVillageInfo));
}

void CDNMasterConnection::SendRequestNextVillageInfo(int nMapIndex, int nEnteredGateIndex, bool bReturnVillage, CDNGameRoom * pRoom)
{
	GAMARequestNextVillageInfo Packet;			//GAMA_REQNEXTVILLAGEINFO
	memset(&Packet, 0, sizeof(GAMARequestNextVillageInfo));

	Packet.cReqGameIDType = pRoom->GetGameType();
	Packet.InstanceID = pRoom->GetInstanceID();
	Packet.cReturnVillage = bReturnVillage == true ? 1 : 0;
	Packet.nMapIndex = nMapIndex;
	Packet.nEnteredGateIndex = nEnteredGateIndex;

	if (pRoom && Packet.cReqGameIDType != REQINFO_TYPE_SINGLE)
	{
		CDNGameRoom::PartyStruct * pStruct = NULL;
		CDNUserSession * pSession = NULL;
#if defined( PRE_PARTY_DB )
		Packet.PartyData.PartyData = pRoom->GetPartyStructData();
		Packet.PartyData.PartyData.iCurMemberCount = 0;
#endif
		for (DWORD i = 0; i < pRoom->GetUserCount(); i++)
		{
			pStruct = pRoom->GetPartyData(i);
			pSession = pStruct->pSession;
			if (pStruct && pSession && !pSession->bIsGMTrace() )
			{
				if (pSession->IsConnected() == false) continue;
				if (pStruct->bLeader == true)
				{
					Packet.PartyData.nLeaderAccountDBID = pSession->GetAccountDBID();
					Packet.PartyData.nLeaderSessionID = pSession->GetSessionID();
#if defined( PRE_PARTY_DB )
					Packet.PartyData.PartyData.biLeaderCharacterDBID = pSession->GetCharacterDBID();
#endif
				}

				Packet.PartyData.MemberInfo[i].nAccountDBID = pSession->GetAccountDBID();
#if defined( PRE_PARTY_DB )
				Packet.PartyData.MemberInfo[i].biCharacterDBID = pSession->GetCharacterDBID();
#endif
				Packet.PartyData.MemberInfo[i].cMemberIndex = (BYTE)pSession->GetPartyMemberIndex();
#if defined( PRE_ADD_NEWCOMEBACK )
				if( pSession->GetComebackAppellation() > 0 )
					Packet.PartyData.MemberInfo[i].bCheckComebackAppellation = true;
#endif
#ifdef _USE_VOICECHAT
				Packet.PartyData.MemberInfo[i].VoiceInfo.cVoiceAvailable = pSession->GetVoiceAvailable() == true ? 1 : 0;
				memcpy(Packet.PartyData.MemberInfo[i].VoiceInfo.nMutedList, pSession->m_nVoiceMutedList, sizeof(UINT[PARTYCOUNTMAX]));
#endif
#if defined( PRE_PARTY_DB )
				Packet.PartyData.PartyData.iCurMemberCount++;
#else
				Packet.PartyData.cMemberCount++;
#endif
			}
			else
				_DANGER_POINT();
		}
		
		pRoom->CopyPartyKickedMemberList( Packet.PartyData.nKickedMemberDBID );

		Packet.PartyData.nChannelID = pRoom->GetPartyChannelID();
		
		
#if !defined( PRE_PARTY_DB )
		Packet.PartyData.ItemLootRule = pRoom->GetPartyItemLootRule();
		Packet.PartyData.ItemLootRank = pRoom->GetPartyItemLootRank();
		Packet.PartyData.cUserLvLimitMin = pRoom->GetPartyLimitLvMin();
		Packet.PartyData.cMemberMax = pRoom->GetPartyMemberMax();
		Packet.PartyData.cUserLvLimitMax = pRoom->GetPartyLimitLvMax();
		Packet.PartyData.nTargetMapIdx = pRoom->GetPartyTargetMapID();
		Packet.PartyData.Difficulty = pRoom->GetPartyDifficulty();
#endif // #if defined( PRE_PARTY_DB )
		

#if defined( PRE_PARTY_DB )		
#else
		Packet.PartyData.cJobDice = pRoom->GetPartyJobDice();
		_wcscpy(Packet.PartyData.wszPartyPass, _countof(Packet.PartyData.wszPartyPass), pRoom->GetPartyPassword(), (int)wcslen(pRoom->GetPartyPassword()));
		_wcscpy(Packet.PartyData.wszPartyName, _countof(Packet.PartyData.wszPartyName), pRoom->GetPartyName(), static_cast<int>(wcslen(pRoom->GetPartyName())) );
		Packet.PartyData.cUpkeepCount = (BYTE)pRoom->GetPartyUpkeepCount();
#endif // #if defined( PRE_PARTY_DB )		
		
#ifdef _USE_VOICECHAT
		Packet.PartyData.nVoiceChannelID = pRoom->m_nVoiceChannelID[0];
#endif
	}

	AddSendData(GAMA_REQNEXTVILLAGEINFO, 0, (char*)&Packet, sizeof(GAMARequestNextVillageInfo));
}

bool CDNMasterConnection::SendRebirthVillageInfo(UINT nAccountDBID, int nLastVillageMapIdx)
{
	GAMARebirthVillageInfo Info;
	memset(&Info, 0, sizeof(GAMARebirthVillageInfo));

	Info.nAccountDBID = nAccountDBID;
	Info.nLastVillageMapIdx = nLastVillageMapIdx;

	int nRet = AddSendData(GAMA_REBIRTHVILLAGEINFO, 0, (char*)&Info, sizeof(GAMARebirthVillageInfo));
	return nRet == 0 ? true : false;
}

void CDNMasterConnection::SendPvPGameToPvPLobby( const UINT uiAccountDBID, const UINT uiPvPIndex, const int iLastVillageMapIndex, bool bIsLadderRoom )
{
	GAMAMovePvPGameToPvPLobby TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID			= uiAccountDBID;
	TxPacket.uiPvPIndex				= uiPvPIndex;
	TxPacket.iLastVillageMapIndex	= iLastVillageMapIndex;
	TxPacket.bIsLadderRoom			= bIsLadderRoom;

	AddSendData( GAMA_MOVEPVPGAMETOPVPLOBBY, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendLoginState(UINT nAccountDBID)
{
	GAMALoginState State = { 0, };
	State.nAccountDBID = nAccountDBID;
	AddSendData(GAMA_LOGINSTATE, 0, (char*)&State, sizeof(GAMALoginState));
}

void CDNMasterConnection::SendVillageState(UINT nAccountDBID, int nMapIndex, int nRoomID)
{
	GAMAVillageState State = { 0, };
	State.nAccountDBID = nAccountDBID;
	State.nMapIndex = nMapIndex;
	State.nRoomID = nRoomID;
	AddSendData(GAMA_VILLAGESTATE, 0, (char*)&State, sizeof(GAMAVillageState));
}

void CDNMasterConnection::SendFriendAddNotify(UINT nAddedAccountDBID, const WCHAR * pAddName)
{
	GAMAFriendAddNotice Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nAddedAccountDBID = nAddedAccountDBID;
	_wcscpy(Packet.wszAddName, _countof(Packet.wszAddName), pAddName, (int)wcslen(pAddName));

	AddSendData(GAMA_FRIENDADD, 0, (char*)&Packet, sizeof(Packet));
}

#ifdef PRE_ADD_DOORS
void CDNMasterConnection::SendPrivateChat(UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen, INT64 biDestCharacterDBID)
#else		//#ifdef PRE_ADD_DOORS
void CDNMasterConnection::SendPrivateChat(UINT nFromAccountDBID, WCHAR *pwszToCharacterName, char cType, WCHAR *pwszChatMsg, short wChatLen)
#endif		//#ifdef PRE_ADD_DOORS
{
	GAMAPrivateChat Private;
	memset(&Private, 0, sizeof(GAMAPrivateChat));

	Private.nAccountDBID = nFromAccountDBID;
	_wcscpy(Private.wszToCharacterName, _countof(Private.wszToCharacterName), pwszToCharacterName, (int)wcslen(pwszToCharacterName));
	Private.cType = cType;
	Private.wChatLen = wChatLen;
	_wcscpy(Private.wszChatMsg, _countof(Private.wszChatMsg), pwszChatMsg, (int)wcslen(pwszChatMsg));
#ifdef PRE_ADD_DOORS
	Private.biDestCharacterDBID = biDestCharacterDBID;
#endif		//#ifdef PRE_ADD_DOORS
	
	AddSendData(GAMA_PRIVATECHAT, 0, (char*)&Private, sizeof(Private) - sizeof(Private.wszChatMsg) + (wChatLen * 2));
}

void CDNMasterConnection::SendChat(char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen)
{
	GAMAChat Chat;
	memset(&Chat, 0, sizeof(GAMAChat));

	Chat.nAccountDBID = nFromAccountDBID;
	Chat.cType = cType;
	Chat.wChatLen = wChatLen;
	_wcscpy(Chat.wszChatMsg, _countof(Chat.wszChatMsg), pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(GAMA_CHAT, 0, (char*)&Chat, sizeof(Chat) - sizeof(Chat.wszChatMsg) + (wChatLen * 2));
}

void CDNMasterConnection::SendWorldSystemMsg(UINT nFromAccountDBID, char cType, int nID, int nValue, const WCHAR* pwszToCharacterName)
{
	GAMAWorldSystemMsg Msg;
	memset(&Msg, 0, sizeof(GAMAWorldSystemMsg));

	Msg.nAccountDBID = nFromAccountDBID;
	Msg.cType = cType;
	Msg.nID = nID;
	Msg.nValue = nValue;
	if (pwszToCharacterName) 
		_wcscpy(Msg.wszToCharacterName, _countof(Msg.wszToCharacterName), pwszToCharacterName, (int)wcslen(pwszToCharacterName));

	AddSendData(GAMA_WORLDSYSTEMMSG, 0, (char*)&Msg, sizeof(GAMAWorldSystemMsg));
}

#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNMasterConnection::SendPrivateChannelChat(char cType, UINT nFromAccountDBID, const WCHAR *pwszChatMsg, short wChatLen, INT64 nChannelID)
{
	GAMAPrivateChannelChatMsg Chat = { 0, };

	Chat.nAccountDBID = nFromAccountDBID;
	Chat.cType = cType;
	Chat.wChatLen = wChatLen;
	Chat.nChannelID = nChannelID;	
	_wcscpy(Chat.wszChatMsg, _countof(Chat.wszChatMsg), pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(GAMA_PRIVATECHANNELCHAT, 0, (char*)&Chat, sizeof(GAMAPrivateChannelChatMsg));
}
#endif

void CDNMasterConnection::SendNoticeFromClient(const WCHAR * pMsg, const int nLen)
{
	GAMANotice Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.nLen = nLen;
	_wcscpy(Packet.szMsg, _countof(Packet.szMsg), pMsg, (int)wcslen(pMsg));
	AddSendData(GAMA_NOTICE, 0, (char*)&Packet, sizeof(Packet) - sizeof(Packet.szMsg) + (nLen * sizeof(WCHAR)));
}

#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendBanUser(UINT nAccountDBID, const WCHAR * pCharacterName, char cSelectedLang)
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendBanUser(UINT nAccountDBID, const WCHAR * pCharacterName)
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
{
	GAMABanUser Packet;
	memset(&Packet, 0, sizeof(GAMABanUser));

	Packet.nAccountDBID = nAccountDBID;
	_wcscpy(Packet.wszCharacterName, _countof(Packet.wszCharacterName), pCharacterName, (int)wcslen(pCharacterName));
#if defined(PRE_ADD_MULTILANGUAGE)
	Packet.cSelectedLang = cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	AddSendData(GAMA_BANUSER, 0, (char*)&Packet, sizeof(GAMABanUser));
}

void CDNMasterConnection::SendPvPRoomSyncOK( const UINT uiPvPIndex )
{
	GAMAPVP_ROOMSYNCOK TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiPvPIndex	= uiPvPIndex;

	AddSendData( GAMA_PVP_ROOMSYNCOK, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPLadderRoomSync( INT64 biRoomIndex, INT64 biRoomIndex2, LadderSystem::RoomState::eCode State )
{
	GAMAPVP_LADDERROOMSYNC TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biRoomIndexArr[0] = biRoomIndex;
	TxPacket.biRoomIndexArr[1] = biRoomIndex2;
	TxPacket.State = State;

	AddSendData( GAMA_PVP_LADDERROOMSYNC, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPBreakIntoOK( const UINT uiPvPIndex, const UINT uiAccountDBID )
{
	GAMAPVP_BREAKINTOOK TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiPvPIndex		= uiPvPIndex;
	TxPacket.uiAccountDBID	= uiAccountDBID;

	AddSendData( GAMA_PVP_BREAKINTOOK, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPCommand( const UINT uiCommand, const UINT uiPvPIndex, const UINT uiRoomIndex )
{
	GAMAPVP_COMMAND TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiCommandType	= uiCommand;
	TxPacket.uiPvPIndex		= uiPvPIndex;
	TxPacket.uiRoomIndex	= uiRoomIndex;
	
	AddSendData( GAMA_PVP_COMMAND, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendPvPChangeMemberGrade(UINT nAccountDBID, USHORT nType, UINT nSessionID, bool bAsign)
{
	GAMAPVPChangeMemberGrade Packet;
	memset(&Packet, 0, sizeof(GAMAPVPChangeMemberGrade));

	Packet.nAccountDBID = nAccountDBID;
	Packet.nType = nType;
	Packet.nTargetSessionID = nSessionID;
	Packet.bAsign = bAsign;

	AddSendData(GAMA_PVP_CHANGEMEMBER_GRADE, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendPvPSwapMemberIndex(UINT nAccountDBID, BYTE cCount, const TSwapMemberIndex * pIndex)
{
	if (pIndex == NULL) return;

	GAMAPVPSwapMemberIndex Packet;
	memset(&Packet, 0, sizeof(GAMAPVPSwapMemberIndex));

	Packet.nAccountDBID = nAccountDBID;
	Packet.cCount = cCount;
	memcpy(Packet.Index, pIndex, sizeof(Packet.Index));

	AddSendData( GAMA_PVP_SWAPTMEMBER_INDEX, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) - sizeof(Packet.Index) + (sizeof(TSwapMemberIndex) * Packet.cCount));
}

void CDNMasterConnection::SendPvPRealTimeScore(UINT nGuildDBID, int nScore)
{
	GAMAPvPGuildWarScore Packet;
	memset(&Packet, 0, sizeof(GAMAPvPGuildWarScore));

	Packet.nGuildDBID = nGuildDBID;
	Packet.nScore = nScore;

	AddSendData( GAMA_PVP_GUILDWARSCORE, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
}

void CDNMasterConnection::SendPvPGuildWarResult(UINT nWinGuildDBID, UINT nLoseGuildDBID, bool bIsDraw)
{
	GAMAPvPGuildWarResult Packet;
	memset(&Packet, 0, sizeof(GAMAPvPGuildWarResult));

	Packet.nWinGuildDBID = nWinGuildDBID;
	Packet.nLoseGuildDBID = nLoseGuildDBID;
	Packet.bIsDraw = bIsDraw;

	AddSendData( GAMA_PVP_GUILDWARRESULT, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
}

void CDNMasterConnection::SendPvPDetectCrash(UINT nRoomID, UINT nPvPIndex)
{
	GAMAPvPGuildWarException Packet;
	memset(&Packet, 0, sizeof(GAMAPvPGuildWarException));

	Packet.nRoomID = nRoomID;
	Packet.nPvPIndex = nPvPIndex;

	AddSendData( GAMA_PVP_GUILDWAREXCEPTION, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
}

void CDNMasterConnection::SendChangeGuildInfo(const TGuildUID pGuildUID, UINT nAccountDBID, INT64 nCharacterDBID, BYTE btGuildUpdate, int iInt1, int iInt2, int iInt3, int iInt4, INT64 biInt64, LPCWSTR pText)
{
	MAChangeGuildInfo Packet;
	memset(&Packet, 0, sizeof(MAChangeGuildInfo));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = pGuildUID;
	Packet.nAccountDBID = nAccountDBID;
	Packet.nCharacterDBID = nCharacterDBID;
	Packet.btGuildUpdate = btGuildUpdate;
	Packet.Int1 = iInt1;
	Packet.Int2 = iInt2;
	Packet.Int64 = biInt64;
	if (pText) {
		_wcscpy(Packet.Text, _countof(Packet.Text), pText, (int)wcslen(pText));
	}
	Packet.Int3 = iInt3;
	Packet.Int4 = iInt4;

	AddSendData(GAMA_CHANGEGUILDINFO, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}
void CDNMasterConnection::SendChangeGuildMemberInfo(const TGuildUID pGuildUID, UINT nReqAccountDBID, INT64 nReqCharacterDBID, UINT nChgAccountDBID, INT64 nChgCharacterDBID, BYTE btGuildMemberUpdate, int iInt1, int iInt2, INT64 biInt64, LPCWSTR pText, bool bReturn)
{
	MAChangeGuildMemberInfo Packet;
	memset(&Packet, 0, sizeof(MAChangeGuildMemberInfo));

	if(bReturn)		//자신이 패킷을 다시 받아야하는 경우
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

	AddSendData(GAMA_CHANGEGUILDMEMBINFO, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
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

	AddSendData(GAMA_GUILDCHAT, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}

void CDNMasterConnection::SendGuildChangeName(const TGuildUID pGuildUID, LPCWSTR lpwszGuildName)
{
	MAGuildChangeName Packet;
	memset(&Packet, 0, sizeof(MAGuildChangeName));

	Packet.GuildUID = pGuildUID;
	_wcscpy( Packet.wszGuildName, _countof(Packet.wszGuildName), lpwszGuildName, (int)wcslen(lpwszGuildName) );

	AddSendData(GAMA_GUILDCHANGENAME, 0, reinterpret_cast<char*>(&Packet), sizeof(MAGuildChangeName));
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

	AddSendData(GAMA_UPDATEGUILDEXP, 0, reinterpret_cast<char*>(&Packet), sizeof(MAUpdateGuildExp));
}

void CDNMasterConnection::SendAddGuildWarPoint(char cTeamType, TGuildUID GuildUID, int nAddPoint)
{
	MAAddGuildWarPoint Packet;
	memset(&Packet, 0, sizeof(MAAddGuildWarPoint));

	Packet.cTeamType = cTeamType;
	Packet.GuildUID = GuildUID;
	Packet.nAddPoint = nAddPoint;	

	AddSendData(GAMA_ADDGUILDWAR_POINT, 0, reinterpret_cast<char*>(&Packet), sizeof(MAAddGuildWarPoint));
}

//Party
bool CDNMasterConnection::SendInvitePartyMember(CDNGameRoom * pGameRoom, int nMapIdx, int nChannelID, UINT nAccountDBID, const WCHAR * pwszInviterName, const WCHAR * pwszInvitedName, int * pPassClassIds, int nPermitLevel)
{
	if (pwszInviterName == NULL || pwszInvitedName == NULL || pPassClassIds == NULL) return false;
	GAMAInvitePartyMember Packet;
	memset(&Packet, 0, sizeof(GAMAInvitePartyMember));

	Packet.PartyID = pGameRoom->GetPartyIndex();
	Packet.cLimitLevelMin = (BYTE)pGameRoom->GetPartyLimitLvMin();
#if defined( PRE_PARTY_DB )
#else
	Packet.cLimitLevelMax = (BYTE)pGameRoom->GetPartyLimitLvMax();
#endif // #if defined( PRE_PARTY_DB )
	Packet.cPartyMemberMax = (BYTE)pGameRoom->GetPartyMemberMax();
	Packet.cPartyMemberCount = (BYTE)pGameRoom->GetUserCount();
	Packet.cPartyAvrLevel = (BYTE)pGameRoom->GetPartyAvrLevel();
	_wcscpy(Packet.wszPartyName, _countof(Packet.wszPartyName), pGameRoom->GetPartyName(), (int)wcslen(pGameRoom->GetPartyName()));
	Packet.nMapIdx = nMapIdx;
	Packet.nChannelID = nChannelID;
	Packet.nInviterAccountDBID = nAccountDBID;
	_wcscpy(Packet.wszInviterName, _countof(Packet.wszInviterName), pwszInviterName, (int)wcslen(pwszInviterName));
	_wcscpy(Packet.wszInvitedName, _countof(Packet.wszInvitedName), pwszInvitedName, (int)wcslen(pwszInvitedName));

	for (int i = 0; i < PERMITMAPPASSCLASSMAX; i++)
		Packet.cPassClassIds[i] = (BYTE)pPassClassIds[i];
	Packet.cPermitLevel = (BYTE)nPermitLevel;

	return AddSendData(GAMA_INVITEPARTYMEMBER, 0, (char*)&Packet, sizeof(Packet)) < 0 ? false : true;
}

void CDNMasterConnection::SendInvitedMemberReturnMsg(UINT nAccountDBID, int nRetCode)
{
	GAMAInvitedReturnMsg Packet;
	memset(&Packet, 0, sizeof(GAMAInvitedReturnMsg));

	Packet.nAccountDBID = nAccountDBID;
	Packet.nRetCode = nRetCode;

	AddSendData(GAMA_INVITEPARTYMEMBER_RETURNMSG, 0, (char*)&Packet, sizeof(Packet));
}

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
void CDNMasterConnection::SendGetPartyIDResult( UINT nSenderAccountDBID , TPARTYID PartyID )
{
	GAMAGetPartyIDResult TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nSenderAccountDBID = nSenderAccountDBID;
	TxPacket.PartyID = PartyID;

	AddSendData( GAMA_GETPARTYID_RESULT, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendResPartyAskJoin(int iRet, UINT uiReqAccountDBID, const WCHAR* pwszReqChracterName )
{
	GAMAResPartyAskJoin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet = iRet;
	TxPacket.uiReqAccountDBID = uiReqAccountDBID;
	if(pwszReqChracterName)
		_wcscpy(TxPacket.wszReqChracterName, _countof(TxPacket.wszReqChracterName), pwszReqChracterName, static_cast<int>(wcslen(pwszReqChracterName)) );

	AddSendData( GAMA_RESPARTYASKJOIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendAskJoinAgreeInfo(const WCHAR* pwszAskerCharName, TPARTYID PartyID, int iPassword)
{
	GAMAAskJoinAgreeInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	
	TxPacket.PartyID = PartyID;
	TxPacket.iPassword = iPassword;
	_wcscpy(TxPacket.wszAskerCharName, _countof(TxPacket.wszAskerCharName), pwszAskerCharName, static_cast<int>(wcslen(pwszAskerCharName)) );

	AddSendData( GAMA_ASKJOINAGREEINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#endif

#if defined(_KR)
void CDNMasterConnection::SendPCBangResult(UINT nAccountDBID)
{
	GAMAPCBangResult Result;
	Result.nAccountDBID = nAccountDBID;
	AddSendData(GAMA_PCBANGRESULT, 0, (char*)&Result, sizeof(GAMAPCBangResult) );
}
#endif	// _KR

void CDNMasterConnection::SendNotifyGift(UINT nToAccountDBID, INT64 biToCharacterDBID, bool bNew, int nGiftCount)
{
	GAMANotifyGift Result;
	Result.nToAccountDBID = nToAccountDBID;
	Result.biToCharacterDBID = biToCharacterDBID;
	Result.bNew = bNew;
	Result.nGiftCount = nGiftCount;
	AddSendData(GAMA_NOTIFYGIFT, 0, (char*)&Result, sizeof(GAMANotifyGift) );
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDNMasterConnection::SendSyncSystemMail( TASendSystemMail* pMail )
{
	GAMASyncSystemMail TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biReceiverCharacterDBID	= pMail->biReceiverCharacterDBID;
	TxPacket.nMailDBID					= pMail->nMailDBID;
	TxPacket.iTotalMailCount			= pMail->iTotalMailCount;
	TxPacket.iNotReadMailCount			= pMail->iNotReadMailCount;
	TxPacket.i7DaysLeftMailCount		= pMail->i7DaysLeftMailCount;

	AddSendData( GAMA_SYNC_SYSTEMMAIL, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDNMasterConnection::SendMasterSystemSyncSimpleInfo( INT64 biCharacterDBID, MasterSystem::EventType::eCode Type )
{
	MasterSystem::GAMASyncSimpleInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.EventCode			= Type;

	AddSendData( GAMA_MASTERSYSTEM_SYNC_SIMPLEINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncGraduate( INT64 biCharacterDBID, WCHAR* pwszCharName )
{
	MasterSystem::GAMASyncGraduate TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, (int)wcslen(pwszCharName) );

	AddSendData( GAMA_MASTERSYSTEM_SYNC_GRADUATE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNMasterConnection::SendMasterSystemSyncConnect( bool bIsConnect, WCHAR* pwszCharName, const TMasterSystemData* pMasterSystemData )
{
	// 사제 정보가 없으면 동기 맞출 필요가 없다.
	if( pMasterSystemData->SimpleInfo.cCharacterDBIDCount == 0 )
		return;

	MasterSystem::GAMASyncConnect TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.bIsConnect				= bIsConnect;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, (int)wcslen(pwszCharName) );
	TxPacket.cCharacterDBIDCount	= pMasterSystemData->SimpleInfo.cCharacterDBIDCount;
	for( UINT i=0 ; i<TxPacket.cCharacterDBIDCount ; ++i )
		TxPacket.CharacterDBIDList[i] = pMasterSystemData->SimpleInfo.OppositeInfo[i].CharacterDBID;

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.CharacterDBIDList)+TxPacket.cCharacterDBIDCount*sizeof(TxPacket.CharacterDBIDList[0]);
	AddSendData( GAMA_MASTERSYSTEM_SYNC_CONNECT, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

bool CDNMasterConnection::SendFarmDataLoaded(int nGameServerIdx, UINT nRoomID, UINT nFarmDBID, int nMapID, int nFarmMaxUser)
{
	GAMAFarmDataLoaded Packet;
	memset(&Packet, 0, sizeof(GAMAFarmDataLoaded));

	Packet.nGameServerIdx = nGameServerIdx;
	Packet.nRoomID = nRoomID;
	Packet.nFarmDBID = nFarmDBID;
	Packet.nMapID = nMapID;
	Packet.nFarmMaxUser = nFarmMaxUser;

	int nRet = AddSendData(GAMA_FARM_DATALOADED, 0, reinterpret_cast<char*>(&Packet), sizeof(GAMAFarmDataLoaded));
	return nRet == 0 ? true : false;
}

bool CDNMasterConnection::SendFarmUserCount(UINT nFarmDBID, UINT nFarmRoomID, int nCurUserCount, bool bStarted)
{
	GAMAFarmUserCount Packet;
	memset(&Packet, 0, sizeof(GAMAFarmUserCount));

	Packet.nFarmDBID = nFarmDBID;
	Packet.nFarmRoomID = nFarmRoomID;
	Packet.nCurUserCount = nCurUserCount;
	Packet.bStarted = bStarted;

	int nRet = AddSendData(GAMA_FARM_USERCOUNT, 0, reinterpret_cast<char*>(&Packet), sizeof(GAMAFarmUserCount));
	return nRet == 0 ? true : false;
}

bool CDNMasterConnection::SendFarmIntendedDestroy(UINT nFarmDBID)
{
	GAMAFarmIntendedDestroy Packet;
	memset(&Packet, 0, sizeof(GAMAFarmIntendedDestroy));

	Packet.nFarmDBID = nFarmDBID;

	int nRet = AddSendData(GAMA_FARM_INTENDEDDESTROY, 0, reinterpret_cast<char*>(&Packet), sizeof(GAMAFarmIntendedDestroy));
	return nRet == 0 ? true : false;
}

void CDNMasterConnection::SendFarmSync( INT64 biCharacterDBID, Farm::ServerSyncType::eType Type )
{
	GAMAFarmSync TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.Type				= Type;

	AddSendData( GAMA_FARM_SYNC, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket));
}

void CDNMasterConnection::SendFarmSyncAddWater( INT64 biCharacterDBID, WCHAR* pwszCharName, int iAddWaterPoint )
{
	GAMAFarmSyncAddWater TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.iAddPoint			= iAddWaterPoint;
	_wcscpy( TxPacket.wszCharName, _countof(TxPacket.wszCharName), pwszCharName, (int)wcslen(pwszCharName) );

	AddSendData( GAMA_FARM_SYNC_ADDWATER, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}



void CDNMasterConnection::SendGuildMemberLevelUp (TGuildUID GuildUID, INT64 nCharacterDBID, char cLevel)
{
	MAGuildMemberLevelUp Packet;

	Packet.nManagedID = g_Config.nManagedID;
	Packet.GuildUID = GuildUID;
	Packet.nCharacterDBID = nCharacterDBID;
	Packet.cLevel = cLevel;

	AddSendData(GAMA_GUILDMEMBER_LELVEUP, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendLoadUserTempData (UINT uiAccountDBID)
{
	GAMALoadUserTempData Packet;
	Packet.uiAccountDBID = uiAccountDBID;
	
	AddSendData(GAMA_LOAD_USERTEMPDATA, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendSaveUserTempData (CDNUserSession* pSession)
{
	GAMASaveUserTempData Packet;
	Packet.uiAccountDBID = pSession->GetAccountDBID();
	Packet.nDungeonClearCount = pSession->GetDungeonClearCount();
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	Packet.eUserGameQuitRewardType = pSession->GetUserGameQuitRewardType();
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(PRE_ADD_REMOTE_QUEST)
	pSession->GetQuest()->GetAcceptWaitRemoteQuest(&Packet.nAcceptWaitRemoteQuestCount, Packet.AcceptWaitRemoteQuestList);
	AddSendData (GAMA_SAVE_USERTEMPDATA, 0, (char*)&Packet, sizeof(VIMASaveUserTempData) - sizeof(Packet.AcceptWaitRemoteQuestList) + sizeof(int) * Packet.nAcceptWaitRemoteQuestCount );
#else
	AddSendData(GAMA_SAVE_USERTEMPDATA, 0, (char*)&Packet, sizeof(Packet));
#endif
	
}

void CDNMasterConnection::GuildWarReset()
{	
	m_cStepIndex = 0;
	m_wScheduleID = GUILDWARSCHEDULE_DEF;
	m_nBlueTeamPoint = 0;
	m_nRedTeamPoint = 0;
	m_cSecretTeam = 0;
	m_nSecretRandomSeed = 0;
	m_cGuildWarFinalPart = GUILDWAR_FINALPART_NONE;
	memset (m_vSecretMissionID, 0x00, sizeof(m_vSecretMissionID));

	memset(&m_sGuildWarSchedule, 0, sizeof(m_sGuildWarSchedule));
	memset(&m_sGuildWarFinalSchedule, 0, sizeof(m_sGuildWarFinalSchedule));
}

void CDNMasterConnection::SetGuildSecretMission(MASetGuildWarSecretMission* pSecret)
{
	m_cSecretTeam = pSecret->cTeamCode;
	m_nSecretRandomSeed = pSecret->nRandomSeed;
	if( m_nSecretRandomSeed == -1 ) // 해제
	{
		memset (m_vSecretMissionID, 0x00, sizeof(m_vSecretMissionID));
		return;
	}

	g_pDataManager->GetRandomGuildMission(m_nSecretRandomSeed, m_vSecretMissionID);
}

bool CDNMasterConnection::IsSecretMission (BYTE cTeamCode, int nMissionID)
{
	if (m_cSecretTeam != cTeamCode)
		return false;

	for (int i=0; i<GUILDWARMISSION_MAXGROUP; i++)
	{
		if (m_vSecretMissionID[i] == nMissionID)
			return true;
	}

	return false;
}

void CDNMasterConnection::CalcTeamSecret()
{
	if (m_nBlueTeamPoint == m_nRedTeamPoint)
	{
		m_cSecretTeam = 0;
		return;
	}

	float fSecretRate = 0.0f;
	if (m_nBlueTeamPoint > m_nRedTeamPoint)
		fSecretRate = (float)m_nRedTeamPoint / (float)m_nBlueTeamPoint;
	else
		fSecretRate = (float)m_nBlueTeamPoint / (float)m_nRedTeamPoint;

	if (fSecretRate <= 0)
	{
		DN_ASSERT( false, "fSecretRate <= 0" );
		return;
	}

	fSecretRate = 1.0f - fSecretRate;

	// 마스터 서버가 시크릿 여부를 판단 못할 경우를 대비하여 시크릿 중지여부 판단한다.
	if (fSecretRate < 0.02f)		
		m_cSecretTeam = 0;			// 해제
}

void CDNMasterConnection::SetGuildWarSechdule(MASetGuildWarEventTime* SetGuildWarEventTime)
{
	memcpy(m_sGuildWarSchedule, &SetGuildWarEventTime->sGuildWarTime[GUILDWAR_STEP_PREPARATION], sizeof(m_sGuildWarSchedule));
	memcpy(m_sGuildWarFinalSchedule, &SetGuildWarEventTime->sFinalPartTime[GUILDWAR_FINALPART_FINAL], sizeof(m_sGuildWarFinalSchedule));
}

void CDNMasterConnection::SendDuplicateLogin(UINT nAccountDBID, bool bIsDetach, UINT nSessionID)
{
	GAMADuplicateLogin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nAccountDBID = nAccountDBID;
	TxPacket.bIsDetach = bIsDetach;
	TxPacket.nSessionID = nSessionID;

	AddSendData (GAMA_DUPLICATE_LOGIN, 0, (char*)&TxPacket, sizeof(GAMADuplicateLogin) );
}

void CDNMasterConnection::SendCheckLastDungeonInfo( UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsCheck, const WCHAR* pwszPartyName )
{
	GAMACheckLastDungeonInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.bIsCheck			= bIsCheck;
	if( pwszPartyName )
		_wcscpy( TxPacket.wszPartyName, _countof(TxPacket.wszPartyName), pwszPartyName, static_cast<int>(wcslen(pwszPartyName)) );

	AddSendData( GAMA_CHECK_LASTDUNGEONINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined( PRE_PARTY_DB )
void CDNMasterConnection::SendConfirmLastDungeonInfo( int iRet, UINT uiAccountID, BreakInto::Type::eCode BreakIntoType )
#else
void CDNMasterConnection::SendConfirmLastDungeonInfo( int iRet, UINT uiAccountID )
#endif // #if defined( PRE_PARTY_DB )
{
	GAMAConfirmLastDungeonInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet			= iRet;
	TxPacket.uiAccountDBID	= uiAccountID;
#if defined( PRE_PARTY_DB )
	TxPacket.BreakIntoType	= BreakIntoType;
#endif // #if defined( PRE_PARTY_DB )

	AddSendData( GAMA_CONFIRM_LASTDUNGEONINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined( PRE_WORLDCOMBINE_PARTY )
void CDNMasterConnection::SendDelWorldParty( int nPrimaryIndex )
{
	MADelWorldParty Packet;
	memset(&Packet, 0, sizeof(MADelWorldParty));
	
	Packet.nPrimaryIndex= nPrimaryIndex;

	AddSendData(GAMA_DELWORLDPARTY, 0, (char*)&Packet, sizeof(MADelWorldParty));
}

void CDNMasterConnection::SendWorldPartyMember( UINT nAccountDBID, INT64 biCharacterDBID, TPARTYID PartyID, short nCount, Party::MemberData *MemberData, int nRet )
{
	GAMASendWorldPartyMember Packet;
	memset(&Packet, 0, sizeof(GAMASendWorldPartyMember));

	if( nRet == ERROR_NONE )
	{
		Packet.nAccountDBID = nAccountDBID;
		Packet.biCharacterDBID = biCharacterDBID;		
		Packet.PartyID = PartyID;
		Packet.nCount = nCount;
		for( int i=0;i<nCount;i++ )
		{
			Packet.MemberData[i] = MemberData[i];
		}
	}	
	Packet.nRetCode = nRet;

	AddSendData(GAMA_WORLDPARTYMEMBER, 0, (char*)&Packet, sizeof(GAMASendWorldPartyMember));
}

#if defined( PRE_FIX_WORLDCOMBINEPARTY )
void CDNMasterConnection::SendWorldCombineGameServer()
{
	GAMAWorldCombineGameserver Packet;
	memset(&Packet, 0, sizeof(GAMAWorldCombineGameserver));
	
	Packet.bFlag = true;

	AddSendData(GAMA_WORLDCOMBINEGAMESERVER, 0, (char*)&Packet, sizeof(GAMAWorldCombineGameserver));
}
#endif
#endif

#if defined( PRE_ADD_BESTFRIEND )
void CDNMasterConnection::SendLevelUpBestFriend(BYTE cLevel, LPCWSTR lpwszCharacterName)
{
	MALevelUpBestFriend Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.cLevel = cLevel;
	if (lpwszCharacterName)
		_wcscpy(Packet.wszName, _countof(Packet.wszName), lpwszCharacterName, _countof(Packet.wszName));

	AddSendData(GAMA_LEVELUP_BESTFRIEND, 0, (char*)&Packet, sizeof(Packet));
	
}
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNMasterConnection::SendAddPrivateChatChannel(TPrivateChatChannelInfo tPrivateChatChannel)
{
	MAAddPrivateChannel Packet;
	memset(&Packet, 0, sizeof(MAAddPrivateChannel));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.tPrivateChatChannel = tPrivateChatChannel;
	
	AddSendData(GAMA_PRIVATECHATCHANNEL_ADD, 0, (char*)&Packet, sizeof(Packet));

}

void CDNMasterConnection::SendAddPrivateChatChannelMember(INT64 nChannelID, TPrivateChatChannelMember Member)
{
	MAAddPrivateChannelMember Packet;
	memset(&Packet, 0, sizeof(MAAddPrivateChannelMember));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.Member = Member;

	AddSendData(GAMA_PRIVATECHATCHANNEL_MEMBERADD, 0, (char*)&Packet, sizeof(Packet));

}

void CDNMasterConnection::SendInvitePrivateChatChannel(INT64 nChannelID, UINT nAccountDBID, WCHAR* wszInviteCharacterName)
{
	MAInvitePrivateChannelMember Packet;
	memset(&Packet, 0, sizeof(MAInvitePrivateChannelMember));
	
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.nMasterAccountDBID = nAccountDBID;
	if(wszInviteCharacterName)
		_wcscpy(Packet.wszInviteName, _countof(Packet.wszInviteName), wszInviteCharacterName, (int)wcslen(wszInviteCharacterName));

	AddSendData(GAMA_PRIVATECHATCHANNEL_MEMBERINVITE, 0, (char*)&Packet, sizeof(MAInvitePrivateChannelMember));
}

void CDNMasterConnection::SendInvitePrivateChatChannelResult(int nRet, UINT nAccountDBID)
{
	MAInvitePrivateChannelMemberResult Packet;
	memset(&Packet, 0, sizeof(MAInvitePrivateChannelMemberResult));

	Packet.nRet = nRet;
	Packet.nMasterAccountDBID = nAccountDBID;

	AddSendData(GAMA_PRIVATECHATCHANNEL_MEMBERINVITERESULT, 0, (char*)&Packet, sizeof(MAInvitePrivateChannelMemberResult));
}

void CDNMasterConnection::SendDelPrivateChatChannelMember(PrivateChatChannel::Common::eModType eType, INT64 nChannelID, INT64 biCharacterDBID, const WCHAR* wszKickName)
{
	MADelPrivateChannelMember Packet;
	memset(&Packet, 0, sizeof(MADelPrivateChannelMember));

	Packet.nManagedID = g_Config.nManagedID;
	Packet.eType = eType;
	Packet.nPrivateChatChannelID = nChannelID;
	Packet.biCharacterDBID = biCharacterDBID;
	if(wszKickName)
		_wcscpy(Packet.wszName, _countof(Packet.wszName), wszKickName, (int)wcslen(wszKickName));

	AddSendData(GAMA_PRIVATECHATCHANNEL_MEMBERDEL, 0, (char*)&Packet, sizeof(Packet));

}

void CDNMasterConnection::SendKickPrivateChatChannelMemberResult(INT64 biCharacterDBID)
{
	MAKickPrivateChannelMemberResult Packet;
	memset(&Packet, 0, sizeof(MAKickPrivateChannelMemberResult));

	Packet.biCharacterDBID = biCharacterDBID;

	AddSendData(GAMA_PRIVATECHATCHANNEL_MEMBERKICKRESULT, 0, (char*)&Packet, sizeof(MAKickPrivateChannelMemberResult));
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

	AddSendData(GAMA_PRIVATECHATCHANNEL_MOD, 0, (char*)&Packet, sizeof(Packet));

}
#endif

#if defined( PRE_WORLDCOMBINE_PVP )

void CDNMasterConnection::SendDelWorldPvPRoom( char cWorldSetID, int nRoomIndex )
{
	MADelWorldPvPRoom Packet;
	memset(&Packet, 0, sizeof(MADelWorldPvPRoom));

	Packet.nRoomIndex = nRoomIndex;

	AddSendData(GAMA_DEL_WORLDPVPROOM, 0, (char*)&Packet, sizeof(Packet));

}

void CDNMasterConnection::SendWorldPvPRoomJoinResult( char cWorldSetID, int nRet, UINT uiAccountDBID )
{
	MAWorldPvPRoomJoinResult Packet;
	memset(&Packet, 0, sizeof(MAWorldPvPRoomJoinResult));

	Packet.uiAccountDBID = uiAccountDBID;
	Packet.nRet = nRet;

	AddSendData(GAMA_WORLDPVPROOM_JOINRESULT, 0, (char*)&Packet, sizeof(Packet));

}

void CDNMasterConnection::SendWorldPvPRoomGMCreateResult( char cWorldSetID, int nRet, UINT uiAccountDBID, TWorldPvPRoomDBData* Data, UINT uiPvPIndex )
{
	MAWorldPvPRoomGMCreateResult Packet;
	memset(&Packet, 0, sizeof(MAWorldPvPRoomGMCreateResult));
	
	Packet.uiAccountDBID = uiAccountDBID;
	Packet.uiPvPIndex = uiPvPIndex;
	Packet.nRet = nRet;
	memcpy(&Packet.Data, Data, sizeof(Packet.Data));

	AddSendData(GAMA_WORLDPVPROOM_GMCREATERESULT, 0, (char*)&Packet, sizeof(Packet));

}

#endif

#if defined(PRE_ADD_MUTE_USERCHATTING)
#if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendMuteUserFind(UINT uiGmAccountDBID, const WCHAR * pMuteCharacterName, int nMuteTime, int cSelectLanguage )
#else	// #if defined(PRE_ADD_MULTILANGUAGE)
void CDNMasterConnection::SendMuteUserFind(UINT uiGmAccountDBID, const WCHAR * pMuteCharacterName, int nMuteTime)
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
{
	MAMuteUserFind Packet;
	memset(&Packet, 0, sizeof(MAMuteUserFind));

	Packet.uiGmAccountID = uiGmAccountDBID;
	Packet.nMuteMinute = nMuteTime;
#if defined(PRE_ADD_MULTILANGUAGE)
	Packet.cSelectedLanguage = cSelectLanguage;
#endif	// #if defined(PRE_ADD_MULTILANGUAGE)
	
	_wcscpy(Packet.wszMuteUserName, _countof(Packet.wszMuteUserName), pMuteCharacterName, (int)wcslen(pMuteCharacterName));

	AddSendData(GAMA_MUTE_USERFIND, 0, (char*)&Packet, sizeof(Packet));
}
#endif	// #if defined( PRE_ADD_MUTE_USERCHATTING)

#if defined( PRE_MOD_71820 )
void CDNMasterConnection::SendNotifyMail(UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail)
{
	GAMANotifyMail Mail = { 0, };
	Mail.nToAccountDBID = nToAccountDBID;
	Mail.biToCharacterDBID = biToCharacterDBID;
	Mail.wTotalMailCount = wTotalMailCount;
	Mail.wNotReadMailCount = wNotReadMailCount;
	Mail.w7DaysLeftMailCount = w7DaysLeftCount;
	Mail.bNewMail = bNewMail;
	AddSendData(GAMA_NOTIFYMAIL, 0, (char*)&Mail, sizeof(GAMANotifyMail));
}
#endif // #if defined( PRE_MOD_71820 )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
void CDNMasterConnection::SendAddAlteiaWorldSendTicketResult( int nRetCode, INT64 biSendCharacterDBID )
{
	MAAlteiaWorldSendTicketResult Packet;
	memset(&Packet, 0, sizeof(MAAlteiaWorldSendTicketResult));

	Packet.nRetCode = nRetCode;
	Packet.biSendCharacterDBID = biSendCharacterDBID;

	AddSendData(GAMA_ALTEIAWORLD_SENDTICKET_RESULT, 0, (char*)&Packet, sizeof(MAAlteiaWorldSendTicketResult));
}
#endif // #if defined( PRE_MOD_71820 )

#if defined(PRE_ADD_CHNC2C)
void CDNMasterConnection::SendC2CAddCoinResult(int nRetCode, const char* szSeqID)
{
	MAC2CAddCoinResult Packet;
	memset(&Packet, 0, sizeof(MAC2CAddCoinResult));

	Packet.nRetCode = nRetCode;
	memcpy(Packet.szSeqID, szSeqID, sizeof(Packet.szSeqID));

	AddSendData(GAMA_C2C_CHAR_ADDCOIN, 0, (char*)&Packet, sizeof(Packet));
}

void CDNMasterConnection::SendC2CReduceCoinResult(int nRetCode, const char* szSeqID)
{
	MAC2CReduceCoinResult Packet;
	memset(&Packet, 0, sizeof(MAC2CReduceCoinResult));

	Packet.nRetCode = nRetCode;
	memcpy(Packet.szSeqID, szSeqID, sizeof(Packet.szSeqID));

	AddSendData(GAMA_C2C_CHAR_REDUCECOIN, 0, (char*)&Packet, sizeof(Packet));
}
#endif //#if defined(PRE_ADD_CHNC2C)

#if defined(PRE_ADD_DWC)
void CDNMasterConnection::SendDWCTeamChat(UINT nAccountDBID, INT64 biCharacterDBID, WCHAR *pwszChatMsg, short wChatLen)
{
	MADWCTeamChat Packet;
	memset (&Packet, 0, sizeof(Packet));
	
	Packet.nAccountDBID = nAccountDBID;
	Packet.biCharacterDBID = biCharacterDBID;

	_wcscpy( Packet.wszChatMsg, _countof(Packet.wszChatMsg), pwszChatMsg, wChatLen );
	Packet.nLen = wChatLen;

	int iSize = static_cast<int>(sizeof(Packet) - sizeof(Packet.wszChatMsg) + sizeof(Packet.wszChatMsg[0]) * Packet.nLen);

	AddSendData(GAMA_DWC_TEAMCHAT, 0, reinterpret_cast<char*>(&Packet), iSize);
}

void CDNMasterConnection::SendUpdateDWCScore(UINT nTeamID, TDWCScore &DWCScore)
{
	MADWCUpdateScore Packet;
	memset (&Packet, 0, sizeof(Packet));

	Packet.nTeamID = nTeamID;
	Packet.DWCScore = DWCScore;

	AddSendData(GAMA_DWC_UPDATE_SCORE, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}
#endif