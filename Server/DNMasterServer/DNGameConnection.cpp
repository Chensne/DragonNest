#include "StdAfx.h"
#include "DNGameConnection.h"
#include "DNUser.h"
//#include "DNParty.h"
#include "DNVillageConnection.h"
#include "DNLoginConnection.h"
#include "DNExtManager.h"
#include "DNDivisionManager.h"
#include "Log.h"
#include "DNPvP.h"
#include "./boost/format.hpp"
#include "./EtStringManager/EtUIXML.h"
#include "DNGuildWarManager.h"

#if defined(_KR)
#include "DNNexonAuth.h"
#elif defined (_JP) && defined (WIN64)
#include "DNNHNNetCafe.h"
#elif defined(_TH)
#include "DNAsiaSoftPCCafe.h"
#elif defined(_ID) 
#include "DNKreonPCCafe.h"
#endif	// _KR

#if defined(PRE_ADD_CHNC2C)
#include "DNC2C.h"
#endif

extern TMasterConfig g_Config;

CDNGameConnection::CDNGameConnection(void): CConnection(), m_wGameID(0)
{
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)

	m_nRoomCnt = m_nUserCnt = 0;
	m_bZeroPopulation = false;
	m_cAffinityType = _GAMESERVER_AFFINITYTYPE_HYBRYD;
	m_nManagedID = 0;
	m_bConnectComplete = false;
	m_uiLastReqEnterGameTick = 0;
	m_uiLastEnterGameTick	 = 0;
#if defined( PRE_FIX_WORLDCOMBINEPARTY )
	m_bWorldCombineGameServer = false;
#endif
}

CDNGameConnection::~CDNGameConnection(void)
{
	std::vector <TGameInfo*>::iterator ii;
	for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
		SAFE_DELETE((*ii));
}

int CDNGameConnection::MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	switch (iMainCmd)
	{
	case GAMA_REGIST:
		{
			GARegist *pRegist = (GARegist*)pData;

			m_nManagedID = pRegist->nManagedID;
			m_cAffinityType = pRegist->cAffinityType;
			for (int i = 0; i < pRegist->cInfoCount; i++)
			{
				TGameInfo * pInfo = new TGameInfo;
				memset(pInfo, 0, sizeof(TGameInfo));

				pInfo->nServerID = pRegist->ServerInfo[i].cServerIdx;
				pInfo->nServerAttribute = pRegist->ServerInfo[i].nServerAttribute;
				pInfo->nIP = pRegist->ServerInfo[i].nIP;
				pInfo->nPort = pRegist->ServerInfo[i].nPort;
				pInfo->nTcpPort = pRegist->nTcpOpenPort;
				pInfo->bHasMargin = pRegist->ServerInfo[i].cMargin == 1 ? true : false;
				pInfo->bComplete = false;
				pInfo->uiLastDelayReportTick = timeGetTime();
				
				m_GameServerInfoList.push_back(pInfo);
			}

			SendReqUserList(ERROR_NONE);
			g_Log.Log(LogType::_NORMAL, L"GameConnection Regist ServerInfo ID:[%d]\n", m_wGameID);
			return ERROR_NONE;
		}
		break;

	case GAMA_ENDOFREGIST:
		{
			GAMAEndofRegist * pPacket = (GAMAEndofRegist*)pData;

			std::vector <TGameInfo*>::iterator ii;
			for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
			{
				if ((*ii)->nServerID == pPacket->nServerID)
				{
					(*ii)->bComplete = true;
					break;
				}
			}

			bool bAllComplete = true;
			for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
			{
				if ((*ii)->bComplete == false)
				{
					bAllComplete = false;
					break;
				}
			}

			if (bAllComplete)
			{
				SendGameRegistComplete();				
				g_Log.Log(LogType::_NORMAL, L"[GAMA_ENDOFREGIST] GameID[%d]\r\n", m_wGameID);
			}
			return ERROR_NONE;
		}
		break;

	case GAMA_CONNECTCOMPLETE:
		{
			m_bConnectComplete = true;
#if defined(PRE_FIX_LIVE_CONNECT)
			g_pDivisionManager->AddGameServerConnection(this);
#endif //#if defined(PRE_FIX_LIVE_CONNECT)
			g_pDivisionManager->GameServerRegist(this);
#ifdef PRE_MOD_OPERATINGFARM
			g_pDivisionManager->CheckFarmSync(timeGetTime());
#else		//#ifdef PRE_MOD_OPERATINGFARM
			g_pDivisionManager->CheckFarmSync();
#endif		//#ifdef PRE_MOD_OPERATINGFARM
			g_pDivisionManager->SendGuildWarInfoToGame( this );
			g_Log.Log(LogType::_NORMAL, L"[GAMA_CONNECTCOMPLETE] GameID[%d]\r\n", m_wGameID);
		}
		break;

	case GAMA_DELAYREPORT:
		{
			GAMADelayReport * pPacket = (GAMADelayReport*)pData;

			m_nRoomCnt = pPacket->nWholeRoomCnt;	//전체룸사이즈 그냥 올때마다 갱신

			int nCnt = 0;
			std::vector <TGameInfo*>::iterator ii;
			for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
			{
				if ((*ii)->nServerID == pPacket->nServerIdx)
				{
					(*ii)->nRoomCnt = pPacket->nRoomCnt;
					(*ii)->nUserCnt = pPacket->nUserCnt;
					(*ii)->nFrame = pPacket->nAvrFrame;
					(*ii)->bHasMargin = pPacket->cHasMargin == 1 ? true : false;
					(*ii)->uiLastDelayReportTick = timeGetTime();

#ifdef _WORK
					if( pPacket->nAvrFrame < 17 )
						g_Log.Log(LogType::_FILELOG, L"ID[%d]Index[%d]WRC[%d]RCnt[%d]UCnt[%d]MinF[%d]MaxF[%d]AvrF[%d]\n", m_wGameID, (*ii)->nServerID, pPacket->nWholeRoomCnt, pPacket->nRoomCnt, pPacket->nUserCnt, pPacket->nMinFrame, pPacket->nMaxFrame, pPacket->nAvrFrame);
#else
					if (pPacket->nAvrFrame < 17 && pPacket->nAvrFrame != 0)
						g_Log.Log(LogType::_ERROR, L"ID[%d]Index[%d]WRC[%d]RCnt[%d]UCnt[%d]MinF[%d]MaxF[%d]AvrF[%d]\n", m_wGameID, (*ii)->nServerID, pPacket->nWholeRoomCnt, pPacket->nRoomCnt, pPacket->nUserCnt, pPacket->nMinFrame, pPacket->nMaxFrame, pPacket->nAvrFrame);
#endif
				}

				nCnt += (*ii)->nUserCnt;
			}
			m_nUserCnt = nCnt;
			return ERROR_NONE;
		}
		break;

	case GAMA_CHECKUSER:
		{
			/*GAMACheckUser *pCheckUser = (GAMACheckUser*)pData;
			if (g_pDivisionManager->GameCheckUser(this, pCheckUser->nSessionID) == false)
				_DANGER_POINT();			*/
			return ERROR_NONE;
		}
		break;

	case GAMA_ENTERGAME:
		{
			GAMAEnterGame * pPacket = (GAMAEnterGame*)pData;
			g_pDivisionManager->EnterGame(pPacket->nAccountDBID, m_wGameID, pPacket->nRoomID, pPacket->nServerIdx);

			m_uiLastEnterGameTick = timeGetTime();
			return ERROR_NONE;
		}
		break;

	case GAMA_RECONNECTLOGIN:
		{
			GAMAReconnectLogin * pPacket = (GAMAReconnectLogin*)pData;
			g_pDivisionManager->ReconnectLogin(pPacket->nAccountDBID);
			CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
			if(pUser)
			{
				g_pDivisionManager->UpdateUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID(), _LOCATION_NONE, _COMMUNITY_NONE);
			}
			return ERROR_NONE;
		}

	case GAMA_UPDATEWORLDUSERSTATE:
		{
#if defined( STRESS_TEST )
			break;
#endif
			GAMAUpdateWorldUserState * pPacket = (GAMAUpdateWorldUserState*)pData;
			CDNUser * pUser = g_pDivisionManager->GetUserByName(pPacket->szName);
			if (pUser)
			{
				g_pDivisionManager->UpdateUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID(), _LOCATION_GAME, _COMMUNITY_NONE, -1, pPacket->nMapIdx, this);
			}
			else
				_DANGER_POINT();
		}
		break;

	case GAMA_ADDUSERLIST:
		{
			GAMAAddUserList *pAdd = (GAMAAddUserList*)pData;
			// 마스터 내에 User추가해주고

#if defined(PRE_MOD_SELECT_CHAR)
			g_pDivisionManager->AddUser(pAdd, m_wGameID, true);
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pAdd->nAccountDBID );
#if defined(_FINAL_BUILD)			
			if( pUser ){
				pUser->m_nRoomID = pAdd->nRoomID;
			}
#endif		//#if defined(_FINAL_BUILD)
#else	// #if defined(PRE_MOD_SELECT_CHAR)

			BYTE szMID[MACHINEIDMAX] = {0,};
			DWORD dwGRC = 0;
#if defined (_KR)
			memcpy(szMID, pAdd->szMID, sizeof(szMID));
			dwGRC = pAdd->dwGRC;
#endif
			g_pDivisionManager->AddUser(0, pAdd->wChannelID, m_wGameID, pAdd->nThreadIdx, pAdd->nAccountDBID, pAdd->nSessionID, pAdd->biCharacterDBID, pAdd->wszCharacterName, pAdd->wszAccountName, 
#if defined(PRE_ADD_DWC)
				pAdd->cAccountLevel,
#endif
#ifdef PRE_ADD_MULTILANGUAGE
				pAdd->bAdult, pAdd->szIP, pAdd->szVirtualIp, pAdd->PartyID, szMID, dwGRC, pAdd->cPCBangGrade, pAdd->cSelectedLanguage, true);
#else		//#ifdef PRE_ADD_MULTILANGUAGE
				pAdd->bAdult, pAdd->szIP, pAdd->szVirtualIp, pAdd->PartyID, szMID, dwGRC, pAdd->cPCBangGrade, 0, true);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pAdd->nAccountDBID );
			if( pUser ){
				pUser->m_nRoomID = pAdd->nRoomID;
			}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined(_KR) && defined(_FINAL_BUILD)	// nexon pc bang
			if (pUser)
				g_pNexonAuth->SendLogin(pUser);

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
			char szTempAccountName[IDLENMAX * 2];
			WideCharToMultiByte(CP_ACP, 0, pAdd->wszAccountName, -1, szTempAccountName, IDLENMAX * 2, NULL, NULL);

			bool bTempNetCafe = false;
			char szNHNNetCafeCode[NHNNETCAFECODEMAX];
			char szNHNProductCode[NHNNETCAFECODEMAX];

			if (g_pNHNNetCafe->NetCafe_UserLogin(szTempAccountName, pAdd->szIP, bTempNetCafe))
			{
				if (bTempNetCafe)
				{
					if (g_pNHNNetCafe->NetCafe_GetNetCafeCode(szTempAccountName, szNHNNetCafeCode, sizeof(szNHNNetCafeCode)) == false)
						memset(szNHNNetCafeCode, 0, sizeof(szNHNNetCafeCode));

					if (g_pNHNNetCafe->NetCafe_GetProductCode(szTempAccountName, szNHNProductCode, sizeof(szNHNProductCode)) == false)
						memset(szNHNProductCode, 0, sizeof(szNHNProductCode));

					if (g_pDivisionManager->SetNHNNetCafeInfo(pAdd->nAccountDBID, bTempNetCafe, szNHNNetCafeCode, szNHNProductCode) == false)
						_DANGER_POINT();
				}
			}
#elif defined(_TH) && defined(_FINAL_BUILD)			
			if (pUser)			
				g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 1, pUser->GetAccountDBID() );			
#elif defined(_ID) && defined(_FINAL_BUILD)
			if (pUser)		
			{
				_strcpy(pUser->m_szMacAddress, _countof(pUser->m_szMacAddress), pAdd->szMacAddress, (int)strlen(pAdd->szMacAddress));				
				_strcpy(pUser->m_szKey, _countof(pUser->m_szKey), pAdd->szKey, (int)strlen(pAdd->szKey));								
				pUser->m_dwKreonCN = pAdd->dwKreonCN;
				g_pKreonPCCafe->AddProcessCall(pUser->GetAccountDBID());
			}			
#endif	// _KR

			return ERROR_NONE;
		}
		break;	

	case GAMA_DELUSER:
		{
			GAMADelUser *pDelUser = (GAMADelUser*)pData;
			g_pDivisionManager->DelUser(pDelUser->nAccountDBID);
			return ERROR_NONE;
		}
		break;

	case GAMA_SETROOMID:
		{
			GAMASetRoomID *pRoomID = (GAMASetRoomID*)pData;
			if (g_pDivisionManager->SetGameRoom(this, pRoomID) == false)
			{
#if defined( PRE_WORLDCOMBINE_PARTY )
				if( Party::bIsWorldCombineParty(pRoomID->Type) )
				{
					return ERROR_NONE;
				}
#endif
#if defined( STRESS_TEST )
#else
				_DANGER_POINT();
				g_Log.Log( LogType::_GAMECONNECTLOG, L"SetGameRoom() Failed!!!\n" );
#endif // #if defined( STRESS_TEST )
			}
			return ERROR_NONE;
		}
		break;

	case GAMA_SETTUTORIALROOMID:
		{
			GAMASetTutorialRoomID *pRoomID = (GAMASetTutorialRoomID*)pData;
			if (g_pDivisionManager->SetTutorialGameRoom(m_wGameID, pRoomID->nAccountDBID, pRoomID->cGameID, pRoomID->cServerIdx, pRoomID->nRoomID, pRoomID->nLoginServerID) == false) {
				_DANGER_POINT();
			}
			return ERROR_NONE;
		}
		break;

	case GAMA_REQNEXTVILLAGEINFO:
		{
			//어디로 가야할까나~
			GAMARequestNextVillageInfo * pPacket = (GAMARequestNextVillageInfo*)pData;
			g_pDivisionManager->MoveGameToVillage(this, pPacket);
			return ERROR_NONE;
		}

	case GAMA_REBIRTHVILLAGEINFO:
		{
			GAMARebirthVillageInfo *pInfo = (GAMARebirthVillageInfo*)pData;

			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pInfo->nAccountDBID);
			if (!pUser){
				_DANGER_POINT();
				return ERROR_GENERIC_UNKNOWNERROR;
			}

			return _ProcessRebirthVillage( pUser, pInfo->nLastVillageMapIdx );
		}
		break;

	case GAMA_MOVEPVPGAMETOPVPLOBBY:
		{
			GAMAMovePvPGameToPvPLobby* pPacket = reinterpret_cast<GAMAMovePvPGameToPvPLobby*>(pData);

			if( g_pDivisionManager )
			{
				// 유저 검사
				CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->uiAccountDBID );
				if( !pUser )
				{
					_DANGER_POINT();
					return ERROR_GENERIC_UNKNOWNERROR;
				}

				char	szIp[IPLENMAX]	= { 0, };
				USHORT	nPort			= 0;
				int		nChannel		= 0;
				BYTE	cVillageID		= 0;

#if defined( PRE_WORLDCOMBINE_PVP )
				if( pPacket->bIsLadderRoom == true || pPacket->uiPvPIndex > WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex )
#else
				if( pPacket->bIsLadderRoom == true )
#endif
				{
					// PvP 로비 서버가 맛이 간 상태이다. 마을로 보내주자.
					CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
					if( pVillageCon == NULL || pVillageCon->GetActive() == false )
					{
						return _ProcessRebirthVillage( pUser, pPacket->iLastVillageMapIndex );
					}

#if defined( PRE_WORLDCOMBINE_PVP )					
					if( pUser->GetPvPVillageID() == 0 && pUser->GetPvPVillageChannelID() == 0 )
					{
						// 마스터 서버 다운으로 유저에 마을 정보 셋팅이 안된 상태
						if( pPacket->uiPvPIndex > WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex )
							return _ProcessRebirthVillage( pUser, pPacket->iLastVillageMapIndex );
					}
#endif

					nChannel	= g_pDivisionManager->GetPvPLobbyChannelID();
					cVillageID	= g_pDivisionManager->GetPvPLobbyVillageID();
					nPort		= pVillageCon->GetVillageChannelInfo()->nPort;
					_strcpy( szIp, IPLENMAX, pVillageCon->GetVillageChannelInfo()->szIP, (int)strlen(pVillageCon->GetVillageChannelInfo()->szIP) );

					if( g_pDivisionManager->GameToVillage( this, pUser->GetAccountDBID(), cVillageID, nChannel ) )
					{
						SendMovePvPGameToPvPLobby( pPacket->uiAccountDBID, szIp, nPort, ERROR_NONE);
#if defined( PRE_WORLDCOMBINE_PVP )
						if( pPacket->uiPvPIndex > WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex )
						{
							pUser->SetPvPIndex(0);
						}
#endif
						return ERROR_NONE;
					}
				}
				else
				{
					// 마스터서버가 다운되어 방이 폭파된 경우이다. 마을로 보내주자.
					if( pUser->GetPvPIndex() == 0 )
					{
						return _ProcessRebirthVillage( pUser, pPacket->iLastVillageMapIndex );
					}

					// PvPIndex 비교
					if( pUser->GetPvPIndex() != pPacket->uiPvPIndex )
					{
						_DANGER_POINT();
						return ERROR_GENERIC_UNKNOWNERROR;
					}

					CDNPvP* pPvPRoom = g_pDivisionManager->GetPvPRoomByIdx( pPacket->uiPvPIndex );
					if( !pPvPRoom )
					{
						_DANGER_POINT();
						return ERROR_GENERIC_UNKNOWNERROR;
					}

#if defined( PRE_PVP_GAMBLEROOM )					
					if( pPvPRoom->GetGambleRoomType() > 0 )
						pUser->SetPvPIndex(0);
#endif

					if( g_pDivisionManager->GetVillageInfo( pPvPRoom->GetVillageMapIndex(), pPvPRoom->GetVillageChannelID(), nChannel, cVillageID, szIp, nPort ) )
					{
						if( g_pDivisionManager->GameToVillage( this, pUser->GetAccountDBID(), cVillageID, nChannel ) )
						{
							// PvPCommon::UserSate::Playing|PvPCommon::UserState::Ready 해제
							UINT uiUserState = pUser->GetPvPUserState()&~(PvPCommon::UserState::Playing|PvPCommon::UserState::Ready);
							pPvPRoom->SetPvPUserState( pUser, uiUserState );

							SendMovePvPGameToPvPLobby( pPacket->uiAccountDBID, szIp, nPort, ERROR_NONE);
							return ERROR_NONE;
						}
					}
				}
				SendMovePvPGameToPvPLobby( pPacket->uiAccountDBID, szIp, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
				return ERROR_GENERIC_UNKNOWNERROR;
			}
			else
				_DANGER_POINT();
			break;
		}

	case GAMA_LOGINSTATE:
		{
			GAMALoginState *pState = (GAMALoginState*)pData;

			if (!g_pDivisionManager->GetLoginConnection() || g_pDivisionManager->GetLoginConnection()->GetDelete()) {
				SendLoginState(pState->nAccountDBID, ERROR_LOGIN_FAIL);
			}
			else {
				SendLoginState(pState->nAccountDBID, ERROR_NONE);
			}
		}
		break;

	case GAMA_VILLAGESTATE:
		{
			GAMAVillageState *pState = (GAMAVillageState*)pData;

			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pState->nAccountDBID);
			if (!pUser){
				_DANGER_POINT();
				return ERROR_GENERIC_UNKNOWNERROR;
			}

			char szIP[IPLENMAX]	= { 0, };
			USHORT nPort = 0;
			int	nNextChannelIdx = 0;
			BYTE cNextVillageID	= 0;

			if (g_pDivisionManager->GetVillageInfo(pState->nMapIndex, pUser->GetChannelID(), nNextChannelIdx, cNextVillageID, szIP, nPort))
			{
				SendVillageState(pState->nAccountDBID, pState->nRoomID, pState->nMapIndex, ERROR_NONE );
			}
			else {
				SendVillageState(pState->nAccountDBID, pState->nRoomID, pState->nMapIndex, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
			}
		}
		break;

	//case GAMA_DELPARTYMEMBER:
	//	{
	//		GAMADelPartyMember *pDelMember = (GAMADelPartyMember*)pData;
	//		//if (g_pDivisionManager->DelPartyMember(pDelMember->nAccountDBID, true, pDelMember->cKickKind) == false)
	//		//	_DANGER_POINT();
	//		return ERROR_NONE;
	//	}
	//	break;

	case GAMA_FRIENDADD:
		{
			GAMAFriendAddNotice * pPacket = (GAMAFriendAddNotice*)pData;
			g_pDivisionManager->SendFriendAddNotice(pPacket->nAddedAccountDBID, pPacket->wszAddName);
		}
		break;

	case GAMA_PRIVATECHAT:
		{
			GAMAPrivateChat *pChat = (GAMAPrivateChat*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->wChatLen);

#ifdef PRE_ADD_DOORS
			int nRet = g_pDivisionManager->PrivateChat(pChat->nAccountDBID, pChat->wszToCharacterName, pChat->cType, wszChatMsg, pChat->wChatLen, pChat->biDestCharacterDBID);
#else		//#ifdef PRE_ADD_DOORS
			int nRet = g_pDivisionManager->PrivateChat(pChat->nAccountDBID, pChat->wszToCharacterName, pChat->cType, wszChatMsg, pChat->wChatLen);
#endif		//#ifdef PRE_ADD_DOORS
			if (nRet != ERROR_NONE)
				SendPrivateChat(pChat->nAccountDBID, pChat->wszToCharacterName, pChat->cType, wszChatMsg, pChat->wChatLen, nRet);
			return ERROR_NONE;
		}
		break;

	case GAMA_CHAT:
		{
			GAMAChat *pChat = (GAMAChat*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->wChatLen);

			switch( pChat->cType )
			{
				// GAMA_CHAT 패킷이 CHATTYPE_GM 을 지원안해서 추가~! by 김밥
				case CHATTYPE_GM:
				{
					CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pChat->nAccountDBID );
					if( pUser )
					{
						if( pUser->GetUserState() == STATE_VILLAGE )
						{
							CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
							if( pVillageCon )
								pVillageCon->SendChat( pChat->cType, pChat->nAccountDBID, L"", wszChatMsg, pChat->wChatLen );
						}
					}
					return ERROR_NONE;
				}				
				case CHATTYPE_WORLD:
#if defined(PRE_ADD_WORLD_MSG_RED)
				case CHATTYPE_WORLD_POPMSG:
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
				{
#if defined(PRE_ADD_WORLD_MSG_RED)
					if (g_pDivisionManager->WorldChat(pChat->nAccountDBID, wszChatMsg, pChat->wChatLen, pChat->cType) != ERROR_NONE)
#else	// #if defined(PRE_ADD_WORLD_MSG_RED)
					if (g_pDivisionManager->WorldChat(pChat->nAccountDBID, wszChatMsg, pChat->wChatLen) != ERROR_NONE)
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
						_DANGER_POINT();
					return ERROR_NONE;
				}
			}
		}
		break;

	case GAMA_WORLDSYSTEMMSG:
		{
			GAMAWorldSystemMsg *pMsg = (GAMAWorldSystemMsg*)pData;
			g_pDivisionManager->WorldSystemMsg(pMsg->nAccountDBID, pMsg->cType, pMsg->nID, pMsg->nValue, pMsg->wszToCharacterName);
		}
		break;
#if defined( PRE_PRIVATECHAT_CHANNEL )
	case GAMA_PRIVATECHANNELCHAT:
		{
			GAMAPrivateChannelChatMsg *pChat = (GAMAPrivateChannelChatMsg*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->wChatLen);

			if (g_pDivisionManager->PrivateChannelChat(pChat->nAccountDBID, wszChatMsg, pChat->wChatLen, pChat->nChannelID) != ERROR_NONE)
				return ERROR_NONE;
		}
		break;
#endif

	case GAMA_NOTICE:
		{
			GAMANotice * pPacket = (GAMANotice*)pData;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pPacket->szMsg, pPacket->nLen);

			g_pDivisionManager->Notice(wszChatMsg, pPacket->nLen, 0);
			return ERROR_NONE;
		}
		break;

	case GAMA_BANUSER:
		{
			GAMABanUser * pPacket = (GAMABanUser*)pData;

			CDNUser * pUser = g_pDivisionManager->GetUserByName(pPacket->wszCharacterName);
			if(!pUser)
			{
				//std::wstring wString = boost::io::str( boost::wformat( L"%s 님을 찾을 수 없습니다.") % pPacket->wszCharacterName );
#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4418, pPacket->cSelectedLang) ) % pPacket->wszCharacterName );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4418) ) % pPacket->wszCharacterName );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
				SendChat( CHATTYPE_GM, pPacket->nAccountDBID, L"", wString.c_str(), static_cast<short>(wString.size()) );
				return ERROR_NONE;
			}

			g_pDivisionManager->SendDetachUser(pUser->GetAccountDBID());
		}
		break;
		
		case GAMA_PVP_ROOMSYNCOK:
		{
			GAMAPVP_ROOMSYNCOK* pPacket = reinterpret_cast<GAMAPVP_ROOMSYNCOK*>(pData);
			if( g_pDivisionManager )
				g_pDivisionManager->SetPvPRoomSyncOK( pPacket->uiPvPIndex );
			else
				_DANGER_POINT();
			break;
		}

		case GAMA_PVP_LADDERROOMSYNC:
		{
			GAMAPVP_LADDERROOMSYNC* pPacket = reinterpret_cast<GAMAPVP_LADDERROOMSYNC*>(pData);

			CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
			if( pVillageCon )
				pVillageCon->SendLadderSystemRoomSync( pPacket );
			break;
		}

		case GAMA_PVP_BREAKINTOOK:
		{
			GAMAPVP_BREAKINTOOK* pPacket = reinterpret_cast<GAMAPVP_BREAKINTOOK*>(pData);
			if( g_pDivisionManager )
				g_pDivisionManager->SetPvPBreakIntoOK( pPacket );
			else
				_DANGER_POINT();
			break;
		}

		case GAMA_PVP_COMMAND:
		{
			GAMAPVP_COMMAND* pPacket = reinterpret_cast<GAMAPVP_COMMAND*>(pData);

			if( !g_pDivisionManager )
			{
				_DANGER_POINT();
				return ERROR_GENERIC_UNKNOWNERROR;
			}

#if defined( PRE_WORLDCOMBINE_PVP)
			if( pPacket->uiPvPIndex > WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex )
				break;
#endif
			switch( pPacket->uiCommandType )
			{
				case PvPCommon::GAMA_Command::DeleteGameRoom:
				{
					g_pDivisionManager->SetDestroyPvPGameRoom( pPacket->uiPvPIndex, pPacket->uiRoomIndex );
					break;
				}
				case PvPCommon::GAMA_Command::FinishGameMode:
				{
					g_pDivisionManager->SetPvPFinishGameMode( pPacket->uiPvPIndex, pPacket->uiRoomIndex );
					break;
				}
				case PvPCommon::GAMA_Command::NoMoreBreakInto:
				{
					g_pDivisionManager->SetPvPNoMoreBreakInto( pPacket->uiPvPIndex, pPacket->uiRoomIndex );
					break;
				}
			}

			break;
		}
		case GAMA_PVP_SWAPTMEMBER_INDEX:
			{
				GAMAPVPSwapMemberIndex * pPacket = (GAMAPVPSwapMemberIndex*)pData;
				
				bool bRet = false;
				USHORT nTeam = PvPCommon::Team::A;
				if (g_pDivisionManager)
					bRet = g_pDivisionManager->SetPvPMemberIndex(pPacket->nAccountDBID, pPacket->cCount, pPacket->Index, nTeam);

				SendPvPMemberIndex(nTeam, pPacket, bRet ? ERROR_NONE : ERROR_PVP_SWAPTEAM_MEMBERINDEX_FAIL);
				break;
			}

		case GAMA_PVP_CHANGEMEMBER_GRADE:
			{
				GAMAPVPChangeMemberGrade * pPacket = (GAMAPVPChangeMemberGrade*)pData;

				bool bRet = false;
				USHORT nUserState = PvPCommon::UserState::None;
				USHORT nTeam = PvPCommon::Team::A;
				if (g_pDivisionManager)
					bRet = g_pDivisionManager->SetGuildWarMemberGrade(pPacket->nAccountDBID, pPacket->bAsign, pPacket->nType, pPacket->nTargetSessionID, nUserState, nTeam);

				SendPvPMemberGrade(pPacket->nAccountDBID, nTeam, nUserState, pPacket->nTargetSessionID, bRet ? ERROR_NONE : ERROR_PVP_CHANGEMEMBERGRADE_FAIL);
				break;
			}

		case GAMA_PVP_GUILDWARSCORE:
			{
				GAMAPvPGuildWarScore * pPacket = (GAMAPvPGuildWarScore*)pData;
				if( g_pGuildWarManager )
					g_pGuildWarManager->SetGuildWarTournamentPoint(pPacket);
				break;
			}
		case GAMA_PVP_GUILDWARRESULT:
			{
				GAMAPvPGuildWarResult * pPacket = (GAMAPvPGuildWarResult*)pData;
				if( g_pGuildWarManager )
					g_pGuildWarManager->SetGuildWarTournamentResult(pPacket);
				break;
			}
		case GAMA_PVP_GUILDWAREXCEPTION:
			{
				GAMAPvPGuildWarException * pPacket = (GAMAPvPGuildWarException*)pData;
				if (g_pGuildWarManager)
					g_pGuildWarManager->SetDropTournament(pPacket->nPvPIndex);
				else
					_DANGER_POINT();
				break;
			}
	// Guild
	case GAMA_CHANGEGUILDINFO:				// 길드 정보 변경 알림
		{
			g_pDivisionManager->SendChangeGuildInfo(reinterpret_cast<MAChangeGuildInfo*>(pData));
		}
		break;

	case GAMA_CHANGEGUILDMEMBINFO:			// 길드원 정보 변경 알림
		{
			g_pDivisionManager->SendChangeGuildMemberInfo(reinterpret_cast<MAChangeGuildMemberInfo*>(pData));
		}
		break;

	case GAMA_GUILDCHAT:					// 길드 채팅
		{
			g_pDivisionManager->SendGuildChat(reinterpret_cast<MAGuildChat*>(pData));
		}
		break;
	case GAMA_GUILDCHANGENAME :
		{
			g_pDivisionManager->SendChangeGuildName(reinterpret_cast<MAGuildChangeName*>(pData));
		}
		break;
	case GAMA_UPDATEGUILDEXP:
		{
			g_pDivisionManager->SendUpdateGuildExp(reinterpret_cast<MAUpdateGuildExp*>(pData));
		}
		break;
	case GAMA_ADDGUILDWAR_POINT:
		{
			MAAddGuildWarPoint* pPacket = (MAAddGuildWarPoint*)pData;			
			g_pGuildWarManager->AddGuildWarPoint(pPacket->cTeamType, pPacket->nAddPoint);
			g_pDivisionManager->SendAddGuildWarPoint(pPacket);
		}
		break;
	case GAMA_CHANGECHARACTERNAME:
		{
			g_pDivisionManager->SendChangeCharacterName(reinterpret_cast<MAChangeCharacterName*>(pData));
		}
		break;

	case GAMA_INVITEPARTYMEMBER:
		{
			GAMAInvitePartyMember * pPacket = (GAMAInvitePartyMember*)pData;
			int nInviteRet = g_pDivisionManager->SendInvitePartyMember(GetGameID(), pPacket);
			if (nInviteRet != ERROR_NONE)
			{
				g_pDivisionManager->SendInvitePartyMemberResult(GetGameID(), pPacket->wszInviterName, pPacket->wszInvitedName, nInviteRet);
			}
		}
		break;

	case GAMA_INVITEPARTYMEMBER_RETURNMSG:
		{
			GAMAInvitedReturnMsg * pPacket = (GAMAInvitedReturnMsg*)pData;
			if (g_pDivisionManager->SendInvitedPartyMemberReturn(pPacket->nAccountDBID, pPacket->nRetCode) == false)
				_DANGER_POINT();
		}
		break;
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	case GAMA_GETPARTYID_RESULT:
		{
			GAMAGetPartyIDResult * pPacket = (GAMAGetPartyIDResult*)pData;
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nSenderAccountDBID);

			if( pUser && pUser->GetUserState() == STATE_VILLAGE )
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
				{
					pVillageCon->SendGetPartyIDResult( pPacket->nSenderAccountDBID, pPacket->PartyID );
				}
			}
		}
		break;
	case GAMA_RESPARTYASKJOIN:
		{
			GAMAResPartyAskJoin* pPacket = (GAMAResPartyAskJoin*)pData;

			CDNUser* pUser = NULL;
			if(pPacket->uiReqAccountDBID > 0 )
				pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->uiReqAccountDBID);
			else if( pPacket->wszReqChracterName[0] != '\0' )
				pUser = g_pDivisionManager->GetUserByName(pPacket->wszReqChracterName);

			if( pUser && pUser->GetUserState() == STATE_VILLAGE )
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendResPartyAskJoin( pPacket->iRet, pUser->GetAccountDBID());
			}			
		}
		break;
	case GAMA_ASKJOINAGREEINFO:
		{
			GAMAAskJoinAgreeInfo* pPacket = (GAMAAskJoinAgreeInfo*)pData;

			CDNUser* pUser = g_pDivisionManager->GetUserByName(pPacket->wszAskerCharName);
			if( pUser && pUser->GetUserState() == STATE_VILLAGE )
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendAskJoinAgreeInfo( pUser->GetAccountDBID(), pPacket->PartyID, pPacket->iPassword );
			}			
		}
		break;
#endif

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	case GAMA_SYNC_SYSTEMMAIL:
		{
			g_pDivisionManager->SyncSystemMail( reinterpret_cast<VIMASyncSystemMail*>(pData) );
			break;
		}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	case GAMA_PCBANGRESULT:
		{
			GAMAPCBangResult *pResult = (GAMAPCBangResult*)pData;

			CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pResult->nAccountDBID);
			if (!pUser){
				SendPCBangResult(pResult->nAccountDBID, NULL);
				return ERROR_NONE;
			}

			SendPCBangResult(pResult->nAccountDBID, pUser);
		}
		break;

	case GAMA_NOTIFYGIFT:
		{
			GAMANotifyGift *pGift = (GAMANotifyGift*)pData;
			g_pDivisionManager->SendNotifyGift(pGift->nToAccountDBID, pGift->biToCharacterDBID, pGift->bNew, pGift->nGiftCount);
		}
		break;
	case GAMA_MASTERSYSTEM_SYNC_SIMPLEINFO:
		{
			g_pDivisionManager->SyncMasterSystemSimpleInfo( reinterpret_cast<MasterSystem::GAMASyncSimpleInfo*>(pData) );
			break;
		}
	case GAMA_MASTERSYSTEM_SYNC_GRADUATE:
		{
			g_pDivisionManager->SyncMasterSystemGraduate( reinterpret_cast<MasterSystem::GAMASyncGraduate*>(pData) );
			break;
		}
	case GAMA_MASTERSYSTEM_SYNC_CONNECT:
		{
			g_pDivisionManager->SyncMasterSystemConnect( reinterpret_cast<MasterSystem::GAMASyncConnect*>(pData) );
			break;
		}
		//Farm

	case GAMA_FARM_DATALOADED:
		{
			GAMAFarmDataLoaded * pPacket = (GAMAFarmDataLoaded*)pData;
			if (g_pDivisionManager->SetFarmDataLoaded(pPacket->nFarmDBID, GetGameID(), pPacket->nGameServerIdx, pPacket->nRoomID, pPacket->nMapID, pPacket->nFarmMaxUser) == false)
			{
				//먼가 해야할까? 정상적인 설정이 되지 않았다면 먼가 이상한거다. 방폭하던 처리를해야함
				g_Log.Log( LogType::_FARM, L"GAMA_FARM_DATALOADED Failed!\n" );
				break;
			}
			g_Log.Log( LogType::_FARM, L"GAMA_FARM_DATALOADED! FarmID[%d]\n", pPacket->nFarmDBID );
			break;
		}

	case GAMA_FARM_USERCOUNT:
		{
			GAMAFarmUserCount * pPacket = (GAMAFarmUserCount*)pData;
			if (g_pDivisionManager->SetFarmUpdateUserCount(pPacket->nFarmDBID, pPacket->nFarmRoomID, pPacket->nCurUserCount, pPacket->bStarted, m_nManagedID) == false)
			{
				//이러시면 아니데옵니다.
				_DANGER_POINT();
			}
			break;
		}

	case GAMA_FARM_INTENDEDDESTROY:
		{
			GAMAFarmIntendedDestroy * pPacket = (GAMAFarmIntendedDestroy*)pData;
			if (g_pDivisionManager->DestroyFarm(pPacket->nFarmDBID) == false)
				_DANGER_POINT();
			break;
		}
	case GAMA_FARM_SYNC:
		{
			g_pDivisionManager->SyncFarm( reinterpret_cast<GAMAFarmSync*>(pData) );
			break;
		}
	case GAMA_FARM_SYNC_ADDWATER:
		{
			g_pDivisionManager->SyncFarmAddWater( reinterpret_cast<GAMAFarmSyncAddWater*>(pData) );
			break;
		}

	case GAMA_GUILDMEMBER_LELVEUP:
		{
			g_pDivisionManager->SendGuildMemberLevelUp(reinterpret_cast<MAGuildMemberLevelUp*>(pData));	
		}
		break;

	case GAMA_LOAD_USERTEMPDATA:
		{
			GAMALoadUserTempData * pPacket = (GAMALoadUserTempData*)pData;
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->uiAccountDBID );

#if defined(PRE_ADD_REMOTE_QUEST)
			if(pUser)
				SendUserTempDataResult(pPacket->uiAccountDBID, pUser);
#else
#if defined(PRE_ADD_GAMEQUIT_REWARD)
			if( pUser )
				SendUserTempDataResult(pPacket->uiAccountDBID, pUser->GetDungeonClearCount(), pUser->GetUserGameQuitRewardType());
#else	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
			if( pUser )
				SendUserTempDataResult(pPacket->uiAccountDBID, pUser->GetDungeonClearCount());
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#endif	//#if defined(PRE_ADD_REMOTE_QUEST)
		}
		break;

	case GAMA_SAVE_USERTEMPDATA:
		{
			GAMASaveUserTempData * pPacket = (GAMASaveUserTempData*)pData;
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->uiAccountDBID );
			if( pUser )
			{
				pUser->SetDungeonClearCount(pPacket->nDungeonClearCount);
#if defined(PRE_ADD_REMOTE_QUEST)
				pUser->SetAcceptWaitRemoteQuest(pPacket->nAcceptWaitRemoteQuestCount, pPacket->AcceptWaitRemoteQuestList);
#endif
#if defined(PRE_ADD_GAMEQUIT_REWARD)
				pUser->SetUserGameQuitRewardType(pPacket->eUserGameQuitRewardType);
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
			}
		}
		break;

	case GAMA_DUPLICATE_LOGIN :
		{
			GAMADuplicateLogin*  pPacket = (GAMADuplicateLogin*)pData;
			// 여기서 지우자 성공이든 실패든
			//g_pDivisionManager->DelUser(pPacket->nAccountDBID);
			// 실패이면 로그인에 ResetAuth호출하라고 알림
			if( !pPacket->bIsDetach )
			{
				CDNLoginConnection* pLoginConnection = g_pDivisionManager->GetFirstEnableLoginServer();
				if( pLoginConnection )				
				{
					pLoginConnection->SendDuplicateLogin(pPacket->nAccountDBID, pPacket->nSessionID);
					g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, pPacket->nAccountDBID, 0, pPacket->nSessionID, L"[ADBID:%u] GAMA_DUPLICATE_LOGIN \r\n", pPacket->nAccountDBID);
				}
			}
		}
		break;
		case GAMA_CHECK_LASTDUNGEONINFO:
		{
			GAMACheckLastDungeonInfo* pPacket = reinterpret_cast<GAMACheckLastDungeonInfo*>(pData);

			CDNUser* pUser = g_pDivisionManager->GetUserByCharacterDBID( pPacket->biCharacterDBID );
			if( pUser && (pUser->GetUserState() == STATE_VILLAGE || pUser->GetUserState() == STATE_CHECKVILLAGE) )
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendCheckLastDungeonInfo( pPacket->uiAccountDBID, pPacket->biCharacterDBID, pPacket->bIsCheck, pPacket->wszPartyName );
			}
			break;
		}
		case GAMA_CONFIRM_LASTDUNGEONINFO:
		{
			GAMAConfirmLastDungeonInfo* pPacket = reinterpret_cast<GAMAConfirmLastDungeonInfo*>(pData);

			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->uiAccountDBID );
			if( pUser && (pUser->GetUserState() == STATE_VILLAGE || pUser->GetUserState() == STATE_CHECKVILLAGE) )
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
#if defined( PRE_PARTY_DB )
					pVillageCon->SendConfirmLastDungeonInfo( pPacket->iRet, pPacket->uiAccountDBID, pPacket->BreakIntoType );
#else
					pVillageCon->SendConfirmLastDungeonInfo( pPacket->iRet, pPacket->uiAccountDBID );
#endif // #if defined( PRE_PARTY_DB )
			}
			break;
		}

#if defined( PRE_WORLDCOMBINE_PARTY )
		case GAMA_DELWORLDPARTY:
			{
				MADelWorldParty *pPacket = (MADelWorldParty*)pData;
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
				if( pVillageCon )
				{
					pVillageCon->SendDelWorldParty( pPacket );					
				}
			}
			break;
		case GAMA_WORLDPARTYMEMBER:
			{
				GAMASendWorldPartyMember* pPacket = reinterpret_cast<GAMASendWorldPartyMember*>(pData);

				CDNUser* pUser = g_pDivisionManager->GetUserByCharacterDBID( pPacket->biCharacterDBID );
				if( pUser )
				{
					CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
					if( pVillageCon )
						pVillageCon->SendWorldPartyMember( pPacket );
				}
			}
			break;
#endif
#if defined( PRE_ADD_BESTFRIEND )
		case GAMA_LEVELUP_BESTFRIEND:
			{
				g_pDivisionManager->SendLevelBestFriend(reinterpret_cast<MALevelUpBestFriend*>(pData));
			}
			break;
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
		case GAMA_PRIVATECHATCHANNEL_ADD:
			{
				int nRet = g_pDivisionManager->AddPrivateChatChannel( reinterpret_cast<MAAddPrivateChannel*>(pData) );				
			}
			break;
		case GAMA_PRIVATECHATCHANNEL_MEMBERADD:
			{
				int nRet = g_pDivisionManager->AddPrivateChatChannelMember( reinterpret_cast<MAAddPrivateChannelMember*>(pData) );				
			}
			break;
		case GAMA_PRIVATECHATCHANNEL_MEMBERINVITE:
			{
				int nRet = g_pDivisionManager->InvitePrivateChatChannelMember( reinterpret_cast<MAInvitePrivateChannelMember*>(pData) );				
			}
			break;
		case GAMA_PRIVATECHATCHANNEL_MEMBERINVITERESULT:
			{
				int nRet = g_pDivisionManager->InviteResultPrivateChatChannelMember( reinterpret_cast<MAInvitePrivateChannelMemberResult*>(pData) );				
			}
			break;
		case GAMA_PRIVATECHATCHANNEL_MEMBERDEL:
			{
				int nRet = g_pDivisionManager->DelPrivateChatChannelMember( reinterpret_cast<MADelPrivateChannelMember*>(pData) );				
			}
			break;
		case GAMA_PRIVATECHATCHANNEL_MOD:
			{
				int nRet = g_pDivisionManager->ModPrivateChatChannel( reinterpret_cast<MAModPrivateChannel*>(pData) );				
			}
			break;
		case GAMA_PRIVATECHATCHANNEL_MEMBERKICKRESULT:
			{
				int nRet = g_pDivisionManager->KickPrivateChatChannelMemberResult( reinterpret_cast<MAKickPrivateChannelMemberResult*>(pData) );				
			}
			break;
#endif
#if defined( PRE_FIX_WORLDCOMBINEPARTY )
		case GAMA_WORLDCOMBINEGAMESERVER:
			{
				SetWorldCombineGameServer(true);
			}
			break;
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
		case GAMA_DEL_WORLDPVPROOM:
			{
				MADelWorldPvPRoom *pPacket = (MADelWorldPvPRoom*)pData;
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
				if( pVillageCon )
				{
					pVillageCon->SendDelWorldPvPRoom( pPacket );					
				}
			}
			break;
		case GAMA_WORLDPVPROOM_JOINRESULT:
			{	
				MAWorldPvPRoomJoinResult *pPacket = (MAWorldPvPRoomJoinResult*)pData;

				CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->uiAccountDBID);
				if (!pUser) break;				

				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
				{
					UINT uiUserState = pUser->GetPvPUserState()&~PvPCommon::UserState::Syncing;
					pUser->SetPvPUserState( uiUserState );

					pVillageCon->SendWorldPvPRoomJoinResult( pPacket );
					pVillageCon->SendPvPChangeUserState( g_pDivisionManager->GetPvPLobbyChannelID(), 0, pUser->GetAccountDBID(), uiUserState );
				}
			}
			break;
		case GAMA_WORLDPVPROOM_GMCREATERESULT:
			{	
				MAWorldPvPRoomGMCreateResult *pPacket = (MAWorldPvPRoomGMCreateResult*)pData;

				CDNUser *pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->uiAccountDBID);
				if (!pUser) break;				

				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
				{
					UINT uiUserState = pUser->GetPvPUserState()&~PvPCommon::UserState::Syncing;
					pUser->SetPvPUserState( uiUserState );

					pVillageCon->SendWorldPvPRoomGMCreateResult( pPacket );
					pVillageCon->SendPvPChangeUserState( g_pDivisionManager->GetPvPLobbyChannelID(), 0, pUser->GetAccountDBID(), uiUserState );
				}
			}
			break;
#endif
#if defined(PRE_ADD_MUTE_USERCHATTING)
		case GAMA_MUTE_USERFIND:
			{
				MAMuteUserFind *pPacket = (MAMuteUserFind*)pData;
				CDNUser *pUser = g_pDivisionManager->GetUserByName(pPacket->wszMuteUserName);

				if(pUser)
				{
					MAMuteUserChat RestraintData;
					RestraintData.uiGmAccountID = pPacket->uiGmAccountID;
					RestraintData.nMuteMinute = pPacket->nMuteMinute;
					RestraintData.uiMuteUserAccountID = pUser->GetAccountDBID();

					if(pUser->GetUserState() == STATE_VILLAGE)
					{
						CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
						if(pVillageCon)
						{
							pVillageCon->SendMuteUserChatting(&RestraintData);
							return ERROR_NONE;
						}
					}
					else if(pUser->GetUserState() == STATE_GAME)
					{
						CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID(pUser->GetGameID());
						if(pGameCon)
						{
							pGameCon->SendMuteUserChatting(&RestraintData);
							return ERROR_NONE;
						}
					}
				}
#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4418, pPacket->cSelectedLanguage) ) % pPacket->wszMuteUserName );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wString = boost::io::str( boost::wformat( GetEtUIXML().GetUIString(CEtUIXML::idCategory1,4418) ) % pPacket->wszMuteUserName );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
				SendChat( CHATTYPE_GM, pPacket->uiGmAccountID, L"", wString.c_str(), static_cast<short>(wString.size()) );
				break;
			}
#endif	// #if defined(PRE_ADD_MUTE_USERCHATTING)
#if defined( PRE_MOD_71820 )
		case GAMA_NOTIFYMAIL:
			{
				GAMANotifyMail *pMail = (GAMANotifyMail*)pData;
				g_pDivisionManager->SendNotifyMail(pMail->nToAccountDBID, pMail->biToCharacterDBID, pMail->wTotalMailCount, pMail->wNotReadMailCount, pMail->w7DaysLeftMailCount, pMail->bNewMail);
			}
			break;
#endif // #if defined( PRE_MOD_71820 )
#if defined( PRE_ALTEIAWORLD_EXPLORE )
		case GAMA_ALTEIAWORLD_SENDTICKET_RESULT:
			{
				MAAlteiaWorldSendTicketResult *pPacket = (MAAlteiaWorldSendTicketResult*)pData;
				int nRet = g_pDivisionManager->AlteiaSendTicketResult( reinterpret_cast<MAAlteiaWorldSendTicketResult*>(pData) );
			}
			break;
#endif
#if defined(PRE_ADD_CHNC2C)		
		case GAMA_C2C_CHAR_ADDCOIN :
			{
				MAC2CAddCoinResult *pPacket = (MAC2CAddCoinResult*)pData;
				if( g_pDnC2C )				
					g_pDnC2C->SendAddCoin(pPacket->nRetCode, pPacket->szSeqID);
			}
			break;
		case GAMA_C2C_CHAR_REDUCECOIN :
			{
				MAC2CReduceCoinResult *pPacket = (MAC2CReduceCoinResult*)pData;
				if( g_pDnC2C )				
					g_pDnC2C->SendReduceCoin(pPacket->nRetCode, pPacket->szSeqID);
			}
			break;
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined(PRE_ADD_DWC)
		case GAMA_DWC_TEAMCHAT:
			{
				g_pDivisionManager->SendDWCTeamChat(reinterpret_cast<MADWCTeamChat*>(pData));
			}
			break;
		case GAMA_DWC_UPDATE_SCORE:
			{
				g_pDivisionManager->SendUpdateDWCScore(reinterpret_cast<MADWCUpdateScore*>(pData));
			}
			break;
#endif
	}

	return ERROR_NONE;
}

#ifdef _WORK
//For _WORK
void CDNGameConnection::SendReloadExt()
{
	AddSendData(MAGA_RELOADEXT, 0, NULL, 0);
}

void CDNGameConnection::SendReloadAct()
{
	AddSendData(MAGA_RELOADACT, 0, NULL, 0);
}
#endif		//#ifdef _WORK

void CDNGameConnection::SendRegistWorldID()
{
	MARegist Regist = { 0, };

	Regist.cWorldSetID = g_Config.nWorldSetID;
	Regist.cGameID = (BYTE)m_wGameID;

	AddSendData(MAGA_REGISTWORLDINFO, 0, (char*)&Regist, sizeof(MARegist));
}

void CDNGameConnection::SendReqUserList(short nRetCode)
{
	MAReqUserList packet;
	memset(&packet, 0, sizeof(packet));

	packet.nRetCode = nRetCode;

	AddSendData(MAGA_REQUSERLIST, 0, (char*)&packet, sizeof(packet));
}

void CDNGameConnection::SendGameRegistComplete()
{
	AddSendData(MAGA_REGISTCOMPLETE, 0, NULL, 0);
}

void CDNGameConnection::SendAddUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocationState, int nCommunityState, int nChannelID, int nMapIdx)
{
	MAUserState packet;
	memset(&packet, 0, sizeof(packet));

	packet.Type = WorldUserState::Add;
	packet.biCharacterDBID = biCharacterDBID;
	packet.cLocationState = static_cast<BYTE>(nLocationState == -1 ? _LOCATION_LOGIN : nLocationState);		//초기 생성에는 로긴이에요
	packet.cCommunityState = _COMMUNITY_NONE;	//아직 어떻게 쓸지 미정
	packet.cNameLen = (BYTE)wcslen(pName);
	_wcscpy(packet.wszBuf, NAMELENMAX, pName, (BYTE)wcslen(pName));
	packet.nChannelID = nChannelID == -1 ? 0 : nChannelID;
	packet.nMapIdx = nMapIdx == -1 ? 0 : nMapIdx;

	AddSendData(MAGA_USERSTATE, 0, (char*)&packet, sizeof(packet) - sizeof(packet.wszBuf) + (packet.cNameLen * sizeof(WCHAR)));
}

void CDNGameConnection::SendDelUserState(const WCHAR * pName, INT64 biCharacterDBID)
{
	MAUserState packet;
	memset(&packet, 0, sizeof(packet));

	packet.Type = WorldUserState::Delete;
	packet.cNameLen = (BYTE)wcslen(pName);
	_wcscpy(packet.wszBuf, NAMELENMAX, pName, (BYTE)wcslen(pName));
	packet.biCharacterDBID = biCharacterDBID;

	AddSendData(MAGA_USERSTATE, 0, (char*)&packet, sizeof(packet) - sizeof(packet.wszBuf) + (packet.cNameLen * sizeof(WCHAR)));
}

void CDNGameConnection::SendUpdateUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocationState, int nCommunityState, int nChannelID, int nMapIdx)
{
	MAUserState packet;
	memset(&packet, 0, sizeof(packet));

	packet.Type = WorldUserState::Modify;
	packet.biCharacterDBID = biCharacterDBID;
	packet.cNameLen = (BYTE)wcslen(pName);
	_wcscpy(packet.wszBuf, NAMELENMAX, pName, (BYTE)wcslen(pName));
	packet.cLocationState = nLocationState;
	packet.cCommunityState = nCommunityState;
	packet.nChannelID = nChannelID;
	packet.nMapIdx = nMapIdx;

	AddSendData(MAGA_USERSTATE, 0, (char*)&packet, sizeof(packet) - sizeof(packet.wszBuf) + (packet.cNameLen * sizeof(WCHAR)));
}

void CDNGameConnection::SendReqTutorialRoomID(CDNUser *pUser, int nLoginServerID)
{
	MAGAReqTutorialRoomID RoomID = { 0, };

	RoomID.nAccountDBID = pUser->GetAccountDBID();
	RoomID.nSessionID = pUser->GetSessionID();
	RoomID.biCharacterDBID = pUser->GetCharacterDBID();
	RoomID.nRandomSeed = pUser->m_nRandomSeed;
	RoomID.nTutorialMapIndex = pUser->m_nMapIndex;
	RoomID.cTutorialGateNo = pUser->m_cGateNo;
	RoomID.cWorldSetID = (BYTE)g_Config.nWorldSetID;
	RoomID.bAdult = pUser->IsAdult();
#if defined(PRE_ADD_MULTILANGUAGE)
	RoomID.cSelectedLanguage = pUser->m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	_wcscpy(RoomID.wszAccountName, IDLENMAX, pUser->GetAccountName(), (BYTE)wcslen(pUser->GetAccountName()));
	RoomID.nLoginServerID = nLoginServerID;

	AddSendData(MAGA_REQTUTORIALROOMID, 0, (char*)&RoomID, sizeof(MAGAReqTutorialRoomID));
}

bool CDNGameConnection::SendReqRoomID( GameTaskType::eType GameTaskType, CDNUser* pUser, TDUNGEONDIFFICULTY StageDifficulty, int nMeritBonusID, bool bDirectConnect )
{
	MAGAReqRoomID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.GameTaskType = GameTaskType;
	TxPacket.cReqGameIDType	= REQINFO_TYPE_SINGLE;
	TxPacket.nLeaderAccountDBID	= pUser->GetAccountDBID();
	TxPacket.InstanceID = pUser->GetAccountDBID();
	TxPacket.nRandomSeed = pUser->m_nRandomSeed;
	TxPacket.nMapIndex = pUser->m_nMapIndex;
#if defined( PRE_PARTY_DB )
	TxPacket.PartyData.Clear();
#else
	TxPacket.ItemLootRule = ITEMLOOTRULE_NONE;	
#endif
	
	TxPacket.cGateNo = pUser->m_cGateNo;
	TxPacket.cGateSelect = pUser->m_cGateSelect;
	TxPacket.StageDifficulty = StageDifficulty;
	TxPacket.cWorldSetID = g_Config.nWorldSetID;
	TxPacket.cVillageID = pUser->GetVillageID();
	TxPacket.nMeritBonusID = nMeritBonusID;
	TxPacket.bDirectConnect = bDirectConnect;
	TxPacket.nChannelID = pUser->GetChannelID();
	TxPacket.cMemberCount = 1;
	TxPacket.arrMemberInfo[0].MemberAccountDBID = pUser->GetAccountDBID();
	TxPacket.arrMemberInfo[0].MemberSessionID = pUser->GetSessionID();
	TxPacket.arrMemberInfo[0].biMemberCharacterDBID	= pUser->GetCharacterDBID();
	TxPacket.arrMemberInfo[0].MemberTeam = 0;
	TxPacket.arrMemberInfo[0].bAdult = pUser->IsAdult();
	TxPacket.arrMemberInfo[0].cPCBangGrade = pUser->GetPCBangGrade();
#if defined(PRE_ADD_MULTILANGUAGE)
	TxPacket.arrMemberInfo[0].cSelectedLanguage = pUser->m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	_wcscpy(TxPacket.arrMemberInfo[0].wszAccountName, IDLENMAX, pUser->GetAccountName(), (BYTE)wcslen(pUser->GetAccountName()));
#if defined(_ID)
	_strcpy(TxPacket.arrMemberInfo[0].szMacAddress, _countof(TxPacket.arrMemberInfo[0].szMacAddress), pUser->m_szMacAddress, (int)strlen(pUser->m_szMacAddress));				
	_strcpy(TxPacket.arrMemberInfo[0].szKey, _countof(TxPacket.arrMemberInfo[0].szKey), pUser->m_szKey, (int)strlen(pUser->m_szKey));					
#endif	//
	int iLen = sizeof(MAGAReqRoomID)-sizeof(TxPacket.arrMemberInfo)+sizeof(MAGAReqMemberInfo)*TxPacket.cMemberCount;
	int nRet = AddSendData( MAGA_REQROOMID, 0, reinterpret_cast<char*>(&TxPacket), iLen );
	if( nRet < 0 )
		g_Log.Log( LogType::_GAMECONNECTLOG, pUser, L"SendReqRoomID() Failed!!\n" );
	return ( nRet == 0 );
}

void CDNGameConnection::SendReqRoomID(VIMAReqGameID * pPacket, int nMeritBonusID)
{
	MAGAReqRoomID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	
	TxPacket.GameTaskType		= pPacket->GameTaskType;
	TxPacket.cReqGameIDType		= REQINFO_TYPE_PARTY;
	TxPacket.nLeaderAccountDBID	= pPacket->PartyData.nLeaderAccountDBID;
	TxPacket.InstanceID			= pPacket->InstanceID;
	TxPacket.nRandomSeed		= pPacket->nRandomSeed;
	TxPacket.nMapIndex			= pPacket->nMapIndex;
#if defined( PRE_PARTY_DB )
	TxPacket.PartyData = pPacket->PartyData.PartyData;		
#else
	TxPacket.ItemLootRule		= pPacket->PartyData.ItemLootRule;
	TxPacket.ItemLootRank		= pPacket->PartyData.ItemLootRank;
	TxPacket.cUserLvLimitMin = pPacket->PartyData.cUserLvLimitMin;
	TxPacket.nTargetMapIdx = pPacket->PartyData.nTargetMapIdx;
#endif
	TxPacket.cGateNo			= pPacket->cGateNo;
	TxPacket.cGateSelect		= pPacket->cGateSelect;
	TxPacket.StageDifficulty	= pPacket->StageDifficulty;
	TxPacket.cWorldSetID		= g_Config.nWorldSetID;
	TxPacket.nMeritBonusID		= nMeritBonusID;
	TxPacket.bDirectConnect		= pPacket->bDirectConnect;
	TxPacket.nChannelID = pPacket->PartyData.nChannelID;
	
#if defined( PRE_PARTY_DB )	
#else
	TxPacket.cUserLvLimitMax = pPacket->PartyData.cUserLvLimitMax;
	TxPacket.PartyDifficulty = pPacket->PartyData.Difficulty;
	_wcscpy(TxPacket.wszPartyName, PARTYNAMELENMAX, pPacket->PartyData.wszPartyName, (BYTE)wcslen(pPacket->PartyData.wszPartyName));
#endif // #if defined( PRE_PARTY_DB )	
	
	
#if defined( PRE_PARTY_DB )	
#else
	TxPacket.cIsJobDice = pPacket->PartyData.cJobDice;
	_wcscpy(TxPacket.wszPartyPass, PARTYPASSWORDMAX, pPacket->PartyData.wszPartyPass, (BYTE)wcslen(pPacket->PartyData.wszPartyPass));
	TxPacket.cUpkeepCount = pPacket->PartyData.cUpkeepCount;
#endif // #if defined( PRE_PARTY_DB )
	memcpy(&TxPacket.nKickedMemberList, &pPacket->PartyData.nKickedMemberDBID, sizeof(TxPacket.nKickedMemberList));
	
	
	TxPacket.nVoiceChannelID[0] = pPacket->PartyData.nVoiceChannelID;
#if defined(PRE_PARTY_DB )
#else
	TxPacket.cMemberMax = pPacket->PartyData.cMemberMax;
#endif
	TxPacket.cMemberCount = 0;

#if defined( PRE_PARTY_DB )
	for( int i=0 ; i<pPacket->PartyData.PartyData.iCurMemberCount; ++i )
#else
	for( int i=0 ; i<pPacket->PartyData.cMemberCount; ++i )
#endif
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->PartyData.MemberInfo[i].nAccountDBID);
		if( pUser )
		{
			if( pUser->GetAccountDBID() == TxPacket.nLeaderAccountDBID )
				TxPacket.cVillageID = pUser->GetVillageID();

			TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberAccountDBID		= pUser->GetAccountDBID();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberSessionID		= pUser->GetSessionID();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].biMemberCharacterDBID	= pUser->GetCharacterDBID();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberTeam			= 0;
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].bAdult				= pUser->IsAdult();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].cPCBangGrade			= pUser->GetPCBangGrade();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].cMemberIndex = pPacket->PartyData.MemberInfo[i].cMemberIndex;
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].VoiceInfo		= pPacket->PartyData.MemberInfo[i].VoiceInfo;
#if defined( PRE_ADD_NEWCOMEBACK )			
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].bCheckComebackAppellation	= pPacket->PartyData.MemberInfo[i].bCheckComebackAppellation;
#endif
#if defined(PRE_ADD_MULTILANGUAGE)
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].cSelectedLanguage = pUser->m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			_wcscpy( TxPacket.arrMemberInfo[TxPacket.cMemberCount].wszAccountName, IDLENMAX, pUser->GetAccountName(),  (int)wcslen(pUser->GetAccountName()));
#if defined(_ID)
			_strcpy(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szMacAddress, _countof(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szMacAddress), pUser->m_szMacAddress, (int)strlen(pUser->m_szMacAddress));
			_strcpy(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szKey, _countof(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szKey), pUser->m_szKey, (int)strlen(pUser->m_szKey));
#endif	//
			++TxPacket.cMemberCount;
		}
	}

#if defined( PRE_WORLDCOMBINE_PARTY )
	if( Party::bIsWorldCombineParty(pPacket->PartyData.PartyData.Type) )
	{
		// 월드통합룸은 현재로서는 파티장이 없기때문에 빌리지서버아이디 이걸로 셋팅
		TxPacket.cVillageID = (BYTE)pPacket->unVillageChannelID;
	}
#endif
	//
	int iLen = sizeof(MAGAReqRoomID)-sizeof(TxPacket.arrMemberInfo)+sizeof(MAGAReqMemberInfo)*TxPacket.cMemberCount;
	AddSendData( MAGA_REQROOMID, 0, reinterpret_cast<char*>(&TxPacket), iLen );
}

void CDNGameConnection::SendLadderReqRoomID( VIMALadderReqGameID* pPacket )
{
	MAGAReqRoomID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.GameTaskType				= GameTaskType::PvP;
	TxPacket.cReqGameIDType				= REQINFO_TYPE_LADDER;
	TxPacket.nRandomSeed				= pPacket->iRandomSeed;
	TxPacket.nMapIndex					= pPacket->iMapIndex;;
	TxPacket.cWorldSetID				= g_Config.nWorldSetID;
	TxPacket.bDirectConnect				= true;
	TxPacket.cMemberCount				= pPacket->LadderData.cMemberCount;
	TxPacket.biLadderRoomIndexArr[0]	= pPacket->biRoomIndexArr[0];
	TxPacket.biLadderRoomIndexArr[1]	= pPacket->biRoomIndexArr[1];
	TxPacket.MatchType					= pPacket->MatchType;

	for( int i=0 ; i<TxPacket.cMemberCount ; ++i )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->LadderData.MemberInfoArr[i].uiAccountDBID );
		if (pUser)
		{
			if( i == 0 )
			{
				TxPacket.cVillageID = pUser->GetVillageID();
			}

			TxPacket.arrMemberInfo[i].MemberAccountDBID		= pUser->GetAccountDBID();
			TxPacket.arrMemberInfo[i].MemberSessionID		= pUser->GetSessionID();
			TxPacket.arrMemberInfo[i].biMemberCharacterDBID	= pUser->GetCharacterDBID();
			TxPacket.arrMemberInfo[i].MemberTeam			= pPacket->LadderData.MemberInfoArr[i].uiTeam;
			TxPacket.arrMemberInfo[i].bAdult				= pUser->IsAdult();
			TxPacket.arrMemberInfo[i].cPCBangGrade			= pUser->GetPCBangGrade();
#if defined(PRE_ADD_MULTILANGUAGE)
			TxPacket.arrMemberInfo[i].cSelectedLanguage = pUser->m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			_wcscpy( TxPacket.arrMemberInfo[i].wszAccountName, IDLENMAX, pUser->GetAccountName(), (int)wcslen(pUser->GetAccountName()) );
#if defined(_ID)
			_strcpy(TxPacket.arrMemberInfo[i].szMacAddress, _countof(TxPacket.arrMemberInfo[i].szMacAddress), pUser->m_szMacAddress, (int)strlen(pUser->m_szMacAddress));
			_strcpy(TxPacket.arrMemberInfo[i].szKey, _countof(TxPacket.arrMemberInfo[i].szKey), pUser->m_szKey, (int)strlen(pUser->m_szKey));			
#endif	//
		}
	}
	
	int iLen = sizeof(MAGAReqRoomID)-sizeof(TxPacket.arrMemberInfo)+sizeof(MAGAReqMemberInfo)*TxPacket.cMemberCount;
	AddSendData( MAGA_REQROOMID, 0, reinterpret_cast<char*>(&TxPacket), iLen );
}

bool CDNGameConnection::SendReqRoomID( CDNPvP* pPvPRoom, bool bDirectConnect )
{
	MAGAReqRoomID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.GameTaskType		= GameTaskType::PvP;
	TxPacket.cReqGameIDType		= REQINFO_TYPE_PVP;
	TxPacket.nLeaderAccountDBID	= pPvPRoom->GetCaptainAccountDBID();
	TxPacket.InstanceID			= pPvPRoom->GetIndex();
	TxPacket.nRandomSeed		= pPvPRoom->GetRandomSeed();
	TxPacket.nMapIndex			= pPvPRoom->GetMapIndex();
#if defined( PRE_PARTY_DB )
	TxPacket.PartyData.Clear();
#else
	TxPacket.ItemLootRule		= ITEMLOOTRULE_NONE;
#endif
	TxPacket.cGateNo			= 0;
	TxPacket.cWorldSetID			= g_Config.nWorldSetID;
	TxPacket.nMeritBonusID = 0;
	TxPacket.bDirectConnect		= bDirectConnect;

#if defined( PRE_PVP_GAMBLEROOM )
	TxPacket.cGambleType = pPvPRoom->GetGambleRoomType();
	TxPacket.nGamblePrice = pPvPRoom->GetGambleRoomPrice();	
#endif

	if (pPvPRoom->GetGuildWarInfo(TxPacket.nGuildDBID, TxPacket.nGuildQualifyingScore))
		TxPacket.cVillageID = pPvPRoom->GetVillageID();

	TxPacket.nEventRoomIndex	= pPvPRoom->GetEventRoomIndex();
	if (pPvPRoom->GetEventRoomIndex() > 0)
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"SendReqRoomID [Index:%d][Room:%d][Event:%d] \r\n", pPvPRoom->GetIndex(), pPvPRoom->GetGameServerRoomIndex(), pPvPRoom->GetEventRoomIndex());
		
	std::vector<UINT> vAccountDBID;
	pPvPRoom->GetMembersByAccountDBID( vAccountDBID );
	if( pPvPRoom->GetIsGuildWarSystem() == false )
	{
		if (vAccountDBID.size() <= 0)
			return false;
	}
	TxPacket.biCreateRoomCharacterDBID = pPvPRoom->GetCreateRoomCharacterDBID();	
	
	pPvPRoom->GetVoiceChannelID(TxPacket.nVoiceChannelID);
	TxPacket.cMemberCount = 0;
	for( size_t i=0 ; i<vAccountDBID.size() ; ++i )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID(vAccountDBID[i]);
		if( pUser && pPvPRoom->bIsReady( pUser ) && pUser->GetPvPUserState()&PvPCommon::UserState::Syncing )
		{
			if( pUser->GetAccountDBID() == TxPacket.nLeaderAccountDBID )
				TxPacket.cVillageID = pUser->GetVillageID();
#if defined( PRE_PVP_GAMBLEROOM )
			else if( pPvPRoom->GetEventRoomIndex() > 0 || pPvPRoom->GetGambleRoomType() > 0 )
#else
			else if( pPvPRoom->GetEventRoomIndex() > 0 )
#endif
				TxPacket.cVillageID = pPvPRoom->GetVillageID();

			TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberAccountDBID		= pUser->GetAccountDBID();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberSessionID		= pUser->GetSessionID();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].biMemberCharacterDBID	= pUser->GetCharacterDBID();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].cMemberIndex = pPvPRoom->GetPvPTeamIndex(pUser);
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].uiPvPUserState = pUser->GetPvPUserState();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].uiPvPUserState &= ~PvPCommon::UserState::CantLeaveMask;

			// 좀비모드는 강제로 유저팀으로 시작한다.
			if( pPvPRoom->GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival && pUser->GetPvPTeam() != PvPCommon::Team::Observer )
				TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberTeam = PvPCommon::Team::A;
#if defined( PRE_ADD_RACING_MODE)
			// 레이싱 모드는 그냥 같은팀으로 묶음.
			else if( pPvPRoom->GetGameMode() == PvPCommon::GameMode::PvP_Racing && pUser->GetPvPTeam() != PvPCommon::Team::Observer )
				TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberTeam = PvPCommon::Team::A;
#endif 
			else
				TxPacket.arrMemberInfo[TxPacket.cMemberCount].MemberTeam		= pUser->GetPvPTeam();

			TxPacket.arrMemberInfo[TxPacket.cMemberCount].bAdult				= pUser->IsAdult();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].cPCBangGrade			= pUser->GetPCBangGrade();
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].bPvPFatigue			= pUser->GetPvPFatiue();			


#if defined(PRE_ADD_MULTILANGUAGE)
			TxPacket.arrMemberInfo[TxPacket.cMemberCount].cSelectedLanguage = pUser->m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

#if defined(_ID)
			_strcpy(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szMacAddress, _countof(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szMacAddress), pUser->m_szMacAddress, (int)strlen(pUser->m_szMacAddress));
			_strcpy(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szKey, _countof(TxPacket.arrMemberInfo[TxPacket.cMemberCount].szKey), pUser->m_szKey, (int)strlen(pUser->m_szKey));		
#endif	//
			_wcscpy( TxPacket.arrMemberInfo[TxPacket.cMemberCount].wszAccountName, IDLENMAX, pUser->GetAccountName(), (int)wcslen(pUser->GetAccountName()) );
			++TxPacket.cMemberCount;
		}
	}

	//
	int iLen = sizeof(MAGAReqRoomID)-sizeof(TxPacket.arrMemberInfo)+sizeof(MAGAReqMemberInfo)*TxPacket.cMemberCount;
	AddSendData( MAGA_REQROOMID, 0, reinterpret_cast<char*>(&TxPacket), iLen );
	return true;
}

#if defined( PRE_WORLDCOMBINE_PVP )

void CDNGameConnection::SendWorldPvPReqRoomID( VIMACreateWorldPvPRoom* pPacket, BYTE cVillageID )
{
	MAGAReqRoomID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.cVillageID = cVillageID;
	TxPacket.GameTaskType		= GameTaskType::PvP;
	TxPacket.cReqGameIDType		= REQINFO_TYPE_PVP;
	TxPacket.nMapIndex = pPacket->Data.nMapID;
	TxPacket.cWorldSetID = g_Config.nWorldSetID;
	TxPacket.bDirectConnect = true;	
	TxPacket.InstanceID = pPacket->nRoomIndex;
	TxPacket.eWorldReqType = pPacket->Data.eWorldReqType;
	TxPacket.MissonRoomData = pPacket->Data;
	if( pPacket->nGMAccountDBID > 0 )
	{
		TxPacket.cMemberCount++;
		TxPacket.arrMemberInfo[0].MemberAccountDBID = pPacket->nGMAccountDBID;		
	}	

	int iLen = sizeof(MAGAReqRoomID)-sizeof(TxPacket.arrMemberInfo)+sizeof(MAGAReqMemberInfo)*TxPacket.cMemberCount;
	AddSendData( MAGA_REQROOMID, 0, reinterpret_cast<char*>(&TxPacket), iLen );
}

#endif


#if defined( PRE_ADD_FARM_DOWNSCALE )
bool CDNGameConnection::SendReqFarmRoomID(UINT nFarmDBID, int nMapID, int nMaxUser, bool bStart, int iAttr )
#elif defined( PRE_ADD_VIP_FARM )
bool CDNGameConnection::SendReqFarmRoomID(UINT nFarmDBID, int nMapID, int nMaxUser, bool bStart, Farm::Attr::eType Attr )
#else
bool CDNGameConnection::SendReqFarmRoomID(UINT nFarmDBID, int nMapID, int nMaxUser, bool bStart)
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
{
	MAGAReqRoomID TxPacket;

	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.GameTaskType = GameTaskType::Farm;
	TxPacket.cReqGameIDType	= REQINFO_TYPE_FARM;
	TxPacket.InstanceID = nFarmDBID;
	TxPacket.cWorldSetID = g_Config.nWorldSetID;
	TxPacket.nMapIndex = nMapID;
#if defined( PRE_PARTY_DB )
	TxPacket.PartyData.Clear();
	TxPacket.PartyData.nPartyMaxCount = nMaxUser;
#else
	TxPacket.cMemberMax = nMaxUser;
#endif
	TxPacket.nRandomSeed = timeGetTime();
	TxPacket.bStart = bStart;
#if defined( PRE_ADD_FARM_DOWNSCALE )
	TxPacket.iFarmAttr = iAttr;
#elif defined( PRE_ADD_VIP_FARM )
	TxPacket.FarmAttr = Attr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	
	int iLen = sizeof(MAGAReqRoomID)-sizeof(TxPacket.arrMemberInfo);
	int nRet = AddSendData( MAGA_REQROOMID, 0, reinterpret_cast<char*>(&TxPacket), iLen );
	if (nRet < 0)
		g_Log.Log(LogType::_GAMECONNECTLOG, L"SendReqFarmRoomID() Failed!!\n");
	return (nRet == 0);
}


void CDNGameConnection::SetLastReqEnterGameTick()
{
	m_uiLastReqEnterGameTick = timeGetTime();
	if( m_uiLastEnterGameTick == 0 )
		m_uiLastEnterGameTick = m_uiLastReqEnterGameTick;
}

bool CDNGameConnection::SendForceStopPvP(int nGameRoomID, UINT nForceWinGuildDBID)
{
	MAGAForceStopPvP packet;
	memset(&packet, 0, sizeof(MAGAForceStopPvP));

	packet.nGameRoomID = nGameRoomID;
	packet.nForceWinGuildDBID = nForceWinGuildDBID;

	if (AddSendData(MAGA_PVP_FORCESTOP, 0, (char*)&packet, sizeof(MAGAForceStopPvP)) == ERROR_NONE)
		return true;
	return false;
}
void CDNGameConnection::SendGuildWarAllStop()
{
	AddSendData(MAGA_PVP_ALLSTOPGUILDWAR, 0, NULL, 0);
}

void CDNGameConnection::SendInivitePartyMemberResult(UINT nAccountDBID, const WCHAR * pwszInvitedName, int nRetCode)
{
	MAGAInvitePartyMemberResult packet;
	memset(&packet, 0, sizeof(MAGAInvitePartyMemberResult));

	packet.nAccountDBID = nAccountDBID;
	STRCPYW(packet.wszInvitedName, NAMELENMAX, pwszInvitedName);
	packet.nRetCode = nRetCode;

	AddSendData(MAGA_INVITEPARTYMEMBERRESULT, 0, (char*)&packet, sizeof(MAGAInvitePartyMemberResult));
}

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
void CDNGameConnection::SendGetPartyID( UINT nAccountDBID, UINT nSenderAccountDBID )
{
	MAGAGetPartyID packet;
	memset(&packet, 0, sizeof(MAGAGetPartyID));

	packet.nAccountDBID = nAccountDBID;
	packet.nSenderAccountDBID = nSenderAccountDBID;

	AddSendData(MAGA_GETPARTYID, 0, (char*)&packet, sizeof(packet));
}

void CDNGameConnection::SendReqPartyAskJoin( UINT nAccountDBID, const VIMAReqPartyAskJoin* pPacket )
{
	MAGAReqPartyAskJoin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nAccountDBID = nAccountDBID;
	TxPacket.nReqAccountDBID = pPacket->uiReqAccountDBID;
	TxPacket.biReqCharacterDBID = pPacket->biReqCharacterDBID;
	TxPacket.cReqUserJob = pPacket->cReqUserJob;
	TxPacket.cReqUserLevel = pPacket->cReqUserLevel;
	_wcscpy( TxPacket.wszReqChracterName, _countof(TxPacket.wszReqChracterName), pPacket->wszReqChracterName, static_cast<int>(wcslen(pPacket->wszReqChracterName)) );
	

	AddSendData( MAGA_REQPARTYASKJOIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}
#endif

bool CDNGameConnection::GetHasMargin()
{
	if (m_GameServerInfoList.empty()) return false;
	std::vector <TGameInfo*>::iterator ii;

	for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
	{
		if( timeGetTime()-(*ii)->uiLastDelayReportTick >= DELAYED_PRCESS_CHECKTICK+3000 )
		{
			g_Log.Log( LogType::_GAMESERVERDELAY, L"[%d] GameServer Delay Delay=%d!!!\n", m_nManagedID, timeGetTime()-(*ii)->uiLastDelayReportTick );
			return false;
		}
	}

	// 10분동안 아무런 유입이 없다면 점검시간이나 망겜상태로 간주하고 다시 정상상태로 돌린다.
	if( m_uiLastReqEnterGameTick && timeGetTime()-m_uiLastReqEnterGameTick >= 1000*60*10 )
	{
		m_uiLastReqEnterGameTick = timeGetTime();
		m_uiLastEnterGameTick	 = timeGetTime();
		//g_Log.Log( LogType::_ENTERGAMECHECK, L"[%d] 일정동안 아무런 유입이 없어 점검시간이나 기타상태로 간주하고 Tick 초기화!!!\n", m_nManagedID );
	}

	DWORD dwGap = abs( static_cast<int>(m_uiLastReqEnterGameTick-m_uiLastEnterGameTick) );
	if( dwGap >= 60000 )
	{
		//g_Log.Log( LogType::_ENTERGAMECHECK, L"[%d] EnterGameCheck!!! Delay=%d!!!\n", m_nManagedID, dwGap );
		return false;
	}

	for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
		if ((*ii)->bHasMargin == true)
			return true;
	return false;
}

int CDNGameConnection::GetIdleGameServer(int nCnt)
{
	if (m_GameServerInfoList.empty()) return -2;
	if (nCnt > (int)m_GameServerInfoList.size())
		return m_GameServerInfoList[rand()%m_GameServerInfoList.size()]->nServerID;

	std::vector <TGameInfo*>::iterator ii;
	for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
		if ((*ii)->bHasMargin == true)
			return (*ii)->nServerID;
	return -1;
}

int CDNGameConnection::GetIdleGameServerByAtt(int Att)
{
	std::vector <TGameInfo*>::iterator ii;
	for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
		if ((*ii)->bHasMargin == true && (*ii)->nServerAttribute&Att)
			return (*ii)->nServerID;
	return -1;
}

bool CDNGameConnection::GetGameServerInfoByID(int nGameServerID, USHORT * pPort, char * pIP, USHORT * pTcpPort)
{
	std::vector <TGameInfo*>::iterator ii;
	for (ii = m_GameServerInfoList.begin(); ii != m_GameServerInfoList.end(); ii++)
	{
		if ((*ii)->nServerID == nGameServerID)
		{
			*pPort = (*ii)->nPort;
			_inet_addr((*ii)->nIP, pIP);
			*pTcpPort = (*ii)->nTcpPort;
			return true;
		}
	}
	return false;
}

void CDNGameConnection::SendNextVillageServerInfo(UINT nAccountDBID, int nMapIdx, int nNextMapIdx, int nNextGateIdx, const char * pIP, USHORT nPort, short nRet, INT64 nItemSerial)
{
	MAGANextVillageInfo packet = { 0, };

	packet.nAccountDBID = nAccountDBID;
	packet.nMapIdx = nMapIdx;
	packet.nNextMapIdx = nNextMapIdx;
	packet.nNextGateIdx = nNextGateIdx;
	_strcpy( packet.szIP, _countof(packet.szIP), pIP, static_cast<int>(strlen(pIP)) );
	packet.nPort = nPort;
	packet.nRet = nRet;
	packet.nItemSerial = nItemSerial;
	
	AddSendData(MAGA_NEXTVILLAGEINFO, 0, (char*)&packet, sizeof(packet));
}

void CDNGameConnection::SendRebirthVillageInfo(UINT nAccountDBID, int nVillageID, int nChannelID, const char *pIp, USHORT nPort, short nRet)
{
	MAGARebirthVillageInfo Info = { 0, };

	Info.nAccountDBID	= nAccountDBID;
	Info.nVillageID		= nVillageID;
	Info.nChannelID		= nChannelID;
	_strcpy(Info.szIP, _countof(Info.szIP), pIp, (int)strlen(pIp));
	Info.nPort = nPort;
	Info.nRet = nRet;

	AddSendData(MAGA_REBIRTHVILLAGEINFO, 0, (char*)&Info, sizeof(MAGARebirthVillageInfo));
}

void CDNGameConnection::SendMovePvPGameToPvPLobby( const UINT uiAccountDBID, const char* pszIP, const USHORT unPort, const short nRet )
{
	MAGAMovePvPGameToPvPLobby TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.unPort			= unPort;
	TxPacket.nRet			= nRet;
	if( pszIP )
		_strcpy( TxPacket.szIP, _countof(TxPacket.szIP), pszIP, (int)strlen(pszIP) );

	AddSendData( MAGA_MOVEPVPGAMETOPVPLOBBY, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendLoginState(UINT nAccountDBID, short nRet)
{
	MAGALoginState State = { 0, };

	State.nAccountDBID = nAccountDBID;
	State.nRet = nRet;

	AddSendData(MAGA_LOGINSTATE, 0, (char*)&State, sizeof(MAGALoginState));
}

void CDNGameConnection::SendVillageState(UINT nAccountDBID, int nRoomID, int nMapIndex, short nRet)
{
	MAGAVillageState State = { 0, };

	State.nAccountDBID = nAccountDBID;
	State.nRoomID = nRoomID;
	State.nMapIndex = nMapIndex;
	State.nRet = nRet;

	AddSendData(MAGA_VILLAGESTATE, 0, (char*)&State, sizeof(MAGAVillageState));
}

void CDNGameConnection::SendFriendAddNotice(UINT nAddedAccountDBID, const WCHAR * pName)
{
	MAGAFriendAddNotice packet;
	memset(&packet, 0, sizeof(packet));

	packet.nAddedAccountDBID = nAddedAccountDBID;
	_wcscpy(packet.wszAddName, NAMELENMAX, pName, (int)wcslen(pName));

	AddSendData(MAGA_FRIENDADDNOTICE, 0, (char*)&packet, sizeof(packet));
}

//void CDNGameConnection::SendDelPartyMember(int nRoomID, UINT nLeaderAccountDBID, UINT nMemberSessionID, UINT nLeaderSessionID, char cKickKind)
//{
//	MAGADelPartyMember PartyMember = { 0, };
//	
//	PartyMember.nRoomID = nRoomID;
//	PartyMember.nNewLeaderAccountDBID = nLeaderAccountDBID;
//	PartyMember.nMemberSessionID = nMemberSessionID;
//	PartyMember.nNewLeaderSessionID = nLeaderSessionID;
//	PartyMember.cKickKind = cKickKind;
//
//	AddSendData(MAGA_DELPARTYMEMBER, 0, (char*)&PartyMember, sizeof(MAGADelPartyMember));
//}

void CDNGameConnection::SendPrivateChat(UINT nToAccountDBID, const WCHAR *pwszFromCharacterName, char cType, const WCHAR *pwszChatMsg, short wChatLen, int nRet)
{
	MAPrivateChat Private = { 0, };

	Private.nAccountDBID = nToAccountDBID;
	Private.wChatLen = wChatLen;
	Private.cType = cType;
	_wcscpy(Private.wszFromCharacterName, NAMELENMAX, pwszFromCharacterName, (int)wcslen(pwszFromCharacterName));
	Private.nRet = nRet;

	int nSize = 0;
	if (nRet != ERROR_NONE){	// 에러이다
		nSize = sizeof(MAPrivateChat) - sizeof(Private.wszChatMsg);
	}
	else {
		nSize = sizeof(MAPrivateChat) - sizeof(Private.wszChatMsg) + (wChatLen * 2);
		_wcscpy(Private.wszChatMsg, CHATLENMAX, pwszChatMsg, (int)wcslen(pwszChatMsg));
	}

	AddSendData(MAGA_PRIVATECHAT, 0, (char*)&Private, nSize);
}

void CDNGameConnection::SendChat(char cType, UINT nToAccountDBID, const WCHAR *pwszFromCharacterName, const WCHAR *pwszChatMsg, short wChatLen)
{
	MAChat Chat = { 0, };
	
	Chat.nAccountDBID = nToAccountDBID;
	Chat.cType = cType;
	Chat.wChatLen = wChatLen;
	if (pwszFromCharacterName) 	
		_wcscpy(Chat.wszFromCharacterName, NAMELENMAX, pwszFromCharacterName, (int)wcslen(pwszFromCharacterName));

	if (pwszChatMsg) 
		_wcscpy(Chat.wszChatMsg, CHATLENMAX, pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(MAGA_CHAT, 0, (char*)&Chat, sizeof(MAChat) - sizeof(Chat.wszChatMsg) + (wChatLen*sizeof(WCHAR)) );
}

void CDNGameConnection::SendWorldSystemMsg(const WCHAR *pwszFromCharacterName, char cType, int nID, int nValue, WCHAR* pwszToCharacterName)
{
	MAWorldSystemMsg Msg;
	memset(&Msg, 0, sizeof(MAWorldSystemMsg));

	_wcscpy(Msg.wszFromCharacterName, NAMELENMAX, pwszFromCharacterName, (int)wcslen(pwszFromCharacterName));
	Msg.cType = cType;
	Msg.nID = nID;
	Msg.nValue = nValue;
	if (pwszToCharacterName) 
		_wcscpy(Msg.wszToCharacterName, _countof(Msg.wszToCharacterName), pwszToCharacterName, (int)wcslen(pwszToCharacterName));
	
	AddSendData(MAGA_WORLDSYSTEMMSG, 0, (char*)&Msg, sizeof(MAWorldSystemMsg));
}

#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNGameConnection::SendPrivateChannelChat(const WCHAR *pwszFromCharacterName, const WCHAR *pwszChatMsg, short wChatLen, INT64 nChannelID)
{
	MAPrivateChannelChat Chat;
	memset(&Chat, 0, sizeof(MAPrivateChannelChat));

	Chat.cWorldSetID = g_Config.nWorldSetID;
	Chat.wChatLen = wChatLen;
	Chat.nChannelID = nChannelID;
	_wcscpy(Chat.wszFromCharacterName, NAMELENMAX, pwszFromCharacterName, (int)wcslen(pwszFromCharacterName));
	_wcscpy(Chat.wszChatMsg, CHATLENMAX, pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(MAGA_PRIVATECHANNELCHAT, 0, (char*)&Chat, sizeof(Chat) - sizeof(Chat.wszChatMsg) + (wChatLen * 2));
}
#endif

void CDNGameConnection::SendNotice(const WCHAR * pMsg, const int nLen, int nShowSec)
{
	MANotice packet;
	memset(&packet, 0, sizeof(packet));

	packet.Info.nNoticeType = _NOTICETYPE_WORLD;
	packet.Info.nSlideShowSec = nShowSec;
	_wcscpy(packet.wszNoticeMsg, CHATLENMAX, pMsg, (int)wcslen(pMsg));
	packet.nLen = nLen;

	AddSendData(MAGA_NOTICE, 0, (char*)&packet, sizeof(MANotice) - sizeof(packet.wszNoticeMsg) + (packet.nLen * sizeof(WCHAR)));
}

void CDNGameConnection::SendNoticeServer(int nManagedID, const WCHAR * pMsg, int nLen, int nShowSec)
{
	MANotice packet;
	memset(&packet, 0, sizeof(packet));

	packet.Info.nNoticeType = _NOTICETYPE_EACHSERVER;
	packet.Info.nManagedID = nManagedID;
	packet.Info.nSlideShowSec = nShowSec;

	_wcscpy(packet.wszNoticeMsg, CHATLENMAX, pMsg, (int)wcslen(pMsg));
	packet.nLen = nLen;

	AddSendData(MAGA_NOTICE, 0, (char*)&packet, sizeof(MANotice) - sizeof(packet.wszNoticeMsg) + (packet.nLen * sizeof(WCHAR)));
}

void CDNGameConnection::SendNoticeCancel()
{
	AddSendData(MAGA_CANCELNOTICE, 0, NULL, 0);
}

// 치트관련
void CDNGameConnection::SendResRecall( CDNUser* pRecallUser, const VIMAReqRecall* pPacket, const int iRecallMapIndex )
{
	MAGAResRecall TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID			= pPacket->uiAccountDBID;
	TxPacket.uiRecallAccountDBID	= pRecallUser->GetAccountDBID();
	TxPacket.uiRecallChannelID		= pPacket->uiRecallChannelID;
	TxPacket.iRecallMapIndex		= iRecallMapIndex;
#if defined(PRE_ADD_MULTILANGUAGE)
	TxPacket.cRecallerSelectedLang = pPacket->cCallerSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	AddSendData( MAGA_RESRECALL, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendUserRestraint(UINT nAccountDBID)
{
	MARestraint packet;
	memset(&packet, 0, sizeof(MARestraint));

	packet.nAccountDBID = nAccountDBID;

	AddSendData(MAGA_RESTRAINT, 0, reinterpret_cast<char*>(&packet), sizeof(packet));	
}

void CDNGameConnection::SendPvPGameMode( const CDNPvP* pPvPRoom )
{
	MAGAPVP_GAMEMODE TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iGameRoomID			= pPvPRoom->GetGameServerRoomIndex();
	TxPacket.uiPvPIndex				= pPvPRoom->GetIndex();
	TxPacket.uiPvPGameModeTableID	= pPvPRoom->GetGameModeTableID();
	TxPacket.uiWinCondition			= pPvPRoom->GetGameModeWinCondition();
	TxPacket.uiPlayTimeSec			= pPvPRoom->GetGameModePlayTimeSec();
	TxPacket.unGameModeCheck		= pPvPRoom->GetGameModeCheck();
	TxPacket.biSNMain				= pPvPRoom->GetSNMain();
	TxPacket.iSNSub					= pPvPRoom->GetSNSub();
	TxPacket.cIsPWRoom				= pPvPRoom->bIsPWRoom() ? 1 : 0;
	TxPacket.unRoomOptionBit		= pPvPRoom->GetRoomOptionBit();
	TxPacket.cMaxUser				= static_cast<BYTE>(pPvPRoom->GetMaxUser());
	TxPacket.nEventID				= pPvPRoom->GetEventRoomIndex();
	TxPacket.bIsGuildWar = pPvPRoom->GetIsGuildWarSystem();
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	TxPacket.cPvPChannelType = static_cast<char>(pPvPRoom->GetRoomChannelType());
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	AddSendData( MAGA_PVP_GAMEMODE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}
#if defined(PRE_ADD_PVP_TOURNAMENT)
void CDNGameConnection::SendPvPTournamentUserInfo( CDNPvP* pPvPRoom )
{
	MAGAPVP_TOURNAMENT_INFO TxPacket;
	memset(&TxPacket, 0, sizeof(TxPacket));

	TxPacket.nGameRoomID			= pPvPRoom->GetGameServerRoomIndex();

	for( int i=0; i<PvPCommon::Common::PvPTournamentUserMax; ++i)
	{
		UINT uiAccountDBID = pPvPRoom->GetPvPTournamentIndex(i);
		if( uiAccountDBID  == 0)
			break;
		CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(uiAccountDBID);
		if( pUser )
		{
			TxPacket.sTournamentUserInfo[TxPacket.nCount].uiAccountDBID = uiAccountDBID;
			TxPacket.sTournamentUserInfo[TxPacket.nCount].biCharacterDBID = pUser->GetCharacterDBID();
			TxPacket.sTournamentUserInfo[TxPacket.nCount].cJob = pUser->GetUserJob();			
			TxPacket.sTournamentUserInfo[TxPacket.nCount].uiSessionID = pUser->GetSessionID();
			_wcscpy( TxPacket.sTournamentUserInfo[TxPacket.nCount].wszCharName, NAMELENMAX, pUser->GetCharacterName(), (int)wcslen(pUser->GetCharacterName()) );
			++TxPacket.nCount;
		}
	}
	int nLen = sizeof(MAGAPVP_TOURNAMENT_INFO) - sizeof(TxPacket.sTournamentUserInfo) + (sizeof(STournamentUserInfo)*TxPacket.nCount);
	AddSendData( MAGA_PVP_TOURNAMENT_INFO, 0, reinterpret_cast<char*>(&TxPacket), nLen);
}
#endif

void CDNGameConnection::SendPvPLadderGameMode( GAMASetRoomID* pPacket, const TPvPGameModeTable* pGameModeTable )
{
	MAGAPVP_GAMEMODE TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iGameRoomID			= pPacket->iRoomID;
	TxPacket.unGameModeCheck		= PvPCommon::Check::AllCheck;
	TxPacket.bIsLadderRoom			= true;
	TxPacket.uiPvPGameModeTableID	= pGameModeTable->nItemID;

	// 래더는 StartID가 안남아서 EndLog가 이상하게 남음..여기서 셋팅	
	SYSTEMTIME st;
	GetLocalTime( &st );
	char szSN[MAX_PATH];
	sprintf( szSN, "%.4d%.2d%.2d%.2d%.2d%.2d%.3d%", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
	TxPacket.biSNMain = _atoi64( szSN );

	sprintf( szSN, "%.2d%.2d%.4d", g_Config.nWorldSetID, pPacket->cVillageID, pPacket->iRoomID );
	TxPacket.iSNSub = atoi( szSN );

	TxPacket.uiWinCondition			= pGameModeTable->vWinCondition[0];
	TxPacket.uiPlayTimeSec			= pGameModeTable->vPlayTimeSec[0];
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	TxPacket.cPvPChannelType = static_cast<char>(PvPCommon::RoomType::max);
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	AddSendData( MAGA_PVP_GAMEMODE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendLadderObserver( int iGameRoomID, CDNUser* pUser )
{
	MAGABreakIntoRoom TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iGameRoomID			= iGameRoomID;
	TxPacket.cWorldSetID			= g_Config.nWorldSetID;
	TxPacket.cVillageID				= pUser->GetVillageID();
	TxPacket.uiAccountDBID			= pUser->GetAccountDBID();
	TxPacket.uiSessionID			= pUser->GetSessionID();
	TxPacket.biCharacterDBID		= pUser->GetCharacterDBID();
	TxPacket.uiTeam					= PvPCommon::Team::Observer;
	TxPacket.bAdult					= pUser->IsAdult();
	TxPacket.cPCBangGrade			= pUser->GetPCBangGrade();
#if defined(PRE_ADD_MULTILANGUAGE)
	TxPacket.cSelectedLanguage = pUser->m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#if defined(_ID)
	_strcpy(TxPacket.szMacAddress, _countof(TxPacket.szMacAddress), pUser->m_szMacAddress, (int)strlen(pUser->m_szMacAddress));
	_strcpy(TxPacket.szKey, _countof(TxPacket.szKey), pUser->m_szKey, (int)strlen(pUser->m_szKey));	
#endif
	_wcscpy( TxPacket.wszAccountName, IDLENMAX, pUser->GetAccountName(), (int)wcslen(pUser->GetAccountName()) );
	STRCPYW(TxPacket.wszCharacterName, NAMELENMAX, pUser->GetCharacterName());

	AddSendData( MAGA_BREAKINTOROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendPvPMemberGrade(UINT nAccountDBID, USHORT nTeam, UINT uiUserState, UINT nChangedSessionID, int nRetCode)
{
	MAGAPvPMemberGrade packet;
	memset(&packet, 0, sizeof(MAGAPvPMemberGrade));

	packet.nAccountDBID = nAccountDBID;
	packet.nTeam = nTeam;
	packet.uiUserState = uiUserState;
	packet.nChangedSessionID = nChangedSessionID;
	packet.nRetCode = nRetCode;

	AddSendData(MAGA_PVP_MEMBERGRADE, 0, reinterpret_cast<char*>(&packet), sizeof(packet));
}

void CDNGameConnection::SendPvPMemberIndex(USHORT nTeam, const GAMAPVPSwapMemberIndex * pIndex, int nRetCode)
{
	MAGAPvPMemberIndex packet;
	memset(&packet, 0, sizeof(MAGAPvPMemberIndex));
	
	packet.nAccountDBID = pIndex->nAccountDBID;
	packet.nRetCode = nRetCode;
	packet.nTeam = nTeam;
	packet.cCount = pIndex->cCount;
	memcpy(packet.Index, pIndex->Index, sizeof(packet.Index));
	
	AddSendData(MAGA_PVP_MEMBERINDEX, 0, reinterpret_cast<char*>(&packet), sizeof(packet) - sizeof(packet.Index) + (sizeof(TSwapMemberIndex) * packet.cCount));
}

void CDNGameConnection::SendPvPChangeUserState( const UINT nRoomID, const UINT uiAccountDBID, const UINT uiUserState )
{
	MAGAPVPChangeUserState packet;
	memset(&packet, 0, sizeof(MAGAPVPChangeUserState));

	packet.nPvPRoomID = nRoomID;
	packet.nAccountDBID = uiAccountDBID;
	packet.uiUserState = uiUserState;

	AddSendData(MAGA_PVP_MEMBERUSERSTATE, 0, reinterpret_cast<char*>(&packet), sizeof(packet));
}

void CDNGameConnection::SendBreakintoRoom( int iGameRoomID, CDNUser* pJoinUser, BreakInto::Type::eCode BreakIntoType, char cTeamSlotIndex /*= -1*/ )
{
	MAGABreakIntoRoom TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iGameRoomID			= iGameRoomID;
	TxPacket.cWorldSetID			= g_Config.nWorldSetID;
	TxPacket.cVillageID				= pJoinUser->GetVillageID();
	TxPacket.uiAccountDBID			= pJoinUser->GetAccountDBID();
	TxPacket.uiSessionID			= pJoinUser->GetSessionID();
	TxPacket.biCharacterDBID		= pJoinUser->GetCharacterDBID();
	TxPacket.uiTeam					= pJoinUser->GetPvPTeam();
	TxPacket.bAdult					= pJoinUser->IsAdult();
	TxPacket.cPCBangGrade			= pJoinUser->GetPCBangGrade();
	TxPacket.BreakIntoType			= BreakIntoType;
	TxPacket.bPvPFatigue			= pJoinUser->GetPvPFatiue();
	TxPacket.uiPvPUserState = pJoinUser->GetPvPUserState();
	TxPacket.cPvPTeamIndex = cTeamSlotIndex;
#if defined(_ID)
	_strcpy(TxPacket.szMacAddress, _countof(TxPacket.szMacAddress), pJoinUser->m_szMacAddress, (int)strlen(pJoinUser->m_szMacAddress));
	_strcpy(TxPacket.szKey, _countof(TxPacket.szKey), pJoinUser->m_szKey, (int)strlen(pJoinUser->m_szKey));	
#endif

	_wcscpy( TxPacket.wszAccountName, IDLENMAX, pJoinUser->GetAccountName(), (int)wcslen(pJoinUser->GetAccountName()) );
	STRCPYW(TxPacket.wszCharacterName, NAMELENMAX, pJoinUser->GetCharacterName());

	AddSendData( MAGA_BREAKINTOROOM, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendPopulationZero(bool bZero)
{
	MAGAZeroPopulation packet;
	memset(&packet, 0, sizeof(packet));

	packet.bZeroPopulation = bZero;

	AddSendData(MAGA_ZEROPOPULATION, 0, (char*)&packet, sizeof(packet));
}

void CDNGameConnection::SendDetachUser(UINT nAccountDBID, bool bIsDuplicate, UINT nSessionID)
{
	MADetachUser Detach;
	memset(&Detach, 0, sizeof(MADetachUser));

	Detach.nAccountDBID = nAccountDBID;
	Detach.bIsDuplicate = bIsDuplicate;
	Detach.nSessionID = nSessionID;

	AddSendData(MAGA_DETACHUSER, 0, (char*)&Detach, sizeof(MADetachUser));
}

#if defined(_CH)
void CDNGameConnection::SendFCMState(UINT nAccountDBID, int nOnline, bool bSend)
{
	MAFCMState State = { 0, };
	
	State.nAccountDBID = nAccountDBID;
	State.nOnlineMin = nOnline;
	State.bSend = bSend;

	AddSendData(MAGA_FCMSTATE, 0, (char*)&State, sizeof(MAFCMState));
	g_Log.Log(LogType::_NORMAL, L"[ADBID:%u] MAGA_FCMSTATE Online:%d\r\n", nAccountDBID, nOnline);
}
#endif	// _CH

void CDNGameConnection::SendNotifyMail(UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail)
{
	MANotifyMail Mail = { 0, };
	Mail.nToAccountDBID = nToAccountDBID;
	Mail.biToCharacterDBID = biToCharacterDBID;
	Mail.wTotalMailCount = wTotalMailCount;
	Mail.wNotReadMailCount = wNotReadMailCount;
	Mail.w7DaysLeftCount = w7DaysLeftCount;
	Mail.bNewMail = bNewMail;
	AddSendData(MAGA_NOTIFYMAIL, 0, (char*)&Mail, sizeof(MANotifyMail));
}

void CDNGameConnection::SendNotifyMarket(UINT nSellerAccountDBID, INT64 biSellerCharacterDBID, int nItemID, short wCalculationCount)
{
	MANotifyMarket Market = { 0, };
	Market.nSellerAccountDBID = nSellerAccountDBID;
	Market.biSellerCharacterDBID = biSellerCharacterDBID;
	Market.nItemID = nItemID;
	Market.wCalculationCount = wCalculationCount;
	AddSendData(MAGA_NOTIFYMARKET, 0, (char*)&Market, sizeof(MANotifyMarket));
}

void CDNGameConnection::SendNotifyGift(UINT nToAccountDBID, INT64 biToCharacterDBID, bool bNew, int nGiftCount)
{
	MANotifyGift Gift = { 0, };
	Gift.nToAccountDBID = nToAccountDBID;
	Gift.biToCharacterDBID = biToCharacterDBID;
	Gift.bNew = bNew;
	Gift.nGiftCount = nGiftCount;
	AddSendData(MAGA_NOTIFYGIFT, 0, (char*)&Gift, sizeof(MANotifyGift));
}

void CDNGameConnection::SendDismissGuild(MADismissGuild* pPacket)
{
	AddSendData(MAGA_DISMISSGUILD, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendAddGuildMember(MAAddGuildMember* pPacket)
{
	AddSendData(MAGA_ADDGUILDMEMBER, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendDelGuildMember(MADelGuildMember* pPacket)
{
	AddSendData(MAGA_DELGUILDMEMBER, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendChangeGuildInfo(MAChangeGuildInfo* pPacket)
{
	AddSendData(MAGA_CHANGEGUILDINFO, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendChangeGuildMemberInfo(MAChangeGuildMemberInfo* pPacket)
{
	AddSendData(MAGA_CHANGEGUILDMEMBERINFO, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendGuildChat(MAGuildChat* pPacket)
{
	AddSendData(MAGA_GUILDCHAT, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
void CDNGameConnection::SendDoorsGuildChat(MADoorsGuildChat* pPacket)
{
	AddSendData(MAGA_GUILDCHAT_MOBILE, 0, reinterpret_cast<char*>(pPacket), sizeof(MADoorsGuildChat) - sizeof(pPacket->wszChatMsg) + (sizeof(WCHAR) * pPacket->nLen));
}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

void CDNGameConnection::SendGuildMemberLevelUp(MAGuildMemberLevelUp* pPacket)
{
	AddSendData(MAGA_GUILDMEMBER_LEVELUP, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendChangeGuildName(MAGuildChangeName* pPacket)
{
	AddSendData(MAGA_CHANGEGUILDNAME, 0, reinterpret_cast<char*>(pPacket), sizeof(MAGuildChangeName));
}

void CDNGameConnection::SendChangeGuildMark(MAGuildChangeMark* pPacket)
{
	AddSendData(MAGA_CHANGEGUILDMARK, 0, reinterpret_cast<char*>(pPacket), sizeof(MAGuildChangeMark));
}

void CDNGameConnection::SendUpdateGuildExp(MAUpdateGuildExp* pPacket)
{
	AddSendData(MAGA_UPDATEGUILDEXP, 0, reinterpret_cast<char*>(pPacket), sizeof(MAUpdateGuildExp));
}

void CDNGameConnection::SendEnrollGuildWar(MAEnrollGuildWar* pPacket)
{
	AddSendData(MAGA_ENROLLGUILDWAR, 0, reinterpret_cast<char*>(pPacket), sizeof(MAEnrollGuildWar));
}
void CDNGameConnection::SendChangeGuildWarStep(MAChangeGuildWarStep* pPacket)
{
	AddSendData(MAGA_CHANGE_GUILDWAR_STEP, 0, reinterpret_cast<char*>(pPacket), sizeof(MAChangeGuildWarStep));
}
void CDNGameConnection::SendSetGuildWarPoint(MASetGuildWarPoint* pPacket)
{
	AddSendData(MAGA_SETGUILDWAR_POINT, 0, reinterpret_cast<char*>(pPacket), sizeof(MASetGuildWarPoint));	
}
void CDNGameConnection::SendAddGuildWarPoint(MAAddGuildWarPoint* pPacket)
{
	AddSendData(MAGA_ADDGUILDWAR_POINT, 0, reinterpret_cast<char*>(pPacket), sizeof(MAAddGuildWarPoint));
}

void CDNGameConnection::SendSetGuildWarSecretMission(MASetGuildWarSecretMission * pPacket)
{
	AddSendData(MAGA_SETGUILDWAR_SECRETMISSION, 0, reinterpret_cast<char*>(pPacket), sizeof(MASetGuildWarSecretMission));
}

void CDNGameConnection::SendSetGuildwarFinalProcess(char cGuildFinalPart, __time64_t tBeginTime)
{
	MASetGuildWarFinalProcess SetGuildWarFinalProcess;
	memset(&SetGuildWarFinalProcess, 0, sizeof(MASetGuildWarFinalProcess));
	SetGuildWarFinalProcess.cGuildFinalPart = cGuildFinalPart;
	SetGuildWarFinalProcess.tBeginTime = tBeginTime;

	AddSendData(MAGA_SETGUILDWAR_FINALPROCESS, 0, reinterpret_cast<char*>(&SetGuildWarFinalProcess), sizeof(MASetGuildWarFinalProcess));
}

void CDNGameConnection::SendSetGuildWarPreWinGuild(MAGuildWarPreWinGuild* pPacket)
{
	AddSendData(MAGA_SETGUILDWAR_PRE_WIN_GUILD, 0, reinterpret_cast<char*>(pPacket), sizeof(MAGuildWarPreWinGuild));
}
void CDNGameConnection::SendSetGuildWarTournamentWin(MASetGuildWarTournamentWin* pPacket)
{
	AddSendData(MAGA_SETGUILDWAR_TOURNAMENTWIN, 0, reinterpret_cast<char*>(pPacket), sizeof(MASetGuildWarTournamentWin));
}
void CDNGameConnection::SendSetGuildWarSchedule(MASetGuildWarEventTime* pPacket)
{
	AddSendData(MAGA_SETGUILDWAR_EVENT_TIME, 0, reinterpret_cast<char*>(pPacket), sizeof(MASetGuildWarEventTime));
}

void CDNGameConnection::SendGuildRecruitMember(MAGuildRecruitMember* pPacket)
{
	AddSendData(MAGA_GUILDRECRUIT_MEMBER, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}
void CDNGameConnection::SendAddGuildRewardItem(MAGuildRewardItem* pPacket)
{
	AddSendData(MAGA_GUILD_ADD_REWARDITEM, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}
void CDNGameConnection::SendExtendGuildSize(MAExtendGuildSize* pPacket)
{
	AddSendData(MAGA_GUILD_EXTEND_GUILDSIZE, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}

void CDNGameConnection::SendUpdateGuildWare(int nGuildID)
{
	MAUpdateGuildWare packet;
	memset(&packet, 0, sizeof(MAUpdateGuildWare));

	packet.GUildID.Set(g_Config.nWorldSetID, nGuildID);

	AddSendData(MAGI_UPDATEGUILDWARE, 0, reinterpret_cast<char*>(&packet), sizeof(MAUpdateGuildWare));
}

void CDNGameConnection::SendChangeCharacterName(MAChangeCharacterName* pPacket)
{
	AddSendData(MAGA_CHANGE_CHARACTERNAME, 0, reinterpret_cast<char*>(pPacket), sizeof(MAChangeCharacterName));
}

void CDNGameConnection::SendMasterSystemSyncSimpleInfo( UINT uiAccountDBID, INT64 biCharacterDBID, MasterSystem::EventType::eCode EventCode )
{
	MasterSystem::MAGASyncSimpleInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.EventCode			= EventCode;

	AddSendData( MAGA_MASTERSYSTEM_SYNC_SIMPLEINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendMasterSystemSyncJoin( UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsAddPupil )
{
	MasterSystem::MAGASyncJoin TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.bIsAddPupil		= bIsAddPupil;

	AddSendData( MAGA_MASTERSYSTEM_SYNC_JOIN, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendMasterSystemSyncLeave( UINT uiAccountDBID, INT64 biCharacterDBID, bool bIsDelPupil )
{
	MasterSystem::MAGASyncLeave TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.bIsDelPupil		= bIsDelPupil;

	AddSendData( MAGA_MASTERSYSTEM_SYNC_LEAVE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendMasterSystemSyncGraduate( UINT uiAccountDBID, MasterSystem::VIMASyncGraduate* pPacket )
{
	MasterSystem::MAGASyncGraduate TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID = uiAccountDBID;
	_wcscpy( TxPacket.wszCharName, NAMELENMAX, pPacket->wszCharName, (int)wcslen(pPacket->wszCharName) );

	AddSendData( MAGA_MASTERSYSTEM_SYNC_GRADUATE, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendMasterSystemSyncConnect( UINT uiAccountDBID, bool bIsConnect, WCHAR* pwszCharName )
{
	MasterSystem::MAGASyncConnect TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.bIsConnect		= bIsConnect;
	_wcscpy( TxPacket.wszCharName, NAMELENMAX, pwszCharName, (int)wcslen(pwszCharName) );

	AddSendData( MAGA_MASTERSYSTEM_SYNC_CONNECT, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendFarmSync( UINT uiAccountDBID, INT64 biCharacterDBID, Farm::ServerSyncType::eType Type )
{
	MAGAFarmSync TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.Type				= Type;

	AddSendData( MAGA_FARM_SYNC, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendFarmSyncAddWater( UINT uiAccountDBID, WCHAR* pwszCharName, int iAddWaterPoint )
{
	MAGAFarmSyncAddWater TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID	= uiAccountDBID;
	TxPacket.iAddPoint		= iAddWaterPoint;
	_wcscpy( TxPacket.wszCharName, NAMELENMAX, pwszCharName, (int)wcslen(pwszCharName) );

	AddSendData( MAGA_FARM_SYNC_ADDWATER, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}



#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
void CDNGameConnection::SendSyncSystemMail( UINT uiAccountDBID, VIMASyncSystemMail* pMail )
{
	MAGASyncSystemMail TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID			= uiAccountDBID;
	TxPacket.nMailDBID				= pMail->nMailDBID;
	TxPacket.iTotalMailCount		= pMail->iTotalMailCount;
	TxPacket.iNotReadMailCount		= pMail->iNotReadMailCount;
	TxPacket.i7DaysLeftMailCount	= pMail->i7DaysLeftMailCount;

	AddSendData( MAGA_SYNC_SYSTEMMAIL, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

int CDNGameConnection::_ProcessRebirthVillage( CDNUser* pUser, const int iLastVillageMapIndex )
{
	char	szIp[IPLENMAX]	= { 0, };
	USHORT	nPort			= 0;
	int		nChannel		= 0;
	BYTE	cVillageID		= 0;

	if( g_pDivisionManager )
	{
		if( g_pDivisionManager->GetVillageInfo( iLastVillageMapIndex, pUser->GetChannelID(), nChannel, cVillageID, szIp, nPort ) )
		{
			if( g_pDivisionManager->GameToVillage( this, pUser->GetAccountDBID(), cVillageID, nChannel) )
				SendRebirthVillageInfo( pUser->GetAccountDBID(), cVillageID, pUser->GetChannelID(), szIp, nPort, ERROR_NONE );
			else
				SendRebirthVillageInfo( pUser->GetAccountDBID(), cVillageID, pUser->GetChannelID(), szIp, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND );

			return ERROR_NONE;
		}
	}

	SendRebirthVillageInfo( pUser->GetAccountDBID(), 0, 0, szIp, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND );
	return ERROR_GENERIC_UNKNOWNERROR;
}

void CDNGameConnection::SendPCBangResult(UINT nAccountDBID, CDNUser *pUser)
{
	MAPCBangResult Result;
	memset(&Result, 0, sizeof(MAPCBangResult));

	Result.nAccountDBID = nAccountDBID;
	if (pUser){
#if defined(_KR)
		Result.cResult = pUser->m_cPCBangResult;
		Result.cAuthorizeType = pUser->m_cPCBangAuthorizeType;
		Result.cOption = pUser->m_cPCBangOption;
		Result.nArgument = pUser->m_nPCBangArgument;
		Result.bShutdowned = pUser->m_bShutdowned;
		Result.cPolicyError = pUser->m_cPolicyError;
		Result.nShutdownTime = pUser->m_nShutdownTime;
#else	// #if defined(_KR)

#if defined(_ID)
		Result.bBlockPcCafe = pUser->m_bBlockPcCafe;
#endif // #if defined(_ID)
		Result.cPCBangGrade = pUser->GetPCBangGrade();
#if defined(_KRAZ)
		Result.ShutdownData = pUser->m_ShutdownData;
#endif	// #if defined(_KRAZ)

#endif	// #if defined(_KR)
	}

	AddSendData(MAGA_PCBANGRESULT, 0, (char*)&Result, sizeof(MAPCBangResult));
}

#if defined(PRE_ADD_REMOTE_QUEST)
void CDNGameConnection::SendUserTempDataResult(UINT uiAccountDBID, CDNUser *pUser)
{
	if(!pUser) return;
	MAGAUserTempDataResult packet;
	memset(&packet, 0, sizeof(packet));

	packet.uiAccountDBID = uiAccountDBID;
	packet.nDungeonClearCount = pUser->GetDungeonClearCount();
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	packet.eUserGameQuitRewardType = pUser->GetUserGameQuitRewardType();
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(PRE_ADD_REMOTE_QUEST)
	pUser->GetAcceptWaitRemoteQuest(&packet.nAcceptWaitRemoteQuestCount, packet.AcceptWaitRemoteQuestList);
#endif

	AddSendData(MAGA_USERTEMPDATA_RESULT, 0, (char*)&packet, sizeof(MAGAUserTempDataResult) - sizeof(packet.AcceptWaitRemoteQuestList) + sizeof(int) * packet.nAcceptWaitRemoteQuestCount );
}
#else
#if defined(PRE_ADD_GAMEQUIT_REWARD)
void CDNGameConnection::SendUserTempDataResult(UINT uiAccountDBID, int nDungeonClearCount, GameQuitReward::RewardType::eType eUserGameQuitRewardType)
#else	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
void CDNGameConnection::SendUserTempDataResult(UINT uiAccountDBID, int nDungeonClearCount)
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
{
	MAGAUserTempDataResult packet;
	memset(&packet, 0, sizeof(packet));

	packet.uiAccountDBID = uiAccountDBID;
	packet.nDungeonClearCount = nDungeonClearCount;
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	packet.eUserGameQuitRewardType = eUserGameQuitRewardType;
#endif
	AddSendData(MAGA_USERTEMPDATA_RESULT, 0, (char*)&packet, sizeof(MAGAUserTempDataResult));
}
#endif	//#if defined(PRE_ADD_REMOTE_QUEST)

void CDNGameConnection::SendCheckLastDungeonInfo( char cWorldSetID, const VIMACheckLastDungeonInfo* pPacket )
{
	MAGACheckLastDungeonInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.cWorldSetID		= cWorldSetID;
	TxPacket.uiAccountDBID		= pPacket->uiAccountDBID;
	TxPacket.biCharacterDBID	= pPacket->biCharacterDBID;
	TxPacket.iRoomID			= pPacket->iRoomID;

	AddSendData( MAGA_CHECK_LASTDUNGEONINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CDNGameConnection::SendDeleteBackupDungeonInfo( UINT uiAccountDBID, INT64 biCharacterDBID, int iRoomID )
{
	MAGADeleteBackupDungeonInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiAccountDBID		= uiAccountDBID;
	TxPacket.biCharacterDBID	= biCharacterDBID;
	TxPacket.iRoomID			= iRoomID;

	AddSendData( MAGA_DELETE_BACKUPDUNGEONINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

#if defined( PRE_WORLDCOMBINE_PARTY )
void CDNGameConnection::SendGetWorldPartyMember( MAGetWorldPartyMember *Packet)
{	
	AddSendData(MAGA_GET_WORLDPARTYMEMBER, 0, reinterpret_cast<char*>(Packet), sizeof(MAGetWorldPartyMember));
}
#endif

#if defined( PRE_ADD_BESTFRIEND )
void CDNGameConnection::SendCancelBestFriend(MACancelBestFriend* pPacket)
{
	AddSendData(MAGA_CANCEL_BESTFRIEND, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}
void CDNGameConnection::SendCloseBestFriend(MACloseBestFriend* pPacket)
{
	AddSendData(MAGA_CLOSE_BESTFRIEND, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}
void CDNGameConnection::SendLevelBestFriend(MALevelUpBestFriend* pPacket)
{
	AddSendData(MAGA_LEVELUP_BESTFRIEND, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNGameConnection::SendPrivateChatChannelAdd(MAAddPrivateChannel* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_ADD, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelMemberAdd(MAAddPrivateChannelMember* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MEMBERADD, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelMemberInvite(MAInvitePrivateChannelMember* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MEMBERINVITE, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelMemberInviteResult(MAInvitePrivateChannelMemberResult* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MEMBERINVITERESULT, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelMemberDel(MADelPrivateChannelMember* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MEMBERDEL, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelMemberKickResult(MAKickPrivateChannelMemberResult* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MEMBERKICKRESULT, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelMod(MAModPrivateChannel* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MOD, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendPrivateChatChannelModMemberName(MAModPrivateChannelMemberName* pPacket)
{
	AddSendData(MAGA_PRIVATECHATCHANNEL_MODMEMBERNAME, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}
#endif

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
void CDNGameConnection::SendChangeRoomMaster( UINT uiRoomID, INT64 biRoomMasterCharacterDBID, UINT uiRoomMasterSessionID )
{
	MAGAChangeRoomMaster Packet;
	memset(&Packet, 0, sizeof(MAGAChangeRoomMaster));

	Packet.uiRoomID						= uiRoomID;
	Packet.biRoomMasterCharacterDBID	= biRoomMasterCharacterDBID;
	Packet.uiRoomMasterSessionID		= uiRoomMasterSessionID;

	AddSendData(MAGA_PVP_CHANGEROOMMASTER, 0, reinterpret_cast<char*>(&Packet), sizeof(Packet));
}
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
#if defined(PRE_ADD_MUTE_USERCHATTING)
void CDNGameConnection::SendMuteUserChatting(MAMuteUserChat* pPacket)
{
	AddSendData(MAGA_MUTE_USERCHAT, 0, reinterpret_cast<char*>(pPacket), sizeof(*pPacket));
}
#endif	// #if defined(PRE_ADD_MUTE_USERCHATTING)

#if defined( PRE_ALTEIAWORLD_EXPLORE )
void CDNGameConnection::SendAlteiaWorldSendTicket( MAAlteiaWorldSendTicket *pPacket )
{
	AddSendData(MAGA_ALTEIAWORLD_SENDTICKET, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}
void CDNGameConnection::SendAlteiaWorldSendTicketResult( MAAlteiaWorldSendTicketResult *pPacket )
{
	AddSendData(MAGA_ALTEIAWORLD_SENDTICKET_RESULT, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}
#endif

#if defined(PRE_ADD_CHNC2C)
void CDNGameConnection::SendC2CAddCoin( UINT uiAccountDBID, INT64 biCharacterDBID, INT64 biAddCoin, const char* szSeqID, const char* szBookID )
{
	MAC2CAddCoin packet = {0,};
	packet.uiAccountDBID = uiAccountDBID;
	packet.biCharacterDBID = biCharacterDBID;
	packet.biAddCoin = biAddCoin;
	memcpy(packet.szBookID, szBookID, sizeof(packet.szBookID));
	memcpy(packet.szSeqID, szSeqID, sizeof(packet.szSeqID));

	AddSendData(MAGA_C2C_CHAR_ADDCOIN, 0, (char*)&packet, sizeof(packet));	
}

void CDNGameConnection::SendC2CReduceCoin( UINT uiAccountDBID, INT64 biCharacterDBID, INT64 biReduceCoin, const char* szSeqID, const char* szBookID )
{
	MAC2CReduceCoin packet = {0,};
	packet.uiAccountDBID = uiAccountDBID;
	packet.biCharacterDBID = biCharacterDBID;
	packet.biReduceCoin = biReduceCoin;
	memcpy(packet.szBookID, szBookID, sizeof(packet.szBookID));
	memcpy(packet.szSeqID, szSeqID, sizeof(packet.szSeqID));

	AddSendData(MAGA_C2C_CHAR_REDUCECOIN, 0, (char*)&packet, sizeof(packet));	
}
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined(PRE_ADD_DWC)
void CDNGameConnection::SendAddDWCTeamMember(MAAddDWCTeamMember* pPacket)
{
	AddSendData(MAGA_ADD_DWCTEAMMEMBER, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendLeaveDWCTeamMember(MALeaveDWCTeamMember* pPacket)
{
	AddSendData(MAGA_DEL_DWCTEAMMEMB, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendDismissDWCTeam(MADismissDWCTeam* pPacket)
{
	AddSendData(MAGA_DISMISS_DWCTEAM, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendChangeDWCTeamMemberState(MAChangeDWCTeamMemberState* pPacket)
{
	AddSendData(MAGA_CHANGE_DWCTEAMMEMB_STATE, 0, reinterpret_cast<char*>(pPacket), sizeof((*pPacket)));
}

void CDNGameConnection::SendDWCTeamChat(char cType, UINT nToAccountDBID, const WCHAR *pwszFromCharacterName, const WCHAR *pwszChatMsg, short wChatLen)
{
	MAChat Chat = { 0, };

	Chat.nAccountDBID = nToAccountDBID;
	Chat.cType = cType;
	Chat.wChatLen = wChatLen;
	if (pwszFromCharacterName) 	
		_wcscpy(Chat.wszFromCharacterName, NAMELENMAX, pwszFromCharacterName, (int)wcslen(pwszFromCharacterName));

	if (pwszChatMsg) 
		_wcscpy(Chat.wszChatMsg, CHATLENMAX, pwszChatMsg, (int)wcslen(pwszChatMsg));

	AddSendData(MAGA_DWC_TEAMCHAT, 0, (char*)&Chat, sizeof(MAChat) - sizeof(Chat.wszChatMsg) + (wChatLen*sizeof(WCHAR)) );
}
#endif