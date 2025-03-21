#include "StdAfx.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNFriend.h"
#include "DNParty.h"
#include "DNPartyManager.h"
#include "DNMasterConnection.h"
#include "DNGameDataManager.h"
#include "DNFieldDataManager.h"
#include "DNField.h"
#include "DNFieldManager.h"
#include "DNNpcObject.h"
#include "DNPropNpcObject.h"
#include "DNNpcObjectManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNLogConnection.h"
#include "DNIocpManager.h"
#include "DNWorldUserState.h"
#include "DNFriend.h"
#include "DNIsolate.h"
#include "DNGuildSystem.h"
#include "DNGuildVillage.h"
#include "Util.h"
#include "Log.h"
#include "DNLogConnection.h"
#include "DNGameDataManager.h"
#include "DNSecure.h"
#include "SpinBuffer.h"
#include "DNPvPRoomManager.h"
#include "DNPvPRoom.h"
#include "DNMissionSystem.h"
#include "DNAppellation.h"
#include "DNMissionScheduler.h"
#include "NoticeSystem.h"
#include "DNAuthManager.h"
#include "DNGesture.h"
#include "EtUIXML.h"
#include "DNRestraint.h"
#include "VarArg.h"
#include "DNDungeonManager.h"
#include "DNCashConnection.h"
#include "CloseSystem.h"
#include "DNCountryUnicodeSet.h"
#if defined( PRE_ADD_VIP_FARM )
#include "DNCashRepository.h"
#endif // #if defined( PRE_ADD_VIP_FARM )
#include "DNChatRoom.h"
#include "DNChatRoomManager.h"
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "ReputationSystemRepository.h"
#include "ReputationSystemEventHandler.h"
#include "NpcReputationProcessor.h"
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "MasterSystemCacheRepository.h"
#if defined( PRE_ADD_SECONDARY_SKILL )
#include "SecondarySkillRepository.h"
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
#include "DNFarm.h"
#include "DNLadderSystemManager.h"
#include "DNLadderRoom.h"
#include "DNLadderRoomRepository.h"
#include "DNTimeEventSystem.h"
#include "DNGuildWarManager.h"
#include "DNMailSender.h"
#include "TradeCacheRepository.h"
#include "DNGuildRecruitCacheRepository.h"
#if defined (PRE_ADD_BESTFRIEND)
#include "DNBestFriend.h"
#endif
#include "ExceptionReport.h"
#ifdef _USE_VOICECHAT
#include "DNVoiceChat.h"
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
#include "DNPrivateChatChannel.h"
#include "DnPrivateChatManager.h"
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
#include "DNCashRepository.h"
#endif
#include "DNCommonVariable.h"
#if defined( PRE_ADD_STAMPSYSTEM )
#include "DNStampSystem.h"
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_DWC)
#include "DNDWCSystem.h"
#include "DNDWCTeam.h"
#endif

extern TVillageConfig g_Config;

CDNUserSession::CDNUserSession()
{
	Init(1024 * 100, 1024 * 300, 1024 * 6);

	m_bPartyInto = false;
	m_eUserState = STATE_NONE;

	m_GameTaskType = GameTaskType::Max;
	m_cReqGameIDType	= REQINFO_TYPE_SINGLE;
	m_nGameServerIP		= 0;
	m_nGameServerPort	= m_nGameServerTcpPort = m_wGameID = 0;

	m_pSkill = new CDNUserSkill(this);

	m_bChangeSameServer = false;
	m_bFirst = true;
	m_bChannelMove = false;

	memset(&m_MoveToVillageInfo, 0, sizeof(TConnectionInfo));

	m_boPartyLeader = false;//m_boPartyInfo = false;
	m_nPartyMemberIndex = m_cGateNo = m_cGateSelect = -1;
	m_PartyID = 0;
	m_nSingleRandomSeed = 0;

	m_nSingleRandomSeed = timeGetTime();//_rand();

	// PvP
	m_dwPvPRoomListRefreshTime	= 0;
	m_cPvPVillageID				= 0;
	m_unPvPVillageChannelID		= 0;
	m_uiPvPIndex		= 0;
	m_usPvPTeam			= PvPCommon::Team::Max;
	m_uiPvPUserState	= PvPCommon::UserState::None;

	m_bIsChannelCounted = false;

	m_dwNpcTalkLastIndexHashCode = 0;
	m_dwNpcTalkLastTargetHashCode = 0;

	m_dwLastEscapeTime = 0;
#ifdef _USE_VOICECHAT
	m_nVoiceRotate = 0;
#endif

	m_nChatRoomID = 0;

	// m_nCosMixInvenTypeCache = 0;
	m_biCosMixSerialCache = 0;
	m_biCosDesignMixSerialCache = 0;

#if !defined( _FINAL_BUILD )
	m_bIsMasterSystemSkipDate = false;
#endif // #if defined( _FINAL_BUILD )
	m_nLastUseItemSkillID = 0;

	m_bIsMove = false;
	m_bIsStartGame = false;
	m_nInvalidSendCount = 0;
	m_bIsLadderUser = false;
	m_nAbuseLogSendCount = 0;

	GuildWarReset();
	m_dwResellItemTick = 0;

	m_bIsCheckLastDungeonInfo = false;

#if defined( PRE_PARTY_DB )
	memset( &m_PrevPartyListInfo, 0, sizeof(m_PrevPartyListInfo) );
	m_dwPrevPartyListInfoTick = 0;
#endif // #if defined( PRE_PARTY_DB )
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_ePvPChannel = PvPCommon::RoomType::max;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	m_bDeletePCRentalItem = false;
#endif //

#ifdef PRE_FIX_VILLAGEZOMBIE
	m_dwCalledSendInsideDisconnectTick = 0;
	m_bZombieChecked = false;
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE
#if defined( PRE_ADD_NEWCOMEBACK )
	m_nComebackAppellation = false;
#endif
#if defined(PRE_ADD_TRANSFORM_POTION)
	m_nTransformID = 0;
	m_dwTransformTick = 0;
#endif

#if defined (PRE_ADD_COSRANDMIX) && defined (PRE_ADD_COSRANDMIX_ACCESSORY)
	m_CosRandMixOpenType = CostumeMix::RandomMix::OpenFail;
#endif
#if defined (PRE_ADD_COSRANDMIX) && defined (PRE_ADD_COSRANDMIX_MIXERTYPE)
	m_nCosRandMixerEnablePartsType = CostumeMix::RandomMix::MixerCanMixPartsAll;
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )	
	m_cTicketCount = 0;
	m_cSendTicketCount = 0;
	m_cAlteiaBestGoldKeyCount = 0;
	m_uiAlteiaBestPlayTime = 0;
	m_cWeeklyPlayCount = 0;
	m_cDailyPlayCount = 0;
	m_AlteiaSendTicketList.clear();
#endif
#if defined(PRE_ADD_DWC)
	m_nDWCTeamID = 0;
#endif
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	m_uiDBCheckSum = 0;
	m_uiRestoreCheckSum = 0;
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
}

CDNUserSession::~CDNUserSession(void)
{
	// 확인사살
	if( g_pFieldManager )
		g_pFieldManager->FinalUser( this );

	if (m_bIsChannelCounted)
	{
		if (GetChannelID() <= 0) _DANGER_POINT();
		g_pUserSessionManager->DecreaseChannelUserCount(GetChannelID());
		m_bIsChannelCounted = false;
	}

	SAFE_DELETE(m_pSkill);
}

void CDNUserSession::InitAccount(MAVICheckUser* pCheckUser)
{
#if defined(_HSHIELD)
#if defined( PRE_ADD_HSHIELD_LOG )
	g_Log.Log(LogType::_HACKSHIELD, m_pSession, L"[_AhnHS_CreateClientObject before - (%d) ] ServerHandle[%x] : 0x%x\r\n", pCheckUser->nSessionID, g_Config.hHSServer, m_hHSClient);
#endif
	m_hHSClient = _AhnHS_CreateClientObject(g_Config.hHSServer);
#if defined( PRE_ADD_HSHIELD_LOG )
	g_Log.Log(LogType::_HACKSHIELD, m_pSession, L"[_AhnHS_CreateClientObject after  - (%d) ] ServerHandle[%x] : 0x%x\r\n", pCheckUser->nSessionID, g_Config.hHSServer, m_hHSClient);
#endif
	if (m_hHSClient == ANTICPX_INVALID_HANDLE_VALUE){
		DetachConnection(L"Connect|_AhnHS_CreateClientObject Failed\r\n");
		return;
	}

	m_dwCheckLiveTick = timeGetTime();
	m_dwHShieldResponseTick = timeGetTime();
#endif	// _HSHIELD

	m_nWorldSetID = g_Config.nWorldSetID;
	SetAccountName(pCheckUser->wszAccountName);
	m_biCharacterDBID = pCheckUser->biCharacterDBID;
	SetChannelID(pCheckUser->nChannelID);
	m_bAdult = pCheckUser->bAdult;
	SetPCBangGrade(pCheckUser->cPCBangGrade);
#if defined(PRE_ADD_MULTILANGUAGE)
	m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(pCheckUser->cSelectedLanguage);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#ifdef PRE_ADD_COMEBACK
	m_bComebackUser = pCheckUser->bComebackUser;
#endif			//#ifdef PRE_ADD_COMEBACK
#if defined( PRE_ADD_GAMEQUIT_REWARD )
	m_bReConnectUserReward = pCheckUser->bReConnectNewbieReward;
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )
#if defined(_ID)
	_strcpy(m_szMacAddress, _countof(m_szMacAddress), pCheckUser->szMacAddress, (int)strlen(pCheckUser->szMacAddress));
	_strcpy(m_szKey, _countof(m_szKey), pCheckUser->szKey, (int)strlen(pCheckUser->szKey));
#endif

#if defined( PRE_FIX_67546 )
	m_pDBCon = g_pDBConnectionManager->GetVillageDBConnection(m_nChannelID, m_cDBThreadID);
#else
	m_pDBCon = g_pDBConnectionManager->GetDBConnection(m_nChannelID, m_cDBThreadID);
#endif
	if (m_pDBCon == NULL)
	{
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [InitAccount] m_pDBCon Fail!!\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
		return;
	}

	//아 요거 특정채널에 몰릴경우 처리가 늦어저서 최초에만 랜덤하게 수정되어진것임~ 10977 revision
	BYTE cThreadID = 0;
	CDNDBConnection *pDBCon = g_pDBConnectionManager->GetDBConnection(cThreadID);
	if (pDBCon == NULL)
	{
		g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [InitAccount] pDBCon Fail!!\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
		return;
	}

	//데이타베이스컨넥션 객체는 있지만 Active플래그가 아직 활성화가 안되어질 경우 클라이언트가 접속중 멍때리게되서 끊어주고 로그박기 추가
	if (pDBCon->GetActive() == false || pDBCon->GetDelete() == true)
	{
		g_Log.Log(LogType::_DBCONNECTIONLOG, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [InitAccount] pDBCon Active Flag Fail!!\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
		DetachConnection(L"InitAccount pDBCon Flag");
		return;
	}
	pDBCon->QueryGetMasterSystemSimpleInfo( cThreadID, this, false, MasterSystem::EventType::Init );

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	pDBCon->QueryGetListNpcReputation( cThreadID, this );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#if defined( PRE_ADD_SECONDARY_SKILL )
	pDBCon->QueryGetListSecondarySkill( cThreadID, this );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	pDBCon->QueryGetListPvPLadderScore( cThreadID, this );
	pDBCon->QueryGetPVPGhoulScores( cThreadID, this );

	pDBCon->QueryGetListEtcPoint( cThreadID, this );
	pDBCon->QueryGetAbuseMonitor( cThreadID, this );
	pDBCon->QueryGetListVariableReset( cThreadID, this );

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	pDBCon->QueryGetTotalSkillLevel(cThreadID, this);
#endif

	//Query Character Data
#if defined( PRE_PARTY_DB )
	pDBCon->QuerySelectCharacter(cThreadID, this);
#else
	pDBCon->QuerySelectCharacter(cThreadID, this, pCheckUser->PartyID);
#endif

#if defined (PRE_ADD_BESTFRIEND)
	pDBCon->QueryGetBestFriend(cThreadID, this, false);
#endif

	//Query Friend Data
	pDBCon->QueryFriendList(cThreadID, this);

	//Query Isolate Data
	pDBCon->QueryGetIsolateList(cThreadID, this);

	//Query GameOption
	pDBCon->QueryGetGameOption(cThreadID, this);

	//Query Profile
	pDBCon->QueryGetProfile(cThreadID, this);
#if defined(_VILLAGESERVER)
	// 길드전 보상 기간이면 예선 보상을 받을 수 있는지 가져온다.
	if( g_pGuildWarManager && g_pGuildWarManager->GetStepIndex() == GUILDWAR_STEP_REWARD )			
	{
		//Query Reward
		pDBCon->QueryGetGuildWarRewarForCharacter(cThreadID, this);
		pDBCon->QueryGetListGuildWarItemTradeRecord(cThreadID, this);
	}
#endif

#if defined( PRE_ADD_LIMITED_SHOP )
	pDBCon->QueryGetLimitedShopItem(cThreadID, this);
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	pDBCon->QueryGetAlteiaWorldInfo(cThreadID, this);
	pDBCon->QueryGetAlteiaWorldSendTicketList(cThreadID, this);
#endif

#if defined( PRE_ADD_STAMPSYSTEM )
	pDBCon->QueryGetListCompleteChallenges(cThreadID, this);
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

	//g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [InitAccount]\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
}

bool CDNUserSession::EnterWorld()
{
	// g_Log.Log(L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] CDNUserSession::EnterWorld\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
	return CDNBaseObject::EnterWorld();
}

bool CDNUserSession::LeaveWorld()
{
	// 거래 중이면 거래 중단
	if (m_nExchangeTargetSessionID > 0){
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSession(m_nExchangeTargetSessionID);
		if (pUserObj){
			pUserObj->SendExchangeCancel();
			pUserObj->ClearExchangeInfo();
		}
		SendExchangeCancel();
		ClearExchangeInfo();
	}
	if (m_nExchangeSenderSID > 0){
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSession(m_nExchangeSenderSID);
		if (pUserObj){
			pUserObj->SendExchangeReject(GetSessionID());
			pUserObj->ClearExchangeInfo();
		}
	}
	if (m_nExchangeReceiverSID > 0){
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSession(m_nExchangeReceiverSID);
		if (pUserObj){
			pUserObj->SendExchangeRequest(GetSessionID(), ERROR_EXCHANGE_SENDERCANCEL);	// 수락, 거절 버튼창 없애기
			pUserObj->ClearExchangeInfo();
		}
	}

	// 채팅방에서 채팅중이라면, 방에서 나감
	if( m_nChatRoomID > 0 )
	{
		CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( m_nChatRoomID );
		if( pChatRoom != NULL )
		{
			if( pChatRoom->IsLeader( GetAccountDBID() ) )
			{
				// 리더가 끊기는 경우 방이 삭제된다.
				while( pChatRoom->GetUserCount() > 0 )
				{
					UINT UserAID = pChatRoom->GetMemberAIDFromIndex( 0 );
					if( UserAID <= 0 )	break;	// 무한루프 방지를 위해 루프를 탈출하도록 한다.

					CDNUserSession * pUserSession = NULL;
					if( UserAID == GetAccountDBID() )
						pUserSession = this;
					else 
						pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( UserAID );
					if( pUserSession == NULL )	break;	// 무한루프 방지를 위해 루프를 탈출하도록 한다.

					int nRet = pChatRoom->LeaveUser( pUserSession->GetAccountDBID(), CHATROOMLEAVE_DESTROYROOM );
					if( nRet == ERROR_NONE )
					{
						// 방에서 나가게 되는 캐릭터와 주변 캐릭터에게 메시지를 전송한다.
						if( UserAID != GetAccountDBID() )	// 본인은 이미 게임을 끊었으므로 메시지를 전달하지 않는다.	
							pUserSession->SendChatRoomLeaveUser( pUserSession->GetSessionID(), CHATROOMLEAVE_DESTROYROOM );
						pUserSession->BroadcastingChatRoom( eChatRoom::SC_LEAVEUSER, -CHATROOMLEAVE_DESTROYROOM );	// 음수 파라미터 전송
					}
				}
			}
			else
			{
				// 본인은 이미 게임을 끊었으므로 메시지를 전달하지 않는다.
				// 주변에 메시지 전달(방인원 포함)
				BroadcastingChatRoom( eChatRoom::SC_LEAVEUSER, -CHATROOMLEAVE_LEAVE );	// 음수 파라미터 전송

				pChatRoom->LeaveUser( GetAccountDBID(), CHATROOMLEAVE_LEAVE );
			}

			// 인원이 없으면 방을 삭제한다.
			if( pChatRoom->GetUserCount() <= 0 )
			{
				g_pChatRoomManager->DestroyChatRoom( pChatRoom->GetChatRoomID() );
			}
		}
	}

	return CDNBaseObject::LeaveWorld();
}

bool CDNUserSession::InitObject(WCHAR *pName, UINT nUID, int nChannelID, int nMapIndex, TPosition *pCurPos)
{
	return CDNBaseObject::InitObject(pName, nUID, nChannelID, nMapIndex, pCurPos);
}

bool CDNUserSession::FinalObject()
{
#ifdef PRE_FIX_VILLAGEZOMBIE
	if (m_bZombieChecked)
	{
		//좀비체크로 지워질경우 해당 유저가 어디까지 진행을 했는지 전혀 모른다 다른데이터는 건드리지 말고 유저객체만 정리해준다.
		return CDNBaseObject::FinalObject();
	}
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE

	if (GetPartyID() > 0)
	{
		CDNParty * pParty = g_pPartyManager->GetParty(GetPartyID());
		if (pParty)
		{
#if defined( PRE_PARTY_DB )
			g_pPartyManager->QueryOutParty( GetPartyID(), GetAccountDBID(), GetCharacterDBID(), GetSessionID(), Party::QueryOutPartyType::Disconnect );
#else
			g_pPartyManager->DelPartyMember( pParty, this );
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_ADD_NEWCOMEBACK )
			if( GetComebackAppellation() > 0 )
			{
				pParty->DelPartyMemberAppellation(GetComebackAppellation());
			}
#endif // #if defined( PRE_ADD_NEWCOMEBACK )
		}
	}

	if ((m_eUserState != STATE_MOVETOVILLAGE) && (m_eUserState != STATE_MOVETOGAME)){ 
		if (m_bLoadUserData){
			SaveUserData();
			LastUpdateUserData();	// 저장 날려주자
		}
	}

	if ((m_eUserState != STATE_MOVETOVILLAGE) && (m_eUserState != STATE_MOVETOGAME) && (m_eUserState != STATE_MOVETOLOGIN)){ 
		if( m_bCertified ) // 인증을 통과한 애만 마스터한테 지우라고 하자..		
			g_pMasterConnection->SendDelUser(m_nAccountDBID, CConnection::m_nSessionID);
	}

	if (m_eUserState == STATE_MOVETOLOGIN)
	{
		//로그인으로 이동이라면 마스터의 유저데이타를 정리한다
		g_pMasterConnection->SendCheckReconnectLogin(m_nAccountDBID);
	}

	if( m_nAccountDBID > 0 )
		g_pCashConnection->SendDelUser(m_nAccountDBID);	// 걍 나갈때 무조건 날려준다.

	if ((m_eUserState != STATE_MOVETOVILLAGE) && (m_eUserState != STATE_MOVETOGAME) && (m_eUserState != STATE_MOVETOLOGIN)) 
	{
		if( m_pDBCon && m_nAccountDBID > 0)
		{
			m_pDBCon->QueryLogout(this, m_szMID);
		}

		if (m_bCertified) {		// 주의 !!! - 반드시 QUERY_CHECKAUTH 를 통과하여 인증을 받은 사용자에 한해서만 인증정보 리셋이 수행되어야 함 !!!
			// 리스트에 넣지말고 즉시 처리하자
			g_pAuthManager->QueryResetAuth(m_nWorldSetID, m_nAccountDBID, GetSessionID());
		}
	}

	if( m_eUserState != STATE_MOVETOVILLAGE && m_eUserState != STATE_MOVETOGAME )
	{
		if( m_pDBCon ){
			m_pDBCon->QueryLogoutCharacter( this );
#if defined(_KRAZ)
			m_pDBCon->QueryActozUpdateCharacterInfo(this, ActozCommon::UpdateType::Logout);
#endif	// #if defined(_KRAZ)
		}
		if( g_pMasterConnection && g_pMasterConnection->GetActive() )
			g_pMasterConnection->SendMasterSystemSyncConnect( false, GetCharacterName(), GetMasterSystemData() );
	}

	m_bCertified = false;

	if (m_bNeedUpdateOption && m_pDBCon)
	{
		m_pDBCon->QuerySetGameOption(this, &m_GameOption);
	}

	if (m_bIsChannelCounted)
	{
		if (GetChannelID() <= 0) _DANGER_POINT();
		g_pUserSessionManager->DecreaseChannelUserCount(GetChannelID());
		m_bIsChannelCounted = false;
	}


	const TGuildUID GuildUID = this->GetGuildUID();
	if (GuildUID.IsSet())
	{
		CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
		if (pGuild)
		{
#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (TRUE == pGuild->IsEnable())
			{
#endif
				pGuild->ResetMemberWareVersion (GetCharacterDBID());
				if ((m_eUserState != STATE_MOVETOVILLAGE) && (m_eUserState != STATE_MOVETOGAME))
				{ 					
					if (g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendChangeGuildMemberInfo(GuildUID, GetAccountDBID(), GetCharacterDBID(), GetAccountDBID(), GetCharacterDBID(), GUILDMEMBUPDATE_TYPE_LOGINOUT, _LOCATION_NONE, 0, 0, NULL, true);
				}
#if !defined( PRE_ADD_NODELETEGUILD )
			}
#endif
		}
	}
#if defined( PRE_PRIVATECHAT_CHANNEL )
	if ((m_eUserState != STATE_MOVETOVILLAGE) && (m_eUserState != STATE_MOVETOGAME))
	{
		if(GetPrivateChannelID())
		{
			CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo( GetWorldSetID(), GetPrivateChannelID() );
			if(pPrivateChatChannel)
			{
				m_pDBCon->QueryOutPrivateChatChannelMember( m_pSession, PrivateChatChannel::Common::ExitMember );
				pPrivateChatChannel->DelPrivateChannelMember( PrivateChatChannel::Common::OutMember, GetCharacterDBID() );
				if( GetCharacterDBID() == pPrivateChatChannel->GetMasterCharacterDBID() )
				{					
					m_pDBCon->QueryModPrivateChatChannelInfo( m_pSession, pPrivateChatChannel->GetChannelName(), PrivateChatChannel::Common::ChangeMaster, pPrivateChatChannel->GetChannelPassword(), pPrivateChatChannel->GetNextMasterCharacterDBID());
					pPrivateChatChannel->ModPrivateChannelInfo(  PrivateChatChannel::Common::ChangeMaster, pPrivateChatChannel->GetChannelPassword(), pPrivateChatChannel->GetNextMasterCharacterDBID() );

					if(g_pMasterConnection && g_pMasterConnection->GetActive() )
					{
						g_pMasterConnection->SendModPrivateChatChannelInfo( pPrivateChatChannel->GetChannelID(), PrivateChatChannel::Common::ChangeMaster, pPrivateChatChannel->GetChannelPassword(), pPrivateChatChannel->GetMasterCharacterDBID() );
					}
				}		
				
				if(g_pMasterConnection && g_pMasterConnection->GetActive() )
				{					
					g_pMasterConnection->SendDelPrivateChatChannelMember( PrivateChatChannel::Common::OutMember, pPrivateChatChannel->GetChannelID(), GetCharacterDBID(), GetCharacterName() );
				}
				SetPrivateChannelID(0);
			}
		}
	}
#endif

	// g_Log.Log(L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [CDNUserSession::FinalObject]\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
	return CDNBaseObject::FinalObject();
}

void CDNUserSession::SetCharacterName(const WCHAR* wszName)
{
	CDNUserBase::SetCharacterName(wszName);
	CDNBaseObject::SetCharacterName(wszName);
}

void CDNUserSession::SetSessionID(UINT nSessionID)
{
	CConnection::m_nSessionID = nSessionID;
	CDNUserBase::m_nSessionID = nSessionID;
}

void CDNUserSession::SetChannelID(int nChannelID)
{
	m_nChannelID = nChannelID;
	m_BaseData.nChannelID = nChannelID;
}

void CDNUserSession::SendInsideDisconnectPacket(wchar_t *pIdent)
{
	g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] SendInsideDisconnectPacket(%s)\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID, pIdent);

	// 내부로 끊는거 설정
	DNEncryptPacketSeq EnPacket = { 0, };
	EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader));
	EnPacket.Packet.cMainCmd = IN_DISCONNECT;
	EnPacket.Packet.cSubCmd = CONNECTIONKEY_USER;

	EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
	CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );

	SetDelete(true);

#ifdef PRE_FIX_VILLAGEZOMBIE	
	if (AddRecvData( EnPacket ) == COMPLETE)
		g_pIocpManager->AddProcessCall(m_pSocketContext);
	else
		g_Log.Log(LogType::_ERROR, this, L"SendInsideDisconnectPacket AddRecvData Error! AID[%u] SID[%u]\n", GetAccountDBID(), GetSessionID());
	m_dwCalledSendInsideDisconnectTick = timeGetTime();
#else		//#ifdef PRE_FIX_VILLAGEZOMBIE
	AddRecvData( EnPacket );
	g_pIocpManager->AddProcessCall(m_pSocketContext);
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE
}

bool CDNUserSession::LoadUserData(TASelectCharacter *pSelect)
{
	CDNUserBase::LoadUserData(pSelect);

	if( !g_pMasterConnection )
	{
		DetachConnection(L"Connect|MasterConnection Invalid");
		g_Log.Log(LogType::_ERROR, this, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] QUERY_SELECTCHARACTER Ret:%d\r\n", pSelect->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID, pSelect->nRetCode);
		return false;
	}

	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfo( GetChannelID() );
	if( !pChannelInfo )
	{
		DetachConnection(L"Connect|ChannelInfo not found");
		g_Log.Log(LogType::_ERROR, this, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] QUERY_SELECTCHARACTER Ret:%d ChannelID:%d\r\n", pSelect->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID, pSelect->nRetCode, GetChannelID() );
		return false;
	}

	TPosition Pos;
	memset(&Pos, 0, sizeof(TPosition));

	// PVP 마을로 바로 이동한 경우 PVP맵으로 설정하고 시작 위치 설정
#if defined(PRE_ADD_DWC)
	if( pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DWC )
#else
	if( pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP )
#endif
	{
		SetMapIndex(pChannelInfo->nMapIdx);
		g_pFieldDataManager->GetStartPosition( pChannelInfo->nMapIdx, PvPCommon::Common::PvPVillageStartPositionGateNo, Pos );
	}
	// GM마을,다크레어, 농장인 경우 시작 위치 고정
	else if( pChannelInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_GM|GlobalEnum::CHANNEL_ATT_DARKLAIR|GlobalEnum::CHANNEL_ATT_FARMTOWN))
	{
		SetMapIndex(pChannelInfo->nMapIdx);
		g_pFieldDataManager->GetStartPosition( pChannelInfo->nMapIdx, GlobalEnum::DEFAULT_STARTPOSITION_GATENO, Pos );
		SetLastSubVillageMapIndex(pChannelInfo->nMapIdx);
	}
	else
	{
		SetLastSubVillageMapIndex(-1);

		GlobalEnum::eMapTypeEnum MapType = g_pDataManager->GetMapType( GetMapIndex() );
		if ((pChannelInfo->nMapIdx == GetMapIndex()) && MapType == GlobalEnum::MAP_VILLAGE )
		{
			Pos.nX = GetPosX();
			Pos.nY = GetPosY();
			Pos.nZ = GetPosZ();
		}
		else 
		{
			if (GetLastVillageMapIndex() > 0)
			{
#if defined( PRE_PARTY_DB )
				if( GetLastVillageMapIndex() == pChannelInfo->nMapIdx )
				{
					if (!g_pFieldDataManager->GetStartPosition(GetLastVillageMapIndex(), GetLastVillageGateNo(), Pos))
					{
						g_pFieldDataManager->GetRandomStartPosition(GetLastVillageMapIndex(), Pos);
					}
				}
				else
				{					
					g_pFieldDataManager->GetRandomStartPosition(pChannelInfo->nMapIdx, Pos);
				}
#else
				if (!g_pFieldDataManager->GetStartPosition(GetLastVillageMapIndex(), GetLastVillageGateNo(), Pos)){
					g_pFieldDataManager->GetRandomStartPosition(GetLastVillageMapIndex(), Pos);
				}
#endif
				SetMapIndex(GetLastVillageMapIndex());
			}		
		}
	}

	// 빌리지에 완전 처음 들어온 경우
	if (!IsCheckFirstVillage()){
		SetCheckFirstVillage(true);
		m_pDBCon->QueryCheckFirstVillage(this);
		g_pFieldDataManager->GetStartPositionAngle( GetMapIndex(), GetLastVillageGateNo(), m_UserData.Status.fRotate );
		g_pFieldDataManager->GetStartPosition(GetLastVillageMapIndex(), GetLastVillageGateNo(), Pos);
	}

	m_UserData.Status.nPosX = Pos.nX;
	m_UserData.Status.nPosY = Pos.nY;
	m_UserData.Status.nPosZ = Pos.nZ;

	InitObject(m_UserData.Status.wszCharacterName, g_IDGenerator.GetUserID(), GetChannelID(), GetMapIndex(), &Pos);
	m_BaseData.fRotate = GetRotate();	// 090203 Rotate추가

	m_pSkill->LoadUserData();
	for(int PageIndex=0 ; PageIndex < DualSkill::Type::MAX ; PageIndex++)
	{
		for( UINT i=0 ; i<_countof(pSelect->UserData.Skill[PageIndex].SkillList) ; ++i )
		{
			int iSkillIndex = pSelect->UserData.Skill[PageIndex].SkillList[i].nSkillID;
			if( iSkillIndex > 0 && pSelect->UserData.Skill[PageIndex].SkillList[i].nCoolTime > 0 )
			{
				int iCoolTime = g_pDataManager->GetSkillDelayTime( iSkillIndex, GetSkill()->GetSkillLevel(iSkillIndex) );
				if( iCoolTime > pSelect->UserData.Skill[PageIndex].SkillList[i].nCoolTime )
					m_SkillCoolTime.AddCoolTime( iSkillIndex, timeGetTime()+pSelect->UserData.Skill[PageIndex].SkillList[i].nCoolTime );
			}
		}
	}

#if defined( PRE_PARTY_DB )	
	TPARTYID PartyID = g_pPartyManager->GetPartyID( GetAccountDBID() );
	if( PartyID > 0 && GetLastServerType() != SERVERTYPE_LOGIN )
	{
		SetPartyID( PartyID );
	}	
#else
	if (pSelect->PartyID > 0)	// 파티 세팅 해줘야한다.(Game -> Village일때 필요함)
		SetPartyData(pSelect->PartyID);
#endif

	SendConnectVillage(ERROR_NONE, pChannelInfo->nChannelAttribute);

#if defined(_GPK)
	SendGPKCode();	// 보안코드 보내주고
	SendGPKAuthData();
	SendGPKData();
	ULONG nCurTick = timeGetTime();
	m_dwCheckGPKTick = nCurTick;
	m_dwRecvGPKTick = nCurTick;
#endif

	if (!g_pUserSessionManager->InsertSessionByName(this)){
		DetachConnection(L"Connect|InsertSessionByName(LoadUserData) Failed");
		g_Log.Log(LogType::_ERROR, this, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] InsertSessionByName(LoadUserData) failed\r\n", pSelect->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
		return false;
	}

	if (g_pMasterConnection)
		g_pMasterConnection->SendLoadUserTempData(GetAccountDBID());

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(PRE_ADD_DWC)
	if( IsDWCCharacter() )
		m_ePvPChannel = PvPCommon::RoomType::dwc;
	else
	{
#endif	//#if defined(PRE_ADD_DWC)
		if (GetPvPData()->cLevel > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Colosseum_Limit_Rank)))
			m_ePvPChannel = PvPCommon::RoomType::regular;
		else if (GetPvPData()->cLevel <= static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Colosseum_Limit_Rank)))
			m_ePvPChannel = PvPCommon::RoomType::beginner;
#if defined(PRE_ADD_DWC)
	}
#endif
	if (g_pMasterConnection)
		g_pMasterConnection->SendPvPChangeChannel(GetAccountDBID(), static_cast<BYTE>(m_ePvPChannel), GetPvPData()->cLevel, static_cast<BYTE>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Colosseum_Limit_Rank)), false);
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(PRE_ADD_DWC)
	if ( IsDWCCharacter() && g_pDWCTeamManager )
	{	//일단은 빌리지 접속시 마다 팀정보를 DB 에서 긁어온다.
		if (m_pDBCon && m_pDBCon->GetActive())
			m_pDBCon->QueryGetDWCTeamInfo(m_pSession, true);
	}
#endif
	return true;
}

bool CDNUserSession::SaveUserData()
{
	CDNUserBase::SaveUserData();

	if (GetLastVillageGateNo() < 0) SetLastVillageGateNo(0);

	if (m_cGateNo > 0){	// 혹시 게이트 앞에서 접속을 종료했다면 위치를 다시 잡아놓자 090105
		g_pFieldDataManager->GetStartPosition(GetMapIndex(), m_cGateNo, m_BaseData.CurPos);
	}

	m_UserData.Status.nPosX = m_BaseData.CurPos.nX;
	m_UserData.Status.nPosY = m_BaseData.CurPos.nY;
	m_UserData.Status.nPosZ = m_BaseData.CurPos.nZ;
	m_UserData.Status.fRotate = m_BaseData.fRotate - 15.524f; // 오이사!

	// g_Log.Log(L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] CDNUserSession::SaveUserData\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);

	GetSkill()->SaveUserData( &m_UserData );

	std::vector<std::pair<int,int>> vData;
	m_SkillCoolTime.GetCoolTimeData( vData );

	for(int PageIndex=0 ; PageIndex < DualSkill::Type::MAX ; PageIndex++)
	{
		for( UINT i=0 ; i<_countof(m_UserData.Skill[PageIndex].SkillList) ; ++i )
		{
			for( UINT j=0 ; j<vData.size() ; ++j )
			{
				if( m_UserData.Skill[PageIndex].SkillList[i].nSkillID == vData[j].first )
				{
					m_UserData.Skill[PageIndex].SkillList[i].nCoolTime = vData[j].second;
					break;
				}
			}
		}
	}
	return true;
}

void CDNUserSession::UpdateMoveToVillageUserData()
{
	if (GetClassID() <= 0) return;	// 0일때는 저장하면 안된다
	if (IsCertified() == false)	return;		//진입을 제대로 하지 않았다면 저장하지 않는다.

	m_eUserState = STATE_MOVETOVILLAGE;		// 일단 상태값 세팅하고
	m_pTimeEventSystem->SaveUserData( false );

	SaveUserData();	// 일단 먼저 세이브 하고
	m_pDBCon->QueryUpdateUserData(QUERY_CHANGESERVERUSERDATA, this, &m_UserData, true );
	m_pDBCon->QuerySaveItemLocationIndex(this);
#if defined(_KRAZ)
	m_pDBCon->QueryActozUpdateCharacterInfo(this, ActozCommon::UpdateType::Sync);
#endif	// #if defined(_KRAZ)
}

void CDNUserSession::UpdateMoveToGameUserData()
{
	if (GetClassID() <= 0) return;	// 0일때는 저장하면 안된다
	if (IsCertified() == false)	return;		//진입을 제대로 하지 않았다면 저장하지 않는다.

	m_eUserState = STATE_MOVETOGAME;		// 일단 상태값 세팅하고
	m_pTimeEventSystem->SaveUserData( false );

	SaveUserData();	// 일단 먼저 세이브 하고
	m_pDBCon->QueryMapInfo(this);
	m_pDBCon->QueryUpdateUserData(QUERY_CHANGESERVERUSERDATA, this, &m_UserData, false );
	m_pDBCon->QuerySaveItemLocationIndex(this);
#if defined(_KRAZ)
	m_pDBCon->QueryActozUpdateCharacterInfo(this, ActozCommon::UpdateType::Sync);
#endif	// #if defined(_KRAZ)
}

void CDNUserSession::LastUpdateUserData()
{
	if (GetClassID() <= 0) return;	// 0일때는 저장하면 안된다

	m_pTimeEventSystem->SaveUserData( true );

	m_pDBCon->QueryUpdateUserData(QUERY_LASTUPDATEUSERDATA, this, &m_UserData, false );
	m_pDBCon->QuerySaveItemLocationIndex(this);
}

int CDNUserSession::FieldProcess(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData)
{
	CDNBaseObject::FieldProcess(pSender, wMsg, pSenderData, pParamData);

	switch(wMsg)
	{
	case FM_SHOW:
		{
			if (pSender == this){
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
				std::vector<TEffectSkillData> vEffectSkill;
				vEffectSkill.clear();
				m_pItem->GetEffectSkillItem( vEffectSkill );
				SendEffectSkillItemData(m_pSession->GetSessionID(), vEffectSkill, false);
#else	//	#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
				TSourceData source = m_pItem->GetSource();
				SendSourceData(GetSessionID(), source, false);
#endif	//	#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
			}
			else {
				if (g_IDGenerator.IsUser(pSenderData->nObjectID)){
					CDNUserSession *pUserObj = (CDNUserSession*)pSender;
					SendEnterUser(pUserObj);

					// -------------------------------------------------------------
					// 추가정보 - CSEnterUser에 보내기엔 빈도가 적고 큰 데이터
					if( pUserObj->m_nChatRoomID > 0 )	// 채팅방 정보가 있다면 보낸다.
					{
						CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( pUserObj->m_nChatRoomID );
						if( pChatRoom )
						{
							// 방장
							if( pChatRoom->IsLeader( pUserObj->GetAccountDBID() ) )
							{
								TChatRoomView ChatRoomView;
								ChatRoomView.Set( pChatRoom->GetChatRoomID(), pChatRoom->GetChatRoomName(), pChatRoom->GetChatRoomType(), pChatRoom->UsePassword(),
									pChatRoom->GetChatRoomPRLine1(), pChatRoom->GetChatRoomPRLine2(), pChatRoom->GetChatRoomPRLine3() );

								SendChatRoomView( pUserObj->GetSessionID(), ChatRoomView );
							}
							// 참여유저
							else
							{
								SendChatRoomEnterUser( pChatRoom->GetChatRoomID(), pChatRoom->GetChatRoomType(), pUserObj->GetSessionID() );
							}
						}
					}
				}
				else if (g_IDGenerator.IsNpc(pSenderData->nObjectID)) 
				{
					switch(((CDNNpcObject*)pSender)->GetNpcType() ) {
						case CDnNpc::ActorNpc:
							{
								CDNNpcObject *pNpcObj = (CDNNpcObject*)pSender;
								float x = pNpcObj->GetCurrentPos().nX / 1000.0f;
								float y = pNpcObj->GetCurrentPos().nY / 1000.0f;
								float z = pNpcObj->GetCurrentPos().nZ / 1000.0f;
								float fRot = pNpcObj->GetRotate();

								SendEnterNpc(pSenderData->nObjectID, pNpcObj->GetNpcData()->nNpcID, x, y, z, fRot);
							}
							break;
						case CDnNpc::PropNpc:
							{
								CDNPropNpcObject *pNpcObj = (CDNPropNpcObject*)pSender;

								SendEnterPropNpc(pSenderData->nObjectID, pNpcObj->GetPropCreateUniqueID() );
							}
							break;
					}
				}
			}
		}
		break;

	case FM_HIDE:
		{
			if (pSender != this){
				if (g_IDGenerator.IsUser(pSenderData->nObjectID)){
					CDNUserSession *pUserObj = (CDNUserSession*)pSender;
					SendLeaveUser(pUserObj->GetSessionID(), pSender->GetLeaveType(), pUserObj->GetAccountLevel());
					// g_Log.Log(L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] SendLeave\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
				}
				else {
					switch(((CDNNpcObject*)pSender)->GetNpcType() ) {
						case CDnNpc::ActorNpc:
							SendLeaveNpc((pSender)->GetObjectID());
							break;
						case CDnNpc::PropNpc:
							SendLeavePropNpc(((CDNPropNpcObject*)pSender)->GetPropCreateUniqueID() );
							break;
					}
				}
			}
		}
		break;

	case FM_MOVE:
		if ((pSender != this) && (g_IDGenerator.IsUser(pSenderData->nObjectID))){
			SendActorMsg(pParamData->dwSendSessionID, pParamData->wActorProtocol, pParamData->nBufSize, (char*)pParamData->Buffer, pParamData ); // SendSessionID
		}
		break;

	case FM_ACTORMSG:
		if (pSender != this){
			SendActorMsg(pParamData->dwSendSessionID, pParamData->wActorProtocol, pParamData->nBufSize, (char*)pParamData->Buffer, pParamData ); // SendSessionID
		}
		break;

	case FM_CHAT:
	{
		SendChat(pParamData->eChatType, pParamData->nChatSize, pSender->wszName(), pParamData->wszChatMsg, pParamData );
		break;
	}

	case FM_CHANGEEQUIP:
		if (pSender != this){
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendChangeEquip(pUserObj->GetSessionID(), pParamData->ItemInfo);
		}
		else{	// 내자신이면 파티멤버들한테 날려주자
			if (GetPartyID() > 0) {
				if( pParamData->ItemInfo.cSlotIndex == EQUIP_HAIR || pParamData->ItemInfo.cSlotIndex == EQUIP_HELMET || pParamData->ItemInfo.cSlotIndex == EQUIP_EARRING ) {
					g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
				}
			}
		}
		break;

	case FM_CHANGECASHEQUIP:
		if (pSender != this){
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendChangeCashEquip(pUserObj->GetSessionID(), pParamData->CashItemInfo);
		}
		else{	// 내자신이면 파티멤버들한테 날려주자
			if (GetPartyID() > 0) {
				if( pParamData->CashItemInfo.cSlotIndex == CASHEQUIP_HELMET || pParamData->CashItemInfo.cSlotIndex == CASHEQUIP_EARRING ) {
					g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
				}
			}
		}
		break;

	case FM_CHANGEGLYPH:
		if (pSender != this){
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendChangeGlyph(pUserObj->GetSessionID(), pParamData->ItemInfo);
		}
		break;

	case FM_ADDEXP:
		break;

	case FM_LEVELUP:
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendLevelUp(pUserObj->GetSessionID(), pParamData->cLevel, pParamData->nExp);
		}
		break;

	case FM_CHANGEAPPELLATION:
		{
			if (pSender != this) {
				CDNUserSession *pUserObj = (CDNUserSession*)pSender;
				SendChangeAppellation( pUserObj->GetSessionID(), pParamData->nSelectAppellation, pParamData->nCoverAppellation );
			}
		}
		break;

	case FM_CHANGEHIDE:
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendHide(pUserObj->GetSessionID(), pParamData->bHide);
		}
		break;

	case FM_CHANGEGUILDSELFVIEW:	// 길드 시각정보(개인/단체) 주변 동기화 처리
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendGuildSelfView(pUserObj->GetSessionID(), pUserObj->GetGuildSelfView());
		}
		break;

	case FM_CHANGEJOB:
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendChangeJob( pUserObj->GetSessionID(), pParamData->nJob);
		}
		break;

	case FM_EFFECT:
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendBroadcastingEffect(pUserObj->GetSessionID(), pParamData->cType, pParamData->cState);
		}
		break;

	case FM_CHANGECOLOR:
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendChangeColor(pUserObj->GetSessionID(), pParamData->cType, pParamData->dwColor);
		}
		break;

	case FM_CHATROOMVIEW:	// 채팅방 생성/삭제
		{
			if (pSender != this){
				CDNUserSession *pUserObj = (CDNUserSession*)pSender;
				TChatRoomView ChatRoomView;
				if( pParamData->nChatRoomParam <= 0 )	// Param값은 방번호
				{
					// 채팅방이 삭제된 경우
					ChatRoomView.Set( 0, L"", CHATROOMTYPE_NONE, false, L"", L"", L"" );
				}
				else
				{
					// 채팅방이 생성된 경우
					CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( pParamData->nChatRoomParam );
					if( pChatRoom )
					{
						ChatRoomView.Set( pChatRoom->GetChatRoomID(), pChatRoom->GetChatRoomName(), pChatRoom->GetChatRoomType(), pChatRoom->UsePassword(),
										  pChatRoom->GetChatRoomPRLine1(), pChatRoom->GetChatRoomPRLine2(), pChatRoom->GetChatRoomPRLine3() );
					}
				}
				SendChatRoomView( pUserObj->GetSessionID(), ChatRoomView );
			}
		}
		break;

	case FM_CHATROOMUSER:	// 채팅방에 유저 입장/퇴장
		{
			if (pSender != this){
				CDNUserSession *pUserObj = (CDNUserSession*)pSender;
				if( pParamData->nChatRoomParam > 0 )	// ChatRoomID
				{
					CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( pParamData->nChatRoomParam );
					if( pChatRoom )
					{
						SendChatRoomEnterUser( pChatRoom->GetChatRoomID(), pChatRoom->GetChatRoomType(), pUserObj->GetSessionID() );
					}
				}
				else
				{
					// 음수로 퇴장 이유가 설정되어있다.
					SendChatRoomLeaveUser( pUserObj->GetSessionID(), -pParamData->nChatRoomParam );
				}
			}
		}
		break;

	case FM_CHANGEVEHICLE:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendChangeVehicleParts(pSession->GetSessionID(), pParamData->nIndex, pParamData->Vehicle);
		}
		break;

	case FM_CHANGEVEHICLECOLOR:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendChangeVehicleColor(pSession->GetSessionID(), pParamData->Vehicle.nSerial, pParamData->dwColor, pParamData->cPetPartsType);
		}
		break;

	case FM_CHANGEPET:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendChangePetParts(pSession->GetSessionID(), pParamData->nIndex, pParamData->Vehicle);
		}
		break;

	case FM_CHANGEPETBODY:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendChangePetBody(pSession->GetSessionID(), pParamData->PetInfo);
		}
		break;

	case FM_RENAME_PET:
		{
			if (this == pSender)
				break;

			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendChangePetNameResult(ERROR_NONE, pSession->GetSessionID(), 0, pParamData->PetInfo.wszNickName);
		}

	case FM_ENTERPARTY:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendUpdatePartyUI(pSession->GetSessionID(), true);
		}
		break;

	case FM_LEAVEPARTY:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendUpdatePartyUI(pSession->GetSessionID(), false);
		}
		break;

	case FM_CHARACTER_RENAME:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendCharacterRenameResult(ERROR_NONE, pSession->GetSessionID(), pSession->GetCharacterName());
		}
		break;

	case FM_GUILDWARWINSKILL :
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			TGuildUID GuildUID(g_Config.nWorldSetID, pParamData->nExp);
			EtVector3 vPos( pSession->GetCurrentPos().nX/1000.f, pSession->GetCurrentPos().nY/1000.f, pSession->GetCurrentPos().nZ/1000.f );
			SendGuildWarUserWinSkill(GuildUID, vPos, pParamData->nIndex);
		}
		break;

	case FM_USE_SOURCE:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			TSourceData source = pSession->GetItem()->GetSource();
			SendSourceData(pSession->GetSessionID(), source, false);
		}
		break;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	case FM_USE_EFFECTSKILL:
		{			
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			TEffectSkillData* EffectSkill = pSession->GetItem()->GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pParamData->nIndex );	// nIndex에 스킬아이디 저장
			if( EffectSkill )		
			{	
				//SendEffectSkillItemData(pSession->GetSessionID(), *EffectSkill, false);
				if(pParamData->cLevel > EffectSkillNameSpace::ShowEffectType::NONEEFFECT)
					SendShowEffect(EffectSkill->nItemID, pSession->GetSessionID(), pParamData->cLevel);
			}
		}
		break;
	case FM_HAVE_NAMEDITEM:
		{			
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendNamedItemID( pSession->GetSessionID(), pParamData->nIndex );
		}
		break;
#endif
#if defined( PRE_ADD_TRANSFORM_POTION )
	case FM_CHANGE_TRANSFORM:
		{
			CDNUserSession *pSession = (CDNUserSession*)pSender;
			SendChangeTransform( pSession->GetSessionID(), pSession->GetTransformID(), pParamData->nIndex );
		}
		break;
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case FM_CHANGETALISMAN:
		if (pSender != this){
			CDNUserSession *pUserObj = (CDNUserSession*)pSender;
			SendChangeTalisman(pUserObj->GetSessionID(), pParamData->ItemInfo);
		}
		break;
#endif
#if defined(PRE_ADD_DWC)
	case FM_CHANGEDWCTEAM:
		{
			if(!g_pDWCTeamManager->bIsIncludeDWCVillage())
				break;

			if (pSender != this){				
				CDNUserSession *pUserObj = (CDNUserSession*)pSender;
				
				const WCHAR * pDWCTeamName = NULL;
				if(pUserObj->GetDWCTeamID() > 0)
					pDWCTeamName = g_pDWCTeamManager->GetDWCTeamName(pUserObj->GetDWCTeamID());

				SendDWCTeamName(pUserObj->GetSessionID(), pDWCTeamName);
			}
		}
		break;		
#endif
	}

	return 1;
}

void CDNUserSession::DoUpdate(DWORD CurTick)
{
	if (!m_bLoadUserData) return;	// 로드 되기전에 들어오면 나가라
	CDNUserBase::DoUpdate(CurTick);
#if defined(PRE_ADD_TRANSFORM_POTION)
	CalcTransformExpire(CurTick);
#endif //#if defined(PRE_ADD_TRANSFORM_POTION)
}

void CDNUserSession::OnDBRecvAuth(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvAuth(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvCharInfo(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvCharInfo(nSubCmd, pData);

	switch(nSubCmd)
	{
	case QUERY_SELECTCHARACTER:
		{
			TASelectCharacter *pSelect = (TASelectCharacter*)pData;
			if (pSelect->nRetCode != ERROR_NONE){
				DetachConnection(L"Connect|UserObj not found");
				g_Log.Log(LogType::_ERROR, this, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] QUERY_SELECTCHARACTER Ret:%d\r\n", pSelect->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID, pSelect->nRetCode);
				return;
			}

			if( GetCharacterDBID() != pSelect->biCharacterDBID )
			{
				DetachConnection(L"QUERY_SELECTCHARACTER Invalid CharacterDBID");
				g_Log.Log(LogType::_ERROR, this, L"QUERY_SELECTCHARACTER Invalid CharacterDBID=%I64d,%I64d\r\n", GetCharacterDBID(), pSelect->biCharacterDBID );
				return;
			}

#if defined( PRE_ADD_CHARACTERCHECKSUM )
			if( pSelect->tLastLoginDate <= pSelect->tLastLogoutDate && pSelect->uiCheckSum )
			{
				m_uiDBCheckSum = pSelect->uiCheckSum;
				m_uiRestoreCheckSum = MakeCharacterCheckSum( pSelect->biCharacterDBID, pSelect->UserData.Status.cLevel, pSelect->UserData.Status.nExp, pSelect->UserData.Status.nCoin, pSelect->UserData.Status.nWarehouseCoin );
				
				g_Log.Log(LogType::_NORMAL, this, L"Restore Character CheckSum(cid:%I64d, level:%d, exp:%d, coin:%I64d, wcoin:%I64d, restorechecksum:%x, dbchecksum:%x, lastlogin:%I64d, lastlogout:%I64d)\r\n", pSelect->biCharacterDBID, pSelect->UserData.Status.cLevel, pSelect->UserData.Status.nExp, pSelect->UserData.Status.nCoin, pSelect->UserData.Status.nWarehouseCoin, m_uiRestoreCheckSum, m_uiDBCheckSum, pSelect->tLastLoginDate, pSelect->tLastLogoutDate );
			}
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

			if (LoadUserData(pSelect) == false){	// 유저데이터 세팅하고
				return;
			}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
			_ASSERT( m_pReputationSystem && m_pReputationSystem->GetEventHandler() );
			m_pReputationSystem->GetEventHandler()->OnConnect( pSelect->UserData.Status.tLastConnectDate, false );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

			GetItem()->RemoveInstantItemData( false );
			if( !pSelect->GuildSelfView.IsSet() )
				GetItem()->RemoveGuildReversionItem( false );

			g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] QUERY_SELECTCHARACTER\r\n", pSelect->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
		}
		break;

	case QUERY_CHANGESERVERUSERDATA:
		{
			TAUpdateCharacter *pUpdate = (TAUpdateCharacter*)pData;
			if (pUpdate->nRetCode != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] QUERY_CHANGESERVERUSERDATA Ret:%d\r\n", pUpdate->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID, pUpdate->nRetCode);
				return;
			}

			// 유저가 채널 이동하기전에 마스터에 임시데이터를 저장시켜준다.
			g_pMasterConnection->SendSaveUserTempData(this);

			if (m_eUserState == STATE_MOVETOGAME) 
			{
				SendGameInfo( m_GameTaskType, m_cReqGameIDType, m_nGameServerIP, m_nGameServerPort, m_nGameServerTcpPort, GetAccountDBID(), GetCertifyingKey());
			}
			else if (m_eUserState == STATE_MOVETOVILLAGE) {
				SendVillageInfo(m_MoveToVillageInfo.szIP, m_MoveToVillageInfo.nPort, ERROR_NONE, GetAccountDBID(), GetCertifyingKey());
			}

			DelayDetachConnection(L"QUERY_CHANGESERVERUSERDATA");
			g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] QUERY_CHANGESERVERUSERDATA\r\n", pUpdate->nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
		}
		break;

	case QUERY_LASTUPDATEUSERDATA:
		{
			DelayDetachConnection(L"QUERY_LASTUPDATEUSERDATA");
		}
		break;

	case QUERY_GETCHARACTERPARTIALYBYDBID:
	case QUERY_GETCHARACTERPARTIALYBYNAME:
		{
			TAGetCharacterPartialy * pPacket = (TAGetCharacterPartialy*)pData;
			switch(pPacket->cReqType)
			{
			case _REQCHARACTERPARTIALY_FRIEND:
				{
					if (pPacket->nRetCode != ERROR_NONE) return;
					if (pPacket->biCharacterDBID > 0)
					{
						TFriend * pFriend = m_pFriend->GetFriend(pPacket->biCharacterDBID);
						if (pFriend == NULL) return;
						sWorldUserState State;
						memset(&State, 0, sizeof(sWorldUserState));

						if (g_pWorldUserState->GetUserState(pFriend->wszCharacterName, pFriend->biFriendCharacterDBID, &State) == false)
							State.nLocationState = _LOCATION_NONE;

						if (pFriend != NULL)
							SendFriendDetailInfo(pFriend->biFriendCharacterDBID, pFriend->nGroupDBID, pPacket->cClass, pPacket->cLevel, \
							pPacket->cJob, &State, NULL, pFriend->wszFriendMemo);
						else
							_DANGER_POINT();
					}
				}
			}
		}
		break;

	case QUERY_CHANGECHARACTERNAME:
		{
			const TAChangeCharacterName *pPacket = reinterpret_cast<TAChangeCharacterName*>(pData);
			if (ERROR_NONE != pPacket->nRetCode)
			{
				SendCharacterRenameResult(pPacket->nRetCode, GetSessionID(), NULL, pPacket->tNextChangeDate);
				break;
			}

			if (GetCharacterDBID() != pPacket->nCharacterDBID)
			{
				SendCharacterRenameResult(ERROR_INVALIDPACKET, GetSessionID(), NULL);
				break;
			}

			// 아이템삭제, 캐릭터명변경알림
			int nItemID = 0;
			const TItem *pItem = m_pItem->GetCashInventory(pPacket->biItemSerial);
			if (pItem)
				nItemID = pItem->nItemID;

			if (!m_pItem->DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biItemSerial))
				break;

			GetEventSystem()->OnEvent( EventSystem::OnItemUse, 1, EventSystem::ItemID, nItemID );

			// Global Object
			if (g_pUserSessionManager)
				g_pUserSessionManager->SwapUserKeyName(pPacket->wszOriginName, pPacket->wszCharacterName);

			// Guild Member
			const TGuildUID GuildUID = GetGuildUID();
			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (pGuild)
				pGuild->UpdateMemberName(pPacket->nCharacterDBID, pPacket->wszCharacterName);

			// Master System
			TMasterSystemSimpleInfo& MySimpleInfo = const_cast<TMasterSystemSimpleInfo&>(GetMasterSystemData()->SimpleInfo);
			for (int i=0; i<MySimpleInfo.cCharacterDBIDCount; i++)
			{
				if (!g_pUserSessionManager) continue;
				CDNUserSession * pUserSession = g_pUserSessionManager->FindUserSessionByName(MySimpleInfo.OppositeInfo[i].wszCharName);
				if (pUserSession)
				{
					TMasterSystemSimpleInfo& TargetSimpleInfo = const_cast<TMasterSystemSimpleInfo&>(pUserSession->GetMasterSystemData()->SimpleInfo);
					for (int j=0; j<TargetSimpleInfo.cCharacterDBIDCount; j++)
					{
						if (!wcscmp(TargetSimpleInfo.OppositeInfo[j].wszCharName, pPacket->wszOriginName))
						{
							_wcscpy(TargetSimpleInfo.OppositeInfo[j].wszCharName, _countof(TargetSimpleInfo.OppositeInfo[j].wszCharName), pPacket->wszCharacterName, (int)wcslen(pPacket->wszCharacterName));
							pUserSession->SendMasterSystemSimpleInfo(TargetSimpleInfo, true);
						}
					}
				}
			}
#if defined (PRE_ADD_BESTFRIEND)
			CDNUserSession* pBFSession = g_pUserSessionManager->FindUserSessionByName( m_pBestFriend->GetInfo().wszName );
			if (pBFSession)
				pBFSession->GetBestFriend()->ChangeName(pPacket->wszCharacterName);

			g_pMasterConnection->SendChangeCharacterName(GuildUID, GetWorldSetID(), pPacket->nAccountDBID, pPacket->nCharacterDBID, 
														pPacket->wszOriginName, pPacket->wszCharacterName, m_pBestFriend->GetInfo().biCharacterDBID);
#else
			g_pMasterConnection->SendChangeCharacterName(GuildUID, GetWorldSetID(), pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->wszOriginName, pPacket->wszCharacterName);
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
			if( GetPrivateChannelID() > 0 )
			{
				CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo( GetWorldSetID(), GetPrivateChannelID() );

				if(pPrivateChatChannel)
				{					
					pPrivateChatChannel->ModPrivateChannelMemberName( GetCharacterDBID(), GetCharacterName() );
					
					if(g_pMasterConnection && g_pMasterConnection->GetActive() )
					{
						g_pMasterConnection->SendModPrivateChatChannelMemberName( pPrivateChatChannel->GetChannelID(), GetCharacterDBID(), GetCharacterName() );
					}					
				}
			}
#endif

			SendUserLocalMessage(0, FM_CHARACTER_RENAME);

#if defined(_KRAZ)
			m_pDBCon->QueryActozUpdateCharacterName(this);
#endif	// #if defined(_KRAZ)
		}
		break;
	}
}

void CDNUserSession::OnDBRecvEtc(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvEtc(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvQuest(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvQuest(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvMission(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvMission(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvSkill(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvSkill(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvFriend(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvFriend(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvIsolate(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvIsolate(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvPvP(int nSubCmd, char *pData)
{
	switch( nSubCmd )
	{
		case QUERY_GETLIST_PVPLADDERRANKING:
			{
				TAGetListPvPLadderRanking* pPacket = reinterpret_cast<TAGetListPvPLadderRanking*>(pData);
				SendPvPLadderRankBoard( pPacket );
				return;
			}
			break;
#if defined(PRE_ADD_PVP_RANKING)
		case QUERY_GET_PVP_RANKBOARD:
			{
				TAGetPvPRankBoard* pPacket = reinterpret_cast<TAGetPvPRankBoard*>(pData);
				//pvp계급 정보 재설정
				g_pDataManager->SetRelativePvPRank(&pPacket->MyRanking.cPvPLevel, pPacket->MyRanking.uiExp, pPacket->MyRanking.iPvPRank, pPacket->MyRanking.fPvPRaito);
				SendPvPRankBoard( pPacket );
				return;
			}
			break;
		case QUERY_GET_PVP_RANKLIST:
			{
				TAGetPvPRankList* pPacket = reinterpret_cast<TAGetPvPRankList*>(pData);
				//pvp계급 정보 재설정
				for(int i = 0 ; i < (int)pPacket->cRankingCount ; i++)
					g_pDataManager->SetRelativePvPRank(&pPacket->RankingInfo[i].cPvPLevel, pPacket->RankingInfo[i].uiExp, pPacket->RankingInfo[i].iPvPRank, pPacket->RankingInfo[i].fPvPRaito);
				
				SendPvPRankList( pPacket );
				return;
			}
			break;
		case QUERY_GET_PVP_RANKINFO:
			{
				TAGetPvPRankInfo* pPacket = reinterpret_cast<TAGetPvPRankInfo*>(pData);
				//pvp계급 정보 재설정
				g_pDataManager->SetRelativePvPRank(&pPacket->RankingInfo.cPvPLevel, pPacket->RankingInfo.uiExp, pPacket->RankingInfo.iPvPRank, pPacket->RankingInfo.fPvPRaito);

				SendPvPRankInfo( pPacket );
				return;
			}
			break;

		case QUERY_GET_PVP_LADDER_RANKBOARD:
			{
				TAGetPvPLadderRankBoard* pPacket = reinterpret_cast<TAGetPvPLadderRankBoard*>(pData);
				//pvp계급 정보 재설정
				g_pDataManager->SetRelativePvPRank(&pPacket->MyRanking.cPvPLevel, pPacket->MyRanking.uiExp, pPacket->MyRanking.iPvPRank, pPacket->MyRanking.fPvPRaito);

				SendPvPLadderRankBoard( pPacket );
				return;
			}
			break;
		case QUERY_GET_PVP_LADDER_RANKLIST:
			{
				TAGetPvPLadderRankList* pPacket = reinterpret_cast<TAGetPvPLadderRankList*>(pData);
				//pvp계급 정보 재설정
				for(int i = 0 ; i < (int)pPacket->cRankingCount ; i++)
					g_pDataManager->SetRelativePvPRank(&pPacket->RankingInfo[i].cPvPLevel, pPacket->RankingInfo[i].uiExp, pPacket->RankingInfo[i].iPvPRank, pPacket->RankingInfo[i].fPvPRaito);

				SendPvPLadderRankList( pPacket );
				return;
			}
			break;
		case QUERY_GET_PVP_LADDER_RANKINFO:
			{
				TAGetPvPLadderRankInfo* pPacket = reinterpret_cast<TAGetPvPLadderRankInfo*>(pData);
				//pvp계급 정보 재설정
				g_pDataManager->SetRelativePvPRank(&pPacket->RankingInfo.cPvPLevel, pPacket->RankingInfo.uiExp, pPacket->RankingInfo.iPvPRank, pPacket->RankingInfo.fPvPRaito);

				SendPvPLadderRankInfo( pPacket );
				return;
			}
			break;
#endif
	}

	CDNUserBase::OnDBRecvPvP(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvDarkLair(int nSubCmd, char *pData)
{
	switch( nSubCmd )
	{
		case QUERY_GET_DARKLAIR_RANK_BOARD:
		{
			TAGetDarkLairRankBoard* pPacket = reinterpret_cast<TAGetDarkLairRankBoard*>(pData);
			SendDarkLairRankBoard( pPacket );
			return;
		}
	}

	CDNUserBase::OnDBRecvDarkLair(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvGuild(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvGuild(nSubCmd, pData);

	switch(nSubCmd)
	{
		// 길드창설 결과
	case QUERY_CREATEGUILD:				
		{
			// 에러처리
			const TACreateGuild *pPacket = reinterpret_cast<TACreateGuild *>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendCreateGuild(GetSessionID(), pPacket->wszGuildName, pPacket->nRetCode, NULL, NULL, 0, 0, 0, 0, NULL);
				break;
			}

			if (GUILDDBID_DEF == pPacket->nGuildDBID) 
			{
				SendCreateGuild(GetSessionID(), pPacket->wszGuildName, ERROR_DB, NULL, NULL, 0, 0, 0, 0, NULL);
				break;
			}

			// 길드 창설자금 차감 - DB에 선반영되어있기 때문에 서버에서만 처리해준다.
			DelCoin(pPacket->nEstablishExpense, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

			// 길드정보 세팅
			TGuild GuildInfo;
			GuildInfo.GuildView.Set(TGuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID), pPacket->wszGuildName);
			GuildInfo.wGuildSize = g_pGuildManager->GetDefaultSize();

			// 길드권한적용(최초)
			g_pGuildManager->SetAuthBase(GuildInfo.wGuildRoleAuth);

			// 길드권한적용(허용/불가)
			g_pGuildManager->SetAuthMust(GuildInfo.wGuildRoleAuth);

			// 길드자원을새로할당받음
			CDNGuildBase* pGuild = g_pGuildManager->Get(&GuildInfo);
			if (!pGuild) 
			{
				// 길드할당에러
				SendCreateGuild(GetSessionID(), pPacket->wszGuildName, ERROR_GENERIC_UNKNOWNERROR, NULL, NULL, 0, 0, 0, 0, NULL);
				DN_BREAK;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 창고정보를 가져온다.
			pGuild->ReqGetGuildWareInfo();

#if defined(_DEBUG)
			pGuild->m_bSkipDisableOnDebugging = true;	// 디버깅중길드자원해제방지
#endif	// _DEBUG

			TGuildMember GuildMember;
			GuildMember.Set(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), static_cast<char>(GetUserJob()), GetLevel(), GUILDROLE_TYPE_MASTER, pPacket->JoinDate);

			// 길드원을 길드에 추가(기본적으로 길드마스터임)
			if (!pGuild->AddMember(&GuildMember))
			{
				DN_ASSERT(0,	"Check!");
			}

			// 길드 시작정보 설정 및 전송
			TGuildSelfView GuildSelfView;
			GuildSelfView.Set(GuildInfo.GuildView, GUILDROLE_TYPE_MASTER);
			SetGuildSelfView(GuildSelfView);
			SendGuildSelfView(GetSessionID(), GetGuildSelfView());

			// 현재 위치를 저장한다.
			TCommunityLocation Location;

			sWorldUserState WorldUserState;
			bool bRetVal = g_pWorldUserState->GetUserState(GetCharacterName(), GetCharacterDBID(), &WorldUserState);
			if (bRetVal) 
			{
				Location.cServerLocation = WorldUserState.nLocationState;
				Location.nChannelID = WorldUserState.nChannelID;
				Location.nMapIdx = WorldUserState.nMapIdx;
			}
			else
				Location.Reset();

			pGuild->SetRecvAllMember( true );

			// 창설 결과를 통보
			SendCreateGuild(GetSessionID(), pPacket->wszGuildName, ERROR_NONE, &pGuild->GetUID(), &GuildInfo, GetAccountDBID(), GetCharacterDBID(), static_cast<TP_JOB>(GetUserJob()), GetLevel(), &Location);
			GetEventSystem()->OnEvent( EventSystem::OnGuildCreate );
			GetEventSystem()->OnEvent( EventSystem::OnGuildJoin );

#if defined(_DEBUG)
			pGuild->m_bSkipDisableOnDebugging = false;	// 디버깅중길드자원해제방지
#endif	// _DEBUG
		}
		break;

		// 길드해체 결과
	case QUERY_DISMISSGUILD:			
		{
			const TADismissGuild *pPacket = reinterpret_cast<TADismissGuild*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendDismissGuild(GetSessionID(), pPacket->nRetCode, NULL);
				break;
			}

			// 길드자원 얻어오기
			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILD_NOTEXIST_GUILDINFO, NULL);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 길드해산목록에등록
			g_pGuildManager->AddDismiss(GuildUID);

			// 길드시각정보초기화
			ResetGuildSelfView();

			// MA 를통하여다른서버에접속중인길드원들에게도통보(정상적인경우라면이미길드원들은모두탈퇴된상태일것임, 길드자원만해제?)
			g_pMasterConnection->SendDismissGuild(GuildUID);

			// 길드자원반납
#if !defined( PRE_ADD_NODELETEGUILD )
			pGuild->DisableAttach();
#endif

			// 해체됨을 알려준다.
			SendDismissGuild(GetSessionID(), ERROR_NONE, &GuildUID);
#if defined( PRE_ADD_GUILD_RENEWSYSTEM )
			GetItem()->RemoveGuildReversionItem( true );
			GetItem()->RemoveGuildReversionVehicleItemData( true );
			SendGuildRewardItem(NULL);

#endif
			USES_CONVERSION;
		}
		break;

	case QUERY_ADDGUILDMEMBER:			// 길드원추가결과
		{
			const TAAddGuildMember *pPacket = reinterpret_cast<TAAddGuildMember*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), pPacket->nRetCode, 0, 0, 0, 0, NULL, NULL);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0, 0, 0, NULL, NULL);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 위치 초기화
			TCommunityLocation Location;
			Location.Reset();

			TGuildMember GuildMember;
			GuildMember.Set(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), static_cast<TP_JOB>(GetUserJob()), GetLevel(), GUILDROLE_TYPE_JUNIOR, pPacket->JoinDate);

			// 길드원 추가(중복발생 가능있음)
			if (!pGuild->AddMember(&GuildMember, &Location))
				_DANGER_POINT()

			// 창고정보를가져온다.
			pGuild->ReqGetGuildWareInfo();

			// 길드시각정보지정
			SetGuildSelfView(TGuildSelfView(pGuild->GetInfo()->GuildView, GUILDROLE_TYPE_JUNIOR));

			TGuildRewardItem *RewardItemInfo;
			RewardItemInfo = pGuild->GetGuildRewardItem();
			SendGuildRewardItem(RewardItemInfo);

			// 현재서버에접속중인길드원들에게통보
			pGuild->SendAddGuildMember(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), static_cast<TP_JOB>(GetUserJob()), GetLevel(), pPacket->JoinDate, &Location);

			// MA 를통하여다른서버에접속중인길드원들에게도통보
			g_pMasterConnection->SendAddGuildMember(GuildUID, GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), static_cast<TP_JOB>(GetUserJob()), GetLevel(), pPacket->JoinDate);
			GetEventSystem()->OnEvent( EventSystem::OnGuildJoin );
			// 우승 길드인지 확인
			if ( g_pGuildWarManager->GetPreWinGuildUID().IsSet() && pGuild->GetUID() == g_pGuildWarManager->GetPreWinGuildUID())								
				SendGuildWarPreWinGuild(true);
			USES_CONVERSION;
		}
		break;

	case QUERY_DELGUILDMEMBER:			// 길드원제거(탈퇴/추방) 결과
		{
			const TADelGuildMember *pPacket = reinterpret_cast<TADelGuildMember*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				// 오류발생
				if (!pPacket->bIsExiled) {
					SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, pPacket->nRetCode, NULL, false);
				}
				else {
					SendExileGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, pPacket->nRetCode, NULL);
				}
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				if (!pPacket->bIsExiled)
				{
#ifdef PRE_ADD_BEGINNERGUILD
					SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL, pPacket->bIsGraduateBeginnerGuild);
#else		//#ifdef PRE_ADD_BEGINNERGUILD
					SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL, false);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
				}
				else
					SendExileGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif


			TGuildMember aDelGuildMember;
			if (!pGuild->IsMemberExist(pPacket->nDelCharacterDBID, &aDelGuildMember)) 
			{
				if (!pPacket->bIsExiled)
				{
#ifdef PRE_ADD_BEGINNERGUILD
					SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDMEMBERINFO, NULL, pPacket->bIsGraduateBeginnerGuild);
#else		//#ifdef PRE_ADD_BEGINNERGUILD
					SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDMEMBERINFO, NULL, false);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
				}
				else
					SendExileGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDMEMBERINFO, NULL);
				break;
			}

			pGuild->DelMember(pPacket->nDelCharacterDBID);
#ifdef PRE_ADD_BEGINNERGUILD
			CDNUserSession * pDelSession = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nDelAccountDBID);
			if (pDelSession && pDelSession->GetPartyID() > 0)
			{
				CDNParty * pParty = g_pPartyManager->GetParty(pDelSession->GetPartyID());
				if (pParty)
					pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
			}
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

			// 길드시각정보초기화
			if (!pPacket->bIsExiled) 
			{
				// 탈퇴
				ResetGuildSelfView();
#ifdef PRE_ADD_BEGINNERGUILD
				SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_NONE, &GuildUID, pPacket->bIsGraduateBeginnerGuild);
#else		//#ifdef PRE_ADD_BEGINNERGUILD
				SendLeaveGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, ERROR_NONE, &GuildUID, false);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
				GetItem()->RemoveGuildReversionItem( true );
				GetItem()->RemoveGuildReversionVehicleItemData( true );
				SendGuildRewardItem(NULL);
			}
			else 
			{
				// 추방
				CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nDelAccountDBID);
				if (pUserObj && pUserObj->GetGuildUID().IsSet() && pGuild->GetUID() == pUserObj->GetGuildUID() && pUserObj->GetCharacterDBID() == pPacket->nDelCharacterDBID)
				{
					pUserObj->ResetGuildSelfView();
					pUserObj->SendExileGuildMember(pUserObj->GetAccountDBID(), pUserObj->GetCharacterDBID(), ERROR_NONE, &GuildUID);
					pUserObj->GetItem()->RemoveGuildReversionItem( true );
					pUserObj->GetItem()->RemoveGuildReversionVehicleItemData( true );
					pUserObj->SendGuildRewardItem(NULL);
				}
			}

			// 현재서버에접속중인길드원들에게통보
			pGuild->SendDelGuildMember(pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, pPacket->bIsExiled);

			// MA 를통하여다른서버에접속중인길드원들에게도통보
			g_pMasterConnection->SendDelGuildMember(GuildUID, pPacket->nDelAccountDBID, pPacket->nDelCharacterDBID, pPacket->bIsExiled);

			// 추방여부를 판단하여 히스토리에 추가
			BYTE btHistoryType = GUILDHISTORY_TYPE_LEAVE;
			if (pPacket->bIsExiled) 
				btHistoryType = GUILDHISTORY_TYPE_EXILE;
		}
		break;

	case QUERY_CHANGEGUILDINFO:			// 길드정보변경결과
		{
			const TAChangeGuildInfo *pPacket = reinterpret_cast<TAChangeGuildInfo*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendChangeGuildInfo(pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, pPacket->nRetCode, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild)
			{
				SendChangeGuildInfo(pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, ERROR_GUILD_NOTEXIST_GUILDINFO, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) 
			{
				SendChangeGuildInfo(pPacket->nAccountDBID, pPacket->nCharacterDBID, pPacket->btGuildUpdate, ERROR_GUILD_NOTEXIST_GUILDINFO, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
				break;
			}
#endif

			pGuild->UpdateGuildInfo(this, pPacket);
		}
		break;

	case QUERY_CHANGEGUILDMEMBERINFO:		// 길드원정보변경결과
		{
			const TAChangeGuildMemberInfo *pPacket = reinterpret_cast<TAChangeGuildMemberInfo*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) {
				// 오류발생
				SendChangeGuildMemberInfo(pPacket->nReqAccountDBID, pPacket->nReqCharacterDBID, pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, pPacket->nRetCode, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
				break;
			}
			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendChangeGuildMemberInfo(pPacket->nReqAccountDBID, pPacket->nReqCharacterDBID, pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GENERIC_UNKNOWNERROR, pPacket->nInt1, pPacket->nInt2, pPacket->biInt64, pPacket->Text, NULL);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
				pGuildVillage->UpdateMemberInfo(this, pPacket);
		}
		break;

	// 길드원 초대 결과
	case QUERY_INVITEGUILDMEMBER:		
		{
			TAInviteGuildMember *pMsg = reinterpret_cast<TAInviteGuildMember*>(pData);

			TAGetGuildInfo *pPacket = reinterpret_cast<TAGetGuildInfo*>(&pMsg->GuildInfo);

			if (ERROR_NONE != pPacket->nRetCode || GUILDDBID_DEF == pPacket->nGuildDBID) 
			{
				// 해당유저에게길드정보가없다고전송
				SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0, 0, 0, NULL, NULL);
				break;
			}

			/////////////////////////////////////////////////////////////////////////////
			// 길드자원확보
			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);
			bool bIsNew = false;

			// 길드권한적용(허용/불가)
			g_pGuildManager->SetAuthMust(pPacket->Info.wGuildRoleAuth);

			g_pGuildManager->SetRoleLimitation(&pPacket->Info);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				// 길드자원을새로할당받음
				pGuild = g_pGuildManager->Get(&pPacket->Info);
				if (!pGuild) 
				{
					// 할당오류?(확인필요)
					DN_BREAK;
				}
				bIsNew = true;
			}
			else {
				pGuild->Set(&pPacket->Info);
				bIsNew = false;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 길드자원이 할당하자마자 바로 해제하지 않도록 해당플래그를 리셋시킨다.
			pGuild->ResetCheckEmpty();

			/////////////////////////////////////////////////////////////////////////////

			// 인원추가가능여부체크
			if (!pGuild->IsMemberAddable()) 
			{
				SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_GUILD_CANTADD_GUILDMEMBER, 0, 0, 0, 0, NULL, NULL);
				break;
			}

			// 길드원 추가한다.
			m_pDBCon->QueryAddGuildMember(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), pPacket->nGuildDBID, GetLevel(), g_Config.nWorldSetID, pGuild->GetInfo()->wGuildSize);
		}
		break;

	case QUERY_MOVE_ITEM_IN_GUILDWARE:		// 길드창고간 아이템 이동
		{
			TAMoveItemInGuildWare *pPacket = reinterpret_cast<TAMoveItemInGuildWare*>(pData);

			if (ERROR_NONE != pPacket->nRetCode || GUILDDBID_DEF == pPacket->nGuildDBID) 
			{
				SendMoveGuildItem(MoveType_GuildWare, 0, 0, NULL, NULL, pPacket->nRetCode);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = NULL;
				pGuildWare = pGuildVillage->GetGuildWare();

				if (pGuildWare)
				{
					int nRet = pGuildWare->OnMoveItemInGuildWare(this, pPacket);
					if (ERROR_NONE != nRet)
					{
						SendMoveGuildItem(MoveType_GuildWare, 0, 0, NULL, NULL, nRet);
						break;
					}
				}
				else
				SendMoveGuildItem(MoveType_GuildWare, 0, 0, NULL, NULL, ERROR_GUILDWARE_NOT_READY);
			}
		}
		break;

	case QUERY_MOVE_INVEN_TO_GUILDWARE:		// 인벤 -> 길드창고로 아이템 이동
		{
			TAMoveInvenToGuildWare *pPacket = reinterpret_cast<TAMoveInvenToGuildWare*>(pData);

			if (ERROR_NONE != pPacket->nRetCode || GUILDDBID_DEF == pPacket->nGuildDBID) 
			{
				SendMoveGuildItem(MoveType_InvenToGuildWare, 0, 0, NULL, NULL, pPacket->nRetCode);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = pGuildVillage->GetGuildWare();
				if (pGuildWare)
				{
					int nRet = pGuildWare->OnMoveInvenToGuildWare(this, pPacket);
					if (ERROR_NONE != nRet)
					{
						SendMoveGuildItem(MoveType_InvenToGuildWare, 0, 0, NULL, NULL, nRet);
						break;
					}
				}
				else
					SendMoveGuildItem(MoveType_InvenToGuildWare, 0, 0, NULL, NULL, ERROR_GUILDWARE_NOT_READY);
			}
		}
		break;

	case QUERY_MOVE_GUILDWARE_TO_INVEN:		// 길드창고 -> 인벤로 아이템 이동
		{
			TAMoveGuildWareToInven *pPacket = reinterpret_cast<TAMoveGuildWareToInven*>(pData);

			if (GUILDDBID_DEF == pPacket->nGuildDBID) 
			{
				SendMoveGuildItem(MoveType_GuildWareToInven, 0, 0, NULL, NULL, ERROR_GUILD_NOTEXIST_GUILD);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				if (ERROR_NONE != pPacket->nRetCode)
				{
					SendMoveGuildItem(MoveType_GuildWareToInven, 0, 0, NULL, NULL, pPacket->nRetCode);
					pGuildVillage->SendGuildWareList (this, 0);
					break;
				}
				CDNGuildWare* pGuildWare = pGuildVillage->GetGuildWare();
				if (pGuildWare)
				{	
					int nRet = pGuildWare->OnMoveGuildWareToInven(this, pPacket);
					if (ERROR_NONE != nRet)
					{
						SendMoveGuildItem(MoveType_GuildWareToInven, 0, 0, NULL, NULL, nRet);
						break;
					}
				}
				else
					SendMoveGuildItem(MoveType_GuildWareToInven, 0, 0, NULL, NULL, ERROR_GUILDWARE_NOT_READY);
			}
		}
		break;

	case QUERY_GUILDWARE_COIN:		// 코인이동
		{
			TAGuildWareHouseCoin *pPacket = reinterpret_cast<TAGuildWareHouseCoin*>(pData);

			if (ERROR_NONE != pPacket->nRetCode || GUILDDBID_DEF == pPacket->nGuildDBID) 
			{
				SendMoveGuildCoin(pPacket->cMoveType, 0, 0, 0, 0, pPacket->nRetCode);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = NULL;
				pGuildWare = pGuildVillage->GetGuildWare();

				if (pGuildWare)
				{	
					int nRet = pGuildWare->OnModGuildCoin(this, pPacket);
					if (ERROR_NONE != nRet)
					{
						SendMoveGuildCoin(pPacket->cMoveType, 0, 0, 0, 0, pPacket->nRetCode);
						break;
					}
				}
				else
					SendMoveGuildCoin(pPacket->cMoveType, 0, 0, 0, 0, ERROR_GUILDWARE_NOT_READY);
			}
		}
		break;

	case QUERY_GETGUILDHISTORYLIST:		// 길드히스토리목록결과
		{
			const TAGetGuildHistoryList *pPacket = reinterpret_cast<TAGetGuildHistoryList*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendGetGuildHistoryList(TGuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID), 0, 0, 0, 0, pPacket->nRetCode, NULL, 0);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendGetGuildHistoryList(GuildUID, 0, 0, 0, 0, ERROR_GENERIC_UNKNOWNERROR, NULL, 0);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			SendGetGuildHistoryList(GuildUID, pPacket->biIndex, pPacket->bDirection, pPacket->nCurCount, pPacket->nTotCount, ERROR_NONE, pPacket->HistoryList, MAX(pPacket->nCount, 0));
		}
		break;

	case QUERY_GET_GUILDWAREHISTORY:	// 길드창고 히스토리 결과
		{
			TAGetGuildWareHistory *pPacket = reinterpret_cast<TAGetGuildWareHistory*>(pData);

			// 패킷 조사
			if (ERROR_NONE != pPacket->nErrNo || GUILDDBID_DEF == pPacket->nGuildDBID)
			{
				SendGetGuildWareHistoryList(TGuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID), 0, 0, 0, pPacket->nErrNo, NULL, 0);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendGetGuildWareHistoryList(GuildUID, 0, 0, 0, ERROR_GENERIC_UNKNOWNERROR, NULL, 0);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			SendGetGuildWareHistoryList(GuildUID, 0, pPacket->nCurrCount, pPacket->nTotalCount, pPacket->nErrNo, pPacket->HistoryList, pPacket->nCount);
		}
		break;

	case QUERY_CHANGEGUILDWARESIZE:		// 길드창고 변경결과
		{
			TAChangeGuildWareSize *pPacket = reinterpret_cast<TAChangeGuildWareSize*>(pData);

			// 패킷 조사
			if (ERROR_NONE != pPacket->nRetCode || GUILDDBID_DEF == pPacket->nGuildDBID)
				break;

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			// 길드창고 정보 반영
			pGuild->SetInfo()->wGuildWareSize = pPacket->wStorageSize;

			// 마스터서버를 통해 다른 길드에 알려준다
			if (g_pMasterConnection)
				g_pMasterConnection->SendExtendGuildWareSize(GuildUID, pPacket->wStorageSize);

			// 길드원에게 알려준다
			pGuild->SendExtendGuildWare((short)pPacket->wStorageSize);
		}
		break;

	case QUERY_CHANGEGUILDNAME :
		{
			const TAChangeGuildName *pPacket = reinterpret_cast<TAChangeGuildName*>(pData);
			if (ERROR_NONE != pPacket->nRetCode)
			{
				SendGuildRenameResult(pPacket->nRetCode);
				break;
			}
			// 아이템삭제, 길드 이름 변경 Send 
			int nItemID = 0;
			const TItem *pItem = m_pItem->GetCashInventory(pPacket->biItemSerial);
			if (pItem)
				nItemID = pItem->nItemID;

			if (m_pItem->DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biItemSerial))
			{
				GetEventSystem()->OnEvent( EventSystem::OnItemUse, 1, EventSystem::ItemID, nItemID );
				const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);
				g_pMasterConnection->SendGuildChangeName(GuildUID, pPacket->wszGuildName);
				SendGuildRenameResult(ERROR_NONE);
			}			
		}
		break;

	case QUERY_CHANGEGUILDMARK :
		{
			const TAChangeGuildMark *pPacket = reinterpret_cast<TAChangeGuildMark*>(pData);
			if (ERROR_NONE != pPacket->nRetCode)
			{
				SendGuildMarkResult(pPacket->nRetCode, 0, 0, 0);
				break;
			}
			
			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);
			if (g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendGuildChangeMark(GuildUID, pPacket->wGuildMark, pPacket->wGuildMarkBG, pPacket->wGuildMarkBorder);

			SendGuildMarkResult(pPacket->nRetCode, pPacket->wGuildMark, pPacket->wGuildMarkBG, pPacket->wGuildMarkBorder);
		}
		break;

	case QUERY_ENROLL_GUILDWAR:				// 길드전 신청
		{
			const TAEnrollGuildWar * pPacket = reinterpret_cast<TAEnrollGuildWar*>(pData);
			// 패킷 조사
			if (ERROR_NONE != pPacket->nRetCode)
			{
				SendEnrollGuildWar(pPacket->nRetCode);
				break;
			}

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendEnrollGuildWar(ERROR_GENERIC_UNKNOWNERROR);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			if (pPacket->wScheduleID <= 0 || pPacket->cTeamColorCode <= 0)
			{
				SendEnrollGuildWar(ERROR_GENERIC_UNKNOWNERROR);
				break;
			}

			pGuild->UpdateWarInfoOnAllMember(pPacket->wScheduleID, pPacket->cTeamColorCode);

			// 마스터를 통해 알린다.
			if (g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendEnrollGuildWar(GuildUID, pPacket->wScheduleID, pPacket->cTeamColorCode);

		}
		break;
	case QUERY_ADD_GUILDREWARDITEM:
		{
			//하나씩 받은 경우
			TAAddGuildRewardItem *pGuildRewardItem = (TAAddGuildRewardItem*)pData;

			if (ERROR_NONE != pGuildRewardItem->nRetCode)
			{
				SendBuyGuildRewardItem(pGuildRewardItem->nRetCode, pGuildRewardItem->nItemID);
				break;
			}

			SendBuyGuildRewardItem(pGuildRewardItem->nRetCode, pGuildRewardItem->nItemID);

			const TGuildUID GuildUID(pGuildRewardItem->cWorldSetID, pGuildRewardItem->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendBuyGuildRewardItem(ERROR_GENERIC_UNKNOWNERROR, pGuildRewardItem->nItemID);
				break;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif

			TGuildRewardItem RewardItemInfo;
			memset( &RewardItemInfo, 0, sizeof(RewardItemInfo) );
			pGuild->AddGuildRewardItem(pGuildRewardItem->nItemID, pGuildRewardItem->m_tExpireDate, RewardItemInfo, this);
			pGuild->SendAddGuildRewardItem(RewardItemInfo);

			// 마스터를 통해 알린다.
			if (g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendAddGuildRewardItem(GuildUID, pGuildRewardItem);
		}
		break;
	case QUERY_CHANGE_GUILDSIZE:
		{
			TAChangeGuildSize *pPacket = (TAChangeGuildSize*)pData;

			if (ERROR_NONE != pPacket->nRetCode)			
				break;

			const TGuildUID GuildUID(pPacket->cWorldSetID, pPacket->nGuildDBID);

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
				break;

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) break;
#endif
			
			pGuild->SetInfo()->wGuildSize = pPacket->nGuildSize;

			// 마스터서버를 통해 다른 길드에 알려준다
			if (g_pMasterConnection)
				g_pMasterConnection->SendExtendGuildSize(GuildUID, pPacket->nGuildSize);

			// 길드원에게 알려준다
			pGuild->SendExtendGuildSize((short)pPacket->nGuildSize);
		}
		break;
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
	case QUERY_GET_GUILDCONTRIBUTION_POINT:
		{
			TAGetGuildContributionPoint* pPacket = reinterpret_cast<TAGetGuildContributionPoint*>(pData);

			if (pPacket->nRetCode != ERROR_NONE)
				break;

			SendGuildContributionPoint(pPacket->nWeeklyGuildContributionPoint, pPacket->nTotalGuildContributionPoint);
		}
		break;
	case QUERY_GET_GUILDCONTRIBUTION_WEEKLYRANKING:
		{
			TAGetGuildContributionRanking* pPacket = reinterpret_cast<TAGetGuildContributionRanking*>(pData);

			if (pPacket->nRetCode != ERROR_NONE)
				break;
		
			SendGuildWeeklyContributionRankList(pPacket);
		}
		break;
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	}
}

void CDNUserSession::OnDBRecvMail(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvMail(nSubCmd, pData);

	switch(nSubCmd)
	{
	case QUERY_GETPAGERECEIVEMAIL:
		{
			const TAGetPageReceiveMail *pMail = (TAGetPageReceiveMail*)pData;

			switch(pMail->nRetCode)
			{
			case ERROR_NONE:
				m_pItem->SetDailyMailCount(pMail->iBasicMailCount);
				break;

			case 103165:	// 103165 받은 메일이 없습니다.
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pMail->nRetCode);
				DetachConnection(L"QUERY_GETPAGERECEIVEMAIL");
				return;
			}

			SendMailBox(pMail->sMail, pMail->iTotalMailCount, pMail->iNotReadMailCount, pMail->cMailCount, pMail->iBasicMailCount, pMail->nRetCode);
		}
		break;


	case QUERY_SENDMAIL:
		{
			const TASendMail *pSendMail = (TASendMail*)pData;

			switch(pSendMail->nRetCode)
			{
			case ERROR_NONE:
				{
					m_pItem->SetDailyMailCount(pSendMail->nBasicMailCount);

					if (pSendMail->biAttachCoin > 0){
						DelCoin(pSendMail->biAttachCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고
					}
					// 수수료
					if (pSendMail->nTax > 0){
						DelCoin(pSendMail->nTax, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고
					}

					if (pSendMail->Code == MailType::Premium){	// 프리미엄이면 우표 지워줘야함
						if (!m_pItem->UseCashItemByType(ITEMTYPE_PREMIUM_POST, 1, true)){
							DetachConnection(L"QUERY_SENDMAIL");
							return;
						}
					}

					for (int i = 0; i < MAILATTACHITEMMAX; i++){
						if (pSendMail->AttachItemArr[i].Item.nItemID <= 0) continue;

						if (m_pItem->IsValidInventorySlot(pSendMail->AttachItemArr[i].cSlotIndex, pSendMail->AttachItemArr[i].Item.nSerial)){	// 인벤에 아이템이 있으면...
							if (!m_pItem->DeleteInventoryBySlot(pSendMail->AttachItemArr[i].cSlotIndex, pSendMail->AttachItemArr[i].Item.wCount, pSendMail->AttachItemArr[i].Item.nSerial, 0)){	// 인벤에서 빼주고
								DetachConnection(L"QUERY_SENDMAIL");
								return;
							}
						}
						else
						{
							DetachConnection(L"QUERY_SENDMAIL");
							return;
						}
					}

					if (pSendMail->nReceiverAccountDBID != m_nAccountDBID){
						CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pSendMail->nReceiverAccountDBID);	// 받는이 찾자
						if (pUserObj && (pUserObj->GetCharacterDBID() == pSendMail->biReceiverCharacterDBID)){	// 같은 서버 내에 있다면 걍 처리
							pUserObj->SendNotifyMail(pSendMail->nReceiverTotalMailCount, pSendMail->nReceiverNotReadMailCount, pSendMail->nReceiver7DaysLeftMailCount, true);	// 메일 온거 알려준다
						}
						else {	// 없으면 master로 
							g_pMasterConnection->SendNotifyMail(pSendMail->nReceiverAccountDBID, pSendMail->biReceiverCharacterDBID, pSendMail->nReceiverTotalMailCount, pSendMail->nReceiverNotReadMailCount, pSendMail->nReceiver7DaysLeftMailCount, true);
						}
					}
				}
				break;

			case 103123:
			case 103162:
			case 103163:
			case 103164:
				m_pItem->DelDailyMailCount();
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pSendMail->nRetCode);
				DetachConnection(L"QUERY_SENDMAIL");
				return;
			}

			SendMailSendResult(pSendMail->nRetCode, m_pItem->GetDailyMailCount());
		}
		break;

	case QUERY_READMAIL:
		{
			const TAReadMail *pRead = (TAReadMail*)pData;

			TItem Item[MAILATTACHITEMMAX] = { 0, };
			int nAttachCoin = 0;
			switch(pRead->nRetCode)
			{
			case ERROR_NONE:
				{
#ifdef PRE_ADD_JOINGUILD_SUPPORT
					m_pItem->SetReadMail(pRead->nMailDBID, pRead->biAttachCoin, pRead->sAttachItem, pRead->cMailType);	// 일단 읽은메일 세팅해놓자. 첨부받기때 참조하기위해
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
					m_pItem->SetReadMail(pRead->nMailDBID, pRead->biAttachCoin, pRead->sAttachItem);	// 일단 읽은메일 세팅해놓자. 첨부받기때 참조하기위해
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
					nAttachCoin = (int)pRead->biAttachCoin;
				}
				break;

			case 103166:	// 103166 존재하지 않는 메일입니다.
			{
				int iInvalidCount = 5;
				g_Log.Log( LogType::_HACK, this, L"HackChecker(Mail) : CharName=%s InvalidCount=%d \n", GetCharacterName(), iInvalidCount );
				GetDBConnection()->QueryAddAbuseMonitor( this, iInvalidCount, 0);
				break;
			}

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pRead->nRetCode);
				DetachConnection(L"QUERY_READMAIL");
				return;
			}

			SendReadMailResult(pRead, nAttachCoin);
		}
		break;

	case QUERY_DELMAIL:
		{
			const TADelMail *pMail = (TADelMail*)pData;

			if (pMail->nRetCode != ERROR_NONE){
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pMail->nRetCode);
				DetachConnection(L"QUERY_DELMAIL");
				break;
			}

			SendDeleteMailResult(pMail->nRetCode);
		}
		break;

	case QUERY_GETLISTMAILATTACHMENT:
		{
			const TAGetListMailAttachment *pMail = (TAGetListMailAttachment*)pData;

			if (pMail->nRetCode != ERROR_NONE){
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pMail->nRetCode);
				DetachConnection(L"QUERY_GETLISTMAILATTACHMENT");
				break;
			}

			if (pMail->cCount == 0){	// 첨부된 우편이 없는경우
				SendAttachAllMailResult(NULL, ERROR_MAIL_DOESNOTEXIST_ATTACHED_THING);
				break;
			}

			int nRet = m_pItem->VerifyAttachItemList(pMail);
			if (nRet != ERROR_NONE){
				int nMailDBID[MAILPAGEMAX] = { 0, };
				for (int i = 0; i < MAILPAGEMAX; i++){
					nMailDBID[i] = pMail->MailAttachArray[i].nMailDBID;
				}

				SendAttachAllMailResult(nMailDBID, nRet);
				break;
			}
		}
		break;

	case QUERY_TAKEATTACHMAILLIST:
		{
			TATakeAttachMailList *pMail = (TATakeAttachMailList*)pData;

			if (pMail->nRetCode != ERROR_NONE){
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pMail->nRetCode);
				DetachConnection(L"QUERY_TAKEATTACHMAILLIST");
				break;
			}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
			bool bAddGuildWare = false;
			int nMailDBID[MAILPAGEMAX] = { 0, };
			for (int i = 0; i < MAILPAGEMAX; i++)
			{
				if (pMail->TakeMailList[i].bAddGuildWare)
					bAddGuildWare = true;
				
				nMailDBID[i] = pMail->TakeMailList[i].nMailDBID;
			}

			int nRet = m_pItem->TakeAttachItemList(pMail);
			SendAttachAllMailResult(nMailDBID, nRet);

			CDNGuildBase* pGuild = NULL;
			if (bAddGuildWare)
			{
				if (GetGuildUID().IsSet())
				{
					if (pGuild == NULL)
						pGuild = g_pGuildManager->At(GetGuildUID());

					if (pGuild) 
					{
						bool bGuildCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
						CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
						if (FALSE == pGuild->IsEnable())
							bGuildCheck = false;
#endif

						if (bGuildCheck)
						{
							// 빌리지길드로변환
							_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
							CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
							if (pGuildVillage)
							{
								CDNGuildWare* pGuildWare = pGuildVillage->GetGuildWare();
								if (pGuildWare)
								{
									for (int i = 0; i < pMail->cPageCount; i++)
									{
										if (pMail->TakeMailList[i].bAddGuildWare)
										{
											if (pMail->TakeMailList[i].nMailDBID <= 0) continue;
											if (pMail->TakeMailList[i].biAttachCoin > 0)
												pGuildWare->OnAddGuildCoin(this, pMail->TakeMailList[i].biAttachCoin);
											pGuildWare->OnAddInGuildWare(this, &pMail->TakeMailList[i]);
										}
									}
								}
								/*else
									SendMoveGuildItem(MoveType_InvenToGuildWare, 0, 0, NULL, NULL, ERROR_GUILDWARE_NOT_READY);*/
							}
						}
						else
							_DANGER_POINT();
					}
				}
				else
					g_Log.Log(LogType::_ERROR, this, L"QUERY_TAKEATTACHMAILLIST AddGuildWare NoGuild!!! [%d]\n", GetGuildUID().nDBID);
			}
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
			int nMailDBID[MAILPAGEMAX] = { 0, };
			for (int i = 0; i < MAILPAGEMAX; i++)
			{
				nMailDBID[i] = pMail->TakeMailList[i].nMailDBID;
			}

			int nRet = m_pItem->TakeAttachItemList(pMail);
			SendAttachAllMailResult(nMailDBID, nRet);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
		}
		break;

	case QUERY_TAKEATTACHMAIL:
		{
			TATakeAttachMail *pMail = (TATakeAttachMail*)pData;

			if (pMail->nRetCode != ERROR_NONE){
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MAIL, nSubCmd, pMail->nRetCode);
				DetachConnection(L"QUERY_TAKEATTACHMAIL");
				break;
			}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
			if (pMail->TakeMail.bAddGuildWare == false)
			{
				int nRet = m_pItem->TakeAttachItem(pMail);
				SendAttachMailResult(nRet);
			}
			else
			{
				if (pMail->nRetCode == ERROR_NONE && GetGuildUID().IsSet())
				{
					CDNGuildBase* pGuild = g_pGuildManager->At(GetGuildUID());
					if (pGuild) 
					{
						bool bGuildCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
						CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
						if (FALSE == pGuild->IsEnable())
							bGuildCheck = false;
#endif

						if (bGuildCheck)
						{
							// 빌리지길드로변환
							_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
							CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
							if (pGuildVillage)
							{
								CDNGuildWare* pGuildWare = pGuildVillage->GetGuildWare();
								if (pGuildWare)
								{
									if (pMail->TakeMail.biAttachCoin > 0)
										pGuildWare->OnAddGuildCoin(this, pMail->TakeMail.biAttachCoin);
									pGuildWare->OnAddInGuildWare(this, &pMail->TakeMail);
								}
								else
									_DANGER_POINT();
							}
							else
								_DANGER_POINT();
						}
						else
							_DANGER_POINT();
					}
					else
						_DANGER_POINT();
				}
				else
					g_Log.Log(LogType::_ERROR, this, L"QUERY_TAKEATTACHMAILLIST AddGuildWare NoGuild!!! [%d]\n", GetGuildUID().nDBID);

				SendAttachMailResult(pMail->nRetCode);
			}
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
			int nRet = m_pItem->TakeAttachItem(pMail);
			SendAttachMailResult(nRet);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
		}
		break;

#ifdef PRE_ADD_BEGINNERGUILD
	case QUERY_GETWILLSENDMAIL:
		{
			TAGetWillMails * pPacket = (TAGetWillMails*)pData;

			if (pPacket->nRetCode == ERROR_NONE && GetDBConnection())
			{
				for (int i = 0; i < pPacket->cCount; i++)
				{	
					GetDBConnection()->QueryDelWillSendMails(this, pPacket->Mails[i].nMailID);
					CDNMailSender::Process(this, pPacket->Mails[i].nMailID, DBDNWorldDef::PayMethodCode::Comeback, 0);
					g_Log.Log(LogType::_BEGINNETGUILD, this, L"AccountDBID[%d] BeginnerGuild!! MailSended! MailID[%d]\n", GetAccountDBID(), pPacket->Mails[i].nMailID);
				}
			}
		}
		break;
#endif	//#ifdef PRE_ADD_BEGINNERGUILD

#if defined(PRE_ADD_CADGE_CASH)
	case QUERY_SENDWISHMAIL:
		{
			const TASendWishMail *pMail = (TASendWishMail*)pData;

			switch(pMail->nRetCode)
			{
			case ERROR_NONE:
				{
					if (pMail->nReceiverAccountDBID != m_nAccountDBID){
						CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pMail->nReceiverAccountDBID);	// 받는이 찾자
						if (pUserObj && (pUserObj->GetCharacterDBID() == pMail->biReceiverCharacterDBID)){	// 같은 서버 내에 있다면 걍 처리
							pUserObj->SendNotifyMail(pMail->nReceiverTotalMailCount, pMail->nReceiverNotReadMailCount, pMail->nReceiver7DaysLeftMailCount, true);	// 메일 온거 알려준다
						}
						else {	// 없으면 master로 
							g_pMasterConnection->SendNotifyMail(pMail->nReceiverAccountDBID, pMail->biReceiverCharacterDBID, pMail->nReceiverTotalMailCount, pMail->nReceiverNotReadMailCount, pMail->nReceiver7DaysLeftMailCount, true);
						}
					}
				}
				break;

			case 103339:	// 1일 가능 조르기 횟수를 초과하였습니다.
				break;
			}

			SendCashShopCadge(pMail->nRetCode);
		}
		break;

	case QUERY_READWISHMAIL:
		{
			const TAReadWishMail *pMail = (TAReadWishMail*)pData;

			switch(pMail->nRetCode)
			{
			case ERROR_NONE:
				{

				}
				break;

			case 103166:	// 존재하지 않는 메일입니다.
			case 103340:	// 조르기 아이템이 없습니다.
				break;
			}

			SendReadCadgeMailResult(pMail);
		}
		break;
#endif	// #if defined(PRE_ADD_CADGE_CASH)

#if defined( PRE_MOD_71820 )
#else
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	case QUERY_GUILDSUPPORT_REWARDINFO:
		{
			TAGuildSupportRewardInfo * pPacket = (TAGuildSupportRewardInfo*)pData;

			if (pPacket->nRetCode == ERROR_NONE)
			{
				m_bWasRewardedGuildMaxLevel = pPacket->bWasGuildSupportRewardFlag;
				m_cGuildJoinLevel = pPacket->cJoinGuildLevel;

				if (m_bWasRewardedGuildMaxLevel == false && GetLevel() >= (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerLevelLimit))
				{
					int nMailID = g_pDataManager->GetRewardGuildSupportMailID(m_cGuildJoinLevel);
					if (nMailID > 0)
					{
						const TGuildUID GuildUID = GetGuildUID();
						if (GuildUID.IsSet())
						{
							CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
							if (pGuild) 
							{
#if !defined( PRE_ADD_NODELETEGUILD )
								CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
								if (TRUE == pGuild->IsEnable())
								{
									TGuildMember * pGuildMaster = pGuild->GetGuildMaster();
									if (pGuildMaster)
									{
										CDNMailSender::Process(pGuildMaster->nAccountDBID, pGuildMaster->nCharacterDBID, GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), GetWorldSetID(), nMailID);
										m_bWasRewardedGuildMaxLevel = true;
									}
								}
#else		//#if !defined( PRE_ADD_NODELETEGUILD )
								TGuildMember * pGuildMaster = pGuild->GetGuildMaster();
								if (pGuildMaster)
								{
									CDNMailSender::Process(pGuildMaster->nAccountDBID, pGuildMaster->nCharacterDBID, GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), GetWorldSetID(), nMailID);
									m_bWasRewardedGuildMaxLevel = true;
								}
#endif		//#if !defined( PRE_ADD_NODELETEGUILD )
							}
						}
						else
							_DANGER_POINT();
					}
					else
						g_Log.Log(LogType::_ERROR, m_pSession, L"QUERY_GUILDSUPPORT_REWARDINFO Error MailID Invalid [%d]\n", nMailID);
				}
			}			
		}
		break;

	case QUERY_SENDGUILDMAIL:
		{
			TASendGuildMail * pPacket = (TASendGuildMail*)pData;
			if (pPacket->nRetCode == ERROR_NONE)
			{
				if (pPacket->nReceiverAccountDBID != GetAccountDBID())
				{
					CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nReceiverAccountDBID);	// 받는이 찾자
					if (pUserObj && (pUserObj->GetCharacterDBID() == pPacket->biReceiverCharacterDBID)){	// 같은 서버 내에 있다면 걍 처리
						pUserObj->SendNotifyMail(pPacket->iTotalMailCount, pPacket->iNotReadMailCount, pPacket->i7DaysLeftMailCount, true);	// 메일 온거 알려준다
					}
					else {	// 없으면 master로 
						g_pMasterConnection->SendNotifyMail(pPacket->nReceiverAccountDBID, pPacket->biReceiverCharacterDBID, pPacket->iTotalMailCount, pPacket->iNotReadMailCount, pPacket->i7DaysLeftMailCount, true);
					}
				}
			}
		}
		break;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
#endif // #if defined( PRE_MOD_71820 )
	}
}

void CDNUserSession::OnDBRecvMarket(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvMarket(nSubCmd, pData);

	switch(nSubCmd)
	{
	case QUERY_GETPAGETRADE:
		{
			TAGetPageTrade *pMarketList = (TAGetPageTrade*)pData;

			switch(pMarketList->nRetCode)
			{
			case ERROR_NONE:
			case 103171:	// 103171 조건을 만족하는 아이템이 없습니다.
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarketList->nRetCode);
				DetachConnection(L"QUERY_GETPAGETRADE");
				return;
			}

			SendMarketList(pMarketList->MarketInfo, pMarketList->cMarketCount, pMarketList->nMarketTotalCount, pMarketList->nRetCode);
		}
		break;

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	case QUERY_GETLISTMINITRADE:
		{
			TAGetListMiniTrade *pMarketList = (TAGetListMiniTrade*)pData;

			switch(pMarketList->nRetCode)
			{
			case ERROR_NONE:
			case 103171:	// 103171 조건을 만족하는 아이템이 없습니다.
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarketList->nRetCode);
				DetachConnection(L"QUERY_GETPAGETRADE");
				return;
			}

			SendMarketMiniList(pMarketList->MarketInfo, pMarketList->cMarketCount, pMarketList->nMarketTotalCount, pMarketList->nRetCode);
		}
		break;
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

	case QUERY_GETLISTMYTRADE:
		{
			TAGetListMyTrade *pMarketList = (TAGetListMyTrade*)pData;

			switch(pMarketList->nRetCode)
			{
			case ERROR_NONE:
				m_pItem->SetRegisterMarketCount(pMarketList->wWeeklyRegisterCount, pMarketList->wRegisterItemCount);
				break;

			case 103172:	// 103172 무인 상점에 등록한 아이템이 없습니다.
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarketList->nRetCode);
				DetachConnection(L"QUERY_GETLISTMYTRADE");
				return;
			}

			m_pItem->SetMyMarketList(pMarketList->MarketInfo, pMarketList->cMarketCount);
			bool bPremiumTrade = ( m_pItem && m_pItem->GetCashItemCountByType( ITEMTYPE_PREMIUM_TRADE ) > 0 ) ? true : false;
			SendMyMarketList(pMarketList->MarketInfo, pMarketList->cMarketCount, pMarketList->wSellingCount, pMarketList->wExpiredCount, pMarketList->wWeeklyRegisterCount, pMarketList->wRegisterItemCount, pMarketList->nRetCode, bPremiumTrade);
		}
		break;

	case QUERY_ADDTRADE:
		{
			TAAddTrade *pMarket = (TAAddTrade*)pData;

			switch(pMarket->nRetCode)
			{
			case ERROR_NONE:
				{
					const TItem *pItem = NULL;
					switch(pMarket->cInvenType)
					{
					case ITEMPOSITION_INVEN:
						pItem = m_pItem->GetInventory(pMarket->cInvenIndex);
						break;

					case ITEMPOSITION_CASHINVEN:
						pItem = m_pItem->GetCashInventory(pMarket->biSerial);
						break;
					}

					if (!pItem)
					{
						DetachConnection(L"QUERY_ADDTRADE");
						return;
					}
					if (pItem->nSerial != pMarket->biSerial){
						DetachConnection(L"QUERY_ADDTRADE");
						return;
					}

					TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
					if (!pItemData){
						DetachConnection(L"QUERY_ADDTRADE");
						return;
					}

					INT64 nCharge = 0;
					if (pItem->nItemID > 0)
					{
#if defined(PRE_ADD_PETALTRADE)
						if ( pMarket->cMethodCode == DBDNWorldDef::PayMethodCode::Coin) // 코인일때만 등록 수수료.
#endif
						{					
							nCharge = g_pDataManager->CalcTax(TAX_TRADEREGISTER, GetLevel(), pMarket->nPrice);
							if (nCharge <= 0) nCharge = 1;	// 최소 1로 수수료 뗀다.
							if ( pMarket->bPremiumTrade ) {	//  프리미엄서비스 수수료로 다시 계산
								nCharge = g_pDataManager->CalcTax(TAX_TRADEREGISTER_CASH, GetLevel(), pMarket->nPrice);
								if (nCharge <= 0) nCharge = 0;	// 최소 0로 수수료 뗀다.
							}

							if (!CheckEnoughCoin(nCharge)){
								SendMarketRegister(pMarket->nMarketDBID, ERROR_ITEM_INSUFFICIENCY_MONEY);
								return;
							}
							DelCoin(nCharge, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 수수료 깎고
						}

						switch(pMarket->cInvenType)
						{
						case ITEMPOSITION_INVEN:
							if (m_pItem->IsValidInventorySlot(pMarket->cInvenIndex, pMarket->biSerial))
							{	// 인벤에 아이템이 있다면
								if (!m_pItem->DeleteInventoryBySlot(pMarket->cInvenIndex, pMarket->wCount, pMarket->biSerial, 0)){
									DetachConnection(L"QUERY_ADDTRADE");
									return;
								}
							}
							else
							{
								DetachConnection(L"QUERY_ADDTRADE");
								return;
							}
							break;

						case ITEMPOSITION_CASHINVEN:
							if (!m_pItem->DeleteCashInventoryBySerial(pMarket->biSerial, pMarket->wCount, false))
							{
								DetachConnection(L"QUERY_ADDTRADE");
								return;
							}
							break;
						}
					}

					m_pItem->AddRegisterItemCount();
					if ( pMarket->bPremiumTrade == false )	// 프리미엄 서비스가 아닌 경우만 증가시킨다.
						m_pItem->AddWeeklyRegisterCount();
				}
				break;

			case 103151:	// 103151 아이템이 존재하지 않습니다.
			case 103169:	// 103169 무인 상점 등록 가능 횟수를 초과하였습니다.
			case 103186:	// 103186 일주일간 무인 상점에 등록 가능한 총 횟수를 초과하였습니다.
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarket->nRetCode);
				DetachConnection(L"QUERY_ADDTRADE");
				return;
			}

			SendMarketRegister(pMarket->nMarketDBID, pMarket->nRetCode);
		}
		break;

	case QUERY_CANCELTRADE:
		{
			TACancelTrade *pMarket = (TACancelTrade*)pData;

			switch(pMarket->nRetCode)
			{
			case ERROR_NONE:
				{
					if (pMarket->Item.nItemID > 0){
						int nRet = ERROR_DB;

						pMarket->Item.bEternity = true;
						if (pMarket->bCashItem){
							nRet = m_pItem->CreateCashInvenWholeItem(pMarket->Item);
						}
						else{
							nRet = m_pItem->CreateInvenWholeItemByIndex(pMarket->cInvenSlotIndex, pMarket->Item);
						}

						if (nRet != ERROR_NONE){
							SendMarketInterrupt(pMarket->nMarketDBID, m_pItem->GetRegisterItemCount(), nRet);
							return;
						}
					}
					m_pItem->DelRegisterItemCount();

					// DelRegisterItemCount 이후에 처리해야 합니다.
					m_pItem->DelMyMarketList(pMarket->nMarketDBID);
				}
				break;

			case 103173:	// 103173 무인 상점에 등록 취소할 아이템이 존재하지 않습니다.
			case 103174:	// 103174 이미 팔린 아이템입니다.
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarket->nRetCode);
				DetachConnection(L"QUERY_CANCELTRADE");
				return;
			}

			SendMarketInterrupt(pMarket->nMarketDBID, m_pItem->GetRegisterItemCount(), pMarket->nRetCode);
		}
		break;

	case QUERY_BUYTRADEITEM:
		{
			TABuyTradeItem *pMarket = (TABuyTradeItem*)pData;

			switch(pMarket->nRetCode)
			{
			case ERROR_NONE:
				{
					if (pMarket->Item.nItemID > 0){	// 인벤체크
						if ((!g_pDataManager->IsCashItem(pMarket->Item.nItemID)) && (m_pItem->FindBlankInventorySlotCount() == 0)){
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							SendMarketBuy(pMarket->nMarketDBID, ERROR_ITEM_INVENTORY_NOTENOUGH, pMarket->bMini);
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							SendMarketBuy(pMarket->nMarketDBID, ERROR_ITEM_INVENTORY_NOTENOUGH, false);
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							return;
						}
					}

					if (pMarket->cPayMethodCode == DBDNWorldDef::PayMethodCode::Coin)
					{				
						if (!CheckEnoughCoin(pMarket->nPrice)){
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							SendMarketBuy(pMarket->nMarketDBID, ERROR_ITEM_INSUFFICIENCY_MONEY, pMarket->bMini);
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							SendMarketBuy(pMarket->nMarketDBID, ERROR_ITEM_INSUFFICIENCY_MONEY, false);
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							return;
						}
						DelCoin(pMarket->nPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 
					}

					if (pMarket->Item.nItemID > 0){	// 아이템 넣어주기
						int nRet = ERROR_DB;
						if (g_pDataManager->IsCashItem(pMarket->Item.nItemID)){
							pMarket->Item.bEternity = true;
							nRet = m_pItem->CreateCashInvenWholeItem(pMarket->Item);
						}
						else{
							nRet = m_pItem->CreateInvenWholeItemByIndex(pMarket->cInvenSlotIndex, pMarket->Item);
						}
						if (nRet != ERROR_NONE){
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							SendMarketBuy(pMarket->nMarketDBID, nRet, pMarket->bMini);
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							SendMarketBuy(pMarket->nMarketDBID, nRet, false);
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
							return;
						}
					}

					if ((pMarket->nSellerADBID > 0) && (pMarket->nSellerADBID != m_nAccountDBID)){	// 판매됐다고 알려준다
						CDNUserSession *pUser = g_pUserSessionManager->FindUserSessionByAccountDBID(pMarket->nSellerADBID);
						if (pUser && (pUser->GetCharacterDBID() == pMarket->biSellerCharacterDBID)){
							pUser->SendNotifyMarket(pMarket->Item.nItemID, 0);
						}
						else {	// 없으면 master로
							g_pMasterConnection->SendNotifyMarket(pMarket->nSellerADBID, pMarket->biSellerCharacterDBID, pMarket->Item.nItemID, 0);
						}
					}					
				}
				break;
				
			case 103123:	// 103123 코인이 부족합니다
			case 103174:	// 103174 이미 팔린 아이템입니다.
			case 103175:	// 103175 등록되어 있지 않은 아이템입니다.
			case 101136:	// 페탈 잔액 부족
			case 103325:	// 103325 자신이 등록한 아이템을 구입할 수 없습니다.
			case ERROR_ITEM_INVENTORY_NOTENOUGH:
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarket->nRetCode);
				DetachConnection(L"QUERY_BUYTRADEITEM");
				return;
			}

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
			SendMarketBuy(pMarket->nMarketDBID, pMarket->nRetCode, pMarket->bMini);
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
			SendMarketBuy(pMarket->nMarketDBID, pMarket->nRetCode, false);
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
		}
		break;

	case QUERY_GETLISTTRADEFORCALCULATION:
		{
			TAGetListTradeForCalculation *pMarketList = (TAGetListTradeForCalculation*)pData;

			if (pMarketList->nRetCode != ERROR_NONE){
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarketList->nRetCode);
				DetachConnection(L"QUERY_GETLISTTRADEFORCALCULATION");
				break;
				// _DANGER_POINT();
			}
			SendMarketCalculationList(pMarketList->MarketInfo, pMarketList->cMarketCount, pMarketList->nRetCode);
		}
		break;

	case QUERY_TRADECALCULATE:
		{
			TATradeCalculate *pMarket = (TATradeCalculate*)pData;

			switch(pMarket->nRetCode)
			{
			case ERROR_NONE:
				{
					if (pMarket->nCalculatePrice < 0)
						pMarket->nCalculatePrice = 0;
#if defined(PRE_ADD_PETALTRADE)
					if ( pMarket->cPayMethodCode == DBDNWorldDef::PayMethodCode::Petal)						
						AddPetal(pMarket->nCalculatePrice);
					else
#endif
					{						
						AddCoin(pMarket->nCalculatePrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0, true);
					}
				}
				break;
				
			case 103176:	// 103176 정산할 아이템이 없습니다.
				break;

			case 103204:	// 최대 소지 금액을 초과 하였습니다(?)
				pMarket->nRetCode = ERROR_ITEM_OVERFLOWMONEY;
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarket->nRetCode);
				DetachConnection(L"QUERY_TRADECALCULATE");
				return;
			}

			SendMarketCalculation(pMarket->nMarketDBID, pMarket->nRetCode);
		}
		break;

	case QUERY_TRADECALCULATEALL:
		{
			TATradeCalculateAll *pMarket = (TATradeCalculateAll*)pData;

			switch(pMarket->nRetCode)
			{
			case ERROR_NONE:
				{
#if defined(PRE_ADD_PETALTRADE)
					if (pMarket->nCalculatePetal < 0) pMarket->nCalculatePetal = 0;
					AddPetal(pMarket->nCalculatePetal);
#endif
					if (pMarket->nCalculatePrice < 0) pMarket->nCalculatePrice = 0;
					AddCoin(pMarket->nCalculatePrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0, true);
				}
				break;

			case 103176:	// 103176 정산할 아이템이 없습니다.
				break;

			case 103204:	// 최대 소지 금액을 초과 하였습니다(?)
				pMarket->nRetCode = ERROR_ITEM_OVERFLOWMONEY;
				break;

			default:
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarket->nRetCode);
				DetachConnection(L"QUERY_TRADECALCULATEALL");
				return;
			}

			SendMarketCalculationAll(pMarket->nRetCode);
		}
		break;

#if !defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	case QUERY_GETTRADEITEMID:
		{
			TAGetTradeItemID *pMarket = (TAGetTradeItemID*)pData;

#if defined(PRE_ADD_PETALTRADE)
			int nRet = 0;
			if ( pMarket->cMethodCode == DBDNWorldDef::PayMethodCode::Petal )
				nRet = m_pItem->BuyMarketItem(pMarket->nMarketDBID, pMarket->nMarketItemID, pMarket->nPrice);
			else
				nRet = m_pItem->BuyMarketItem(pMarket->nMarketDBID, pMarket->nMarketItemID, 0);
#else
			int nRet = m_pItem->BuyMarketItem(pMarket->nMarketDBID, pMarket->nMarketItemID);
#endif
			if (nRet != ERROR_NONE){
				SendMarketBuy(0, nRet, false);
				return;
			}
		}
		break;
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

	case QUERY_GETPETALBALANCE:
		{
			TAGetPetalBalance* pMarket = (TAGetPetalBalance*)pData;

			if ( pMarket->nRetCode == ERROR_NONE )
			{
				SetPetal(pMarket->nPetalBalance);
			}
			SendMarketPetalBalance(pMarket->nPetalBalance, pMarket->nRetCode);
		}
		break;

	case QUERY_GETTRADEPRICE:
		{
			
			TAGetTradePrice* pMarket = (TAGetTradePrice*)pData;
			if (pMarket->nRetCode != ERROR_NONE)
			{
				g_Log.Log(LogType::_FILELOG, m_nWorldSetID, m_nAccountDBID, m_biCharacterDBID, GetSessionID(), L"[M:%d S:%d] Ret:%d", MAINCMD_MARKET, nSubCmd, pMarket->nRetCode);
				DetachConnection(L"QUERY_GETTRADEPRICE");
				break;
			}

			// 캐쉬 등록
			TradeSystem::CMarketPriceCache::GetInstance().SetTradePrice(pMarket->nItemID, pMarket->cLevel, pMarket->cOption, pMarket->ItemPrices);

			SendMarketPrice(pMarket->nRetCode, pMarket->nMarketDBID, pMarket->ItemPrices);
		}
	}
}

void CDNUserSession::OnDBRecvItem(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvItem(nSubCmd, pData);

	switch( nSubCmd )
	{
		case QUERY_GETPAGEMATERIALIZEDITEM:
		{
			TAGetPageMaterializedItem *pItem = (TAGetPageMaterializedItem*)pData;

			int nRemain = m_pItem->GetCashInventoryTotalCount() - m_pItem->GetCashInventoryCount();
			if (m_bFirst)
			{
				SendCashInvenList( m_pItem->GetCashInventoryTotalCount(), pItem->cCount, pItem->CashItem );
#if defined (PRE_ADD_BESTFRIEND)
				m_pBestFriend->RemoveReward();
#endif
			}
			break;
		}

		case QUERY_GETPAGEVEHICLE:
		{
			m_pItem->RemoveInstantVehicleItemData( !m_bFirst );

			TAGetPageVehicle *pItem = (TAGetPageVehicle*)pData;

			if (m_bFirst){
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM )
				bool bCheckDelete = m_pItem->RemovePCBangRentalItemVehicle( false );
				if( m_bDeletePCRentalItem || bCheckDelete )									
					SendServerMsg( CHATTYPE_SYSTEM, CEtUIXML::idCategory1, SERVERMSG_INDEX6211, 3, 4); // Caption3, 4초				
#endif
				SendVehicleInvenList( m_pItem->GetVehicleInventoryTotalCount(), pItem->cCount, pItem->VehicleItem);
				m_pItem->SendPetSatiety();
				// 탈것인벤 다 날리면 그때 퀵슬롯 날려준다
				if (m_pItem->GetVehicleInventoryTotalCount() == m_pItem->GetVehicleInventoryItemCount() )
				{
					SendQuickSlotList(GetStatusData()->QuickSlot);
					if(!m_GuildSelfView.IsSet())
						GetItem()->RemoveGuildReversionVehicleItemData( true );
				}
			}
			break;
		}

#if !defined (PRE_MOD_GAMESERVERSHOP)
		case QUERY_GETLIST_REPURCHASEITEM:
		{
			TAGetListRepurchaseItem* pA = reinterpret_cast<TAGetListRepurchaseItem*>(pData);

			GetItem()->OnRecvGetListRepurchaseItem( pA );
			break;
		}
#endif // #if !defined (PRE_MOD_GAMESERVERSHOP)
	}
}

void CDNUserSession::OnDBRecvCash(int nSubCmd, char *pData)
{
	CDNUserBase::OnDBRecvCash(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvMasterSystem(int nSubCmd, char* pData)
{
	return CDNUserBase::OnDBRecvMasterSystem( nSubCmd, pData );
}

void CDNUserSession::OnDBRecvFarm(int nSubCmd, char * pData)
{
	switch(nSubCmd)
	{
		case QUERY_GETLIST_FIELD_BYCHARACTER:
		{
			TAGetListFieldByCharacter* pPacket = reinterpret_cast<TAGetListFieldByCharacter*>(pData);
				
			if( pPacket->nRetCode == ERROR_NONE )
			{
				TFarmItem FarmItems[Farm::Max::FARMCOUNT];
				memset(FarmItems, 0, sizeof(FarmItems));

				BYTE cCount = 0;
				g_pFarm->GetActivateFarmList(FarmItems, cCount);
				SendFarmInfo(FarmItems, cCount, pPacket->Fields, pPacket->cCount, pPacket->bRefreshGate);
			}
			else
			{
				//쿼리가 문제가 있을경우 스테이트를 풀어준다.
				m_cGateNo = -1;
				m_eUserState = STATE_NONE;
				SendCancelStage(pPacket->nRetCode);
			}
		}
		break;
	}
	
	return CDNUserBase::OnDBRecvFarm(nSubCmd, pData);
}

void CDNUserSession::OnDBRecvGuildRecruit(int nSubCmd, char * pData)
{
	switch(nSubCmd)
	{
	case QUERY_REGISTERON_GUILDRECRUIT:
		{
			//비동기(갱신안됨)
			TARegisterOnGuildRecruit* pPacket = reinterpret_cast<TARegisterOnGuildRecruit*>(pData);
			SendRegisterGuildRecruitList( pPacket->nRetCode, GuildRecruitSystem::RegisterType::RegisterOn );
			return;
		}
		break;
	case QUERY_REGISTERMOD_GUILDRECRUIT:
		{
			//비동기(갱신안됨)
			TARegisterModGuildRecruit* pPacket = reinterpret_cast<TARegisterModGuildRecruit*>(pData);
			SendRegisterGuildRecruitList( pPacket->nRetCode, GuildRecruitSystem::RegisterType::RegisterMod );
			return;
		}
		break;
	case QUERY_REGISTEROFF_GUILDRECRUIT:
		{
			//비동기(갱신안됨)
			TARegisterOffGuildRecruit* pPacket = reinterpret_cast<TARegisterOffGuildRecruit*>(pData);
			SendRegisterGuildRecruitList( pPacket->nRetCode, GuildRecruitSystem::RegisterType::RegisterOff );
			return;
		}
		break;
	case QUERY_REQUESTON_GUILDRECRUIT:
		{
			//자신이 신청한 길드 목록 갱신 필요
			TAGuildRecruitRequestOn* pPacket = reinterpret_cast<TAGuildRecruitRequestOn*>(pData);
			SendGuildRecruitRequest( pPacket->nRetCode, GuildRecruitSystem::RequestType::RequestOn );
			GuildRecruitSystem::CCacheRepository::GetInstance().DelMyGuildRecruit( this );
			return;
		}
		break;
	case QUERY_REQUESTOFF_GUILDRECRUIT:
		{
			//자신이 신청한 길드 목록 갱신 필요
			TAGuildRecruitRequestOff* pPacket = reinterpret_cast<TAGuildRecruitRequestOff*>(pData);
			SendGuildRecruitRequest( pPacket->nRetCode, GuildRecruitSystem::RequestType::RequestOff );
			GuildRecruitSystem::CCacheRepository::GetInstance().DelMyGuildRecruit( this );
			return;
		}
		break;
	case QUERY_ACCEPTON_GUILDRECRUIT:
		{
			// 길드 가입 처리 필요(온라인, 오프라인 상태 다 처리 가능해야함)
			TAGuildRecruitAcceptOn* pPacket = reinterpret_cast<TAGuildRecruitAcceptOn*>(pData);
			if(pPacket->nRetCode == ERROR_NONE )
			{
				// 길드 가입 성공하면 길드에 저장
				const TGuildUID GuildUID = GetGuildUID();
				if( !GuildUID.IsSet())
				{					
					return;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

				if (!pGuild) 
				{					
					return;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);

				if (FALSE == pGuild->IsEnable())
				{					
					return;
				}
#endif

				// 위치 초기화
				TCommunityLocation Location;
				Location.Reset();

				TGuildMember GuildMember;
				GuildMember.Set(pPacket->nAddAccountDBID, pPacket->biAddCharacterDBID, pPacket->wszToCharacterName, static_cast<TP_JOB>(pPacket->cJob), pPacket->cLevel, GUILDROLE_TYPE_JUNIOR, pPacket->JoinDate);

				// 길드원 추가(중복발생 가능있음)
				if (!pGuild->AddMember(&GuildMember, &Location))
					_DANGER_POINT()					

				// 길드시각정보지정
				TGuildView GuildView = pGuild->GetInfo()->GuildView;

				// 현재서버에접속중인길드원들에게통보
				pGuild->SendAddGuildMember(pPacket->nAddAccountDBID, pPacket->biAddCharacterDBID, pPacket->wszToCharacterName, static_cast<TP_JOB>(pPacket->cJob), pPacket->cLevel, pPacket->JoinDate, &Location);
				
				// MA 를통하여다른서버에접속중인길드원들에게도통보
				g_pMasterConnection->SendAddGuildMember(GuildUID, pPacket->nAddAccountDBID, pPacket->biAddCharacterDBID, pPacket->wszToCharacterName, static_cast<TP_JOB>(pPacket->cJob), pPacket->cLevel, pPacket->JoinDate);

				SendGuildRecruitAccept( pPacket->nRetCode, pPacket->biAddCharacterDBID, pPacket->bDelGuildRecruit, GuildRecruitSystem::AcceptType::AcceptOn );
				GuildRecruitSystem::CCacheRepository::GetInstance().DelGuildRecruitCharacter( GuildUID );
				
				// 현재 서버에 유저가 있는 경우
				CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToCharacterName);

				// 현재서버에 유저를 찾지 못하면 마스터서버에 요청하여 찾아본다.
				if (!pUserObj)
				{
					g_pMasterConnection->SendGuildRecruitMemberResult(GuildUID, pPacket->wszToCharacterName, pPacket->biAddCharacterDBID, GuildView, GuildRecruitSystem::AcceptType::AcceptOn);
					return;
				}				
				// 길드시각정보지정
				pUserObj->SetGuildSelfView(TGuildSelfView(GuildView, GUILDROLE_TYPE_JUNIOR));
				pUserObj->SendGuildRecruitMemberResult(pPacket->nRetCode, GuildView.wszGuildName, GuildRecruitSystem::AcceptType::AcceptOn);

				TGuildRewardItem *RewardItemInfo;
				RewardItemInfo = pGuild->GetGuildRewardItem();
				pUserObj->SendGuildRewardItem(RewardItemInfo);
				pUserObj->GetEventSystem()->OnEvent( EventSystem::OnGuildJoin );
			}
			else
			{
				SendGuildRecruitAccept( pPacket->nRetCode, pPacket->biAddCharacterDBID, pPacket->bDelGuildRecruit, GuildRecruitSystem::AcceptType::AcceptOn );
			}
			return;
		}
		break;
	case QUERY_ACCEPTOFF_GUILDRECRUIT:
		{
			//자신의 길드에 신청한 목록 갱신(서버별로 동기 안맞음) 거절당한 상대가 온라인인 경우 처리 필요
			TAGuildRecruitAcceptOff* pPacket = reinterpret_cast<TAGuildRecruitAcceptOff*>(pData);

			if( pPacket->nRetCode == ERROR_NONE )
			{			
				const TGuildUID GuildUID = GetGuildUID();
				if( !GuildUID.IsSet())
				{					
					return;
				}

				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

				if (!pGuild) 
				{					
					return;
				}

#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);

				if (FALSE == pGuild->IsEnable())
				{					
					return;
				}
#endif
				
				TGuildView GuildView = pGuild->GetInfo()->GuildView;

				SendGuildRecruitAccept( pPacket->nRetCode, pPacket->biDenyCharacterDBID, false, GuildRecruitSystem::AcceptType::AcceptOff );
				GuildRecruitSystem::CCacheRepository::GetInstance().DelGuildRecruitCharacter( GuildUID );

				// 현재 서버에 유저가 있는 경우
				CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToCharacterName);

				// 현재서버에 유저를 찾지 못하면 마스터서버에 요청하여 찾아본다.
				if (!pUserObj)
				{
					g_pMasterConnection->SendGuildRecruitMemberResult(GuildUID, pPacket->wszToCharacterName, pPacket->biDenyCharacterDBID, GuildView, GuildRecruitSystem::AcceptType::AcceptOff);
					return;
				}
				
				pUserObj->SendGuildRecruitMemberResult(pPacket->nRetCode, GuildView.wszGuildName, GuildRecruitSystem::AcceptType::AcceptOff);
			}
			else
			{
				SendGuildRecruitAccept( pPacket->nRetCode, pPacket->biDenyCharacterDBID, false, GuildRecruitSystem::AcceptType::AcceptOff );
			}
			return;
		}
		break;
	}
	return CDNUserBase::OnDBRecvGuildRecruit(nSubCmd, pData);
}

#if defined (PRE_ADD_DONATION)
void CDNUserSession::OnDBRecvDonation(int nSubCmd, char* pData)
{
	switch (nSubCmd)
	{
	case QUERY_DONATE:
		{
			TADonate* pA = reinterpret_cast<TADonate*>(pData);
			SendDonationResult(pA->nRetCode);
		}
		break;

	case QUERY_DONATION_RANKING:
		{
			TADonationRaking* pA = reinterpret_cast<TADonationRaking*>(pData);
			SendDonationRanking(pA->Ranking, pA->nMyRanking, pA->nMyCoin);
		}
		break;
	}
}
#endif // #if defined (PRE_ADD_DONATION)

#if defined( PRE_PARTY_DB )

int CDNUserSession::_RecvDBCreateParty( const TAAddParty* pA )
{
	if( pA->nRetCode != ERROR_NONE )
		return ERROR_PARTY_CREATE_FAIL;

#if defined( _FINAL_BUILD )
	if( GetPartyID() > 0 )
	{
		g_pPartyManager->QueryDelParty( pA->Data.PartyData.PartyID );
		return ERROR_NONE;
	}
#else
	if( pA->bCheat == false )
	{
		if( GetPartyID() > 0 )
			return ERROR_NONE;
	}
#endif // #if defined( _FINAL_BUILD )

	CDNParty* pParty = g_pPartyManager->CreateParty( this, pA->Data.PartyData.PartyID, &pA->Data );
	if( pParty )
	{
		int iMemberIdx = -1;
		if( pParty->AddPartyMember( GetAccountDBID(), GetSessionID(), GetCharacterName(), iMemberIdx, true ) == true )
		{
#if defined(PRE_FIX_62281)
			SendCreateParty( ERROR_NONE, pA->Data.PartyData.PartyID, ( static_cast<int>(wcslen(pA->Data.wszAfterInviteCharacterName)) > 0 ? true : false), pA->Data.PartyData.Type );
#else
			SendCreateParty( ERROR_NONE, pA->Data.PartyData.PartyID, (pA->Data.uiAfterInviteAccountDBID > 0 ? true : false), pA->Data.PartyData.Type );
#endif
			SetPartyID( pParty->GetPartyID() );
			m_nPartyMemberIndex = iMemberIdx;
			m_boPartyLeader = true;

			pParty->SendAllRefreshParty( PARTYREFRESH_NONE );
			pParty->SendPartyMemberMoveEachOther( GetSessionID(), GetTargetPos().nX, GetTargetPos().nY, GetTargetPos().nZ );

#if defined(PRE_FIX_62281)
			if( static_cast<int>(wcslen(pA->Data.wszAfterInviteCharacterName)) > 0 )
			{
				CDNUserSession* pInviteUser = g_pUserSessionManager->FindUserSessionByName( pA->Data.wszAfterInviteCharacterName );
#else
			if( pA->Data.uiAfterInviteAccountDBID > 0 )
			{
				CDNUserSession* pInviteUser = g_pUserSessionManager->FindUserSessionByAccountDBID( pA->Data.uiAfterInviteAccountDBID );
#endif
				if( pInviteUser )
				{
					//초대할넘 상태 검사
#if defined(PRE_FIX_62281)
					int nRet = pInviteUser->CheckPartyInvitableState(GetCharacterName(), pA->Data.PartyData.cMinLevel);
					if( nRet != ERROR_NONE )
						return nRet;
#else
					if( !pInviteUser->IsNoneWindowState() )
						return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;
#endif

					if( pParty->InvitePartyMember(pInviteUser->GetCharacterName()) == true )
					{
						pInviteUser->SetWindowState(WINDOW_ISACCEPT);
#if defined(PRE_FIX_62281)
						pInviteUser->SendPartyInviteMsg( pParty->GetPartyID(), pParty->GetPartyName(), pParty->GetPassword(), GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv(), -1, GetMapIndex());
#else
						pInviteUser->SendPartyInviteMsg( pParty->GetPartyID(), pParty->GetPartyName(), pParty->GetPassword(), GetCharacterName(), (BYTE)pParty->GetMemberMax(), (BYTE)pParty->GetMemberCount(), (USHORT)pParty->GetAvrUserLv());
#endif
					}
					else
					{
						return ERROR_PARTY_INVITEFAIL_ALREADY_INVITED;
					}
				}
#if defined(PRE_FIX_62281)
				else
				{	//마스터로 보내장
					g_pMasterConnection->SendReqPartyInvite( this, pA->Data.wszAfterInviteCharacterName, pParty );
				}
#endif
			}

#if defined( _USE_VOICECHAT )
			if( pA->Data.PartyData.iBitFlag&Party::BitFlag::VoiceChat )
			{
				if( g_pVoiceChat && g_pMasterConnection )
					g_pMasterConnection->SendReqVoiceChannelID( pParty->GetPartyID(), _VOICECHANNEL_REQTYPE_PARTY );
			}
#endif // #if defined( _USE_VOICECHAT )
		}
		else
		{
			g_pPartyManager->DestroyParty( pA->Data.PartyData.PartyID );
			return ERROR_PARTY_CREATE_FAIL;
		}
	}
#if defined(PRE_FIX_62281)
	else
		return ERROR_PARTY_CREATE_FAIL;
#endif

	return ERROR_NONE;
}

int CDNUserSession::_RecvDBJoinParty( const TAJoinParty* pA )
{
	if( m_eUserState == STATE_READY )
		return ERROR_PARTY_JOINFAIL_INREADYSTATE;

	if( GetPartyID() > 0 )
		return ERROR_PARTY_JOINFAIL;

	if( m_bIsMove || m_bIsStartGame )
		return ERROR_PARTY_JOINFAIL;

	// 상태 해제
	IsWindowStateNoneSet(WINDOW_ISACCEPT);

	CDNParty* pParty = g_pPartyManager->GetParty( pA->PartyID );
	if( pParty == NULL )
		return ERROR_PARTY_JOINFAIL;

	if( pParty->CheckSameGateNo() != -1 || pParty->GetCompleteMember() || pParty->bIsMove() || pParty->bIsStartGame() )
		return ERROR_PARTY_JOINFAIL_INREADYSTATE;

	if (pParty->HasEmptyPlace() == false)
		return ERROR_PARTY_JOINFAIL;

	if (pParty->GetChannelID() != GetChannelID())
	{
		const sChannelInfo * pChInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
		const sChannelInfo * pPartyChInfo = g_pMasterConnection->GetChannelInfo(pParty->GetChannelID());
		if (pChInfo && pPartyChInfo)
		{
			if (pChInfo->nMapIdx != pPartyChInfo->nMapIdx)
				return ERROR_PARTY_JOINFAIL;
		}
		else
			return ERROR_PARTY_JOINFAIL;

		if (!g_pMasterConnection || g_pMasterConnection->GetActive() == false)
			return ERROR_PARTY_JOINFAIL;
	}

	int nMemberIdx = -1;
	if( pParty->AddPartyMember(GetAccountDBID(), GetSessionID(), GetCharacterName(), nMemberIdx ) == false )
		return ERROR_PARTY_JOINFAIL;
#if defined( PRE_PARTY_DB )
#else
	if( pParty->HasEmptyPlace() == false )
		g_pPartyManager->EraseWaitPartyList(pParty);
#endif // #if defined( PRE_PARTY_DB )

	m_PartyID = pParty->GetPartyID();
	m_nPartyMemberIndex = nMemberIdx;

	SendJoinParty(ERROR_NONE, pParty->GetPartyID(), pParty->GetPartyName() );
	pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
	pParty->SendPartyMemberMoveEachOther(GetSessionID(), m_BaseData.TargetPos.nX, m_BaseData.TargetPos.nY, m_BaseData.TargetPos.nZ);
	if( GetProfile()->bOpenPublic )
		pParty->SendUserProfile( GetSessionID(), *GetProfile() );

	if( pParty->GetChannelID() != GetChannelID() )
		g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE_SAMEDEST, GetAccountDBID(), GetMapIndex(), -1, pParty->GetChannelID());

	return ERROR_NONE;
}

int CDNUserSession::_RecvDBOutParty( const TAOutParty* pA )
{
	CDNParty* pParty = g_pPartyManager->GetParty( pA->PartyID );
	if( pParty == NULL )
		return ERROR_PARTY_LEAVEFAIL;

	return g_pPartyManager->DelPartyMember( pParty, this, pA );
}

void CDNUserSession::OnDBRecvParty( int nSubCmd, char* pData )
{
	switch( nSubCmd )
	{
		case QUERY_ADDPARTY:
		{
			int iRet = _RecvDBCreateParty( reinterpret_cast<TAAddParty*>(pData) );
			switch( iRet )
			{
				case ERROR_NONE:
				{
					break;
				}
				case ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED:
				case ERROR_PARTY_INVITEFAIL_ALREADY_INVITED:
				{
					SendPartyInviteFail( iRet );
					break;
				}
				default:
				{
					SendCreateParty( iRet, 0 );
					break;
				}
			}
			break;
		}
		case QUERY_JOINPARTY:
		{
			const TAJoinParty* pPacket = reinterpret_cast<const TAJoinParty*>(pData);
			int iRet = _RecvDBJoinParty( pPacket );
			if( iRet != ERROR_NONE )
				g_pPartyManager->QueryOutParty( pPacket->PartyID, pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID, Party::QueryOutPartyType::Normal );
			break;
		}
		case QUERY_OUTPARTY:
		{
			const TAOutParty* pPacket = reinterpret_cast<const TAOutParty*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				if( _RecvDBOutParty( pPacket ) != ERROR_NONE )
					DetachConnection( L"QUERY_OUTPARTY Failed!" );
			}
			else
			{
				if( pPacket->Type == Party::QueryOutPartyType::Normal )
					SendPartyOut( ERROR_PARTY_LEAVEFAIL );
			}
			break;
		}
		case QUERY_GETPARTY_JOINMEMBERS:
		{
			const TAGetListPartyMember* pPacket = reinterpret_cast<const TAGetListPartyMember*>(pData);

#if defined( PRE_WORLDCOMBINE_PARTY )
			if( (pPacket->nRetCode == ERROR_NONE && pPacket->nCount > 0) 
				|| Party::bIsWorldCombineParty(pPacket->PartyType) )
#else
			if( pPacket->nRetCode == ERROR_NONE && pPacket->nCount > 0 )
#endif
			{
				SCPartyMemberInfo TxPacket;
				memset( &TxPacket, 0, sizeof(TxPacket) );

				TxPacket.PartyID = pPacket->PartyID;
				TxPacket.PartyType = pPacket->PartyType;;
				TxPacket.nCount = pPacket->nCount;
				for( int i=0 ; i<pPacket->nCount ; ++i )
					TxPacket.MemberData[i] = pPacket->MemberData[i];

				SendPartyMemberInfo( &TxPacket );
			}
			else
			{
				m_PrevPartyListInfo.Clear();
				g_Log.Log(LogType::_PARTYERROR, this, L"[QUERY_GETPARTY_JOINMEMBERS]Error PartyID:%I64d MemberCount:%d nRet:%d\n", pPacket->PartyID, pPacket->nCount, pPacket->nRetCode);
				SendPartyInfoErr( ERROR_PARTY_NOTFOUND_MEMBERINFO );
			}
			break;
		}
	}
}
#endif // #if defined( PRE_PARTY_DB )

#if defined (PRE_ADD_BESTFRIEND)
void CDNUserSession::OnDBRecvBestFriend(int nSubCmd, char* pData)
{
	CDNUserBase::OnDBRecvBestFriend(nSubCmd, pData);

	switch( nSubCmd )
	{
	case QUERY_REGIST_BESTFRIEND:
		{
			const TARegistBestFriend *pPacket = reinterpret_cast<TARegistBestFriend*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendRegistBestFriendAck(pPacket->nRetCode, GetAccountDBID(), GetCharacterName(), 0, NULL);
				break;
			}
			// 절친 등록서 삭제 (DB에서 이미 삭제한 상태임)
			m_pItem->DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biRegistSerial, false);

			// 절친 보상
			m_pBestFriend->ExecuteReward(pPacket->nItemID);

			// 절친 정보 가져오기
			m_pDBCon->QueryGetBestFriend(m_cDBThreadID, this, true);

			// 절친 등록 완료
			SendCompleteBestFriend(ERROR_NONE, pPacket->wszFromName);

			// 상대편에게도 알려준다.
			g_pMasterConnection->SendCompleteBestFriend(pPacket->nToAccountDBID, GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), pPacket->nItemID);

			// 월드쳇으로 알려준다
			g_pMasterConnection->SendWorldSystemMsg(GetAccountDBID(), WORLDCHATTYPE_BESTFRIEND, 0, 0, pPacket->wszToName);
		}
		break;
	case QUERY_CANCEL_BESTFRIEND:
		{
			const TACancelBestFriend *pPacket = reinterpret_cast<TACancelBestFriend*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendCancelBestFriend(pPacket->nRetCode, NULL);
				break;
			}

			m_pDBCon->QueryGetBestFriend(m_cDBThreadID, this, true);

			g_pMasterConnection->SendCancelBestFriend(GetCharacterName(), m_pBestFriend->GetInfo().wszName, pPacket->bCancel);

			SendCancelBestFriend(ERROR_NONE, GetCharacterName(), pPacket->bCancel);
		}
		break;
	case QUERY_CLOSE_BESTFRIEND:
		{
			const TACloseBestFriend *pPacket = reinterpret_cast<TACloseBestFriend*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				g_Log.Log(LogType::_BESTFRIEND, this, L"QUERY_CLOSE_BESTFRIEND [Result:%d] \r\n", pPacket->nRetCode);
				break;
			}

			// 파기 메일 전송
			int nMailID = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::BestFriend_CloseMail );
			CDNMailSender::Process( GetCharacterDBID(), GetWorldSetID(), nMailID );
			CDNMailSender::Process( m_pBestFriend->GetInfo().biCharacterDBID, GetWorldSetID(), nMailID );

			g_pMasterConnection->SendCloseBestFriend(GetCharacterName(), m_pBestFriend->GetInfo().wszName);

			SendCloseBestFriend(ERROR_NONE, m_pBestFriend->GetInfo().wszName);

			m_pBestFriend->Close();
		}
		break;
	case QUERY_EDIT_BESTFRIENDMEMO:
		{
			const TAEditBestFriendMemo *pPacket = reinterpret_cast<TAEditBestFriendMemo*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendEditBestFriendMemo(pPacket->nRetCode, true, NULL);
				break;
			}

			GetBestFriend()->ChangeMemo(true, pPacket->wszMemo);
			g_pMasterConnection->SendEditBestFriendMemo(GetAccountDBID(), m_pBestFriend->GetInfo().biCharacterDBID, pPacket->wszMemo);

			SendEditBestFriendMemo(pPacket->nRetCode, true, pPacket->wszMemo);
		}
		break;
	default:
		break;
	}
}
#endif // #if defined (PRE_ADD_BESTFRIEND)
#if defined( PRE_PRIVATECHAT_CHANNEL )
void CDNUserSession::OnDBRecvPrivateChatChannel(int nSubCmd, char* pData)
{
	CDNUserBase::OnDBRecvPrivateChatChannel(nSubCmd, pData);
}
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )

void CDNUserSession::OnDBRecvAlteiaWorld(int nSubCmd, char* pData)
{
	CDNUserBase::OnDBRecvAlteiaWorld(nSubCmd, pData);
	switch( nSubCmd )
	{
	case QUERY_GET_ALTEIAWORLDINFO:
		{
			const TAGetAlteiaWorldInfo* pPacket = reinterpret_cast<const TAGetAlteiaWorldInfo*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				SetAlteiaBestGoldKeyCount( (BYTE)pPacket->nBestGoldKeyCount );
				SetAlteiaBestPlayTime( pPacket->uiBestPlayTime );
				SetAlteiaWeeklyPlayCount( (BYTE)pPacket->nWeeklyPlayCount );
				SetAlteiaDailyPlayCount( (BYTE)pPacket->nDailyPlayCount );
			}
		}
		break;
	case QUERY_GET_ALTEIAWORLDPRIVATEGOLDKEYRANK:
		{
			const TAGetAlteiaWorldPrivateGoldKeyRank* pPacket = reinterpret_cast<const TAGetAlteiaWorldPrivateGoldKeyRank*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				SendAlteiaWorldPrivateGoldKeyRank( pPacket );
			}
		}
		break;
	case QUERY_GET_ALTEIAWORLDPRIVATEPLAYTIMERANK:
		{
			const TAGetAlteiaWorldPrivatePlayTimeRank* pPacket = reinterpret_cast<const TAGetAlteiaWorldPrivatePlayTimeRank*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				SendAlteiaWorldPrivatePlayTimeRank( pPacket );
			}
		}
		break;
	case QUERY_GET_ALTEIAWORLDGUILDGOLDKEYRANK:
		{
			const TAGetAlteiaWorldGuildGoldKeyRank* pPacket = reinterpret_cast<const TAGetAlteiaWorldGuildGoldKeyRank*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				SendAlteiaWorldGuildGoldKeyRank( pPacket );
			}
		}
		break;

	case QUERY_GET_ALTEIAWORLDSENDTICKETLIST:
		{
			const TAGetAlteiaWorldSendTicketList* pPacket = reinterpret_cast<const TAGetAlteiaWorldSendTicketList*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				BYTE cMaxSendTicketCount = static_cast<BYTE>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldSendTicketCount ));
				BYTE cDailyTicketCount = static_cast<BYTE>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldDailyTicketCount ));
				BYTE cJoinTicketCount = static_cast<BYTE>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldJoinTicketCount ));
				
				m_cTicketCount = pPacket->nRecvTicketCount + cDailyTicketCount;

				if( cJoinTicketCount * m_cDailyPlayCount <= m_cTicketCount )
					m_cTicketCount -= cJoinTicketCount * m_cDailyPlayCount;
				else
					m_cTicketCount = 0;
				
				if( cMaxSendTicketCount > pPacket->nSendTicketCount )
					m_cSendTicketCount =  cMaxSendTicketCount - pPacket->nSendTicketCount;
				else
					m_cSendTicketCount = 0;

				for( int i=0;i<AlteiaWorld::Common::MaxSendCount;i++ )
				{
					if( pPacket->biCharacterDBID[i] > 0 )
						AddAlteiaSendTicketList(pPacket->biCharacterDBID[i]);
					else
						break;
				}				
			}
		}
		break;
	case QUERY_ADD_ALTEIAWORLDSENDTICKETLIST:
		{	
			const TAAddAlteiaWorldSendTicketList* pPacket = reinterpret_cast<TAAddAlteiaWorldSendTicketList*>(pData);
			if( pPacket->nRetCode == ERROR_NONE )
			{
				AddAlteiaTicket();
				SendAlteiaWorldSendTicket( pPacket->wszSendCharacterName, GetAlteiaTicketCount() );
				
				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszSendCharacterName);
				if( pSession )
				{			
					pSession->DelAlteiaSendTicket();
					pSession->SendAlteiaWorldSendTicketResult( pPacket->nRetCode, pSession->GetAlteiaSendTicketCount() );
				}
				else
				{
					if( g_pMasterConnection )
						g_pMasterConnection->SendAddAlteiaWorldSendTicketResult( pPacket->nRetCode, pPacket->biSendCharacterDBID );
				}
			}
			else
			{
				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszSendCharacterName);
				if( pSession )
				{
					pSession->SendAlteiaWorldSendTicketResult( pPacket->nRetCode, pSession->GetAlteiaSendTicketCount() );
				}
				else
				{
					if( g_pMasterConnection )
						g_pMasterConnection->SendAddAlteiaWorldSendTicketResult( pPacket->nRetCode, pPacket->biSendCharacterDBID );
				}
			}
		}
		break;
	default:
		break;
	}	
}

#endif

#if defined(PRE_ADD_DWC)
void CDNUserSession::OnDBRecvDWC( int nSubCmd, char* pData )
{
	CDNUserBase::OnDBRecvDWC(nSubCmd, pData);
	switch( nSubCmd )
	{
	case QUERY_CREATE_DWC_TEAM:
		{
			const TACreateDWCTeam * pPacket = reinterpret_cast<TACreateDWCTeam*>(pData);
			if( ERROR_NONE != pPacket->nRetCode )
			{
				SendCreateDWCTeam(pPacket->nRetCode, GetSessionID(), L"", 0);
				return;
			}
			
			TDWCTeam TeamInfo;
			memset( &TeamInfo, 0, sizeof(TeamInfo) );
			//TeamInfo.nDWCPoint = 1500;	//dWC 평점 포인트 시작 점수 설정해야함
			_wcscpy(TeamInfo.wszTeamName, _countof(TeamInfo.wszTeamName), pPacket->wszTeamName, (int)wcslen(pPacket->wszTeamName));
			
			CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->AddDWCTeamResource(pPacket->nTeamID, &TeamInfo);
			if(!pDWCTeam)
				break;

			TDWCTeamMember MemberInfo;
			memset( &MemberInfo, 0, sizeof(MemberInfo) );

			MemberInfo.nAccountDBID = GetAccountDBID();
			MemberInfo.biCharacterDBID = GetCharacterDBID();
			_wcscpy(MemberInfo.wszCharacterName, _countof(MemberInfo.wszCharacterName), GetCharacterName(), (int)wcslen(GetCharacterName()));
			MemberInfo.cJobCode = GetUserJob();
			MemberInfo.bTeamLeader = true;

			if( !pDWCTeam->AddMember(&MemberInfo, NULL) )
				_DANGER_POINT();

			SetDWCTeamID(pPacket->nTeamID);
			SendCreateDWCTeam(pPacket->nRetCode, GetSessionID(), pPacket->wszTeamName, pPacket->nTeamID);			

			std::vector<INT64> vecMemberList;
			vecMemberList.push_back(GetCharacterDBID());
			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendDWCTeamMemberList(GetAccountDBID(), GetDWCTeamID(), vecMemberList);

			RefresDWCTeamName();
		}
		break;
	case QUERY_ADD_DWC_TEAMMEMBER:
		{
			const TAAddDWCTeamMember *pPacket = reinterpret_cast<TAAddDWCTeamMember*>(pData);
			if (ERROR_NONE != pPacket->nRetCode) 
			{
				SendInviteDWCTeamMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), pPacket->nRetCode, 0, 0, 0, NULL, NULL);
				break;
			}

			CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
			if (!pDWCTeam) 
			{
				SendInviteDWCTeamMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_DWC_NOTEXIST_TEAMINFO, 0, 0, 0, NULL, NULL);
				break;
			}

			// 위치 초기화
			TCommunityLocation Location;
			Location.Reset();
			
			TDWCTeamMember DWCTeamMember(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), static_cast<TP_JOB>(GetUserJob()), false);
			
			if (!pDWCTeam->AddMember(&DWCTeamMember, &Location))
				_DANGER_POINT();
			SetDWCTeamID(pPacket->nTeamID);
			
			pDWCTeam->SendAddDWCTeamMember(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), static_cast<TP_JOB>(GetUserJob()), &Location);
			
			std::vector<INT64> vecMemberList;
			pDWCTeam->GetMemberCharacterDBIDList(vecMemberList);
			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendDWCTeamMemberList(GetAccountDBID(), GetDWCTeamID(), vecMemberList);

			RefresDWCTeamName();
		}
		break;
	case QUERY_DWC_INVITE_MEMBER:
		{
			TADWCInviteMember *pPacket = reinterpret_cast<TADWCInviteMember*>(pData);

			if( pPacket->nTeamID <= 0 || pPacket->cMemberCount <= 0 )
			{
				SendInviteDWCTeamMemberAckResult(ERROR_DWC_NOTEXIST_TEAMINFO);
				break;
			}

			g_pDWCTeamManager->UpdateDWCTeamResource(pPacket->nTeamID, &pPacket->Info);
			g_pDWCTeamManager->UpdateTeamMember(pPacket->nTeamID, pPacket->cMemberCount, pPacket->MemberList);

			CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
			if (!pDWCTeam) 
			{
				SendInviteDWCTeamMemberAckResult(ERROR_DWC_NOTEXIST_TEAMINFO);
				break;
			}

			// 인원추가 가능여부 체크
			if (!pDWCTeam->IsMemberAddable()) 
			{
				SendInviteDWCTeamMemberAckResult(ERROR_DWC_CANTADD_TEAMMEMBER);
				break;
			}

			if ( !pDWCTeam->CheckDuplicationJob(GetUserJob()) ) 
			{
				SendInviteDWCTeamMemberAckResult(ERROR_DWC_DUPLICATION_JOB);
				break;
			}

			m_pDBCon->QueryAddDWCTeamMember(m_cDBThreadID, m_nAccountDBID, m_biCharacterDBID, pPacket->nTeamID, g_Config.nWorldSetID);
		}
		break;
	case QUERY_DEL_DWC_TEAMMEMBER:
		{
			const TADelDWCTeamMember* pPacket = reinterpret_cast<TADelDWCTeamMember*>(pData);
			if( ERROR_NONE != pPacket->nRetCode )
			{
				SendLeaveDWCTeamMember(pPacket->nTeamID, pPacket->biCharacterDBID, L"", pPacket->nRetCode);
				return;
			}

			CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->GetDWCTeam(pPacket->nTeamID);
			if(!pDWCTeam)
				break;

			//팀리더가 탈퇴면 팀 해체
			if( GetCharacterDBID() == pDWCTeam->GetLeaderCharacterDBID())
			{
				// 현재서버에접속중인길드원들에게통보
				pDWCTeam->SendDismissDWCTeam(pPacket->nTeamID, ERROR_NONE);
				g_pDWCTeamManager->DelDWCTeamResource(pPacket->nTeamID);	//팀 정보 삭제
			}
			else	//일반 유저 탈퇴면 기록 리셋
			{
				pDWCTeam->SendLeaveDWCTeamMember(pPacket->nTeamID, GetCharacterDBID(), GetCharacterName(), ERROR_NONE);

				pDWCTeam->DelMember(GetCharacterDBID());
				pDWCTeam->ResetTeamScore();	//팀 스코어 초기화
				SetDWCTeamID(0);

				RefresDWCTeamName();
			}
		}
		break;
	case QUERY_GET_DWC_TEAMINFO:
		{
			TAGetDWCTeamInfo *pPacket = reinterpret_cast<TAGetDWCTeamInfo*>(pData);
			if(ERROR_NONE != pPacket->nRetCode || 0 == pPacket->nTeamID)
				break;

			SetDWCTeamID(pPacket->nTeamID);
			CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->UpdateDWCTeamResource(pPacket);
			if(pDWCTeam)
				SendGetDWCTeamInfo(pDWCTeam, ERROR_NONE);
		}
		break;
	case QUERY_GET_DWC_TEAMMEMBER:
		{
			TAGetDWCTeamMember *pPacket = reinterpret_cast<TAGetDWCTeamMember*>(pData);

			if(ERROR_NONE != pPacket->nRetCode)
				break;

			CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->UpdateTeamMember(pPacket);
			if(pDWCTeam)
				SendGetDWCTeamMember(pDWCTeam, ERROR_NONE);
		}
		break;

	case QUERY_GET_DWC_RANKLIST:
		{
			TAGetDWCRankPage* pPacket = (TAGetDWCRankPage*)pData;
			
			if (pPacket->nRetCode == ERROR_NONE)
				SendDWCRankResult(pPacket->nRetCode, pPacket->nTotalRankSize, pPacket->nPageNum, pPacket->nPageSize, pPacket->RankList);
			else
				SendDWCRankResult(pPacket->nRetCode, 0, 0, 0, NULL);
		}
		break;

	case QUERY_GET_DWC_FINDRANK:
		{
			TAGetDWCFindRank* pPacket = (TAGetDWCFindRank*)pData;

			if (pPacket->nRetCode == ERROR_NONE)
				SendDWCFindRankResult(pPacket->nRetCode, &pPacket->Rank);
			else
				SendDWCFindRankResult(pPacket->nRetCode, NULL);
		}
		break;

	case QUERY_ADD_DWC_RESULT:
		{
			//빌리지로는 치트키 입력했을때만 넘어온다.
			TAAddPvPDWCResult* pPacket = (TAAddPvPDWCResult*)pData;
			MADWCUpdateScore Packet;
			Packet.nTeamID = pPacket->nTeamID;
			Packet.DWCScore = pPacket->DWCScore;
			if(g_pDWCTeamManager && g_pDWCTeamManager->bIsIncludeDWCVillage())
			{
				CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->GetDWCTeam(Packet.nTeamID);
				if(pDWCTeam)
					pDWCTeam->UpdateDWCScore(&Packet);
			}
		}
		break;

	default:
		break;
	}	
}
#endif

int CDNUserSession::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	CExceptionReport::GetInstancePtr()->SetValues(GetAccountDBID(), GetCharacterDBID(), nMainCmd, nSubCmd);

	if (m_bIsMove || m_bIsStartGame)
	{
#ifdef _FINAL_BUILD
		if (CS_PVP != nMainCmd && CS_ACTOR != nMainCmd && (CS_SYSTEM != nMainCmd && CS_CONNECTVILLAGE != nSubCmd))
		{
			g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"Already Move After Recv MessageProcess [MCMD:%d] [SCMD:%d]\n", nMainCmd, nSubCmd);
			return ERROR_NONE;
		}
#else
		if (CS_ACTOR != nMainCmd && (CS_SYSTEM != nMainCmd && CS_CONNECTVILLAGE != nSubCmd))
		{
			g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"Already Move After Recv MessageProcess [MCMD:%d] [SCMD:%d]\n", nMainCmd, nSubCmd);
			return ERROR_NONE;
		}
#endif
		
		return ERROR_NONE;
	}

	if (m_bCertified == false)
	{
#ifdef _FINAL_BUILD
		if (CS_PVP != nMainCmd && CS_ACTOR != nMainCmd && (CS_SYSTEM != nMainCmd && CS_CONNECTVILLAGE != nSubCmd))
		{
			g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"Cheat User plz Check this user [MCMD:%d] [SCMD:%d] [InvalidCnt:%d]n", nMainCmd, nSubCmd, m_nInvalidSendCount);
			m_nInvalidSendCount++;
			return m_nInvalidSendCount >= 10 ? ERROR_INVALIDPACKET : ERROR_NONE;
		}
#else
		if (CS_ACTOR != nMainCmd && (CS_SYSTEM != nMainCmd && CS_CONNECTVILLAGE != nSubCmd))
		{
			g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"Cheat User plz Check this user [MCMD:%d] [SCMD:%d] [InvalidCnt:%d]n", nMainCmd, nSubCmd, m_nInvalidSendCount);
			m_nInvalidSendCount++;
			return m_nInvalidSendCount >= 10 ? ERROR_INVALIDPACKET : ERROR_NONE;
		}
#endif
	}
	else
		m_nInvalidSendCount = 0;

	DWORD dwCurTick = timeGetTime();
	if ((m_cLastMainCmd == CS_TRADE) && (m_cLastSubCmd == nSubCmd) && (nMainCmd == CS_TRADE)){
		if (dwCurTick - m_dwLastMessageTick <= 5){
			g_Log.Log(LogType::_ERROR, this, L"CDNUserSession::MessageProcess repeat command(%d, %d)\n", nMainCmd, nSubCmd);
			return ERROR_GENERIC_UNKNOWNERROR;
		}
	}
	m_dwLastMessageTick = dwCurTick;
	m_cLastMainCmd = nMainCmd;
	m_cLastSubCmd = nSubCmd;

	switch(nMainCmd)
	{
	case CS_SYSTEM: return OnRecvSystemMessage(nSubCmd, pData, nLen);
	case CS_CHAR: return OnRecvCharMessage(nSubCmd, pData, nLen);
	case CS_ACTOR: return OnRecvActorMessage(nSubCmd, pData, nLen);
	case CS_PROP: return OnRecvPropMessage(nSubCmd, pData, nLen);
	case CS_PARTY: return OnRecvPartyMessage(nSubCmd, pData, nLen);
	case CS_ITEM: return OnRecvItemMessage(nSubCmd, pData, nLen);
	case CS_ITEMGOODS: return OnRecvItemGoodsMessage(nSubCmd, pData, nLen);
	case CS_NPC: return OnRecvNpcMessage(nSubCmd, pData, nLen);
	case CS_QUEST: return OnRecvQuestMessage(nSubCmd, pData, nLen);
	case CS_CHAT: return OnRecvChatMessage(nSubCmd, pData, nLen);
	case CS_SKILL: return OnRecvSkillMessage(nSubCmd, pData, nLen);
	case CS_TRADE: return OnRecvTradeMessage(nSubCmd, pData, nLen);
	case CS_FRIEND: return OnRecvFriendMessage(nSubCmd, pData, nLen);
	case CS_GUILD: return OnRecvGuildMessage(nSubCmd, pData, nLen);
	case CS_PVP: return OnRecvPvPMessage(nSubCmd, pData, nLen);
	case CS_ISOLATE: return OnRecvIsolateMessage(nSubCmd, pData, nLen);
	case CS_APPELLATION: return OnRecvAppellationMessage(nSubCmd, pData, nLen);
	case CS_GAMEOPTION: return OnRecvGameOptionMessage(nSubCmd, pData, nLen);
	case CS_RADIO: return OnRecvRadioMessage(nSubCmd, pData, nLen);
	case CS_ETC: return OnRecvEtcMessage(nSubCmd, pData, nLen);
#ifdef _USE_VOICECHAT
	case CS_VOICECHAT: return OnRecvVoiceChatMessage(nSubCmd, pData, nLen);
#endif
	case CS_CASHSHOP: return OnRecvCashShopMessage(nSubCmd, pData, nLen);
	case CS_FARM: return OnRecvFarmMessage( nSubCmd, pData, nLen );
	case CS_CHATROOM: return OnRecvChatRoomMessage( nSubCmd, pData, nLen );
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	case CS_REPUTATION: return OnRecvReputationMessage( nSubCmd, pData, nLen );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	case CS_MASTERSYSTEM: return OnRecvMasterSystemMessage( nSubCmd, pData, nLen );
#if defined( PRE_ADD_SECONDARY_SKILL )
	case CS_SECONDARYSKILL: return OnRecvSecondarySkillMessage( nSubCmd, pData, nLen );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	case CS_GESTURE: return OnRecvGestureMessage( nSubCmd, pData, nLen );
	case CS_CUSTOMEVENTUI: return OnRecvPlayerCustomEventUI(nSubCmd, pData, nLen);
	case CS_GUILDRECRUIT:	return OnRecvGuildRecruitMessage(nSubCmd, pData, nLen);
#if defined (PRE_ADD_DONATION)
	case CS_DONATION: return OnRecvDonation(nSubCmd, pData, nLen);
#endif // #if defined (PRE_ADD_DONATION)
#if defined (PRE_ADD_BESTFRIEND)
	case CS_BESTFRIEND: return OnRecvBestFriendMessage(nSubCmd, pData, nLen);
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	case CS_PRIVATECHAT_CHANNEL: return OnRecvPrivateChatChannelMessage(nSubCmd, pData, nLen);
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	case CS_ALTEIAWORLD: return OnRecvWorldAlteiaMessage(nSubCmd, pData, nLen);
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
#if defined(PRE_ADD_DWC)
	case CS_DWC: return OnRecvDWCMessage(nSubCmd, pData, nLen);
#endif
#if defined(PRE_ADD_CHAT_MISSION)
	case CS_MISSION: return OnRecvMissionMessage(nSubCmd, pData, nLen);
#endif
	}

	g_Log.Log(LogType::_ERROR, this, L"Report|[ADBID:%u, SID:%u] [MCMD:%d] [SCMD:%d] MessageProcess UNKNOWN_HEADER\n", m_nAccountDBID, GetSessionID(), nMainCmd, nSubCmd);
	return ERROR_UNKNOWN_HEADER;
}

int CDNUserSession::OnRecvSystemMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
	case CS_CONNECTVILLAGE:
		{
			if (sizeof(CSConnectVillage) != nLen)
				return ERROR_INVALIDPACKET;

			CSConnectVillage *pConnectVillage = (CSConnectVillage*)pData;

			_wcscpy(m_wszVirtualIp, _countof(m_wszVirtualIp), pConnectVillage->wszVirtualIp, IPLENMAX);
			MAKE_STRINGW(m_wszVirtualIp);	// 20090923 수정(b4nfter)
			WideCharToMultiByte(CP_ACP, 0, m_wszVirtualIp, -1, m_szVirtualIp, IPLENMAX, NULL, NULL);

#if defined(_KR)
			memcpy(m_szMID, pConnectVillage->szMID, sizeof(m_szMID));
			m_dwGRC = pConnectVillage->dwGRC;
#endif

			// 새로 들어온 유저가 아니고 같은 서버에서 맵만 이동
			if (m_bChangeSameServer)
			{	
				if ( !g_pMasterConnection )
				{
					DetachConnection(L"Connect|MasterConnection Invalid");
					return ERROR_GENERIC_UNKNOWNERROR;
				}

				const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfo( GetChannelID() );
				if( !pChannelInfo )
				{
					DetachConnection(L"Connect|ChannelInfo not found");
					return ERROR_GENERIC_UNKNOWNERROR;
				}

				if (GetPartyID() > 0 && m_bPartyInto == false)
				{
					CDNParty* pParty = g_pPartyManager->GetParty(GetPartyID());
					if (pParty)
					{
						pParty->SetChannelID( GetChannelID() );
#if defined( PRE_PARTY_DB )
#else
						g_pPartyManager->PushWaitPartyList(pParty);
#endif // #if defined( PRE_PARTY_DB )
					}
					else
						_DANGER_POINT();
				}

				SendConnectVillage(ERROR_NONE, pChannelInfo->nChannelAttribute, m_bPartyInto);
				m_bPartyInto = false;
				m_eUserState = STATE_NONE;
			}
			else {
				if (pConnectVillage->nSessionID > 0){

					if (this->GetSessionID() > 0 && this->GetSessionID() != pConnectVillage->nSessionID)
					{
						DetachConnection(L"Connect|Invalid SessionID");
						return ERROR_GENERIC_UNKNOWNERROR;
					}

					if (g_pUserSessionManager->FindUserSessionByAccountDBID(pConnectVillage->nAccountDBID) != NULL)
					{
						DetachConnection(L"Connect|Duplicate AccountDBID");
						return ERROR_GENERIC_UNKNOWNERROR;
					}

					if (g_pUserSessionManager->FindUserSession(pConnectVillage->nSessionID) != NULL)
					{
						DetachConnection(L"Connect|Duplicate SessionID");
						return ERROR_GENERIC_UNKNOWNERROR;
					}


					SetSessionID(pConnectVillage->nSessionID);
					if (!g_pUserSessionManager->InsertTempSession(this))
					{
						DetachConnection(L"Connect|Duplicate TempSession");
						return ERROR_GENERIC_UNKNOWNERROR;
					}

					m_bCertified = false;
					g_pAuthManager->QueryCheckAuth(GetDBConnection(), pConnectVillage->biCertifyingKey, GetDBThreadID(), pConnectVillage->nAccountDBID, GetSessionID());
				}
				else
				{
					DetachConnection(L"Connect|AccountDBID invalid");
					return ERROR_GENERIC_UNKNOWNERROR;
				}
			}
			return ERROR_NONE;
		}
		break;

	case CS_VILLAGEREADY:
		{
			if (sizeof(CSVillageReady) != nLen)
				return ERROR_INVALIDPACKET;

			CSVillageReady *pReady = (CSVillageReady*)pData;

			if (GetChannelID() <= 0)
			{
				//빌리지나 마스터가 느려서 빌리지에서 쏜 VIMA_CHECKUSER 메세지가 마스터에서 늦게 들어올 경우(이메세지 보다) 발생
				_DANGER_POINT();
				return ERROR_GENERIC_UNKNOWNERROR;
			}

			m_bFirst = pReady->boFirst;	// bofirst가 아니면 아이템이나 이런거 보내지말것 080728 saset
			m_bChannelMove = pReady->boChannelMove;
			const sChannelInfo * pChanInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
			if (pChanInfo && GetMapIndex() != pChanInfo->nMapIdx)
			{
				SetMapIndex(pChanInfo->nMapIdx);
				SetLastMapIndex(pChanInfo->nMapIdx);
			}
			SendMapInfo(GetMapIndex(), 0, 0, 0, GetChannelID(), pChanInfo?pChanInfo->nChannelIdx:0, pChanInfo? pChanInfo->nMeritBonusID : 0);
			return ERROR_NONE;
		}
		break;

	case CS_RECONNECTLOGIN:
		{
			if (m_eUserState == STATE_MOVETOLOGIN){
				g_Log.Log(LogType::_ERROR, this, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [CS_RECONNECTLOGIN] STATE_MOVETOLOGIN\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
				return ERROR_NONE;
			}

			if (g_pMasterConnection->GetActive() == false)
				SendReconnectLogin(ERROR_GENERIC_MASTERCON_NOT_FOUND, 0, 0);
			else
				g_pMasterConnection->SendLoginState(GetSessionID());
			
			return ERROR_NONE;
		}
		break;

	case CS_MOVE_PVPVILLAGETOLOBBY:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			return _RecvMovePvPVillageToLobby();
		}
	case CS_MOVE_PVPLOBBYTOPVPVILLAGE:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			return _RecvMovePvPLobbyToPvPVillage();
		}
	}

	return CDNUserBase::OnRecvSystemMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvCharMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
	case CS_ENTER:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			CheckInitializeSchedule();

			if (m_bFirst) {
#if defined( PRE_ADD_CHARACTERCHECKSUM )
				if( 0 != m_uiDBCheckSum && m_uiDBCheckSum != m_uiRestoreCheckSum ) {
					if( GetDBConnection() ) {
#if defined(PRE_ADD_MULTILANGUAGE)
						std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8418, m_eSelectedLanguage);
						std::wstring wszRestraintReasonForDolis = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8419, m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
						std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8418);
						std::wstring wszRestraintReasonForDolis = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8419);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

						// 안정화 되면 제재 걸도록 하자.
						//GetDBConnection()->QueryAddRestraint(this, DBDNWorldDef::RestraintTargetCode::Account, DBDNWorldDef::RestraintTypeCode::ConnectRestraint, wszRestraintReason.c_str(), wszRestraintReasonForDolis.c_str(), 9999);
					}
					g_Log.Log(LogType::_ERROR, this, L"Invalid Character CheckSum(restorechecksum:%x, dbchecksum:%x)\r\n", m_uiRestoreCheckSum, m_uiDBCheckSum );
					//DetachConnection(L"QUERY_SELECTCHARACTER Invalid Character CheckSum");
					//return ERROR_NONE;
				}
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

				m_pAppellation->CheckCollectionBookReward();

				if( GetDBConnection() ){
					GetDBConnection()->QueryLoginCharacter( this );

#if defined(_KRAZ)
					m_pDBCon->QueryActozUpdateCharacterInfo(this, ActozCommon::UpdateType::Login);
#endif	// #if defined(_KRAZ)
				}

#if defined( PRE_PARTY_DB )
				SetPartyID(0);
				ModCommonVariableData( CommonVariable::Type::MOVESERVER_PARTYID, 0 );
#endif // #if defined( PRE_PARTY_DB )
			}

#if defined( PRE_PARTY_DB )			
			if( GetPartyID() > 0 && !m_bFirst )
			{
				SetPartyData( GetPartyID() );
			}
#endif
#if defined(PRE_ADD_TRANSFORM_POTION)
			if( m_nTransformID > 0)
			{
				m_dwTransformTick = 0;
				m_nTransformID = 0;
				SendChangeTransform( GetSessionID(),0,0);
			}
#endif
			if( m_bChannelMove ) {
				AddSendData( SC_CHAR, eChar::SC_ENTER, NULL, 0 );
			}
			else {
				GetAppellation()->SetPCBang();
#if defined (PRE_ADD_BESTFRIEND)
				m_pBestFriend->CheckAndSendData();
#endif
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
				if( m_bFirst )
					m_bDeletePCRentalItem = GetItem()->RemovePCBangRentalItem(false);
#endif
				SendEnter(this);
				SendEquipList( m_pItem );
				SendVehicleEquipList(m_pItem->GetVehicleEquip());
				SendPetEquipList(m_pItem->GetPetEquip());

			}
#if defined(PRE_ADD_EQUIPLOCK)
			SendLockItems(m_pItem);
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
#if defined( PRE_PVP_GAMBLEROOM )
			m_pDBCon->QueryGetPetalBalance(this);
#endif
			// PvP로비에서는 아래 정보를 보내주지 않아도 된다.
			CDNField* pField = g_pFieldManager->GetField( GetChannelID() );
			if( pField )
			{
				if (m_bIsChannelCounted == false)
				{
					g_pUserSessionManager->IncreaseChannelUserCount(GetChannelID());
					m_bIsChannelCounted = true;
				}
				else
					_DANGER_POINT();

				if( pField->bIsPvPLobby() )
				{
					if( LadderSystem::CManager::GetInstance().OnReconnect( this ) == false )
					{
						if( g_pMasterConnection && g_pMasterConnection->GetActive() )
							g_pMasterConnection->SendPvPEnterLobby( GetAccountDBID(), GetChannelID() );
					}
					m_bLoadUserData = true;	// 로딩완료
#if defined(PRE_ADD_PVPLEVEL_MISSION)					
					if( GetLastServerType() == SERVERTYPE_GAME )
						GetEventSystem()->OnEvent( EventSystem::OnPvPLevelChange );

					m_cLastServerType = SERVERTYPE_MAX;
#endif
					return ERROR_NONE;
				}
			}
			else
			{
				//필드가(채널이) 존재하지 않습니다. 여기서 끊긴다면 아마 내부적인 문제
				DetachConnection(L"Connect|Enterworld fail [Field NotFound]");
				return ERROR_NONE;
			}

			if (m_bFirst) {
#ifdef _PACKET_COMP
				//ULONG nPreTick = timeGetTime();
				//SetPacketComp(true);
#endif
#if defined( PRE_USA_FATIGUE )
				SendChannelInfoFatigueInfo();
#endif // #if defined( PRE_USA_FATIGUE )

#ifdef _USE_VOICECHAT
				SendVoiceChatInfo(g_Config.szPublicDolbyIp, g_Config.nControlPort, g_Config.nAudioPort, m_nAccountDBID);	// dolby info
#endif
				SendWarehouseList(m_UserData.Status.nWarehouseCoin, m_pItem);
#if defined(PRE_PERIOD_INVENTORY)
				SendPeriodWarehouse(m_pItem->IsEnablePeriodWarehouse(), m_pItem->GetPeriodWarehouseExpireDate());
#endif	// #if defined(PRE_PERIOD_INVENTORY)
#if defined(PRE_ADD_SERVER_WAREHOUSE)
				SendServerWareHouseList(m_pItem);
				SendServerWareHouseCashList(m_pItem);
#endif
				SendSkill(m_UserData.Skill[DualSkill::Type::Primary].SkillList, DualSkill::Type::Primary);
				SendSkill(m_UserData.Skill[DualSkill::Type::Secondary].SkillList, DualSkill::Type::Secondary);
				SendSkillPageCount( GetItem()->GetSkillPageCount() );
				SendQuestInvenList(m_pItem);
				SendCashGestureList();
				SendEffectItemGestureList();
				SendMissionList(m_UserData.Mission.nMissionScore, m_UserData.Mission.MissionGain, m_UserData.Mission.MissionAchieve, m_UserData.Mission.wLastMissionAchieve);
				SendGlyphTimeInfo(m_UserData.Status.nGlyphDelayTime, m_UserData.Status.nGlyphRemainTime);
				SendDailyMissionList( 0, m_UserData.Mission.DailyMission );
				SendDailyMissionList( 1, m_UserData.Mission.WeeklyMission );
				SendDailyMissionList( 2, m_UserData.Mission.GuildWarMission );
				SendDailyMissionList( 4, m_UserData.Mission.GuildCommonMission );
				SendDailyMissionList( 5, m_UserData.Mission.WeekendEventMission );
				SendDailyMissionList( 6, m_UserData.Mission.WeekendRepeatMission );
#if defined(PRE_ADD_MONTHLY_MISSION)
				SendDailyMissionList( 7, m_UserData.Mission.MonthlyMission );
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
				SendGuildWarBuyedItemList(m_GuildWarBuyedItem);
				if (m_pRestraint) m_pRestraint->SendRestraintList();
				SendKeySetting( &m_KeySetting );
				SendPadSetting( &m_PadSetting );
				SendNestClear(m_UserData.Status.NestClear);

#ifdef _PACKET_COMP
				//SetPacketComp(false);
				//printf("Test %d", timeGetTime() - nPreTick);
#endif

#if defined(_KR) || defined(_TH) || defined(_ID)
				g_pMasterConnection->SendPCBangResult(m_nAccountDBID);
#endif	// _KR

#if defined( PRE_ADD_SECONDARY_SKILL )
				if( m_pSecondarySkillRepository )
					m_pSecondarySkillRepository->SendList();
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
				SendPvPLadderScoreInfo( &m_PvPLadderScoreInfo );
				if (!GetGuildUID().IsSet())
					GuildRecruitSystem::CCacheRepository::GetInstance().GetMyGuildRecruit( this );
#if defined( PRE_ADD_NOTIFY_ITEM_COMPOUND )
				m_pCommonVariable->SendCommonVariableData(CommonVariable::Type::NotifyCompound);
#endif

				if( pField->GetChannelAtt()&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX )
				{
					ModCommonVariableData( CommonVariable::Type::LastPartyDungeonInfo, 0 );
				}
				else
				{
					INT64 biValue=0;
					if( GetCommonVariableDataValue( CommonVariable::Type::LastPartyDungeonInfo, biValue) == true && biValue > 0 )
						g_pMasterConnection->SendCheckLastDungeonInfo( this, biValue );

				}
#if defined( PRE_ADD_LIMITED_SHOP )
#if defined( PRE_FIX_74404 )
				SendLimitedShopItemData(m_LimitedShopBuyedItemList, true);
#else // #if defined( PRE_FIX_74404 )
				SendLimitedShopItemData(m_LimitedShopBuyedItem, true);
#endif // #if defined( PRE_FIX_74404 )
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )	
				if(GetPrivateChannelID() > 0)
				{
					CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo( GetWorldSetID(), GetPrivateChannelID() );
					if(pPrivateChatChannel)
					{
						if(pPrivateChatChannel->bIsLoaded())
							m_pDBCon->QueryAddPrivateChatChannelMember( m_pSession, GetPrivateChannelID(), PrivateChatChannel::Common::EnterMember );
						else
						{
							m_pDBCon->QueryGetPrivateChatChannelMember( m_pSession, GetPrivateChannelID() );
						}
					}
					else
						SetPrivateChannelID(0);
				}
#endif

#if defined( PRE_ADD_STAMPSYSTEM )
				if(m_pStampSystem)
					m_pStampSystem->SendInitData();
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
			}
			else // if (m_bFirst)
			{
#if defined( PRE_PARTY_DB )
				TPARTYID PartyID = 0;
				GetCommonVariableDataValue( CommonVariable::Type::MOVESERVER_PARTYID, PartyID );
				if( PartyID > 0 )
				{
					CDNParty* pParty = g_pPartyManager->GetParty( PartyID );
					if( pParty )
					{
						CSJoinParty TxPacket;
						memset( &TxPacket, 0, sizeof(TxPacket) );

						TxPacket.PartyID = PartyID;
						TxPacket.iPassword = pParty->GetPassword();

						OnRecvPartyMessage( CS_JOINPARTY, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
					}
					else
					{
						SendJoinParty( ERROR_PARTY_JOINFAIL, 0, NULL );
					}
					ModCommonVariableData( CommonVariable::Type::MOVESERVER_PARTYID, 0 );
				}
#endif // #if defined( PRE_PARTY_DB )
				GetDBConnection()->QueryMapInfo(this);

				//이동시 초기에 보내야할 것들은 이곳에 전부 몰아 주세요 요타이밍에 클라이언트 각 세션들이 전부 살아 있습니다.
				if (GetPartyID() > 0){
					CDNParty * pParty = g_pPartyManager->GetParty(GetPartyID());
					if (pParty != NULL)
					{
						pParty->SendAllRefreshGateInfo();	// 게이트 정보 날려주쟈~
						pParty->SendAllRefreshParty(PARTYREFRESH_RETURNVILLAGE);		// 셋되니까 정보도 날려주자
						pParty->SendPartyMemberMoveEachOther(GetSessionID(), m_BaseData.TargetPos.nX, m_BaseData.TargetPos.nY, m_BaseData.TargetPos.nZ);
					}
				}

				//hide show 동기만 맞추기 위해서 자기자신에게만 쏜다
				if ((GetAccountLevel() >= AccountLevel_New && GetAccountLevel() <= AccountLevel_Developer) && m_bHide == false)
					SendHide(GetSessionID(), m_bHide);

#if defined( PRE_PRIVATECHAT_CHANNEL )
				if(GetPrivateChannelID() > 0)
				{
					CDNPrivateChaChannel* pPrivateChatChannel = g_pPrivateChatChannelManager->GetPrivateChannelInfo( GetWorldSetID(), GetPrivateChannelID() );
					if(pPrivateChatChannel)
					{
						if( pPrivateChatChannel->bIsLoaded() )
						{
							if( pPrivateChatChannel->CheckPrivateChannelMember(GetCharacterDBID()) )
							{
								pPrivateChatChannel->SetPrivateChannelMemberAccountDBID(GetAccountDBID(), GetCharacterDBID());
								SendPrivateChatChannel(pPrivateChatChannel);
								m_pDBCon->QueryModPrivateMemberServerID( m_pSession );
							}
						}
						else
						{
							m_pDBCon->QueryGetPrivateChatChannelMember( m_pSession, GetPrivateChannelID() );
						}
					}
					else
						SetPrivateChannelID(0);
				}
#endif
			}

			SendInvenList(m_pItem);
#if defined(PRE_PERIOD_INVENTORY)
			SendPeriodInventory(m_pItem->IsEnablePeriodInventory(), m_pItem->GetPeriodInventoryExpireDate());
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			ModifyCompleteQuest();

			//초기 접속이 아니더라도 칭호를 날려줘야해서 이곳으로 옮김
			SendAppellationList(m_UserData.Appellation.Appellation);
			GetAppellation()->SendPeriodAppellationTime();
#if defined( PRE_ADD_NEWCOMEBACK )
			GetAppellation()->CheckComebackAppellation( GetAppellationData()->nSelectAppellation, GetAppellationData()->nCoverAppellation );

#endif
			if( g_pDataManager->GetEveryDayEventRewardMailID(1) )	//출석 이벤트 기간에만 함수 호출
				UpdateAttendanceEventData(false);

			if( GetAppellation()->CheckResetPcbangAppellation() )
			{
				//피씨방 칭호 초기화 알림
				GetAppellation()->SetResetPcbangAppellation(false);
				SendResetPcbangAppellation();
			}

			SendGlyphExpireData(m_pItem->GetGlyphExpireDate());
#if defined(PRE_ADD_TALISMAN_SYSTEM)
			SendTalismanExpireData(m_pItem->IsTalismanCashSlotEntend(), m_pItem->GetTalismanExpireDate());
#endif
			SendPvPGhoulScores(&m_PvPTotalGhoulScores);
			// Cash Inventory
			m_pDBCon->QueryGetPageMaterializedItem(GetDBThreadID(), this, 1, CASHINVENTORYPAGEMAX);
			// Vehicle Inventory
			m_pDBCon->QueryGetPageVehicle(this, 1, VEHICLEINVENTORYPAGEMAX);
#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
			// Payment Inventory
			m_pDBCon->QueryGetListRefundableProducts(this);
#endif //#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
			SetGuildWarScheduleID(g_pGuildWarManager->GetScheduleID());
			if( g_pGuildWarManager->GetScheduleID() != 0 )
			{
				SendGuildWarEventTime(g_pGuildWarManager->GetGuildWarSchedule(), g_pGuildWarManager->GetGuildWarFinalSchedule(), g_pGuildWarManager->GetFinalProgress());
			}			
			if (GetGuildUID().IsSet())
			{
				CDNGuildBase* pGuild = g_pGuildManager->At(GetGuildUID());
				if( pGuild && pGuild->IsEnrollGuildWar() ) 
				{					
					// 예선기간이거나 보상기간이면 포인트 얻어오기
					if( g_pGuildWarManager->GetStepIndex() == GUILDWAR_STEP_TRIAL || g_pGuildWarManager->GetStepIndex() == GUILDWAR_STEP_REWARD )
					{
						m_pDBCon->QueryGetGuildWarPoint(m_cDBThreadID, GetWorldSetID(), m_nAccountDBID, 'C', m_biCharacterDBID);
					}
				}
				// 우승길드인지 ?
				if( g_pGuildWarManager->GetPreWinGuildUID() == GetGuildUID() )				
					SendGuildWarPreWinGuild(true);
				if(pGuild)
				{
					TGuildRewardItem *RewardItemInfo;
					RewardItemInfo = pGuild->GetGuildRewardItem();
					SendGuildRewardItem(RewardItemInfo);

#if defined(PRE_ADD_GUILD_CONTRIBUTION)
					m_pDBCon->QueryGetGuildContributionPoint( m_cDBThreadID, m_nWorldSetID, GetAccountDBID(), GetCharacterDBID());
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
				}					
			}		
			SendGuildWarFestivalPoint();
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
			SendTotalLevel(GetSessionID(), m_nTotalLevelSKillLevel);
			for(int i=0;i<TotalLevelSkill::Common::MAXSLOTCOUNT;i++)
			{
				if(g_pDataManager->bIsTotalLevelSkillCashSlot(i))
					m_pSession->SendTotalLevelSkillCashSlot(m_pSession->GetSessionID(), i, m_pSession->bIsTotalLevelSkillCashSlot(i), m_pSession->GetTotalLevelSkillCashSlot(i));
			}
			SendTotalLevelSkillList(GetSessionID(), m_nTotalLevelSKillData);
#endif
			m_bLoadUserData = true;	// 로딩완료

			m_pTimeEventSystem->RequestSyncTimeEvent();
			// PvP데이터는 서버에 들어올때마다 보내준다.
			SendPvPData( m_UserData.PvP );
			m_pDBCon->GetCountReceiveMail(this);	// 메일 카운트 요청
			m_pDBCon->QueryNotifyGift(this, false);	// 선물 있는지 요청
#if defined(PRE_SPECIALBOX)
			m_pDBCon->QueryGetCountEventReward(this);	// 특수보관함 카운트 요청
#endif	// #if defined(PRE_SPECIALBOX)
			m_pDBCon->QueryGetCountHarvestDepotItem( this );	// 농장창고 카운트 요청
#ifdef PRE_ADD_BEGINNERGUILD
			m_pDBCon->QueryGetWillSendMails(this);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
#if defined( PRE_ALTEIAWORLD_EXPLORE )
			SendAlteiaWorldJoinInfo( m_cWeeklyPlayCount, m_cDailyPlayCount, m_cTicketCount );
#endif
			// OnMissionEvent
			GetEventSystem()->OnEvent( EventSystem::OnDungeonEnter );

#if defined(PRE_ADD_DWC)
			if( IsDWCCharacter() )
				SendDWCChannelInfo();
#endif
			// 최초 접속시
			if( GetLastServerType() == SERVERTYPE_LOGIN )
			{
				if(GetGuildUID().IsSet())	//사제 시스템 디파인 통합할때 아래에 합치자
				{
					CDNGuildBase* pGuild = g_pGuildManager->At(GetGuildUID());
					if (pGuild && pGuild->GetInfo()->cGuildType != BeginnerGuild::Type::Beginner)	//초보자 길드는 가입미션 안됨
						GetEventSystem()->OnEvent( EventSystem::OnGuildJoin );
				}		

				if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				{
					g_pMasterConnection->SendMasterSystemSyncConnect( true, GetCharacterName(), GetMasterSystemData() );
					if(GetGuildUID().IsSet())
					{
						g_pMasterConnection->SendChangeGuildMemberInfo(GetGuildUID(), GetAccountDBID(), GetCharacterDBID(), GetAccountDBID(), GetCharacterDBID(), GUILDMEMBUPDATE_TYPE_LOGINOUT, _LOCATION_MOVE, 0, 0, NULL, true);
						CDNGuildBase* pGuild = g_pGuildManager->At(GetGuildUID());
						if (pGuild)
						{
#if !defined( PRE_ADD_NODELETEGUILD )
							CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
#endif
#if defined( PRE_ADD_NODELETEGUILD )
							if (pGuild->GetRecvAllMember())
							{
								SendGetGuildInfo(pGuild, ERROR_NONE);
								SendGetGuildMember(pGuild, ERROR_NONE);
							}
#else	// #if defined( PRE_ADD_NODELETEGUILD )
							if (TRUE == pGuild->IsEnable() && pGuild->GetRecvAllMember())
							{
								SendGetGuildInfo(pGuild, ERROR_NONE);
								SendGetGuildMember(pGuild, ERROR_NONE);
							}
#endif	// #if defined( PRE_ADD_NODELETEGUILD )
						}						
					}
				}
				OnMasterSystemEvent( MasterSystem::EventType::VillageFirstConnect );
				GetEventSystem()->OnEvent( EventSystem::OnJobChange );
#if defined(PRE_ADD_REMOTE_QUEST)
				GetQuest()->OnEvent(EventSystem::OnQuestClear);	//최초 로그인시 메타퀘스트 타입을 체크하기 위한 구조
#endif
#if defined(PRE_ADD_DWC)
				if( IsDWCCharacter() )
				{
					if( GetDWCTeamID() > 0 && g_pDWCTeamManager )
					{
						CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(GetDWCTeamID());
						if(pDWCTeam && pDWCTeam->GetRecvMemberList())
						{	// CDnDWCTeam::UpdateMemberList 에서 이미 등록 처리 된다.
							//g_pDWCTeamManager->RegistDWCCharacter(GetCharacterDBID(), GetDWCTeamID());
// 							SendGetDWCTeamInfo(pDWCTeam, ERROR_NONE);
// 							SendGetDWCTeamMember(pDWCTeam, ERROR_NONE);
							
							TCommunityLocation Location;
							Location.Reset();
							Location.cServerLocation = _LOCATION_MOVE;
							pDWCTeam->ChangeDWCTeamMemberState(GetCharacterDBID(), &Location, true);

							std::vector<INT64> vecMemberList;
							pDWCTeam->GetMemberCharacterDBIDList(vecMemberList);
							if( g_pMasterConnection && g_pMasterConnection->GetActive() )
								g_pMasterConnection->SendDWCTeamMemberList(GetAccountDBID(), GetDWCTeamID(), vecMemberList);
						}
						else
							SendGetDWCTeamInfo(NULL, ERROR_DWC_NOTEXIST_TEAMINFO);
					}
					else
						SendGetDWCTeamInfo(NULL, ERROR_DWC_HAVE_NOT_TEAM);
				}
#endif
			}
			SendMasterSystemSimpleInfo( m_MasterSystemData.SimpleInfo );
#if defined(PRE_ADD_PVPLEVEL_MISSION)
			if( GetLastServerType() == SERVERTYPE_LOGIN || GetLastServerType() == SERVERTYPE_GAME )
				GetEventSystem()->OnEvent( EventSystem::OnPvPLevelChange );
#endif
#if defined(PRE_ADD_MISSION_COUPON)
			if( GetExpiredPetID() > 0 )
			{
				GetEventSystem()->OnEvent( EventSystem::OnPetExpired, 1, EventSystem::ItemID, GetExpiredPetID() );
				SetExpiredPetID(0);
			}
#endif

#if defined( PRE_ADD_VIP_FARM )
			if( GetEffectRepository() )
				GetEffectRepository()->SendEffectItem();
#endif // #if defined( PRE_ADD_VIP_FARM )
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
			_ASSERT( m_pReputationSystem && m_pReputationSystem->GetEventHandler() );
			m_pReputationSystem->GetEventHandler()->OnConnect( 0, true );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#if defined( PRE_ADD_NEW_MONEY_SEED )
			SendSeedPoint();
#endif
			m_cLastServerType = SERVERTYPE_MAX;
			m_bSecurityUpdate = true;
#if defined(_WORK)
			wstring wszString = FormatW(L"맵 이동 -> ID:[%d]\r\n", GetStatusData()->nMapIndex);
			m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif
			return ERROR_NONE;
		}
		break;

	case CS_COMPLETELOADING:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			if (!EnterWorld()){
				DetachConnection(L"Connect|Enterworld fail");
				g_Log.Log(LogType::_ERROR, this, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [CS_CHAR_ENTER] Enterworld fail!! Char:%u\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
				return ERROR_GENERIC_UNKNOWNERROR;
			}

#ifdef _FINAL_BUILD
			if (GetAccountLevel() < AccountLevel_New && GetField()->GetPermitLevel() > 0)
			{	//운영자 등등등이 아니시면
				if (GetField()->GetPermitLevel() > GetLevel())
				{
#if defined(PRE_ADD_MULTILANGUAGE)
					std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6474, m_eSelectedLanguage);
					std::wstring wszRestraintReasonForDolis = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6475, m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
					std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6474);
					std::wstring wszRestraintReasonForDolis = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6475);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
					m_pDBCon->QueryAddRestraint(m_pSession, DBDNWorldDef::RestraintTargetCode::Account, DBDNWorldDef::RestraintTypeCode::ConnectRestraint, wszRestraintReason.c_str(), wszRestraintReasonForDolis.c_str(), 30);

					DetachConnection(L"Connect|PermitLevel fail");
					g_Log.Log(LogType::_ERROR, this, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [CS_CHAR_ENTER] PermitLevel fail!! Char:%u\r\n", m_nAccountDBID, m_biCharacterDBID, GetSessionID(), m_BaseData.nObjectID);
					return ERROR_GENERIC_UNKNOWNERROR;
				}
			}
#endif

			if (g_pMasterConnection && g_pMasterConnection->GetActive())
				g_pMasterConnection->SendEnterVillage(GetAccountDBID(), m_pField->GetChnnelID());
			else
			{
				DetachConnection(L"Connect|MasterCon Not Found");
				return ERROR_NONE;
			}

			m_bCharOutLog = true;	// leave할때 log 남겨지게

			TNoticeInfo Notice;
			memset(&Notice, 0, sizeof(Notice));

			if (g_pNoticeSystem)
			{
				g_pNoticeSystem->GetNotice(m_pField->GetChnnelID(), m_pField->GetMapIndex(), Notice);
				if (Notice.nCreateTime > 0)
					SendNotice(Notice.wszMsg, (int)wcsnlen(Notice.wszMsg, CHATLENMAX), Notice.TypeInfo.nSlideShowSec);
			}

			if (g_pCloseSystem)
			{
				__time64_t _tNow, _tOderedTime, _tCloseTime;
				if (g_pCloseSystem->IsClosing(_tOderedTime, _tCloseTime))
				{
					time(&_tNow);
					SendCloseService(_tNow, _tOderedTime, _tCloseTime);
				}
			}

#ifdef PRE_ADD_COMEBACK
			if (m_bComebackUser)
			{
				if (m_pDBCon)
				{
					m_pDBCon->QueryModLastConnectDate(this);
#if defined( PRE_ADD_NEWCOMEBACK )
					m_pDBCon->QueryModComebackFlag(this, false);
#endif
				}

				bool bComebackSended = false;
				TLevelupEvent ComebackMail;
				int nComebackMailID = 0;
				if (g_pDataManager->GetLevelupEventbyType(GetLevel(), TLevelupEvent::CombackUser, ComebackMail))
				{
					nComebackMailID = ComebackMail.nMailID > 0 ? ComebackMail.nMailID : ComebackMail.nCashMailID;
					if (nComebackMailID > 0)
					{
						CDNMailSender::Process(this, nComebackMailID, DBDNWorldDef::PayMethodCode::Comeback);
						bComebackSended = true;
						g_Log.Log(LogType::_COMEBACK, this, L"AccountDBID[%d] ComebackUser!! MailSended! MailID[%d]\n", GetAccountDBID(), nComebackMailID);
						m_bComebackUser = false;
					}
					else
						_DANGER_POINT_MSG(L"nComebackMailID error");
				}
				else
					_DANGER_POINT_MSG(L"GetLevelupEventbyType return false");
				
#if defined( PRE_ADD_NEWCOMEBACK )				
				// 귀환자 이펙트아이템 넣어주기
				int nCombackEffectItemID = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::ComebackEffectItemID);
				if(nCombackEffectItemID > 0)
				{
					TItemData* pItemData = g_pDataManager->GetItemData(nCombackEffectItemID);
					if( pItemData && GetItem() )
					{
						TItem NewItem = {0,};
						GetItem()->MakeItemStruct(pItemData->nItemID, NewItem, (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::ComebackGiveTerm), 0);
						
						if( GetEffectRepository()->Add( NewItem.nSerial, pItemData->nItemID, NewItem.tExpireDate ))
						{
							GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cash, 0, 0, 
								NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, 0);
						}
					}
				}
				TLevelupEvent ComebackInven;
				if (g_pDataManager->GetLevelupEventbyType(GetLevel(), TLevelupEvent::ComeBackUserInven, ComebackInven))
				{					
					CheckComebackRewardItem( ComebackInven );
					bComebackSended = true;
					m_bComebackUser = false;
				}
#endif
				if (bComebackSended == false)
				{
					g_Log.Log(LogType::_COMEBACK, this, L"AccountDBID[%d] ComebackUser!! MailSen Failed!! MailID[%d] Level[%d]\n", GetAccountDBID(), nComebackMailID, GetLevel());
				}
			}
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined( PRE_ADD_GAMEQUIT_REWARD )
			if(m_bReConnectUserReward)
			{
				m_bReConnectUserReward = false;
				GetDBConnection()->QueryModNewbieRewardFlag(this, true);
				g_Log.Log(LogType::_GAMEQUITREWARD, this, L"AccountDBID[%d] ReConnectNewbieReward(DB CHECK)\n", GetAccountDBID());
			}
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )
			return ERROR_NONE;
		}
		break;
	}

	return CDNUserBase::OnRecvCharMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvActorMessage(int nSubCmd, char *pData, int nLen)
{
	CSActorMessage *pActorMsg = (CSActorMessage*)pData;

	m_ParamData.wActorProtocol = nSubCmd;
	memcpy(&m_ParamData.Buffer, pActorMsg->cBuf, 128);

	ScopeEncryptFlag Scope( &m_ParamData.bEncrypt );

	EtVector3 vPos;
	EtVector2 vDir, vLook;

	CPacketCompressStream Stream(pActorMsg->cBuf, 128);

	m_ParamData.dwSendSessionID = ( pActorMsg->nSessionID != 0 ) ? pActorMsg->nSessionID : GetSessionID(); // SendSessionID
	switch(nSubCmd)
	{
	case CS_CMDMOVE:
		{
			Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
			Stream.Seek( sizeof(int), SEEK_CUR, CPacketCompressStream::INTEGER_SHORT );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &vDir, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

			m_BaseData.TargetPos.nX = (int)(vPos.x * 1000.f);
			m_BaseData.TargetPos.nY = (int)(vPos.y * 1000.f);
			m_BaseData.TargetPos.nZ = (int)(vPos.z * 1000.f);

			m_BaseData.MoveDir.nX = (int)(vDir.x * 1000.f);
			m_BaseData.MoveDir.nY = 0;
			m_BaseData.MoveDir.nZ = (int)(vDir.y * 1000.f);

			m_BaseData.fRotate = EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vLook ) ) );

			m_ParamData.nBufSize = nLen;
			SendFieldMessage(FM_MOVE);

			if (GetPartyID() > 0){	// 파티가 있다면
				CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
				if (pParty){
					pParty->SendPartyMemberMove(GetSessionID(), vPos);
				}

#ifdef _USE_VOICECHAT
				m_nVoiceRotate = (int)EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vDir ) ) );
				if ( vDir.x > 0.0f )
					m_nVoiceRotate = 360 - m_nVoiceRotate;
				m_nVoiceRotate = (m_nVoiceRotate + 90) % 360;
				SetVoicePos((int)vPos.x, (int)vPos.y, (int)vPos.z, m_nVoiceRotate);
#endif
			}

			CheckValidCollisionHeight();

			return ERROR_NONE;
		}
		break;
	case CS_CMDMOVEPOS:
		{
			Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
			Stream.Seek( sizeof(int), SEEK_CUR, CPacketCompressStream::INTEGER_SHORT );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Seek( sizeof(EtVector3), SEEK_CUR, CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &vDir, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

			m_BaseData.TargetPos.nX = (int)(vPos.x * 1000.f);
			m_BaseData.TargetPos.nY = (int)(vPos.y * 1000.f);
			m_BaseData.TargetPos.nZ = (int)(vPos.z * 1000.f);

			m_BaseData.MoveDir.nX = (int)(vDir.x * 1000.f);
			m_BaseData.MoveDir.nY = 0;
			m_BaseData.MoveDir.nZ = (int)(vDir.y * 1000.f);

			m_BaseData.fRotate = EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vDir ) ) );

			m_ParamData.nBufSize = nLen;
			SendFieldMessage(FM_MOVE);

			if (GetPartyID() > 0){	// 파티가 있다면
				CDNParty *pParty = g_pPartyManager->GetParty(GetPartyID());
				if (pParty){
					pParty->SendPartyMemberMove(GetSessionID(), vPos);
				}

#ifdef _USE_VOICECHAT
				m_nVoiceRotate = (int)EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vDir ) ) );
				if( vDir.x > 0.0f )
					m_nVoiceRotate = 360 - m_nVoiceRotate;
				m_nVoiceRotate = (m_nVoiceRotate + 90) % 360;
				SetVoicePos((int)vPos.x, (int)vPos.y, (int)vPos.z, (int)m_nVoiceRotate);
#endif
			}

			return ERROR_NONE;
		}
		break;
		/*
	case CS_POSREV:
		{
			bool bMoveVector;

			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
			Stream.Read( &bMoveVector, sizeof(bool) );

			m_BaseData.TargetPos.nX = (int)(vPos.x * 1000.f);
			m_BaseData.TargetPos.nY = (int)(vPos.y * 1000.f);
			m_BaseData.TargetPos.nZ = (int)(vPos.z * 1000.f);

			if( bMoveVector ) {
				Stream.Read( &vDir, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				m_BaseData.MoveDir.nX = (int)(vDir.x * 1000.f);
				m_BaseData.MoveDir.nY = 0;
				m_BaseData.MoveDir.nZ = (int)(vDir.y * 1000.f);
			}

			m_ParamData.nBufSize = nLen;
			SendFieldMessage(FM_MOVE);

#ifdef _USE_VOICECHAT
			//if( m_nPartyIndex > 0 )
			//{
			//	CVoiceChatServer::GetInstance().SetPlayerPos( m_nAccountDBID, ( int )vPos.x, ( int )vPos.y, ( int )vPos.z, ( int )m_BaseData.fRotate );
			//}
#endif

			return ERROR_NONE;
		}
		break;
		*/

	case CS_CMDSTOP:
		{
			Stream.Seek( sizeof(DWORD), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

			m_BaseData.TargetPos.nX = (int)(vPos.x * 1000.f);
			m_BaseData.TargetPos.nY = (int)(vPos.y * 1000.f);
			m_BaseData.TargetPos.nZ = (int)(vPos.z * 1000.f);

			m_BaseData.MoveDir.nX = 0;
			m_BaseData.MoveDir.nY = 0;
			m_BaseData.MoveDir.nZ = 0;

			m_ParamData.nBufSize = nLen;
			SendFieldMessage(FM_MOVE);

#ifdef _USE_VOICECHAT
			SetVoicePos((int)vPos.x, (int)vPos.y, (int)vPos.z, (int)m_nVoiceRotate);
#endif
			return ERROR_NONE;
		}
		break;

	case CS_CMDTOGGLEBATTLE:
		{
			Stream.Read( &m_BaseData.bBattleMode, sizeof(bool) );

			m_ParamData.nBufSize = nLen;
			SendUserLocalMessage(0, FM_ACTORMSG);

			return ERROR_NONE;
		}
		break;

	case CS_CMDTOGGLEWEAPONORDER:
		{
			int nEquipIndex;
			bool bEquipCash;

			Stream.Read( &nEquipIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &bEquipCash, sizeof(bool) );

			if( nEquipIndex == 0 || nEquipIndex == 1 ) {
				SetViewCashEquipBitmap((char)(CASHEQUIP_WEAPON1 + nEquipIndex), bEquipCash);
			}

			m_ParamData.nBufSize = nLen;
			SendUserLocalMessage(0, FM_ACTORMSG);
			return ERROR_NONE;
		}
		break;

	case CS_CMDTOGGLEPARTSORDER:
		{
			int nEquipIndex;
			bool bEquipCash;

			Stream.Read( &nEquipIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &bEquipCash, sizeof(bool) );

			SetViewCashEquipBitmap((char)nEquipIndex, bEquipCash);

			m_ParamData.nBufSize = nLen;
			SendUserLocalMessage(0, FM_ACTORMSG);

			if (GetPartyID() > 0) {
				if( nEquipIndex == CASHEQUIP_HELMET || nEquipIndex == CASHEQUIP_EARRING || nEquipIndex == HIDEHELMET_BITINDEX ) {
					g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
				}
			}

			return ERROR_NONE;
		}
		break;

	case CS_CMDESCAPE:
		{
			// 시간체크 함 해주고..
			DWORD dwCurTime = timeGetTime();
#ifndef _WORK
			if( dwCurTime - m_dwLastEscapeTime < 1000 * 60 * 2 ) break;
#endif
			m_dwLastEscapeTime = dwCurTime;
			////////////////////////
			TPosition Pos = { 0, };
			char cGateNo = g_pFieldDataManager->GetRandomGateNo( m_UserData.Status.nMapIndex );
			g_pFieldDataManager->GetStartPosition( m_UserData.Status.nMapIndex, cGateNo, Pos );
			m_UserData.Status.nPosX = Pos.nX;
			m_UserData.Status.nPosY = Pos.nY;
			m_UserData.Status.nPosZ = Pos.nZ;

			m_BaseData.CurPos = Pos;
			m_BaseData.TargetPos = Pos;

			EtVector3 vPos( Pos.nX / 1000.f, Pos.nY / 1000.f, Pos.nZ / 1000.f );

			BYTE pBuffer[128];
			CPacketCompressStream Stream( pBuffer, 128 );
			Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

			SendActorMsg( GetSessionID(), SC_CMDESCAPE, Stream.Size(), Stream.GetBuffer(), NULL );
		}
		break;

	case CS_USESKILL:
		{
			int nSkillTableID;
			bool bUseApplySkillItem = false;
			Stream.Read( &nSkillTableID, sizeof(int) );
			Stream.Seek( sizeof(char), SEEK_CUR, CPacketCompressStream::NOCOMPRESS );
			Stream.Read( &bUseApplySkillItem, sizeof(bool) );
			if( !bUseApplySkillItem ) {
				if( GetSkill()->FindSkill( nSkillTableID ) == false )
					return ERROR_NONE;
			}
			else {
				if( GetLastUseItemSkillID() != nSkillTableID )
					return ERROR_NONE;
			}
			int iCoolTime = g_pDataManager->GetSkillDelayTime( nSkillTableID, GetSkill()->GetSkillLevel(nSkillTableID) );

			// 글로벌 쿨타임이 설정되어있다면 해당 값으로 쿨타임 대체해서 저장해준다.
			int iGlobalCoolTime = g_pDataManager->GetSkillGlobalCoolTime( nSkillTableID );
			if( 0 < iGlobalCoolTime )
				iCoolTime = iGlobalCoolTime;

			// 글로벌로 묶인 다른 스킬들도 쿨타임 돌려줌.
			int iGlobalSkillGroupID = g_pDataManager->GetSkillGlobalGroupID( nSkillTableID );
			if( 0 < iGlobalSkillGroupID )
			{
				const vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>& vlPossessedSkill = GetSkill()->GetPossessedSkillInfo();
				for( int i = 0; i <(int)vlPossessedSkill.size(); ++i )
				{
					int iNowGroupSkillID = vlPossessedSkill.at( i ).iSkillID;
					int iNowGlobalSkillGroupID = g_pDataManager->GetSkillGlobalGroupID( iNowGroupSkillID );
					if( 0 < iNowGlobalSkillGroupID )
					{
						if( iNowGlobalSkillGroupID == iGlobalSkillGroupID )
						{
							m_SkillCoolTime.Update( iNowGroupSkillID, timeGetTime() );
							m_SkillCoolTime.AddCoolTime( iNowGroupSkillID, timeGetTime()+iCoolTime );
						}
					}
				}
			}

			if( iCoolTime > 0 )
			{
				m_SkillCoolTime.Update( nSkillTableID, timeGetTime() );
				m_SkillCoolTime.AddCoolTime( nSkillTableID, timeGetTime()+iCoolTime );
			}
			m_ParamData.nBufSize = nLen;
			SendUserLocalMessage(0, FM_ACTORMSG);
			return ERROR_NONE;
		}
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	case CS_ADD_TOTAL_LEVEL_SKILL:
		{
			int nSlotIndex = -1;
			int nSkillID = 0;
			Stream.Read( &nSlotIndex, sizeof(int) );
			Stream.Read( &nSkillID, sizeof(int) );

			if(!m_pSession->AddTotalLevelSkillData(nSlotIndex, nSkillID))
			{
				break;
			}

			m_pSession->SendAddTotalLevelSkill(m_pSession->GetSessionID(), nSlotIndex, nSkillID);
		}
		break;
	case CS_REMOVE_TOTAL_LEVEL_SKILL:
		{
			int nSlotIndex = -1;
			
			Stream.Read( &nSlotIndex, sizeof(int) );

			if( nSlotIndex < 0 || nSlotIndex >= TotalLevelSkill::Common::MAXSLOTCOUNT )
				break;

			m_pSession->AddTotalLevelSkillData(nSlotIndex, 0);
			m_pSession->SendDelTotalLevelSkill(m_pSession->GetSessionID(), nSlotIndex);
		}
		break;
#endif
	default:
		m_ParamData.nBufSize = nLen;
		SendUserLocalMessage(0, FM_ACTORMSG);
		return ERROR_NONE;
		break;
	}

	return ERROR_UNKNOWN_HEADER;		//상위에서 리턴되어야 합니다 여까지 오면 처리하지 않는 패킷의 수신!
}

int CDNUserSession::OnRecvPropMessage(int nSubCmd, char *pData, int nLen)
{
	return ERROR_UNKNOWN_HEADER;
}

int CDNUserSession::OnRecvItemMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
	case CS_MOVEITEM:
		{
			if (sizeof(CSMoveItem) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveItem *pMove = (CSMoveItem*)pData;
			// 창고 관련한 애들은 Blind상태로 들어옴..
			if (!IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				int nRet = ERROR_ITEM_FAIL;
				switch(pMove->cMoveType)
				{
				case MoveType_EquipToInven: nRet = ERROR_ITEM_EQUIPTOINVEN_FAIL;	break;
				case MoveType_InvenToEquip: nRet = ERROR_ITEM_INVENTOEQUIP_FAIL;	break;
				}
				SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, NULL, NULL, nRet);
				return ERROR_NONE;
			}

			m_pItem->OnRecvMoveItem(pMove);

			return ERROR_NONE;
		}
		break;

	case CS_MOVECASHITEM:
		{
			if (sizeof(CSMoveCashItem) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveCashItem *pMove = (CSMoveCashItem*)pData;

			if (!IsNoneWindowState())
			{
				int nRet = ERROR_ITEM_FAIL;
				switch(pMove->cMoveType)
				{
					case MoveType_CashEquipToCashInven: 
					{
						nRet = ERROR_ITEM_EQUIPTOINVEN_FAIL;
						break;
					}

					case MoveType_CashInvenToCashEquip: 
					{
						nRet = ERROR_ITEM_INVENTOEQUIP_FAIL;
						break;
					}

					case MoveType_VehicleBodyToVehicleInven:
					case MoveType_VehicleInvenToVehicleBody:
					case MoveType_VehiclePartsToCashInven:
					case MoveType_CashInvenToVehicleParts:
					{
						nRet = ERROR_ITEM_INVENTOEQUIP_FAIL;
						break;
					}

					case MoveType_PetInvenToPetBody:
					{
						nRet = ERROR_CANT_RECALL_PET_STATUS;
						break;
					}
				}
				SendMoveCashItem(pMove->cMoveType, pMove->cCashEquipIndex, NULL, pMove->biCashInvenSerial, NULL, nRet);
				return ERROR_NONE;
			}

			m_pItem->OnRecvMoveCashItem(pMove);

			return ERROR_NONE;
		}
		break;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	case CS_MOVE_SERVERWARE :
		{
			if (sizeof(CSMoveServerWare) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveServerWare *pMove = (CSMoveServerWare*)pData;
			// 창고 관련한 애들은 Blind상태로 들어옴..
			if (!IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				int nRet = ERROR_ITEM_FAIL;
				//SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, NULL, NULL, nRet);
				return ERROR_NONE;
			}
			m_pItem->OnRecvMoveServerWareItem(pMove);
			return ERROR_NONE;
		}
		break;
	case CS_MOVE_CASHSERVERWARE :
		{
			if (sizeof(CSMoveCashServerWare) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveCashServerWare *pMove = (CSMoveCashServerWare*)pData;
			// 창고 관련한 애들은 Blind상태로 들어옴..
			if (!IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				int nRet = ERROR_ITEM_FAIL;
				//SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, NULL, NULL, nRet);
				return ERROR_NONE;
			}
			m_pItem->OnRecvMoveServerWareCashItem(pMove);
			return ERROR_NONE;
		}
		break;
#endif
	case CS_MOVEGUILDITEM:
		{
#if defined(_CH)
			if (GetLevel() <= 40){
				SendMoveGuildItem(0, 0, 0, NULL, NULL, ERROR_GUILD_INVALIDWARE);
				return ERROR_NONE;
			}
#endif	// #if defined(_CH)

			if (sizeof(CSMoveGuildItem) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveGuildItem *pMove = (CSMoveGuildItem*)pData;
			// 길드창고 관련은 블라인드 상태에서만 와야함.
			if (!IsWindowState(WINDOW_BLIND))
			{
				SendMoveGuildItem(pMove->cMoveType, 0, 0, NULL, NULL, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
			{
				SendMoveGuildItem(pMove->cMoveType, 0, 0, NULL, NULL, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			const TGuildUID GuildUID(GetGuildUID());
			if (!GuildUID.IsSet()) 
			{
				SendMoveGuildItem(pMove->cMoveType, 0, 0, NULL, NULL, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_NONE;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			// 길드소속인지 확인
			if (!pGuild)
			{
				SendMoveGuildItem(pMove->cMoveType, 0, 0, NULL, NULL, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_NONE;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) return ERROR_NONE;
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = pGuildVillage->GetGuildWare();

				if (pGuildWare)
					pGuildWare->OnRecvMoveGuildItem(this, pMove);
				else
					SendMoveGuildItem(pMove->cMoveType, 0, 0, NULL, NULL, ERROR_GUILDWARE_NOT_READY);
			}
		}
		break;

	case CS_MOVEGUILDCOIN:
		{
#if defined(_CH)
			if (GetLevel() <= 40){
				SendMoveGuildItem(0, 0, 0, NULL, NULL, ERROR_GUILD_INVALIDWARE);
				return ERROR_NONE;
			}
#endif	// #if defined(_CH)

			if (sizeof(CSMoveGuildCoin) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveGuildCoin *pMove = (CSMoveGuildCoin*)pData;
			if (!IsWindowState(WINDOW_BLIND))
			{
				SendMoveGuildCoin(pMove->cMoveType, 0, 0, 0, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
			{
				SendMoveGuildItem(pMove->cMoveType, 0, 0, NULL, NULL, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			const TGuildUID GuildUID(GetGuildUID());
			if (!GuildUID.IsSet()) 
			{
				SendMoveGuildCoin(pMove->cMoveType, 0, 0, 0, 0, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_NONE;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			// 길드소속인지 확인
			if (!pGuild)
			{
				SendMoveGuildCoin(pMove->cMoveType, 0, 0, 0, 0, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_NONE;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) 
			{
				SendMoveGuildCoin(pMove->cMoveType, 0, 0, 0, 0, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_NONE;
			}
#endif

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = pGuildVillage->GetGuildWare();

				if (pGuildWare)
					pGuildWare->OnRecvMoveGuildCoin(this, pMove);
				else
					SendMoveGuildCoin(pMove->cMoveType, 0, 0, 0, 0, ERROR_GUILDWARE_NOT_READY);
			}
		}
		break;

	case CS_REMOVEITEM:
		{
			if (sizeof(CSRemoveItem) != nLen)
				return ERROR_INVALIDPACKET;

			CSRemoveItem *pRemoveItem = (CSRemoveItem*)pData;

			if (!IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				SendRemoveItem(pRemoveItem->cType, pRemoveItem->cSlotIndex, NULL, ERROR_ITEM_FAIL);
				return ERROR_NONE;
			}

#if defined( PRE_ADD_DWC )
			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false && AccountLevel_DWC != m_UserData.Status.cAccountLevel)
#else // #if defined( PRE_ADD_DWC )
			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
#endif // #if defined( PRE_ADD_DWC )
				return ERROR_NONE;

			m_pItem->OnRecvRemoveItem(pRemoveItem);
			return ERROR_NONE;
		}
		break;

	case CS_EMBLEM_COMPOUND_OPEN_REQ:
		{
			//if (!IsValidPacketByNpcClick()){	// npc 거리체크
			//	return ERROR_GENERIC_INVALIDREQUEST;
			//}

			if (sizeof(CSEmblemCompoundOpenReq) != nLen)
				return ERROR_INVALIDPACKET;

			CSEmblemCompoundOpenReq* pCompoundItem = (CSEmblemCompoundOpenReq*)pData;

			// 문장은 WINDOW_BLIND 상태로 들어와야함.
			if( !IsWindowState(WINDOW_BLIND)) {
				SendEmblemCompoundRes( ERROR_ITEM_EMBLEM_COMPOUND_FAIL, 0 );
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			CDnItemCompounder::S_ITEM_SETTING_INFO ItemSettingInfo;
			CDnItemCompounder::S_OUTPUT Output;
			CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;

			ItemSettingInfo.iCompoundTableID = pCompoundItem->nCompoundTableID;
			ItemSettingInfo.iHasMoney = GetCoin();		// Note: GetCoin 리턴 값이 INT64임.

			const TItem* pPlateItem = m_pItem->GetInventory( pCompoundItem->cPlateItemSlotIndex );
			if( NULL == pPlateItem )
				return ERROR_GENERIC_INVALIDREQUEST;

			if( pCompoundItem->biPlateItemSerialID != pPlateItem->nSerial )
				return ERROR_GENERIC_INVALIDREQUEST;

			g_pDataManager->GetItemCompounder()->GetCompoundInfo( pCompoundItem->nCompoundTableID, &CompoundInfo );

			bool bValidHaveItem = true;
			int iItemSettingArrayCount = 0;
			for( int i = 0; i < COMPOUNDITEMMAX; ++i )
			{
				if( 0 == CompoundInfo.aiItemID[ i ] )
					break;

				int iHaveCount = m_pItem->GetInventoryItemCount(  CompoundInfo.aiItemID[ i ]  );
				if( iHaveCount <  CompoundInfo.aiItemCount[ i ]  )
				{
					bValidHaveItem = false;
					break;
				}

				ItemSettingInfo.aiItemID[ i ] = CompoundInfo.aiItemID[ i ];
				ItemSettingInfo.aiItemCount[ i ] = CompoundInfo.aiItemCount[ i ];
				++iItemSettingArrayCount;
			}
			
			int iResult = ERROR_ITEM_EMBLEM_COMPOUND_FAIL;
			
			if( bValidHaveItem )
			{
				// 플레이트 정보가 있어야 문장 보옥 조합임..
				int iPlateItemID = pPlateItem->nItemID;
				if( -1 != iPlateItemID )
				{
					iResult = ERROR_ITEM_EMBLEM_COMPOUND_FAIL;
					if( GetCheatCommand()->m_bLucky ) g_pDataManager->GetItemCompounder()->CheckPossibility( false );
					g_pDataManager->EmblemCompound( iPlateItemID, ItemSettingInfo, &Output );
					if( GetCheatCommand()->m_bLucky ) g_pDataManager->GetItemCompounder()->CheckPossibility( true );

					if( CDnItemCompounder::R_SUCCESS == Output.eResultCode ||
						CDnItemCompounder::R_POSSIBILITY_FAIL == Output.eResultCode )
					{
						iResult = ERROR_NONE;
						BroadcastingEffect(EffectType_Emblem, EffectState_Start);
					}
				}
			}

			SendEmblemCompoundOpen( iResult );
			return ERROR_NONE;
		}
		break;

	case CS_ITEM_COMPOUND_OPEN_REQ:
		{
			int iResult = ParseItemCompound( reinterpret_cast<CSItemCompoundOpenReq*>(pData), nLen );
			return iResult;
		}
		break;

	case CS_EMBLEM_COMPOUND_REQ:
		{
			/*if(!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}*/

			if (sizeof(CSCompoundEmblemReq) != nLen)
				return ERROR_INVALIDPACKET;

			// Note: 추후에 NPC 별로 다른 리스트를 보여주게 되면 날아온 데이터가 해당 NPC 리스트에 존재하는 것인지 체크해야함.
			CSCompoundEmblemReq* pCompoundItem = (CSCompoundEmblemReq*)pData;

			CDnItemCompounder::S_ITEM_SETTING_INFO ItemSettingInfo;
			CDnItemCompounder::S_OUTPUT Output;
			CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;

			ItemSettingInfo.iCompoundTableID = pCompoundItem->nCompoundTableID;
			ItemSettingInfo.iHasMoney = GetCoin();		// Note: GetCoin 리턴 값이 INT64임.

			const TItem* pPlateItem = m_pItem->GetInventory( pCompoundItem->cPlateItemSlotIndex );
			if( NULL == pPlateItem )
				return ERROR_GENERIC_INVALIDREQUEST;

			if( pCompoundItem->biPlateItemSerialID != pPlateItem->nSerial )
				return ERROR_GENERIC_INVALIDREQUEST;

			g_pDataManager->GetItemCompounder()->GetCompoundInfo( pCompoundItem->nCompoundTableID, &CompoundInfo );

			bool bValidHaveItem = true;
			int iItemSettingArrayCount = 0;
			for( int i = 0; i < COMPOUNDITEMMAX; ++i )
			{
				if( 0 == CompoundInfo.aiItemID[ i ] )
					break;

				int iHaveCount = m_pItem->GetInventoryItemCount( CompoundInfo.aiItemID[ i ] );
				if( iHaveCount < CompoundInfo.aiItemCount[ i ] )
				{
					bValidHaveItem = false;
					break;
				}

				ItemSettingInfo.aiItemID[ i ] = CompoundInfo.aiItemID[ i ];
				ItemSettingInfo.aiItemCount[ i ] = CompoundInfo.aiItemCount[ i ];
				++iItemSettingArrayCount;
			}

			int iResult = ERROR_ITEM_EMBLEM_COMPOUND_FAIL;
			int nResultItemID = 0;
			if( bValidHaveItem )
			{
				int iPlateItemID = pPlateItem->nItemID;
				
				iResult = ERROR_ITEM_EMBLEM_COMPOUND_FAIL;
				if( IsWindowState(WINDOW_BLIND) )
				{
					if( GetCheatCommand()->m_bLucky ) g_pDataManager->GetItemCompounder()->CheckPossibility( false );
					g_pDataManager->EmblemCompound( iPlateItemID, ItemSettingInfo, &Output );
					if( GetCheatCommand()->m_bLucky ) g_pDataManager->GetItemCompounder()->CheckPossibility( true );
				
					if( CDnItemCompounder::R_SUCCESS == Output.eResultCode ||
						CDnItemCompounder::R_POSSIBILITY_FAIL == Output.eResultCode )
					{
						// 비용 빼 준다
						if (!DelCoin( Output.iCost, DBDNWorldDef::CoinChangeCode::CompoundTax, m_nClickedNpcID))
							SendEmblemCompoundRes( iResult, nResultItemID );

						// 결과물이 여러개 나올 수 있도록 한다고 했었는데 기획이 또 바뀌었음.
						// 다시 결과물이 여러개 나올 수 있게 바뀔 수 있으므로 일단 지금은 첫번째 아이템 셋팅해줌.
						// 랜덤 나오는 경우엔 드랍 아이템 테이블 돌려줌
						if( !Output.vlItemCount.empty() && Output.vlItemCount.front() == 0 )
						{
							int nResultItemCount = 0;
							int nLastValue = 1;
							int nDepth = ITEMDROP_DEPTH;
#if defined( PRE_ADD_FARM_DOWNSCALE )
							CDNUserItem::CalcDropItems( this, Output.vlItemID.front(), nResultItemID, nResultItemCount, nLastValue, nDepth );
#else
							m_pItem->CalcDropItems( Output.vlItemID.front(), nResultItemID, nResultItemCount, nLastValue, nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
						}
						else
							nResultItemID = Output.vlItemID.empty() ? 0 : Output.vlItemID.front();

						if (!m_pItem->EmblemCompoundComplete( nResultItemID, pCompoundItem->cPlateItemSlotIndex, pCompoundItem->biPlateItemSerialID, 
															  CompoundInfo.aiItemID, CompoundInfo.aiItemCount, iItemSettingArrayCount ))
							SendEmblemCompoundRes( iResult, nResultItemID );
						
						if( CDnItemCompounder::R_POSSIBILITY_FAIL == Output.eResultCode ){
							iResult = ERROR_ITEM_EMBLEM_COMPOUND_POSSIBILITY_FAIL;
							BroadcastingEffect(EffectType_Emblem, EffectState_Fail);
						}
						else{
							iResult = ERROR_NONE;
							BroadcastingEffect(EffectType_Emblem, EffectState_Success);
						}
					}
					else
					{
						// 실패한 이유를 세분화.. 
						iResult = _MakeEmblemErrorCode( Output );
					}
				}
			}

			// 클라한테 결과 응답 보내준다.
			SendEmblemCompoundRes( iResult, nResultItemID );
			return ERROR_NONE;
		}
		break;

	case CS_ITEM_COMPOUND_REQ:
		{
			return ParseItemCompoundComplete( reinterpret_cast<CSCompoundItemReq*>(pData), nLen );
		}
		break;

	case CS_EMBLEM_COMPOUND_CANCEL_REQ:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (sizeof(CSEmblemCompoundCancelReq) != nLen)
				return ERROR_INVALIDPACKET;

			CSEmblemCompoundCancelReq * pPacket = (CSEmblemCompoundCancelReq*)pData;

			SendEmblemCompoundCancel( ERROR_NONE );
			BroadcastingEffect(EffectType_Emblem, EffectState_Cancel);
			return ERROR_NONE;
		}
		break;

	case CS_ITEM_COMPOUND_CANCEL_REQ:
		{
			int iResult = ParseItemCompoundCancel( reinterpret_cast<CSItemCompoundCancelReq*>(pData), nLen );
			return iResult;
		}
		break;

	case CS_MOVECOIN:
		{
			if (sizeof(CSMoveCoin) != nLen)
				return ERROR_INVALIDPACKET;

			CSMoveCoin *pMove = (CSMoveCoin*)pData;
			if ( !IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				SendMoveCoin(pMove->cMoveType, 0, 0, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			switch(pMove->cMoveType)
			{
			case MoveType_InvenToWare: m_pItem->OnRecvMoveCoinInventoWare(pMove->cMoveType, pMove->nMoveCoin); break;
			case MoveType_WareToInven: m_pItem->OnRecvMoveCoinWaretoInven(pMove->cMoveType, pMove->nMoveCoin); break;
			}
			return ERROR_NONE;
		}
		break;

	case CS_USEITEM:
		{
			if (sizeof(CSUseItem) != nLen)
				return ERROR_INVALIDPACKET;
			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false && AccountLevel_DWC != GetAccountLevel())
				return ERROR_NONE;

			CSUseItem *pUseItem = (CSUseItem*)pData;
			m_pItem->OnRecvUseItem(pUseItem);

			return ERROR_NONE;
		}
		break;

	case CS_COMPLETE_RANDOMITEM:
		{
			if( !m_pItem->IsValidRequestTimer( CDNUserItem::RequestType_UseRandomItem ) ) break;

			if (sizeof(CSCompleteRandomItem) != nLen)
				return ERROR_INVALIDPACKET;

			CSCompleteRandomItem *pCompleteItem = (CSCompleteRandomItem *)pData;
			if( m_pItem->CalcRandomItem( pCompleteItem ) == false )
				_DANGER_POINT();
			//UseItem에서 WINDOW_PROGRESS상태로 변경되어 있음.
			IsWindowStateNoneSet(WINDOW_PROGRESS);
			return ERROR_NONE;
		}
		break;

	case CS_CANCEL_RANDOMITEM:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;
			m_pItem->SetRequestTimer( CDNUserItem::RequestType_None, 0 );
			BroadcastingEffect(EffectType_Random, EffectState_Cancel);
			//UseItem에서 WINDOW_PROGRESS상태로 변경되어 있음.
			IsWindowStateNoneSet(WINDOW_PROGRESS);
			return ERROR_NONE;
		}
		break;

	case CS_ENCHANT:
		{
			return ParseEnchant( reinterpret_cast<CSEnchantItem*>(pData), nLen );
		}
		break;

	case CS_ENCHANTCOMPLETE:
		{
			return ParseEnchantComplete( reinterpret_cast<CSEnchantItem*>(pData), nLen );
		}

	case CS_ENCHANTCANCEL:
		{
			return ParseEnchantCancel( nLen );
		}
		break;

	case CS_SORTINVENTORY:
		{
			CSSortInventory *pSort = (CSSortInventory*)pData;

			if (sizeof(CSSortInventory) - sizeof(pSort->SlotInfo) + (sizeof(TSortSlot) * pSort->cTotalCount) != nLen)
				return ERROR_INVALIDPACKET;

			if( !IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				SendSortInventory(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (pSort->cTotalCount > INVENTORYMAX)
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}

			if (m_pItem->SortInventory(pSort))
				SendSortInventory(ERROR_NONE);
			else
				SendSortInventory(ERROR_GENERIC_INVALIDREQUEST);

			return ERROR_NONE;
		}
		break;

#if defined(PRE_PERIOD_INVENTORY)
	case CS_SORTPERIODINVENTORY:
		{
			CSSortInventory *pSort = (CSSortInventory*)pData;

			if (sizeof(CSSortInventory) - sizeof(pSort->SlotInfo) + (sizeof(TSortSlot) * pSort->cTotalCount) != nLen)
				return ERROR_INVALIDPACKET;

			if( !IsNoneWindowState() && !IsWindowState(WINDOW_BLIND))
			{
				SendSortPeriodInventory(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (pSort->cTotalCount > PERIODINVENTORYMAX)
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}

			if (m_pItem->SortPeriodInventory(pSort))
				SendSortPeriodInventory(ERROR_NONE);
			else
				SendSortPeriodInventory(ERROR_GENERIC_INVALIDREQUEST);

			return ERROR_NONE;
		}
		break;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	case CS_MODITEMEXPIREDATE:
		{
			if (sizeof(CSModItemExpireDate) != nLen)
			{
				SendItemModItemExpireDate(ERROR_INVALIDPACKET);
				return ERROR_INVALIDPACKET;
			}

			CSModItemExpireDate *pItem = (CSModItemExpireDate*)pData;

			const TItem *pExpendItem = m_pItem->GetCashInventory(pItem->biExpireDateItemSerial);
			bool bFlag = false;		//사용 성공인지 실패인지 체크
			if (pExpendItem)
			{
				TItemData *pExpendItemData = g_pDataManager->GetItemData(pExpendItem->nItemID);
				
				if (pExpendItemData)
				{
					int nMin = 0;
					//기간연장 아이템 타입에 따라 대상 아이템 가져오는 위치가 달라짐				
					switch(pExpendItemData->nType)
					{
						case ITEMTYPE_PET_EXPIRE:
							{
								const TVehicle *pPet = m_pItem->GetVehicleInventory(pItem->biItemSerial);
								if (pPet)
								{
									TItemData *pPetItemData = g_pDataManager->GetItemData( pPet->Vehicle[Vehicle::Slot::Body].nItemID );
									if ( pPetItemData && pPetItemData->nType == ITEMTYPE_PET )
									{	
										nMin = pExpendItemData->nTypeParam[0] * 24 * 60;
										bFlag = true;
									}
								}
							}
							break;
						default:
							break;
					}

					if (bFlag)
					{
#ifdef PRE_FIX_63822
						if (m_pSession->AddDataBaseMessageSequence(MAINCMD_ITEM, QUERY_MODITEMEXPIREDATE) == false)
						{
							bFlag = false;
							break;
						}
#endif		//#ifdef PRE_FIX_63822

						m_pItem->ModItemExpireDate(pItem, nMin);
						m_pItem->DeleteCashInventoryBySerial(pItem->biExpireDateItemSerial,1,false); //기간연장아이템 제거함 위ModItemExpireDate() 함수 안에서 아이템 사용하는 쿼리도 실행됨.
					}
				}
			}

			if(!bFlag)
			{
				//성공 패킷은 DB 서버에서 QUERY_MODITEMEXPIREDATE 수신 시점에 전송
				SendItemModItemExpireDate(ERROR_ITEM_FAIL);
				return ERROR_ITEM_FAIL;
			}
			return ERROR_NONE;
		}
		break;
	case CS_DELETE_PET_SKILL :
		{
			if (sizeof(CSPetSkillDelete) != nLen)
				return ERROR_INVALIDPACKET;
			CSPetSkillDelete *pItem = (CSPetSkillDelete*)pData;

			int nRet = m_pItem->DelPetSkill(pItem->cSlotNum);
			SendPetSkillDelete(nRet, pItem->cSlotNum);
		}
		break;

	case CS_SORTWAREHOUSE:
		{
			CSSortWarehouse * pPacket = (CSSortWarehouse*)pData;
			if (sizeof(CSSortWarehouse) - sizeof(pPacket->SlotInfo) + (sizeof(TSortSlot) * pPacket->cTotalCount) != nLen)
				return ERROR_INVALIDPACKET;

			if( !IsWindowState(WINDOW_BLIND) )
			{
				SendSortWarehouse(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (pPacket->cTotalCount > WAREHOUSEMAX)
			{
				SendSortWarehouse(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (m_pItem->SortWarehouse(pPacket))
				SendSortWarehouse(ERROR_NONE);
			else
				SendSortWarehouse(ERROR_GENERIC_INVALIDREQUEST);

			return ERROR_NONE;
		}
		break;

#if defined(PRE_PERIOD_INVENTORY)
	case CS_SORTPERIODWAREHOUSE:
		{
			CSSortWarehouse * pPacket = (CSSortWarehouse*)pData;
			if (sizeof(CSSortWarehouse) - sizeof(pPacket->SlotInfo) + (sizeof(TSortSlot) * pPacket->cTotalCount) != nLen)
				return ERROR_INVALIDPACKET;

			if( !IsWindowState(WINDOW_BLIND) )
			{
				SendSortPeriodWarehouse(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (pPacket->cTotalCount > PERIODWAREHOUSEMAX)
			{
				SendSortPeriodWarehouse(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			if (m_pItem->SortPeriodWarehouse(pPacket))
				SendSortPeriodWarehouse(ERROR_NONE);
			else
				SendSortPeriodWarehouse(ERROR_GENERIC_INVALIDREQUEST);

			return ERROR_NONE;
		}
		break;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

#ifdef PRE_ADD_AUTOUNPACK
	case CS_USE_AUTOUNPACK_CASHITEM_REQ:
		{
			CSAutoUnpack * pPacket = ( CSAutoUnpack * )pData;

			int retCode = ERROR_NONE;
			int typeParam = -1;
	
			// 상태 - 다른처리중.
			if( !IsNoneWindowState() || IsWindowState(WINDOW_BLIND) )	
			{
				retCode = ERROR_GENERIC_INVALIDREQUEST;
			}

			// 상태 - 처리가능.
			else
			{				
				const TItem * pItem = m_pItem->GetCashInventory( pPacket->serial );
				if( pItem )
				{
					TItemData* pItemData = g_pDataManager->GetItemData(  pItem->nItemID );
					if( pItemData )
					{
						typeParam = pItemData->nTypeParam[0];

						// 수량부족.
						int iExistCount = GetItem()->GetCashItemCountByType( ITEMTYPE_AUTOUNPACK );
						if( iExistCount < 1 || 
							( pItem->wCount < 1 ) )
							retCode = ERROR_ITEM_INSUFFICIENCY_ITEM;
					}
					else
						retCode = ERROR_ITEM_FAIL;
				}
				else
				{
					retCode = ERROR_ITEM_NOTFOUND; // 존재하지않는 아이템.
				}
			}
			
			// 클라이언트에 사용OK 전송.
			SendAutoUnpack( pPacket->serial, retCode );
						
			// 수량성. 소모된다.
			if( typeParam == 0 )
			{
				// 아이템소모.
				m_pItem->DeleteItemByUse( ITEMPOSITION_CASHINVEN, -1, pPacket->serial );
			}

			return retCode;

		}
		break;
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case CS_OPEN_TALISMANSLOT:
		{
			if (sizeof(CSOpenTalismanSlot) != nLen)
				return ERROR_INVALIDPACKET;

			CSOpenTalismanSlot *pPacket = (CSOpenTalismanSlot*)pData;

			int nRet = m_pItem->OnRecvOpenTalismanSlot(pPacket);
			if(nRet != ERROR_NONE)
				SendOpenTalismanSlot(nRet, GetItem()->GetTalismanSlotOpenFlag());

			return ERROR_NONE;
		}
		break;
#endif
		/*
	case CS_DISJOINT_REQ:
	{

#ifdef _ADD_NEWDISJOINT
			CSItemDisjointReqNew * pPacket = (CSItemDisjointReqNew*)pData;
			if (sizeof(CSItemDisjointReqNew) != nLen)
				return ERROR_INVALIDPACKET;

			if (m_pSession->GetRestraint()->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			int iResult = ERROR_NONE;
#ifdef _ADD_NEWDISJOINT
			SCItemDisjointResNew *pResultPacket = new SCItemDisjointResNew();
			ZeroMemory(pResultPacket, sizeof(SCItemDisjointResNew));
#endif
			int nLastIndex = 0;
			for (int k = 0; k < 4; k++)
			{
				if (pPacket->cSlotIndex[k] == 0 || pPacket->biItemSerial[k] == 0)
					continue;
				
				const TItem *pItem = m_pItem->GetInventory(pPacket->cSlotIndex[k]);//GetInventoryItem(m_pSession, pPacket->cSlotIndex[k]);
				if (!pItem)
				{
					_DANGER_POINT();
					return ERROR_NONE;
				}
				if (pItem->nSerial != pPacket->biItemSerial[k]) return ERROR_ITEM_FAIL;

				TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
				int nItemID = pItem->nItemID;
				bool bSuccessDisjoint = false;
				if (pItem && pItemData)
				{
					// 아이템 분해기 프랍의 정확한 위치가 필요할 듯. 
					// 일단 지금은 자신의 플레이어의 위치로 하면 될까나.
					DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(pSession->GetGameRoom(), pPacket->nNpcUniqueID);
					if (hNpc) {
						int nDisjointType = 0;
						if (hNpc->IsNpcActor()) {
							CDnNPCActor *pNpc = static_cast<CDnNPCActor *>(hNpc.GetPointer());
							nDisjointType = (pNpc->GetNpcData()) ? pNpc->GetNpcData()->nParam[0] : 0;
						}

						if (pItem->CanDisjoint() && pItem->CanDisjointType(nDisjointType) && pItem->CanDisjointLevel(pSession->GetLevel()))
						{
							// 귀속이면서 봉인상태의 아이템인지 검사
							bool bSealedItem = false;
							if (pItemData->cReversion == ITEMREVERSION_BELONG && pItem->IsSoulbBound() == false)
								bSealedItem = true;
							if (!bSealedItem)
							{
								for (int i = 0; i < pPacket->nCount[k]; ++i)
								{
									bSuccessDisjoint = false;
									iResult = ERROR_NONE;

									// 분해는 가능한 아이템인가. 돈이 충분한가.
									if (pSession->CheckEnoughCoin(pItem->GetDisjointCost()))
									{
										// 아이템 드랍되면 알아서 줏어가므로 성공 여부만 보내주면 됨.
										// 갖고 있던 아이템은 삭제.
										DNVector(CDnItem::DropItemStruct) VecDisjointItems;
										pItem->GetDisjointItems(VecDisjointItems);

										if (!VecDisjointItems.empty())
											bSuccessDisjoint = true;

										int nLocalItemID = pItem->GetClassID();
										bool bDeleteItemResult = pSession->GetItem()->DeleteInventoryBySlot(pPacket->cSlotIndex[k], 1, pPacket->biItemSerial[k], DBDNWorldDef::UseItem::DeCompose);
										_ASSERT(bDeleteItemResult);
										if (bDeleteItemResult)
										{
											EtVector3 vPos = *hNpc->GetPosition();

											// npc 바라보는 위치에서 2 미터 정도 앞으로 밀어서 생성시킴
											vPos += (hNpc->GetMatEx()->m_vZAxis * 200.0f);

											for (DWORD i = 0; i < VecDisjointItems.size(); ++i)
											{
												pResultPacket->nItemID[i + nLastIndex] = VecDisjointItems[i].nItemID;
												pResultPacket->nCount[i + nLastIndex] = VecDisjointItems[i].nCount;
												nLastIndex++;

												//int nRotate = (int)(((VecDisjointItems[i].nSeed % 360) / (float)VecDisjointItems.size()) * i);
												m_pSession->GetItem()->CreateInvenItem1(VecDisjointItems[i].nItemID, VecDisjointItems[i].nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::ItemDecompose, 0, CREATEINVEN_ETC);

												//RequestDropItem(STATIC_INSTANCE(CDnDropItem::s_dwUniqueCount)++, vPos, VecDisjointItems[i].nItemID,
												//	VecDisjointItems[i].nSeed, VecDisjointItems[i].nCount, nRotate, pSession->GetSessionID());
											}

											pSession->DelCoin(pItem->GetDisjointCost(), DBDNWorldDef::CoinChangeCode::DisjointTax, pSession->m_nClickedNpcID);
											pSession->GetEventSystem()->OnEvent(EventSystem::OnItemDisjoint, 1, EventSystem::ItemID, nLocalItemID);
										}
										else
											iResult = ERROR_ITEM_FAIL;		// 아이템 삭제가 안되는 이상한 에러.
									}
									else
									{
										iResult = ERROR_ITEM_INSUFFICIENCY_MONEY;
									}

									if (iResult != ERROR_NONE)
										break;
								}
							}
							else
								iResult = ERROR_ITEM_CAN_NOT_DISJOINT;
						}
						else
							iResult = ERROR_ITEM_CAN_NOT_DISJOINT;
					}
					else iResult = ERROR_ITEM_FAIL;
				}
				else
					iResult = ERROR_ITEM_NOTFOUND;		// 존재하지 않는 아이템을 분해하려고..
				pResultPacket->bSuccess = bSuccessDisjoint;
				// 아이템 분해 성공. 클라로 결과 보냄.
			}
			if (ERROR_NONE == iResult)
				SendItemDisjointResNew(pSession, pResultPacket); //wtf
			else
				SendItemDisjointResNew(pSession, pResultPacket);		// 0 은 실패

			return iResult;

	}
	break;
		*/
	}

	return CDNUserBase::OnRecvItemMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvItemGoodsMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
		// 일본 가챠폰 요청
#ifdef PRE_ADD_GACHA_JAPAN
	case CS_JP_GACHAPON_REQ:
		{
			if (sizeof(CSGachaRequest_JP) != nLen)
				return ERROR_INVALIDPACKET;

			int nResultItemID = 0;
			CSGachaRequest_JP* pGachaReq = (CSGachaRequest_JP*)pData;
			int nResult = m_pItem->RunGachapon_JP( pGachaReq->nJobClassID, pGachaReq->nSelectedPart, nResultItemID );

			SendGachaResult_JP( nResult, nResultItemID );

			return ERROR_NONE;
		}
		break;
#endif // PRE_ADD_GACHA_JAPAN
	case CS_COSMIXOPEN:
		{
			// CS_COMIXOPEN 안쓰고 UseItem에서 셋팅을 해줌..이거 안쓰는듯.ㅠㅠ
			if( !IsNoneWindowState() )
			{
				SendCosMixOpen(ERROR_ITEM_COSMIX_OPEN_FAIL);
				return ERROR_NONE;
			}

			const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
			if (mgr.IsEnableCostumeMix() == false)
			{
				SendCosMixOpen(ERROR_ITEM_COSMIX_OPEN_FAIL);
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;
			SetWindowState(WINDOW_COSMIX);
			SendCosMixOpen(ERROR_NONE);
			return ERROR_NONE;
		}
		break;

	case CS_COSMIXCLOSE:
	case CS_COSMIXCOMPLETECLOSE:
		{
			if ( IsWindowState(WINDOW_COSMIX) == false )
			{
				OnEndCostumeMix();
				SendCosMixClose(ERROR_ITEM_COSMIX_CLOSE_FAIL, nSubCmd);
				return ERROR_NONE;
			}

			OnEndCostumeMix();
			SendCosMixClose(ERROR_NONE, nSubCmd);

			return ERROR_NONE;
		}
		break;

	case CS_COSMIXCOMPLETE:
		{
			if (sizeof(CSCosMix) != nLen)
				return ERROR_INVALIDPACKET;

			const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
			if (mgr.IsEnableCostumeMix() == false)
			{
				SendCosMixComplete(ERROR_ITEM_COSMIX_FAIL, 0, 0);
				OnEndCostumeMix();
				return ERROR_NONE;
			}

			if (m_biCosMixSerialCache == 0/* || m_nCosMixInvenTypeCache == 0*/)
			{
				SendCosMixComplete(ERROR_ITEM_COSMIX_FAIL, 0, 0);
				OnEndCostumeMix();
				return ERROR_NONE;
			}

			CSCosMix* pCosMix = (CSCosMix*)pData;
			if( !IsWindowState(WINDOW_COSMIX) )
			{
				SendCosMixComplete(ERROR_ITEM_COSMIX_FAIL, 0, 0);

				OnEndCostumeMix();
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if (!m_pItem->DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, m_biCosMixSerialCache)){
				SendCosMixComplete(ERROR_ITEM_COSMIX_FAIL, 0, 0);
				OnEndCostumeMix();
				return ERROR_NONE;
			}

			int nItemId = 0;
			int nRet = m_pItem->MixCostume(nItemId, pCosMix->nStuffSerialID, pCosMix->nResultItemID, pCosMix->cOption);
			if (nRet != ERROR_NONE || nItemId == 0)
			{
				SendCosMixComplete(ERROR_ITEM_COSMIX_FAIL, 0, 0);
				OnEndCostumeMix();
				return ERROR_NONE;
			}

			// m_nCosMixInvenTypeCache = 0;
			m_biCosMixSerialCache = 0;

			SendCosMixComplete(nRet, nItemId, pCosMix->cOption);

			return ERROR_NONE;
		}
		break;

	case CS_DESIGNMIXCLOSE:
	case CS_DESIGNMIXCOMPLETECLOSE:
		{
			if( !IsWindowState(WINDOW_DESIGNMIX) )
			{
				OnEndCostumeDesignMix();
				SendCosDesignMixClose(ERROR_ITEM_DESIGNMIX_CLOSE_FAIL, nSubCmd);
				return ERROR_NONE;
			}

			OnEndCostumeDesignMix();
			SendCosDesignMixClose(ERROR_NONE, nSubCmd);

			return ERROR_NONE;
		}
		break;

	case CS_DESIGNMIXCOMPLETE:
		{
			if (sizeof(CSCosDesignMix) != nLen)
				return ERROR_INVALIDPACKET;

			const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
			if (mgr.IsEnableCostumeMix() == false)
			{
				SendCosDesignMixComplete(ERROR_ITEM_DESIGNMIX_FAIL);
				OnEndCostumeDesignMix();
				return ERROR_NONE;
			}

			if (m_biCosDesignMixSerialCache == 0)
			{
				SendCosDesignMixComplete(ERROR_ITEM_DESIGNMIX_FAIL);
				OnEndCostumeDesignMix();
				return ERROR_NONE;
			}

			CSCosDesignMix* pCosMix = (CSCosDesignMix*)pData;
			if ( !IsWindowState(WINDOW_DESIGNMIX) )
			{
				SendCosDesignMixComplete(ERROR_ITEM_DESIGNMIX_FAIL);
				OnEndCostumeDesignMix();
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if (m_pItem->MixCostumeDesign(pCosMix->nStuffSerialID) != ERROR_NONE)
			{
				SendCosDesignMixComplete(ERROR_ITEM_DESIGNMIX_FAIL);
				OnEndCostumeDesignMix();
				return ERROR_NONE;
			}
		}
		break;

#ifdef PRE_ADD_COSRANDMIX
	case CS_RANDOMMIXCLOSE:
	case CS_RANDOMMIXCOMPLETECLOSE:
		{
			if( !IsWindowState(WINDOW_RANDOMMIX) )
			{
				OnEndCostumeRandomMix();
				SendCosRandomMixClose(ERROR_ITEM_RANDOMMIX_CLOSE_FAIL, nSubCmd);
				return ERROR_NONE;
			}

			OnEndCostumeRandomMix();
			SendCosRandomMixClose(ERROR_NONE, nSubCmd);

			return ERROR_NONE;
		}
		break;

	case CS_RANDOMMIXCOMPLETE:
		{
			if (sizeof(CSCosRandomMix) != nLen)
				return ERROR_INVALIDPACKET;

			const CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
			if (mgr.IsEnableCosRandomMix() == false)
			{
				SendCosRandomMixComplete(ERROR_ITEM_RANDOMMIX_FAIL, 0);
				OnEndCostumeRandomMix();
				return ERROR_NONE;
			}

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
			eError cannotMix = ERROR_NONE;
			if (m_CosRandMixOpenType == CostumeMix::RandomMix::OpenByItem)
			{
				if (m_biCosRandomMixSerialCache == 0)
					cannotMix = ERROR_ITEM_RANDOMMIX_FAIL;
			}

			if (m_CosRandMixOpenType == CostumeMix::RandomMix::OpenFail)
				cannotMix = ERROR_ITEM_RANDOMMIX_FAIL;

			if (cannotMix != ERROR_NONE)
			{
				SendCosRandomMixComplete(ERROR_ITEM_RANDOMMIX_FAIL, 0);
				OnEndCostumeRandomMix();
				return ERROR_NONE;
			}
#else
			if (m_biCosRandomMixSerialCache == 0)
			{
				SendCosRandomMixComplete(ERROR_ITEM_RANDOMMIX_FAIL, 0);
				OnEndCostumeRandomMix();
				return ERROR_NONE;
			}
#endif

			CSCosRandomMix* pCosMix = (CSCosRandomMix*)pData;
			if ( !IsWindowState(WINDOW_RANDOMMIX) )
			{
				SendCosRandomMixComplete(ERROR_ITEM_RANDOMMIX_FAIL, 0);
				OnEndCostumeRandomMix();
				return ERROR_NONE;
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			int nResultItemId = 0;
			eError mixError = m_pItem->MixRandomCostume(nResultItemId, pCosMix->nStuffSerialID);
			if (mixError != ERROR_NONE)
			{
				SendCosRandomMixComplete(mixError, 0);
#ifdef PRE_MOD_COSRANDMIX_NPC_CLOSE
				if (m_CosRandMixOpenType != CostumeMix::RandomMix::OpenByNpc)
					OnEndCostumeRandomMix();
#else
				OnEndCostumeRandomMix();
#endif
				return ERROR_NONE;
			}
		}
		break;
#endif // PRE_ADD_COSRANDMIX

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
		case CS_USE_CHANGEJOB_CASHITEM_REQ:
			{
				if( m_eUserState != STATE_NONE || GetPartyID() > 0 || !IsNoneWindowState() )
					return ERROR_ITEM_CHANGEJOB_ILLEGAL_STATE;

				CSUseChangeJobCashItemReq* pPacket = reinterpret_cast<CSUseChangeJobCashItemReq*>(pData);

#ifdef PRE_ADD_JOBCHANGE_RESTRICT_JOB
				int nTemp1, nTemp2;
				if( !CheckAndCalcParallelChangeJob( pPacket->iJobToChange, nTemp1, nTemp2 ) )
				{
					SendChangeJobCashItemRes( ERROR_GENERIC_INVALIDREQUEST, 0, 0, 0 );
					return ERROR_GENERIC_INVALIDREQUEST;
				}
#endif // PRE_ADD_JOBCHANGE_RESTRICT_JOB

				const TItem* pItem = m_pItem->GetCashInventory(pPacket->biItemSerial);
				if( pItem )
				{
					TItemData* pItemData = g_pDataManager->GetItemData( pItem->nItemID );
					if( pItemData )
					{
						if( !m_pItem->DeleteItemByUse( ITEMPOSITION_CASHINVEN, -1, pPacket->biItemSerial) ) 
							return ERROR_GENERIC_INVALIDREQUEST;
						
						int iResult = m_pItem->UseChangeJobCashItem( pItemData, pPacket->iJobToChange );

						if( ERROR_NONE != iResult )
							return iResult;
					}
				}
				return ERROR_NONE;
			}
			break;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

	}

	return CDNUserBase::OnRecvItemGoodsMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvNpcMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
	case CS_NPCTALK:
		{
#if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
			if (g_ConfigWork.HasCommand(L"ExceptScript"))
			{
				SendNextTalkError();
				return ERROR_NONE;
			}
#endif // #if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)

			if (sizeof(CSNpcTalk) != nLen)
			{
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](0) ACC:%s(%d), CHR:%s(%I64d), SIZ:%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), sizeof(CSNpcTalk), nLen);
				return ERROR_INVALIDPACKET;
			}

			CSNpcTalk *pTalk = (CSNpcTalk*)pData;

			CDNNpcObject* pNpc = g_pFieldManager->GetNpcObjectByNpcObjID(GetChannelID(), pTalk->nNpcUniqueID);
			if( !pNpc )
			{
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](1) ACC:%s(%d), CHR:%s(%I64d), PAK:%d/%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode);
				SendNextTalkError();
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			std::wstring wszIndex;
			std::wstring wszTarget;

			bool bResult = g_pDataManager->GetTalkIndexName(pTalk->dwIndexHashCode, wszIndex);
			if( !bResult ) 
			{
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](2) ACC:%s(%d), CHR:%s(%I64d), PAK:%d/%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode);
				SendNextTalkError();
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			bResult =  g_pDataManager->GetTalkFileName(pTalk->dwTargetHashCode, wszTarget);
			if( !bResult ) 
			{
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](3) ACC:%s(%d), CHR:%s(%I64d), PAK:%d/%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode);
				SendNextTalkError();
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if(0 != m_dwNpcTalkLastIndexHashCode && ::wcscmp(wszIndex.c_str(), L"start") && pTalk->dwIndexHashCode == m_dwNpcTalkLastIndexHashCode && pTalk->dwTargetHashCode == m_dwNpcTalkLastTargetHashCode)
			{
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](4) ACC:%s(%d), CHR:%s(%I64d), PAK:%d/%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode);
				SendNextTalkError();
				return ERROR_GENERIC_INVALIDREQUEST;
			}
			
			TALK_PARAGRAPH& para = GetLastTalkParagraph();

			// NPC와의 대화가 시작인 경우 기억하고 있던 이전 대화를 초기화한다.
			if( ::wcscmp(wszIndex.c_str(), NpcTalkStringGroup[NPCTALK_START].c_str()) == 0 )
#if defined(PRE_FIX_QUEST_GUILD_BOARD)
			{
				para = g_pDataManager->GetEmptyTalkParagraph();
				GetQuest()->ResetRewardFlag();
			}
#else
				para = g_pDataManager->GetEmptyTalkParagraph();
#endif
			bool bValidAnswer = false;
			for (int i = NPCTALK_START; i<NPCTALK_END; i++)
			{
				if( ::wcscmp(wszIndex.c_str(), NpcTalkStringGroup[i].c_str()) == 0)
					bValidAnswer = true;
			}

			if(bValidAnswer == false)
			{
				if( para.Answers.size() )
				{
					for( std::vector<TALK_ANSWER>::iterator i = para.Answers.begin(); i != para.Answers.end(); i++ )
					{
						if((*i).szLinkIndex == wszIndex )
						{
							bValidAnswer = true;
							break;
						}
					}
				}
#if !defined(PRE_FIX_QUEST_GUILD_BOARD)	//아래 구문 때문에 선택지가 없는 막다른 지문인 경우 검증없이 다른 문맥으로 통과가 된다.
				else
					bValidAnswer = true;
#endif
			}

			if( bValidAnswer == false )
			{
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](5) PAK:%d/%d/%d INDEX:%s\r\n", pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode, wszIndex.c_str());
				SendNextTalkError();
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			m_dwNpcTalkLastIndexHashCode = pTalk->dwIndexHashCode;
			m_dwNpcTalkLastTargetHashCode = pTalk->dwTargetHashCode;

			m_nClickedNpcID = pNpc->GetNpcData()->nNpcID;	// 클릭할때마다 갱신
			m_nClickedNpcObjectID = pTalk->nNpcUniqueID;

			if(!IsValidPacketByNpcClick())
			{	// npc 거리체크
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](6) ACC:%s(%d), CHR:%s(%I64d), PAK:%d/%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode);
				SendNextTalkError();
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			bool bReturn = pNpc->Talk(this, pTalk->nNpcUniqueID, wszIndex, wszTarget);
			if( !bReturn ) 
			{
				SendNextTalkError();
				g_Log.Log(LogType::_ERROR, this, L"[CS_NPCTALK](6) ACC:%s(%d), CHR:%s(%I64d), PAK:%d/%d/%d\n", GetAccountName(), GetAccountDBID(), GetCharacterName(), GetCharacterDBID(), pTalk->nNpcUniqueID, pTalk->dwIndexHashCode, pTalk->dwTargetHashCode);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if(!IsNpcTalk()) 
				SendTalkEnd(pTalk->nNpcUniqueID);	// 본 패킷을 클라이언트에서 수신 기존 NPC 대사창이 닫김
			
			if( ::wcscmp(wszIndex.c_str(), NpcTalkStringGroup[NPCTALK_START].c_str()) == 0 )
				GetEventSystem()->OnEvent( EventSystem::OnNpcTalk, 1, EventSystem::NpcID, m_nClickedNpcID );

			return ERROR_NONE;
		}
		break;
#if defined(PRE_FIX_QUEST_GUILD_BOARD)
	case CS_NPCTALKEND:
		{
			GetQuest()->ResetRewardFlag();
			return ERROR_NONE;
		}
		break;
#endif

	}

	SendNextTalkError();
	_DANGER_POINT();
	//처리하지 않는 헤더를 보내오면 끊깁니다.
	return ERROR_UNKNOWN_HEADER;
}

int CDNUserSession::OnRecvQuestMessage(int nSubCmd, char *pData, int nLen)
{
	return CDNUserBase::OnRecvQuestMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvChatMessage(int nSubCmd, char *pData, int nLen)
{
	//모든 대화메세지는 나 또는 상대방의 제재상태를 체크해야한다.
	if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_CHAT) == false)
		return ERROR_NONE;

	switch(nSubCmd)
	{
	case CS_CHATMSG:
		{
			if( !m_pField )
				return ERROR_NONE;

			CSChat *pChat = (CSChat*)pData;

			if (sizeof(CSChat) - sizeof(pChat->wszChatMsg) +(pChat->nLen * sizeof(WCHAR)) != nLen)
				return ERROR_INVALIDPACKET;

			if (pChat->nLen > CHATLENMAX-1)	return ERROR_NONE;

			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->nLen);

			if (GetAccountLevel() >= AccountLevel_New && GetAccountLevel() <= AccountLevel_Developer){
				wstring WChatMsg = wszChatMsg;
				if (WChatMsg.find(L"/") == 0){
					if (GetAccountLevel() == AccountLevel_QA){	// QA는 치트 되게 한다
						if (m_pCheatCommand){
							if (m_pCheatCommand->Command(WChatMsg) > 0) return ERROR_NONE;
						}
					}
					if (m_pGMCommand){ 
						if (m_pGMCommand->GMCommand(WChatMsg)) return ERROR_NONE;
					}

#if defined( _WORK )
					// 개발버젼에서는 코드 돌게 변경...
#else
					return ERROR_NONE;
#endif // #if defined( _WORK )
				}
			}

#if defined(_WORK)
			if (m_pCheatCommand){
				if (m_pCheatCommand->Command(wstring(wszChatMsg)) > 0) return ERROR_NONE;
			}
#endif
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
			if( GetSpammer() ) // 스패머는 머든 패쓰..
				return ERROR_NONE;
#endif
#if defined(PRE_ADD_MUTE_USERCHATTING)
			if( IsMuteChatting() ) //체팅 금지면 패스
				return ERROR_NONE;
#endif	//	#if defined(PRE_ADD_MUTE_USERCHATTING)
			switch(pChat->eType)
			{
			case CHATTYPE_NORMAL:
				{
					// 클라이언트단에서 기본적으로 하니 서버에서는 응답같은거 안보내고 끝내는걸로 하겠다.
					if( CheckPapering(eChatPapering) )
						return ERROR_NONE;

					// EncryptFlag 설정
					ScopeEncryptFlag Scope( &m_ParamData.bEncrypt );

					if( m_pField->bIsPvPLobby() )
					{
						memset( &m_ParamData, 0, sizeof(TParamData) );

						m_ParamData.nChatSize = pChat->nLen;
						if ((GetAccountLevel() >= AccountLevel_New) && (GetAccountLevel() <= AccountLevel_QA)) m_ParamData.eChatType = CHATTYPE_GM;
						else m_ParamData.eChatType = pChat->eType;
						_wcscpy(m_ParamData.wszChatMsg, _countof(m_ParamData.wszChatMsg), wszChatMsg, (int)wcslen(wszChatMsg));
						m_ParamData.wszChatMsg[m_ParamData.nChatSize] = 0;

						m_pField->SendFieldMessage( this, FM_CHAT, NULL, &m_ParamData );
					}
					else
					{
						memset(&m_ParamData, 0, sizeof(TParamData));

						m_ParamData.nChatSize = pChat->nLen;
						if ((GetAccountLevel() >= AccountLevel_New) && (GetAccountLevel() <= AccountLevel_QA)) m_ParamData.eChatType = CHATTYPE_GM;
						else m_ParamData.eChatType = pChat->eType;
						_wcscpy(m_ParamData.wszChatMsg, _countof(m_ParamData.wszChatMsg), wszChatMsg, (int)wcslen(wszChatMsg));
						m_ParamData.wszChatMsg[m_ParamData.nChatSize] = 0;

						SendUserLocalMessage(0, FM_CHAT);
					}
					break;
				}

			case CHATTYPE_PARTY:
			case CHATTYPE_RAIDNOTICE:
				{
					if (GetPartyID() > 0)
					{	// 파티일 경우만 보내준다
						if ((GetAccountLevel() >= AccountLevel_New) && (GetAccountLevel() <= AccountLevel_QA)) 
							pChat->eType = CHATTYPE_PARTY_GM;

						CDNParty * pParty = g_pPartyManager->GetParty(GetPartyID());
						if (pParty)
							pParty->SendPartyChat(pChat->eType, pChat->nLen, GetCharacterName(), wszChatMsg );
					}
					break;
				}

			case CHATTYPE_GUILD:
				{
					const TGuildUID GuildUID(GetGuildUID());
					if (!GuildUID.IsSet()) {
						break;
					}

					CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

					if (!pGuild)
						break;

#if !defined( PRE_ADD_NODELETEGUILD )
					CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
					if (FALSE == pGuild->IsEnable()) break;
#endif

					g_pMasterConnection->SendGuildChat(GuildUID, GetAccountDBID(), GetCharacterDBID(), pChat->wszChatMsg, pChat->nLen);
				}
				break;

			case CHATTYPE_CHANNEL:
			case CHATTYPE_WORLD:
#if defined(PRE_ADD_WORLD_MSG_RED)
			case CHATTYPE_WORLD_POPMSG:
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
				{
					//아마 아이템으로 채팅을 가능하게 한다고 함. 실재 적용시점에서는 아이템의 소유 또는 사용 여부 확인후에 플로우 진행
					//머 없다 아이템 또는 쏠 수 있는지 등의 확인만 하고 바로 마스터로 날린다 마스터는 브로드 캐스팅~
					if (g_pMasterConnection && g_pMasterConnection->GetActive()) {
						int nItemID = 0;
#if defined(PRE_ADD_WORLD_MSG_RED)
						if( !m_pItem->IsVaildWorldChatItem(pChat->eType, pChat->biItemSerial, nItemID) )
						{
							g_Log.Log(LogType::_ERROR, this, L"[OnRecvItem] WorldChat Item Find Failed(ChatType : %d, Serial: %I64d\r\n", pChat->eType, pChat->biItemSerial);
							break;
						}
#else
						const TItem *pItem = m_pItem->GetCashInventory(pChat->biItemSerial);
						if (pItem) nItemID = pItem->nItemID;
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
						if (m_pItem->DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pChat->biItemSerial)){
							g_pMasterConnection->SendChat(pChat->eType, m_nAccountDBID, wszChatMsg, pChat->nLen, GetField()->GetMapIndex());
							GetEventSystem()->OnEvent( EventSystem::OnItemUse, 1, EventSystem::ItemID, nItemID );
						}
					}
				}
				break;

#if defined( PRE_PRIVATECHAT_CHANNEL )
			case CHATTYPE_PRIVATECHANNEL:
				{					
					if (g_pMasterConnection && g_pMasterConnection->GetActive()) 
					{
						if(GetPrivateChannelID())
							g_pMasterConnection->SendPrivateChannelChat(pChat->eType, m_nAccountDBID, wszChatMsg, pChat->nLen, GetPrivateChannelID());
					}
				}
				break;
#endif
#if defined(PRE_ADD_DWC)
			case CHATTYPE_DWC_TEAM:
				{
					if(!IsDWCCharacter() || GetDWCTeamID() <= 0 )
						break;
					if(g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendDWCTeamChat(GetDWCTeamID(), GetAccountDBID(), GetCharacterDBID(), pChat->wszChatMsg, pChat->nLen);
				}
				break;
#endif

			default:
				_DANGER_POINT();
			}
			return ERROR_NONE;
		}
		break;

	case CS_CHAT_PRIVATE:
		{
			CSChatPrivate *pChat = (CSChatPrivate*)pData;

			if (sizeof(CSChatPrivate) - sizeof(pChat->wszChatMsg) +(pChat->nLen * sizeof(WCHAR)) != nLen)
				return ERROR_INVALIDPACKET;

			if (pChat->nLen > CHATLENMAX-1)	return ERROR_NONE;
			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChat->wszChatMsg, pChat->nLen);

			if (m_pIsolate->IsIsolateItem(pChat->wszToCharacterName))
			{
				SendIsolateResult(ERROR_ISOLATE_REQUESTFAIL);
				return ERROR_NONE;
			}

			if (CheckLastSpace(pChat->wszToCharacterName))
			{
				SendIsolateResult(ERROR_ISOLATE_REQUESTFAIL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (__wcsicmp_l(pChat->wszToCharacterName, m_BaseData.wszName) == 0){		// 이름 비교해서 본인한테 보낸거면 다시 되돌리자
				SendChat(CHATTYPE_PRIVATE, pChat->nLen, m_BaseData.wszName, wszChatMsg, NULL, ERROR_CHAT_DONTSAYYOURSELF);		// -2는 자기자신일때 에러값이다
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			int nChatType = CHATTYPE_PRIVATE;
			if ((GetAccountLevel() >= AccountLevel_New) && (GetAccountLevel() <= AccountLevel_QA)) nChatType = CHATTYPE_PRIVATE_GM;
			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pChat->wszToCharacterName);
			if (pUserObj)
			{
#if defined(PRE_ADD_DWC)
				if( IsDWCCharacter() ^ pUserObj->IsDWCCharacter() )	//같은 타입끼리만 대화 가능
				{	//없는 사람 처리한다
					SendChat(nChatType, pChat->nLen, m_BaseData.wszName, wszChatMsg, NULL, ERROR_CHAT_USERNOTFOUND);
					return ERROR_NONE;
				}
#endif
				// 이쪽 서버에 있다면 상대방이 나를 차단했는지 확인한다
				if (pUserObj->GetIsolate()->IsIsolateItem(GetCharacterName()))
					return ERROR_NONE;

				// GM이면 없는 사람처럼 해야함
				if ((pUserObj->GetAccountLevel() >= AccountLevel_New) && (pUserObj->GetAccountLevel() <= AccountLevel_QA)){
					if (!pUserObj->GetGMCommand()->IsWhisperName(m_BaseData.wszName)){
						SendChat(nChatType, pChat->nLen, m_BaseData.wszName, wszChatMsg, NULL, ERROR_CHAT_USERNOTFOUND);
						return ERROR_NONE;
					}
				}

				// 클라이언트단에서 기본적으로 하니 서버에서는 응답같은거 안보내고 끝내는걸로 하겠다.CS_INVITEGUILDMEMBREQ
				if ( CheckPapering(eChatPapering) )
					return ERROR_NONE;

				pUserObj->SendChat(nChatType, pChat->nLen, m_BaseData.wszName, wszChatMsg);
			}
			else {	// 없다면
#if defined(PRE_ADD_DWC)
				if(IsDWCCharacter())
					nChatType = CHATTYPE_DWC_PRIVATE;	// 마스터로 쏠때는 DWC 타입이라고 알려서 보내준다.
#endif
#ifdef PRE_ADD_DOORS
				INT64 biDoorsDestChracterDBID = 0;
				if (GetFriend())
				{
					TFriend * pFriend = GetFriend()->GetFriend(pChat->wszToCharacterName);
					if (pFriend)
					//if (pFriend && pFriend->bMobileAuthentication)
						biDoorsDestChracterDBID = pFriend->biFriendCharacterDBID;
				}
				g_pMasterConnection->SendPrivateChat(m_nAccountDBID, pChat->wszToCharacterName, nChatType, wszChatMsg, pChat->nLen, biDoorsDestChracterDBID);
#else		//#ifdef PRE_ADD_DOORS
				g_pMasterConnection->SendPrivateChat(m_nAccountDBID, pChat->wszToCharacterName, nChatType, wszChatMsg, pChat->nLen);
#endif		//#ifdef PRE_ADD_DOORS
			}
			return ERROR_NONE;
		}
		break;

	case CS_DICE:
		{
		}
		break;

	case CS_CHAT_CHATROOM:
		{
			CSChatChatRoom * pChatChatRoom = (CSChatChatRoom*)pData;

			if (sizeof(CSChatChatRoom) - sizeof(pChatChatRoom->wszChatMsg) +(pChatChatRoom->nLen * sizeof(WCHAR)) != nLen)
				return ERROR_INVALIDPACKET;

			if (pChatChatRoom->nLen > CHATLENMAX-1)	return ERROR_NONE;
			WCHAR wszChatMsg[CHATLENMAX] = { 0, };
			_wcscpy(wszChatMsg, CHATLENMAX, pChatChatRoom->wszChatMsg, pChatChatRoom->nLen);

			if( m_nChatRoomID <= 0 )	// 채팅방에 참여하지 않은 유저이다.
			{
				return ERROR_CHATROOM_UNKNOWNUSER;
			}

			CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( m_nChatRoomID );
			if( pChatRoom )
			{
				pChatRoom->SendChatMessageToAll( pChatChatRoom->nLen, GetCharacterName(), wszChatMsg );
			}

			return ERROR_NONE;

		}
		break;
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	case CS_CHAT_SPAMMER :
		{
			bool* bSpammer = (bool*)pData;
			// 스패머로 신고 및 해제 들어옴..
			SetSpammer(*bSpammer);
			if( *bSpammer )
			{
				int iInvalidCount = 1;
				g_Log.Log( LogType::_HACK, this, L"HackChecker(Spammer) : CharName=%s InvalidCount=%d\n", GetCharacterName(), iInvalidCount );
				GetDBConnection()->QueryAddAbuseMonitor( this, 0, iInvalidCount );
			}
		}
		break;
#endif
	}

	return ERROR_UNKNOWN_HEADER;		//상위에서 리턴되어야 합니다 여까지 오면 처리하지 않는 패킷의 수신!
}

int CDNUserSession::OnRecvTradeMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
#if !defined (PRE_MOD_GAMESERVERSHOP)
		// Shop
#if defined(PRE_ADD_REMOTE_OPENSHOP)
	case eTrade::CS_SHOP_REMOTEOPEN:
		{
			if (sizeof(CSShopBuy) != nLen)
				return ERROR_INVALIDPACKET;

			CSShopRemoteOpen *pShop = (CSShopRemoteOpen*)pData;

			m_pItem->OnRecvShopRemoteOpen(pShop->Type);
			SendShopOpen(m_nShopID, pShop->Type);
			return ERROR_NONE;
		}
		break;

#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)

	case CS_SHOP_BUY:	// 구매
		{
#if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!m_bRemoteShopOpen && !IsValidPacketByNpcClick())	// npc 거리체크
#else	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!IsValidPacketByNpcClick())	// npc 거리체크
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
				return ERROR_GENERIC_INVALIDREQUEST;

#if defined(_CH)
			if (m_cFCMState != FCMSTATE_NONE){
				SendShopBuyResult(ERROR_FCMSTATE);
				return ERROR_NONE;
			}
#endif	// _CH

			if( !IsWindowState(WINDOW_BLIND) ){
				SendShopBuyResult(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

#if defined( PRE_ADD_DWC )
			if (AccountLevel_DWC != m_UserData.Status.cAccountLevel && m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false) // DWC캐릭터의 구입 제한을 허용한다
#else // #if defined( PRE_ADD_DWC )
			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
#endif // #if defined( PRE_ADD_DWC )			
				return ERROR_NONE;

			if (sizeof(CSShopBuy) != nLen)
				return ERROR_INVALIDPACKET;

			CSShopBuy *pBuy = (CSShopBuy*)pData;
			int nRet = m_pItem->OnRecvBuyNpcShopItem(m_nShopID, pBuy->cTapIndex, pBuy->cShopIndex, pBuy->wCount);
			SendShopBuyResult(nRet);
			return ERROR_NONE;
		}
		break;

	case CS_SHOP_SELL:	// 판매
		{
#if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!m_bRemoteShopOpen && !IsValidPacketByNpcClick())	// npc 거리체크
#else	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!IsValidPacketByNpcClick())	// npc 거리체크
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
				return ERROR_GENERIC_INVALIDREQUEST;

#if defined(_CH)
			if (m_cFCMState != FCMSTATE_NONE){
				SendShopSellResult(ERROR_FCMSTATE);
				return ERROR_NONE;
			}
#endif	// _CH
			if( !IsWindowState(WINDOW_BLIND) ){ // 블라인드 상태가 아니면..
				SendShopSellResult(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if (sizeof(CSShopSell) != nLen)
				return ERROR_INVALIDPACKET;
			
			// 상점팔기 0.5초 딜레이 갑니다.~~
			DWORD dwCurTick = timeGetTime();
			if( m_dwResellItemTick && dwCurTick-m_dwResellItemTick < 500 )
			{
				//까불지 말고 쉬었다해라..
				SendShopSellResult(ERROR_GENERIC_TIME_WAIT);
				return ERROR_NONE;
			}			
#if defined(PRE_FIX_74387)
			if( m_bNowResellItem )
			{
				//이넘은 확실히 봇인데..
				int iInvalidCount = 1;
				g_Log.Log( LogType::_HACK, this, L"HackChecker(ResellItem) : CharName=%s InvalidCount=%d\n", GetCharacterName(), iInvalidCount );
				GetDBConnection()->QueryAddAbuseMonitor( this, 0, iInvalidCount );
				SendShopSellResult(ERROR_GENERIC_TIME_WAIT);
				return ERROR_NONE;
			}
#endif
			CSShopSell *pSell = (CSShopSell*)pData;
			int nRet = m_pItem->OnRecvSellNpcShopItem(pSell);
#if defined(PRE_FIX_74387)
			if( nRet != ERROR_NONE ) // DB에 갔다와서 응답 보내줌.			
				SendShopSellResult(nRet);
			else
				m_bNowResellItem = true;
#else
			SendShopSellResult(nRet);
#endif //#if defined(PRE_FIX_74387)
			m_dwResellItemTick = dwCurTick;
			return ERROR_NONE;
		}
		break;

		case CS_SHOP_REPURCHASE:
		{
#if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!m_bRemoteShopOpen && !IsValidPacketByNpcClick())	// npc 거리체크
#else	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!IsValidPacketByNpcClick())	// npc 거리체크
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
				return ERROR_GENERIC_INVALIDREQUEST;

#if defined(_CH)
			if (m_cFCMState != FCMSTATE_NONE){
				SendShopRepurchase(ERROR_FCMSTATE);
				return ERROR_NONE;
			}
#endif	// _CH
			if ( !IsWindowState(WINDOW_BLIND)){
				SendShopRepurchase(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if ( sizeof(CSShopRepurchase) != nLen)
				return ERROR_INVALIDPACKET;

			CSShopRepurchase* pPacket = reinterpret_cast<CSShopRepurchase*>(pData);
			int iRet = m_pItem->OnRecvShopRepurchase( pPacket );
			SendShopRepurchase( iRet, pPacket->iRepurchaseID );
			return ERROR_NONE;
		}

		case CS_SHOP_GETLIST_REPURCHASE:
		{
#if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!m_bRemoteShopOpen && !IsValidPacketByNpcClick())	// npc 거리체크
#else	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
			if (!IsValidPacketByNpcClick())	// npc 거리체크
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
				return ERROR_GENERIC_INVALIDREQUEST;

#if defined(_CH)
			if (m_cFCMState != FCMSTATE_NONE){
				SendShopRepurchaseList(ERROR_FCMSTATE);
				return ERROR_NONE;
			}
#endif	// _CH
			if ( !IsWindowState(WINDOW_BLIND)){
				SendShopRepurchaseList(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if (0 != nLen)
				return ERROR_INVALIDPACKET;

			GetDBConnection()->QueryGetListRepurchaseItem( this );
			return ERROR_NONE;
		}

	case CS_REPAIR_EQUIP:	// 장착중 수리
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

#if defined(_CH)
			if (m_cFCMState != FCMSTATE_NONE){
				SendRepairEquip(ERROR_FCMSTATE);
				return ERROR_NONE;
			}
#endif	// _CH

			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			m_pItem->OnRecvRepairEquip();
			return ERROR_NONE;
		}
		break;

	case CS_REPAIR_ALL:		// 전체수리
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

#if defined(_CH)
			if (m_cFCMState != FCMSTATE_NONE){
				SendRepairAll(ERROR_FCMSTATE);
				return ERROR_NONE;
			}
#endif	// _CH

			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			m_pItem->OnRecvRepairAll();
			return ERROR_NONE;
		}
		break;
#endif // #if !defined (PRE_MOD_GAMESERVERSHOP)

		// Mail
	case CS_MAILBOX:
		{
			if( !IsWindowState(WINDOW_BLIND) ){
				SendMailBox(NULL, 0, 0, 0, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (sizeof(CSMailBox) != nLen)
				return ERROR_INVALIDPACKET;

			CSMailBox *pMailBox = (CSMailBox*)pData;
			m_pDBCon->QueryGetPageReceiveMail(this, pMailBox->cPageNum);
			return ERROR_NONE;
		}
		break;

	case CS_MAIL_SEND:
		{
			if( !IsWindowState(WINDOW_BLIND) ){
				SendMailSendResult(ERROR_GENERIC_INVALIDREQUEST, 0);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if (sizeof(CSSendMail) < nLen)	// 가변
				return ERROR_INVALIDPACKET;

			if ( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::LimitLevel_SendMail )) )
			{
				SendMailSendResult( ERROR_GENERIC_LEVELLIMIT, 0 );
				return ERROR_NONE;
			}
			CSSendMail *pSendMail = (CSSendMail*)pData;

			if (CheckLastSpace(pSendMail->wszToCharacterName))
			{
				SendMailSendResult(ERROR_GENERIC_INVALIDREQUEST, 0);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (__wcsicmp_l(pSendMail->wszToCharacterName, m_BaseData.wszName) == 0){		// 이름 비교해서 본인한테 보낸거면 에러
				SendMailSendResult(ERROR_MAIL_DONTSENDYOURSELF, 0);
				return ERROR_GENERIC_UNKNOWNERROR;
			}

			if (m_pIsolate->IsIsolateItem(pSendMail->wszToCharacterName))
			{
				SendMailSendResult(ERROR_ISOLATE_REQUESTFAIL, 0);
				return ERROR_GENERIC_UNKNOWNERROR;
			}
#if defined(PRE_ADD_36935)
			if ((GetAccountLevel() >= AccountLevel_New) && (GetAccountLevel() <= AccountLevel_QA))
			{
				if (pSendMail->nAttachCoin > 0 || pSendMail->cAttachItemTotalCount > 0)
				{
					SendMailSendResult(ERROR_MAIL_CANT_RECVMAIL, 0);
					return ERROR_GENERIC_UNKNOWNERROR;
				}
			}
#endif // #if defined(PRE_ADD_36935)
			
			int nRet = m_pItem->CheckSendMailItem(pSendMail);
			if (nRet != ERROR_NONE){
				SendMailSendResult(nRet, 0);
				return ERROR_GENERIC_UNKNOWNERROR;
			}
			return ERROR_NONE;
		}
		break;

	case CS_MAIL_READ:
		{
			if( !IsWindowState(WINDOW_BLIND) ){ // 블라인드 모드가 아닌데 메일을 열수 없음.
				SendReadMailResult(NULL, 0);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (sizeof(CSReadMail) != nLen) return ERROR_INVALIDPACKET;

			CSReadMail *pMail = (CSReadMail*)pData;

			if (pMail->nMailDBID <= 0){	// 아무것도 없다
				SendReadMailResult(NULL, 0);
				return ERROR_NONE;
			}

			m_pDBCon->QueryReadMail(this, pMail->nMailDBID);
			return ERROR_NONE;
		}
		break;

#if defined(PRE_ADD_CADGE_CASH)
	case CS_MAIL_READCADGE:
		{
			if( !IsWindowState(WINDOW_BLIND) ){ // 블라인드 모드가 아닌데 메일을 열수 없음.
				SendReadCadgeMailResult(NULL);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (sizeof(CSReadMail) != nLen) return ERROR_INVALIDPACKET;

			CSReadMail *pMail = (CSReadMail*)pData;

			if (pMail->nMailDBID <= 0){	// 아무것도 없다
				SendReadCadgeMailResult(NULL);
				return ERROR_NONE;
			}

			m_pDBCon->QueryReadWishMail(this, pMail->nMailDBID);
			return ERROR_NONE;
		}
		break;
#endif	// #if defined(PRE_ADD_CADGE_CASH)

	case CS_MAIL_DELETE:
		{
			if( !IsWindowState(WINDOW_BLIND) ) { // 블라인드 모드가 아닌데 메일을 열수 없음.
				SendDeleteMailResult(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (sizeof(CSMailDBIDArray) != nLen)
				return ERROR_INVALIDPACKET;

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			CSMailDBIDArray *pMail = (CSMailDBIDArray*)pData;

			std::vector <int> VecMailDBID;
			int nCount = 0;
			for (int i = 0; i < MAILPAGEMAX; i++)
			{
				if (pMail->nMailDBID[i] > 0)
				{
					if (find(VecMailDBID.begin(), VecMailDBID.end(), pMail->nMailDBID[i]) == VecMailDBID.end())
						VecMailDBID.push_back(pMail->nMailDBID[i]);
				}
			}

			if (VecMailDBID.size() == 0){	// 아무것도 없다
				SendDeleteMailResult(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			// 중복키가 발생하지 않도록 처리한다.
			memset(pMail->nMailDBID, 0x00, sizeof(pMail->nMailDBID));
			for (int i = 0; i <(int)VecMailDBID.size(); i++)
			{
				if (i < MAILPAGEMAX)
					pMail->nMailDBID[i] = VecMailDBID[i];
			}

			m_pDBCon->QueryDelMail(this, pMail->nMailDBID);
			return ERROR_NONE;
		}
		break;

	case CS_MAIL_ATTACHALL:
		{
			if (sizeof(CSMailDBIDArray) != nLen)
				return ERROR_INVALIDPACKET;

			CSMailDBIDArray *pMail = (CSMailDBIDArray*)pData;

			if( !IsWindowState(WINDOW_BLIND) ){
				SendAttachAllMailResult(pMail->nMailDBID, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			std::vector <int> VecMailDBID;
			int nCount = 0;
			for (int i = 0; i < MAILPAGEMAX; i++)
			{
				if (pMail->nMailDBID[i] > 0)
				{
					if (find(VecMailDBID.begin(), VecMailDBID.end(), pMail->nMailDBID[i]) == VecMailDBID.end())
						VecMailDBID.push_back(pMail->nMailDBID[i]);
				}
			}

			if (VecMailDBID.size() == 0){	// 아무것도 없다
				SendAttachAllMailResult(pMail->nMailDBID, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			// 중복키가 발생하지 않도록 처리한다.
			memset(pMail->nMailDBID, 0x00, sizeof(pMail->nMailDBID));
			for (int i = 0; i <(int)VecMailDBID.size(); i++)
			{
				if (i < MAILPAGEMAX)
					pMail->nMailDBID[i] = VecMailDBID[i];
			}

			m_pDBCon->QueryGetListMailAttachment(this, pMail->nMailDBID);	// 일단 몇개인지 검사들어간다(?)
			return ERROR_NONE;
		}
		break;

	case CS_MAIL_ATTACH:
		{
			if( !IsWindowState(WINDOW_BLIND)) {
				SendAttachMailResult(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (sizeof(CSAttachMail) != nLen) return ERROR_INVALIDPACKET;

			CSAttachMail *pMail = (CSAttachMail*)pData;

			if (pMail->nMailDBID <= 0){	// 아무것도 없다
				SendAttachMailResult(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;
			}

			int nRet = m_pItem->VerifyAttachItem(pMail);
			if (nRet != ERROR_NONE){
				SendAttachMailResult(nRet);
				return ERROR_NONE;
			}
			return ERROR_NONE;
		}
		break;

		// 무인상점
	case CS_MARKETLIST:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}
			if( !IsWindowState(WINDOW_BLIND) ){
				SendMarketList(NULL, 0, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다			
			}

			if (sizeof(CSMarketList) != nLen)
				return ERROR_INVALIDPACKET;

			CSMarketList *pMarketList = (CSMarketList*)pData;

			// cLevel 이 char 형이기 때문에 자료형최대치값 넘는지 static_assert 걸어둔다.
			BOOST_STATIC_ASSERT( CHARLEVELMAX <= SCHAR_MAX );
			pMarketList->cMinLevel = (pMarketList->cMinLevel < 1)			 ? 1			: pMarketList->cMinLevel;
			pMarketList->cMaxLevel = (pMarketList->cMaxLevel > CHARLEVELMAX) ? CHARLEVELMAX : pMarketList->cMaxLevel;

			char cItemExchangeArray[ITEMCATAGORY_MAX];
			memset(cItemExchangeArray, 0, sizeof(cItemExchangeArray));
			if (pMarketList->nExchangeItemID >= 0)
			{
				// 아이템 카테고리 그룹 변환
				TExchangeTrade* pExchange = g_pDataManager->GetExchageTradeData(pMarketList->nExchangeItemID);
				if (!pExchange)
					return ERROR_INVALIDPACKET;

				if (pExchange->nCategoryType == 0) 
				{
					if (!g_pDataManager->GetExchangeGroupID(pExchange->nGroupID, cItemExchangeArray))
						return ERROR_INVALIDPACKET;
				}
				else // 카테고리 타입이 서브일 경우 단독으로 보내서 처리한다.
				{
					cItemExchangeArray[0] = pExchange->ExchangeType;
				}
			}
			else
			{
				cItemExchangeArray[0] = -1;	// 전체검색
			}

			// 직업코드 그룹 변환
			char cJobCodeArray[ITEMJOB_MAX];
			memset(cJobCodeArray, 0, sizeof(cJobCodeArray));

			if (pMarketList->cJob >= 0)
			{

				if (pMarketList->bLowJobGroup)
				{
					g_pDataManager->GetJobGroupID(pMarketList->cJob, cJobCodeArray);
				}
				else
				{
					cJobCodeArray[0] = pMarketList->cJob;
				}
			}
			else
			{
				cJobCodeArray[0] = -1; // 전체검색
			}

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
			std::wstring wstrSearchItemName;
			if (pMarketList->cCount > 0)
			{
				if (pMarketList->cCount == 1)
				{
					if (pMarketList->SearchList[0].nSearchItemID > 0 || pMarketList->SearchList[0].nSearchNameID)
					{
						if (pMarketList->SearchList[0].nSearchItemID > 0)
						{
							TItemData * pSearchItemData = g_pDataManager->GetItemData(pMarketList->SearchList[0].nSearchItemID);
							if (pSearchItemData)
								wstrSearchItemName = pSearchItemData->wszItemName;
						}

						if (wstrSearchItemName.empty() && pMarketList->SearchList[0].nSearchNameID)
							wstrSearchItemName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pMarketList->SearchList[0].nSearchNameID, MultiLanguage::eDefaultLanguage);
					}
				}
				else
				{
					for (int nSearchCount = 0; nSearchCount < pMarketList->cCount; nSearchCount++)
					{
						if (pMarketList->SearchList[nSearchCount].nSearchNameID)
						{
							wstrSearchItemName += GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pMarketList->SearchList[nSearchCount].nSearchNameID, MultiLanguage::eDefaultLanguage);
							if (nSearchCount != pMarketList->cCount - 1)
								wstrSearchItemName += L" ";
						}
					}
				}
			}

			if (wstrSearchItemName.empty())
				wstrSearchItemName = pMarketList->wszSearchItemName;

			/*if (pMarketList->nSearchItemID > 0 || pMarketList->nSearchNameID)
			{
				if (pMarketList->nSearchItemID > 0)
				{
					TItemData * pSearchItemData = g_pDataManager->GetItemData(pMarketList->nSearchItemID);
					if (pSearchItemData)
						wstrSearchItemName = pSearchItemData->wszItemName;
				}

				if (wstrSearchItemName.empty() && pMarketList->nSearchNameID)
					wstrSearchItemName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pMarketList->nSearchNameID, MultiLanguage::eDefaultLanguage);
			}

			if (wstrSearchItemName.empty())
				wstrSearchItemName = pMarketList->wszSearchItemName;*/
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE 

#if defined(PRE_ADD_PETALTRADE)
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
			m_pDBCon->QueryGetPageTrade(this, pMarketList, cJobCodeArray, cItemExchangeArray, const_cast<wchar_t*>(wstrSearchItemName.c_str()));
#else		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
			m_pDBCon->QueryGetPageTrade(this, pMarketList, cJobCodeArray, cItemExchangeArray, pMarketList->wszSearchItemName);
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#else //#if defined(PRE_ADD_PETALTRADE)	
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
			m_pDBCon->QueryGetPageTrade(this, pMarketList, cJobCodeArray, cItemExchangeArray, const_cast<wchar_t*>(wstrSearchItemName.c_str()));
#else		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
			m_pDBCon->QueryGetPageTrade(this, pMarketList, cJobCodeArray, cItemExchangeArray, pMarketList->wszSearchItemName);
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#endif //#if defined(PRE_ADD_PETALTRADE)

			return ERROR_NONE;
		}
		break;

	case CS_MYMARKETLIST:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (nLen != 0)
				return ERROR_INVALIDPACKET;
			if( !IsWindowState(WINDOW_BLIND) ){
				SendMyMarketList(NULL, 0, 0, 0, 0, 0, ERROR_GENERIC_INVALIDREQUEST, false);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}
			m_pDBCon->QueryGetListMyTrade(this);
			return ERROR_NONE;
		}
		break;

	case CS_MARKET_REGISTER:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (sizeof(CSMarketRegister) != nLen)
				return ERROR_INVALIDPACKET;

			if( !IsWindowState(WINDOW_BLIND)){
				SendMarketRegister(0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			CSMarketRegister *pMarket = (CSMarketRegister*)pData;

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if ( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::LimitLevel_Market )) )
			{
				SendMarketRegister( 0, ERROR_GENERIC_LEVELLIMIT );
				return ERROR_NONE;
			}

			int nRet = m_pItem->CheckRegisterMarketItem(pMarket);
			if (nRet != ERROR_NONE){
				SendMarketRegister(0, nRet);
			}

			return ERROR_NONE;
		}
		break;

	case CS_MARKET_INTERRUPT:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (sizeof(CSMarketDBID) != nLen)
				return ERROR_INVALIDPACKET;

			if( !IsWindowState(WINDOW_BLIND)){
				SendMarketInterrupt(0, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			CSMarketDBID *pMarket = (CSMarketDBID*)pData;

			if (!m_pItem->IsCashMyMarketItem(pMarket->nMarketDBID) && (m_pItem->FindBlankInventorySlotCount() == 0)){	// 물건 빼오기 전에 인벤 공간있는지 검사(캐쉬템 아닐때만)
				SendMarketInterrupt(pMarket->nMarketDBID, 0, ERROR_ITEM_INVENTORY_NOTENOUGH);
				return ERROR_NONE;
			}
			int nRet = m_pItem->CancelMarketItem(pMarket->nMarketDBID);
			if (nRet != ERROR_NONE){
				SendMarketInterrupt(pMarket->nMarketDBID, 0, nRet);
				return ERROR_NONE;
			}

			return ERROR_NONE;
		}
		break;

	case CS_MARKET_BUY:
		{
			if (!IsValidPacketByNpcClick())		// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;

			if (sizeof(CSMarketBuy) != nLen)
				return ERROR_INVALIDPACKET;

			CSMarketBuy *pMarket = (CSMarketBuy*)pData;

			if( !IsWindowState(WINDOW_BLIND)){
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
				SendMarketBuy(0, ERROR_GENERIC_INVALIDREQUEST, pMarket->bMini);
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
				SendMarketBuy(0, ERROR_GENERIC_INVALIDREQUEST, false);
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
				return ERROR_NONE;

			if ( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::LimitLevel_Market )) )
			{
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
				SendMarketBuy( 0, ERROR_GENERIC_LEVELLIMIT, pMarket->bMini );
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
				SendMarketBuy( 0, ERROR_GENERIC_LEVELLIMIT, false );
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
				return ERROR_NONE;
			}

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
			int nRet = m_pItem->BuyMarketItem(pMarket->nMarketDBID, pMarket->bMini);
			if (nRet != ERROR_NONE){
				SendMarketBuy(0, nRet, pMarket->bMini);
				return ERROR_NONE;
			}

#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
			m_pDBCon->QueryGetTradeItemID(this, pMarket->nMarketDBID);
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

			return ERROR_NONE;
		}
		break;

	case CS_MARKET_CALCULATIONLIST:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (nLen != 0)
				return ERROR_INVALIDPACKET;
			if( !IsWindowState(WINDOW_BLIND)){
				SendMarketCalculationList(NULL, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}
			m_pDBCon->QueryGetListTradeforCalculation(this);
			return ERROR_NONE;
		}
		break;

	case CS_MARKET_CALCULATION:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (sizeof(CSMarketDBID) != nLen)
				return ERROR_INVALIDPACKET;
			if( !IsWindowState(WINDOW_BLIND)){
				SendMarketCalculation(0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}

			CSMarketDBID *pMarket = (CSMarketDBID*)pData;
			m_pDBCon->QueryTradeCalculation(this, pMarket->nMarketDBID);
			return ERROR_NONE;
		}
		break;

	case CS_MARKET_CALCULATIONALL:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (nLen != 0)
				return ERROR_INVALIDPACKET;
			if( !IsWindowState(WINDOW_BLIND)){
				SendMarketCalculationAll(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}
			m_pDBCon->QueryTradeCalculationAll(this);
			return ERROR_NONE;
		}
		break;

	case CS_MARKET_PETALBALANCE:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (nLen != 0)
				return ERROR_INVALIDPACKET;
			if( !IsWindowState(WINDOW_BLIND)){
				SendMarketCalculation(0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다
			}
			m_pDBCon->QueryGetPetalBalance(this);
			return ERROR_NONE;
		}
		break;

	case CS_MARKET_PRICE:
		{
			if (!IsValidPacketByNpcClick()){	// npc 거리체크
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (sizeof(CSMarketPrice) != nLen)
				return ERROR_INVALIDPACKET;

			CSMarketPrice *pMarket = (CSMarketPrice*)pData;

			TMarketPrice ItemPrices[2];
			if (TradeSystem::CMarketPriceCache::GetInstance().GetTradePrice(pMarket->nItemID, pMarket->cLevel, pMarket->cOption, ItemPrices))
			{
				SendMarketPrice(ERROR_NONE, pMarket->nMarketDBID, ItemPrices);
				return ERROR_NONE;
			}

			m_pDBCon->QueryGetTradePrice(this, pMarket);

			return ERROR_NONE;
		}
		break;

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	case CS_MARKETMINILIST:
		{
			if( !IsWindowState(WINDOW_BLIND) ){
				SendMarketMiniList(NULL, 0, 0, ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_NONE;	// 다른창이 열려있으면 안된다			
			}

			if (sizeof(CSMarketMiniList) != nLen)
				return ERROR_INVALIDPACKET;

			CSMarketMiniList *pMarketList = (CSMarketMiniList*)pData;

			if (!g_pDataManager->GetItemData(pMarketList->nItemID)){
				SendMarketMiniList(NULL, 0, 0, ERROR_ITEM_NOTFOUND);
				return ERROR_NONE;
			}

			m_pDBCon->QueryGetListMiniTrade(this, pMarketList->nItemID, pMarketList->cPayMethodCode);
			return ERROR_NONE;
		}
		break;
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	}
	return CDNUserBase::OnRecvTradeMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvSkillMessage(int nSubCmd, char *pData, int nLen)
{
	switch(nSubCmd)
	{
	/*case CS_ADDSKILL:
		{
			CSAddSkill *pAddSkill = (CSAddSkill*)pData;
			m_pSkill->AddSkill(pAddSkill->nSkillID);
			return ERROR_NONE;
		}
		break;

	case CS_DELSKILL:
		{
			CSDelSkill *pDelSkill = (CSDelSkill*)pData;
			m_pSkill->DelSkill(pDelSkill->nSkillID);
			return ERROR_NONE;
		}
		break;*/

	case CS_SKILLLEVELUP_REQ:
		{
			if (sizeof(CSSkillLevelUpReq) != nLen)
				return ERROR_INVALIDPACKET;

			CSSkillLevelUpReq* pSkillLevelUp = (CSSkillLevelUpReq *)pData;
			m_pSkill->SkillLevelUp( pSkillLevelUp->nSkillID, pSkillLevelUp->cUseSkillPoint );
			return ERROR_NONE;
		}
		break;

	case CS_ACQUIRESKILL_REQ:
		{
			if (sizeof(CSAcquireSkillReq) != nLen)
				return ERROR_INVALIDPACKET;

			CSAcquireSkillReq* pAcquireSkillReq = (CSAcquireSkillReq*)pData;
			m_pSkill->AcquireSkill( pAcquireSkillReq->nSkillID );
			return ERROR_NONE;
		}
		break;

	case CS_UNLOCKSKILL_BY_MONEY_REQ:
		{
			if (sizeof(CSUnlockSkillByMoneyReq) != nLen)
				return ERROR_INVALIDPACKET;

			CSUnlockSkillByMoneyReq* pUnlockSkillByMoneyReq = (CSUnlockSkillByMoneyReq*)pData;
			m_pSkill->UnLockSkillByMoney( pUnlockSkillByMoneyReq->nSkillID );
			return ERROR_NONE;
		}
		break;
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	case CS_RESERVATION_SKILL_LIST_REQ :
		{
			CSReservationSkillListReq* pReservationSkillList = (CSReservationSkillListReq*)pData;
			m_pSkill->ReservationSkillList(pReservationSkillList);
			return ERROR_NONE;
		}
		break;
#endif

#ifdef PRE_ADD_PRESET_SKILLTREE
	case CS_LIST_SKILLSET:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			if (GetDBConnection())
				GetDBConnection()->QueryGetSkillPresetList(this);
			return ERROR_NONE;
		}
		break;

	case CS_SAVE_SKILLSET:
		{
			CSSaveSkillSet * pPacket = (CSSaveSkillSet*)pData;

			if (sizeof(CSSaveSkillSet) - sizeof(pPacket->SKills) + (sizeof(TSkillSetPartialData) * pPacket->cCount) != nLen)
				return ERROR_INVALIDPACKET;

			if (wcslen(pPacket->wszSetName) <= 0)
				return ERROR_GENERIC_UNKNOWNERROR;

			if (pPacket->cIndex < 0 || pPacket->cIndex >= SKILLPRESETMAX)
				return ERROR_GENERIC_UNKNOWNERROR;

			if (GetDBConnection())
				GetDBConnection()->QueryAddSkillPreset(this, pPacket);
			return ERROR_NONE;
		}
		break;

	case CS_DELETE_SKILLSET:
		{
			if (sizeof(CSDeleteSkillSet) != nLen)
				return ERROR_INVALIDPACKET;

			CSDeleteSkillSet * pPacket = (CSDeleteSkillSet*)pData;

			if (pPacket->cIndex < 0 || pPacket->cIndex >= SKILLPRESETMAX)
				return ERROR_GENERIC_UNKNOWNERROR;

			if (GetDBConnection())
				GetDBConnection()->QueryDelSkillPreset(this, pPacket->cIndex);
			return ERROR_NONE;
		}
		break;
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE

	case CS_USE_SKILLRESET_CASHITEM_REQ:
		{
			CSUseSkillResetCashItemReq* pPacket = reinterpret_cast<CSUseSkillResetCashItemReq*>(pData);
#if defined(PRE_MOD_SKILLRESETITEM )
			const TItem* pItem = NULL;
			if( pPacket->cInvenType == ITEMPOSITION_INVEN )
				pItem = m_pItem->GetInventory(pPacket->cInvenIndex);
			else if( pPacket->cInvenType == ITEMPOSITION_CASHINVEN )
				pItem = m_pItem->GetCashInventory(pPacket->biItemSerial);
#else			
			const TItem* pItem = m_pItem->GetCashInventory(pPacket->biItemSerial);
#endif // #if defined(PRE_MOD_SKILLRESETITEM )
			if( pItem )
			{
				TItemData* pItemData = g_pDataManager->GetItemData( pItem->nItemID );
				if( pItemData )
				{
					bool bDeleteItem = true;
					
					// 무제한 스킬 리셋 아이템이면 실제로 존재하는지 검사한다.
					if( pItemData->nType == ITEMTYPE_INFINITY_RESET_SKILL )
					{
#if defined(PRE_MOD_SKILLRESETITEM)
						int iExistCount = 0;
						if( pPacket->cInvenType == ITEMPOSITION_INVEN )
							iExistCount = GetItem()->GetInventoryItemCountByType(ITEMTYPE_INFINITY_RESET_SKILL);
						else if( pPacket->cInvenType == ITEMPOSITION_CASHINVEN )
							iExistCount = GetItem()->GetCashItemCountByType( ITEMTYPE_INFINITY_RESET_SKILL );
#else
						int iExistCount = GetItem()->GetCashItemCountByType( ITEMTYPE_INFINITY_RESET_SKILL );
#endif // #if defined(PRE_MOD_SKILLRESETITEM)
						if( iExistCount <= 0 )
							return ERROR_GENERIC_INVALIDREQUEST;
						bDeleteItem = false;
					}

					if( bDeleteItem == true )
					{
#if defined(PRE_MOD_SKILLRESETITEM)
						if( !m_pItem->DeleteItemByUse( pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biItemSerial) ) 
#else
						if( !m_pItem->DeleteItemByUse( ITEMPOSITION_CASHINVEN, -1, pPacket->biItemSerial) ) 
#endif
							return ERROR_GENERIC_INVALIDREQUEST;
					}
					m_pItem->UseSkillResetCashItem( pItemData );
				}
			}
			return ERROR_NONE;
		}
		break;
	case CS_CHANGE_SKILLPAGE:
		{
			SCChangeSkillPage* pPacket = reinterpret_cast<SCChangeSkillPage*>(pData);
			if( pPacket->cSkillPage >= 0 && pPacket->cSkillPage < GetItem()->GetSkillPageCount())
			{
				SetSkillPage(pPacket->cSkillPage);
				GetDBConnection()->QueryChagneSkillPage( this , pPacket->cSkillPage );
				SendChangeSkillPage(pPacket->cSkillPage);
			}		
		}
		break;
	}

	return ERROR_UNKNOWN_HEADER;		//상위에서 리턴되어야 합니다 여까지 오면 처리하지 않는 패킷의 수신!
}

int CDNUserSession::OnRecvFriendMessage(int nSubCmd, char * pData, int nLen)
{
	return CDNUserBase::OnRecvFriendMessage(nSubCmd, pData, nLen);
}


int CDNUserSession::OnRecvGuildMessage(int nSubCmd, char * pData, int nLen)
{
	switch(nSubCmd)
	{
	case eGuild::CS_CREATEGUILD:			// 길드 창설 요청
		{
			const CSCreateGuild *pPacket = reinterpret_cast<CSCreateGuild*>(pData);

			// 패킷사이즈 검사
			if (sizeof(CSCreateGuild) != nLen)
				return ERROR_INVALIDPACKET;

			// 길드명 사이즈 검사
			if (0 >= ::wcslen(pPacket->wszGuildName)) 
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GENERIC_INVALIDREQUEST, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			// 20100629 길드명 작성 시 국가별 유니코드 문자대역 확인하도록 기능 추가
			if (!g_CountryUnicodeSet.Check(pPacket->wszGuildName)) 
			{	
				SendCreateGuild(GetSessionID(), L"", ERROR_GENERIC_INVALIDREQUEST, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

#ifdef PRE_MOD_74203
#if defined(PRE_ADD_MULTILANGUAGE)
			if (g_pDataManager->CheckProhibitWord(static_cast<int>(m_eSelectedLanguage), pPacket->wszGuildName))
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GENERIC_INVALIDREQUEST, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			if (g_pDataManager->CheckProhibitWord(pPacket->wszGuildName))
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GENERIC_INVALIDREQUEST, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#endif		//#ifdef PRE_MOD_74203

			// 해당 유저가 기존 길드정보를 가지고 있는지 확인
			const TGuildUID GuildUID = GetGuildUID();
			if (GuildUID.IsSet()) 
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD;
			}

			// 길드 창설에 필요한 최소 캐릭터 레벨 체크
			if (GUILDCREATECHARLEVEL_DEF > GetLevel()) 
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GUILD_LACKOFCOUNT_CREATELEVEL, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GUILD_LACKOFCOUNT_CREATELEVEL;
			}

			TGuildLevel* GuildLevel = g_pDataManager->GetGuildLevelData(1);
			if (!GuildLevel)
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GENERIC_UNKNOWNERROR, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GENERIC_UNKNOWNERROR;
			}

			int nReqCoin = GuildLevel->nReqGold;
			if (nReqCoin > GetCoin()) 
			{
				SendCreateGuild(GetSessionID(), L"", ERROR_GUILD_LACKOFCOUNT_SELFMONEY, NULL, NULL, 0, 0, 0, 0, NULL);
				return ERROR_GUILD_LACKOFCOUNT_SELFMONEY;
			}

			int wGuildRoleAuth[GUILDROLE_TYPE_CNT] = { 0, };

			// 길드 권한 적용(최초)
			g_pGuildManager->SetAuthBase(wGuildRoleAuth);

			// DB 데이터 전송
			m_pDBCon->QueryCreateGuild(m_cDBThreadID, m_nAccountDBID, m_biCharacterDBID, pPacket->wszGuildName, GetMapIndex(), GetLevel(), nReqCoin, g_Config.nWorldSetID, wGuildRoleAuth);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_DISMISSGUILD:			// 길드 해체 요청
		{
			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendDismissGuild(GetSessionID(), ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, NULL);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			// 이미 해체된 길드인지 체크
			if (g_pGuildManager->IsDismissExist(GuildUID)) 
			{
				ResetGuildSelfView();
				SendDismissGuild(GetSessionID(), ERROR_GUILD_ALREADYDISMISS_GUILD, NULL);
				return ERROR_GUILD_ALREADYDISMISS_GUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			// 길드존재여부 검사
			if (!pGuild) 
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILD_NOTEXIST_GUILDINFO, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable()) 
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILD_NOTEXIST_GUILDINFO, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 마스터 권한 검사
			if (GUILDROLE_TYPE_MASTER != GetGuildSelfView().btGuildRole) 
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILD_HAS_NO_AUTHORITY, NULL);
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			// 길드멤버인지 확인
			if (!pGuild->GetMemberInfo(GetCharacterDBID())) 
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILD_HAS_NO_AUTHORITY, NULL);
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			// 소속 길드원수가 1명인지 확인
			if (pGuild->GetMemberCount() > 1) 
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILD_STILLEXIST_MEMBER, NULL);
				return ERROR_GUILD_STILLEXIST_MEMBER;
			}

			if (g_pGuildWarManager && pGuild->IsEnrollGuildWar() && g_pGuildWarManager->GetRestriction(nSubCmd, pGuild->GetUID()))
			{
				SendDismissGuild(GetSessionID(), ERROR_GUILDWAR_CANTDISMISS, NULL);
				return ERROR_GUILDWAR_CANTDISMISS;
			}

			// 빌리지길드로변환
			_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

			if (pGuildVillage)
			{
				CDNGuildWare* pGuildWare = NULL;
				pGuildWare = pGuildVillage->GetGuildWare();

				if (pGuildWare)
				{
					// 코인확인
					if (pGuildWare->GetWarehouseCoin() != 0)
					{
						SendDismissGuild(GetSessionID(), ERROR_GUILD_CANTDISMISS_MONEY, NULL);
						return ERROR_GUILD_CANTDISMISS_MONEY;
					}

					// 아이템 갯수 확인
					if (pGuildWare->GetWareItemCount() > 0)
					{
						SendDismissGuild(GetSessionID(), ERROR_GUILD_CANTDISMISS_ITEM, NULL);
						return ERROR_GUILD_CANTDISMISS_ITEM;
					}
				}			
			}

			m_pDBCon->QueryDismissGuild(this);

			return ERROR_NONE;
		}
		break;

	case eGuild::CS_INVITEGUILDMEMBREQ:		// 길드원 초대 요청
		{
			const CSInviteGuildMemberReq *pPacket = reinterpret_cast<CSInviteGuildMemberReq*>(pData);

			if (sizeof(CSInviteGuildMemberReq) != nLen)
				return ERROR_INVALIDPACKET;

			if (0 >= ::wcslen(pPacket->wszToCharacterName)) 
			{
				SendInviteGuildMemberReq(GetGuildUID(), GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GENERIC_INVALIDREQUEST, 0, 0);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) 
			{
				SendInviteGuildMemberReq(GetGuildUID(), GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, 0, 0);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			if (!pGuild) 
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 권한 체크(초대)
			DN_ASSERT(CHECK_LIMIT(GetGuildSelfView().btGuildRole, GUILDROLE_TYPE_CNT),	"Check!");
			if (!pGuild->CheckGuildInfoAuth(static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_INVITE))) 
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0);
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			// 인원추가 가능여부 체크
			if (!pGuild->IsMemberAddable()) 
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_CANTADD_GUILDMEMBER, 0, 0);
				return ERROR_GUILD_CANTADD_GUILDMEMBER;
			}

			// 길드전 예선/본선 기간에는 가입을 못시킨다. 
			if (g_pGuildWarManager && pGuild->IsEnrollGuildWar() && g_pGuildWarManager->GetRestriction(nSubCmd, pGuild->GetUID()) )
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILDWAR_CANTADD_GUILDMEMBER, 0, 0);
				return ERROR_GUILDWAR_CANTADD_GUILDMEMBER;
			}

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pPacket->wszToCharacterName);

			// 현재서버에 유저를 찾지 못하면 마스터서버에 요청하여 찾아본다.
			if (!pUserObj)
			{
				g_pMasterConnection->SendInviteGuildMember(GuildUID, GetAccountDBID(), GetSessionID(), GetCharacterDBID(), GetCharacterName(), pPacket->wszToCharacterName, pGuild->GetInfo()->GuildView.wszGuildName );
				return ERROR_NONE;
			}

			// 현재서버에 유저가 존재하는 경우

			// 로딩상태 확인
			if (!pUserObj->m_bLoadUserData) 
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE, 0, 0);
				return ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE;
			}

			if (pUserObj->GetGuildUID().IsSet()) 
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_ALREADYINGUILD_OPPOSITEUSER, 0, 0);
				return ERROR_GUILD_ALREADYINGUILD_OPPOSITEUSER;
			}

			if (GetAccountDBID() == pUserObj->GetAccountDBID()) 
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_CANTINVITE_YOURSELF, 0, 0);
				return ERROR_GUILD_CANTINVITE_YOURSELF;
			}

			if (pUserObj->IsAcceptAbleOption(GetCharacterDBID(), GetAccountDBID(), _ACCEPTABLE_CHECKTYPE_GUILDINVITE) == false)
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_REFUESED_GUILDINVITATION, 0, 0);
				return ERROR_GUILD_REFUESED_GUILDINVITATION;
			}

			// 유저 상태 검사 및 변경.
			if ( !pUserObj->IsNoneWindowState() )
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_GUILD_CANTACCEPT_GUILDINVITATION, 0, 0);
				return ERROR_GUILD_CANTACCEPT_GUILDINVITATION;
			}
#if defined(PRE_ADD_DWC)
			if(pUserObj->IsDWCCharacter())
			{
				SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), pPacket->wszToCharacterName, ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED, 0, 0);
				return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;
			}
#endif
			pUserObj->SetWindowState(WINDOW_ISACCEPT);
			pUserObj->SendInviteGuildMemberReq(GuildUID, GetAccountDBID(), GetSessionID(), GetCharacterName(), ERROR_NONE, pUserObj->GetAccountDBID(), pUserObj->GetSessionID(), pGuild->GetInfo()->GuildView.wszGuildName);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_INVITEGUILDMEMBACK:		// 길드원 초대 응답
		{
			const CSInviteGuildMemberAck *pPacket = reinterpret_cast<CSInviteGuildMemberAck*>(pData);

			if (sizeof(CSInviteGuildMemberAck) != nLen)
				return ERROR_INVALIDPACKET;

			//const TGuildUID GuildUID = pPacket->GuildUID;
			IsWindowStateNoneSet(WINDOW_ISACCEPT);
			const TGuildUID GuildUID = GetGuildUID();
			if (GuildUID.IsSet()) 
			{
				SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD, 0, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD;
			}

			// 수락/거절여부를 확인
			if (!pPacket->bAccept) 
			{
				// 길드에초대해준사용자에게알림
				CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(pPacket->nFromAccountID);

				if (pUserObj) 
					pUserObj->SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_GUILD_REFUESED_GUILDINVITATION, pUserObj->GetAccountDBID(), pUserObj->GetSessionID(), 0, 0, NULL, NULL);
				else
				{
					// 마스터서버를 통해 초대한자에게 거절했음을 알린다.
					g_pMasterConnection->SendInviteGuildResult(pPacket->nFromAccountID, ERROR_GUILD_REFUESED_GUILDINVITATION, true, GetCharacterName());
				}
				break;
			}

			// 길드매니져에서 해당길드를 찾는다.
			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild)
			{
				// 서버상에 길드가 존재하지 않으므로 DB조회를 하여 처리하도록 한다.
				m_pDBCon->QueryGuildInviteMember(m_cDBThreadID, m_nAccountDBID, pPacket->GuildUID.nDBID, m_nWorldSetID, GetSessionID(), true);
				m_pDBCon->QueryGetGuildRewardItem( m_cDBThreadID, m_nWorldSetID, m_nAccountDBID, pPacket->GuildUID.nDBID );
				return ERROR_NONE;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				// 서버상에 길드가 존재하지 않으므로 DB조회를 하여 처리하도록 한다.
				m_pDBCon->QueryGuildInviteMember(m_cDBThreadID, m_nAccountDBID, pPacket->GuildUID.nDBID, m_nWorldSetID, GetSessionID(), true);
				m_pDBCon->QueryGetGuildRewardItem( m_cDBThreadID, m_nWorldSetID, m_nAccountDBID, pPacket->GuildUID.nDBID );
				return ERROR_NONE;
			}
#endif

			// 인원추가 가능여부 체크
			DN_ASSERT(!pGuild->IsLock(),	"Already Locked!");
			if (!pGuild->IsMemberAddable()) 
			{
				SendInviteGuildMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_GUILD_CANTADD_GUILDMEMBER, 0, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_CANTADD_GUILDMEMBER;
			}

			m_pDBCon->QueryAddGuildMember(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), pPacket->GuildUID.nDBID, GetLevel(), g_Config.nWorldSetID, pGuild->GetInfo()->wGuildSize);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_LEAVEGUILDMEMB:			// 길드원 탈퇴 요청
		{
			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) 
			{
				SendLeaveGuildMember(GetAccountDBID(), GetCharacterDBID(), ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, NULL, false);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendLeaveGuildMember(GetAccountDBID(), GetCharacterDBID(), ERROR_GUILD_NOTEXIST_GUILDINFO, NULL, false);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendLeaveGuildMember(GetAccountDBID(), GetCharacterDBID(), ERROR_GUILD_NOTEXIST_GUILDINFO, NULL, false);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 길드장은 탈퇴할 수 없음.
			if (GUILDROLE_TYPE_MASTER == static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole)) 
			{
				SendLeaveGuildMember(GetAccountDBID(), GetCharacterDBID(), ERROR_GUILD_NOTAVAILABLE_GUILDMASTER, &GuildUID, false);
				return ERROR_GUILD_NOTAVAILABLE_GUILDMASTER;
			}

			if (g_pGuildWarManager &&  pGuild->IsEnrollGuildWar() && g_pGuildWarManager->GetRestriction(nSubCmd, pGuild->GetUID()))
			{
				SendLeaveGuildMember(GetAccountDBID(), GetCharacterDBID(), ERROR_GUILDWAR_CANTLEAVE_GUILDMEMBER, &GuildUID, false);
				return ERROR_GUILDWAR_CANTLEAVE_GUILDMEMBER;
			}

			m_pDBCon->QueryDelGuildMember(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), GetAccountDBID(), GetCharacterDBID(), GuildUID.nDBID, GetLevel(), g_Config.nWorldSetID, false, false);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_EXILEGUILDMEMB:			// 길드원 추방 요청
		{
			const CSExileGuildMember *pPacket = reinterpret_cast<CSExileGuildMember*>(pData);

			if (sizeof(CSExileGuildMember) != nLen)
				return ERROR_INVALIDPACKET;

			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) 
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, NULL);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			if (!pGuild) 
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 권한 체크(추방)
			DN_ASSERT(CHECK_LIMIT(GetGuildSelfView().btGuildRole, GUILDROLE_TYPE_CNT),	"Check!");
			if (!pGuild->CheckGuildInfoAuth(static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_EXILE))) 
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_HAS_NO_AUTHORITY, NULL);
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			TGuildMember* pGuildMember = pGuild->GetMemberInfo(pPacket->nCharacterDBID);
			if (!pGuildMember)
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_NOTOURGUILD_OPPOSITEUSER, NULL);
				return ERROR_GUILD_NOTOURGUILD_OPPOSITEUSER;
			}

			// 길마는 추방을 못시킨다.
			if (pGuildMember->btGuildRole == GUILDROLE_TYPE_MASTER)
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_HAS_NO_AUTHORITY, NULL);
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			// 해당 캐릭터아이디 체크
			if (pPacket->nCharacterDBID == GetCharacterDBID()) 
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILD_CANTEXILE_YOURSELF, NULL);
				return ERROR_GUILD_CANTEXILE_YOURSELF;
			}
			// 길드전예선/본선기간에는가입을못시킨다.
			if (g_pGuildWarManager && pGuild->IsEnrollGuildWar() && g_pGuildWarManager->GetRestriction(nSubCmd, pGuild->GetUID()) )
			{
				SendExileGuildMember(pPacket->nAccountDBID, pPacket->nCharacterDBID, ERROR_GUILDWAR_CANTEXILE_GUILDMEMBER, NULL);
				return ERROR_GUILDWAR_CANTEXILE_GUILDMEMBER;
			}
			m_pDBCon->QueryDelGuildMember(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), pPacket->nAccountDBID, pPacket->nCharacterDBID, GuildUID.nDBID, GetLevel(), g_Config.nWorldSetID, true, false);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_CHANGEGUILDINFO:		// 길드 정보 변경 요청
		{
			const CSChangeGuildInfo *pPacket = reinterpret_cast<CSChangeGuildInfo*>(pData);

			if (sizeof(CSChangeGuildInfo) != nLen)
				return ERROR_INVALIDPACKET;

			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) 
			{
				SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}


			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			if (!pGuild) 
			{
				SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 해당유저에 대한 권한변수 설정
			eGuildRoleType btGuildRole = static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole);

			// 길드 일반 포인트 변화량(차감은 음수이어야 함)
			long nAddCommonPoint = 0;

			// 길드 정보 변경 타입 별 체크 및 수행
			switch(pPacket->btGuildUpdate) 
			{
			case GUILDUPDATE_TYPE_NOTICE:			// 공지 변경
				{
					// 권한 체크(공지)
					DN_ASSERT(CHECK_LIMIT(btGuildRole, GUILDROLE_TYPE_CNT),	"Check!");
					if (!pGuild->CheckGuildInfoAuth(btGuildRole, static_cast<eGuildAuthType>(GUILDAUTH_TYPE_NOTICE))) 
					{
						SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GUILD_HAS_NO_AUTHORITY;
					}
				}
				break;

			case GUILDUPDATE_TYPE_ROLEAUTH:			// 직급 권한 변경(중요! 길드장이 길드원으로 변경되는 현상이 발생)
				{

					// 아이템 제한 검사
					if (0 > pPacket->Int3 || pPacket->Int3 > g_pDataManager->GetGuildLimitMaxItem())
					{
						// 추후 에러변경
						SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GUILD_HAS_NO_AUTHORITY;
					}

					// 출금액 제한 검사
					if (0 > pPacket->Int4 || pPacket->Int4 > g_pDataManager->GetGuildLimitMaxCoin())
					{
						// 추후 에러변경
						SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GUILD_HAS_NO_AUTHORITY;
					}

					// 길드마스터는 언제나 직급에 대한 권한을 변경할 수 있다.
					if (GUILDROLE_TYPE_MASTER == btGuildRole) 
						break; // DB세션에서 처리

					// 길드부장이 권한변경시 조건처리
					if (GUILDROLE_TYPE_SUBMASTER == btGuildRole)
					{
						// 길드마스터 권한 변경X
						if (GUILDROLE_TYPE_MASTER == pPacket->Int1)
						{

						}
						else
							break; // DB세션에서 처리
					}
				
					// 그 외에 길드원이 변경을 시도할시 에러처리!
					SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
					return ERROR_GUILD_HAS_NO_AUTHORITY;
					
				}
				break;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			case GUILDUPDATE_TYPE_HOMEPAGE:
				{
					// 권한 체크(홈페이지)
					DN_ASSERT(CHECK_LIMIT(btGuildRole, GUILDROLE_TYPE_CNT),	"Check!");
					if (!pGuild->CheckGuildInfoAuth(btGuildRole, static_cast<eGuildAuthType>(GUILDAUTH_TYPE_HOMEPAGE))) 
					{
						SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GUILD_HAS_NO_AUTHORITY;
					}
				}
				break;
#endif
			default:
				{
					SendChangeGuildInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->btGuildUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
				}
				DN_RETURN(ERROR_GENERIC_INVALIDREQUEST);
			}

			m_pDBCon->QueryChangeGuildInfo(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), GuildUID.nDBID, g_Config.nWorldSetID, pPacket->btGuildUpdate, pPacket->Int1, pPacket->Int2, pPacket->Int3, pPacket->Int4, pPacket->Int64, pPacket->Text, nAddCommonPoint);
			return ERROR_NONE;
		}
		break;

	// 길드원 정보 변경 요청
	case eGuild::CS_CHANGEGUILDMEMBINFO:	
		{
		
			const CSChangeGuildMemberInfo *pPacket = reinterpret_cast<CSChangeGuildMemberInfo*>(pData);

			if (sizeof(CSChangeGuildMemberInfo) != nLen)
				return ERROR_INVALIDPACKET;

			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_NOTEXIST_GUILDINFO, 0, 0, 0, NULL, NULL);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			eGuildRoleType btGuildRole = static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole);

			UINT nChgAccountDBID = 0;
			INT64 nChgCharacterDBID = 0;

			// 길드원 정보 변경 타입 별 체크 및 수행
			switch(pPacket->btGuildMemberUpdate) 
			{
			case GUILDMEMBUPDATE_TYPE_INTRODUCE:	// 자기소개 변경(길드원 자신)
				{
					nChgAccountDBID = GetAccountDBID();
					nChgCharacterDBID = GetCharacterDBID();
				}
				break;
			case GUILDMEMBUPDATE_TYPE_ROLE:			// 직급 변경(길드장 → 길드원 ?)
				{
					// 길드부장 이상만 직급을 변경할 수 있다.
					if (GUILDROLE_TYPE_SUBMASTER < btGuildRole) 
					{
						SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GUILD_HAS_NO_AUTHORITY;
					}

					if (GUILDROLE_TYPE_MASTER >= pPacket->Int1 || GUILDROLE_TYPE_CNT <= pPacket->Int1) {
						SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GENERIC_INVALIDREQUEST;
					}

					// 길드부장은 길드장 및 자신의 직급을 변경할 수 없다.
					if (GUILDROLE_TYPE_SUBMASTER == btGuildRole)
					{
						if (GUILDROLE_TYPE_MASTER == pPacket->Int1 || GUILDROLE_TYPE_SUBMASTER == pPacket->Int1)
						{
							SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
							return ERROR_GENERIC_INVALIDREQUEST;
						}
					}

					nChgAccountDBID = pPacket->nChgAccountDBID;
					nChgCharacterDBID = pPacket->nChgCharacterDBID;
				}
				break;
			case GUILDMEMBUPDATE_TYPE_GUILDMASTER:	// 길드장 위임(길드장 → 길드원 ?) 추후 변경!!!
				{
					if (GUILDROLE_TYPE_MASTER != static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole)) {
						SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GUILD_HAS_NO_AUTHORITY, 0, 0, 0, NULL, NULL);
						return ERROR_GUILD_HAS_NO_AUTHORITY;
					}

					nChgAccountDBID = pPacket->nChgAccountDBID;
					nChgCharacterDBID = pPacket->nChgCharacterDBID;
				}
				break;
			// 	case GUILDMEMBUPDATE_TYPE_CMMPOINT:		// 일반 포인트 증가/감소(기여분)
			default:
				{
					SendChangeGuildMemberInfo(GetAccountDBID(), GetCharacterDBID(), pPacket->nChgAccountDBID, pPacket->nChgCharacterDBID, pPacket->btGuildMemberUpdate, ERROR_GENERIC_INVALIDREQUEST, 0, 0, 0, NULL, NULL);
				}
				DN_RETURN(ERROR_GENERIC_INVALIDREQUEST);
			}
			DN_ASSERT(0 != nChgAccountDBID,		"Invalid!");
			DN_ASSERT(0 != nChgCharacterDBID,	"Invalid!");

			m_pDBCon->QueryChangeGuildMemberInfo(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), nChgAccountDBID, nChgCharacterDBID, GuildUID.nDBID, g_Config.nWorldSetID, pPacket->btGuildMemberUpdate, pPacket->Int1, pPacket->Int2, pPacket->Int64, pPacket->Text, GetLevel());
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_GETGUILDHISTORYLIST:	// 길드 히스토리 목록 요청
		{
			const CSGetGuildHistoryList *pPacket = reinterpret_cast<CSGetGuildHistoryList*>(pData);

			if (sizeof(CSGetGuildHistoryList) != nLen)
				return ERROR_INVALIDPACKET;

			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendGetGuildHistoryList(GetGuildUID(), 0, 0, 0, 0, ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, NULL, 0);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendGetGuildHistoryList(GetGuildUID(), 0, 0, 0, 0, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL, 0);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendGetGuildHistoryList(GetGuildUID(), 0, 0, 0, 0, ERROR_GUILD_NOTEXIST_GUILDINFO, NULL, 0);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif
		
			m_pDBCon->QueryGetGuildHistoryList(this, pPacket->biIndex, pPacket->bDirection);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_GETGUILDINFO:			// 길드 정보 요청
		{
			if (sizeof(CSGetGuildInfo) != nLen)
				return ERROR_INVALIDPACKET;

			g_pGuildManager->OnRecvCsGetGuildInfo(this, reinterpret_cast<CSGetGuildInfo*>(pData));
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_GET_GUILDWARE_HISTORY:	// 길드창고 요청
		{
			if (sizeof(CSGetGuildWareHistory) != nLen)
				return ERROR_INVALIDPACKET;

			g_pGuildManager->OnRecvCsGetGuildWareHistory(this, reinterpret_cast<CSGetGuildWareHistory*>(pData));
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_CLOSE_GUILDWARE:		// 길드창고 닫기
		{
			g_pGuildManager->OnRecvCsCloseGuildWare(this);
			return ERROR_NONE;
		}
		break;

	case eGuild::CS_ENROLL_GUILDWAR:		// 길드전 참가신청
		{
			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendEnrollGuildWar(ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendEnrollGuildWar(ERROR_GUILD_NOTEXIST_GUILDINFO);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendEnrollGuildWar(ERROR_GUILD_NOTEXIST_GUILDINFO);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			eGuildRoleType btGuildRole = static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole);

			// 권한 체크
			if (GUILDROLE_TYPE_MASTER != btGuildRole)
			{
				SendEnrollGuildWar(ERROR_GUILD_HAS_NO_AUTHORITY);
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			// 기간 체크
			if (g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_PREPARATION) 
			{
				SendEnrollGuildWar(ERROR_GUILDWAR_CANTENROLL);
				return ERROR_GUILDWAR_CANTENROLL;
			}

			// 신청여부 
			if (pGuild->IsEnrollGuildWar())
			{
				SendEnrollGuildWar(ERROR_GUILDWAR_ALREADY_ENROLL);
				return ERROR_GUILDWAR_ALREADY_ENROLL;
			}

			if (m_pDBCon)
				m_pDBCon->QueryEnrollGuildWar(m_cDBThreadID, GetGuildUID().nDBID, g_Config.nWorldSetID, GetAccountDBID());
		}
		break;
	case eGuild::CS_GUILDWAR_VOTE : // 길드전 투표
		{
			CSGuildWarVote *pPacket = reinterpret_cast<CSGuildWarVote*>(pData);

			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendGuildWarVote(ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild) 
			{
				SendGuildWarVote(ERROR_GUILD_NOTEXIST_GUILDINFO);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendGuildWarVote(ERROR_GUILD_NOTEXIST_GUILDINFO);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif
			// 투표기간인지.
			if ( g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_REWARD )
			{
				SendGuildWarVote(ERROR_GUILDWAR_CANT_VOTE);
				return ERROR_GUILDWAR_CANT_VOTE;
			}
			// 길드전에 참여안했거나 결승 진출팀이거나 예선에 참가한 캐릭터가 아니면
			if( !pGuild->IsEnrollGuildWar() || g_pGuildWarManager->IsGuildWarFinal(GetGuildUID()) || !GetGuildWarPoint() )
			{
				SendGuildWarVote(ERROR_GUILDWAR_CANT_VOTE);
				return ERROR_GUILDWAR_CANT_VOTE;
			}
			// 투표여부
			if ( GetGuildWarVote() )
			{
				SendGuildWarVote(ERROR_GUILDWAR_ALREADY_VOTE);
				return ERROR_GUILDWAR_ALREADY_VOTE;
			}
			if (m_pDBCon)
				m_pDBCon->QueryAddGuildWarPopularityVote(m_cDBThreadID, g_Config.nWorldSetID, GetAccountDBID(), GetCharacterDBID(), pPacket->GuildUID.nDBID );
		}
		break;
	case eGuild::CS_GUILDWAR_COMPENSATION : // 길드전 예선 보상받기
		{
			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendGuildWarReward(ERROR_GUILD_YOUR_NOT_BELONGANYGUILD);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild)
			{
				SendGuildWarReward(ERROR_GUILD_NOTEXIST_GUILDINFO);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendGuildWarReward(ERROR_GUILD_NOTEXIST_GUILDINFO);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif
			// 보상기간인지.
			if ( g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_REWARD || !IsGuildWarReward() || GetGuildWarRewardEnable() == GUILDWAR_REWARD_CHAR_MAX )
			{
				SendGuildWarReward(ERROR_GUILDWAR_CANT_COMPENSATION);
				return ERROR_GUILDWAR_CANT_COMPENSATION;
			}			

			// 예선에 참가한 흔적이 있는지 검사.
			if( GetGuildWarPoint() == 0)
			{
				SendGuildWarReward(ERROR_GUILDWAR_CANT_COMPENSATION);
				return ERROR_GUILDWAR_CANT_COMPENSATION;
			}

			if (m_pDBCon)
			{		
				CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
				SetGuildWarRewardGuildPoint(pGuildVillage->CalcGuildWarTrialPoint());
				SetGuildWarRewardFestivalPoint(g_pGuildWarManager->CalcGuildWarFestivalPoint(GetGuildSelfView().cTeamColorCode, GetCharacterDBID(), GetGuildWarPoint()));

				//길드 포인트 받았는지 체크하고 안 받았으면 받자..DB에서도 한번 체크해줌.
				if( !pGuild->GetGuildWarRewardEnable(GUILDWAR_REWARD_GUILD_TRIAL_POINT) )
				{					
					m_pDBCon->QueryAddGuildWarRewarForGuild(m_cDBThreadID, g_Config.nWorldSetID,  g_pGuildWarManager->GetScheduleID(), pGuild->GetUID().nDBID, GUILDWAR_REWARD_GUILD_TRIAL_POINT);
				}
				m_pDBCon->QueryAddGuildWarRewarForCharacter(m_cDBThreadID, this);
			}
		}
		break;
	case eGuild::CS_GUILDWAR_WIN_SKILL :
		{
			CSGuildWarWinSkill *pPacket = reinterpret_cast<CSGuildWarWinSkill*>(pData);
			const TGuildUID GuildUID = GetGuildUID();
			if (!GuildUID.IsSet()) {
				SendGuildWarWinSkill(ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, 0);
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}
			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
			if (!pGuild)
			{
				SendGuildWarWinSkill(ERROR_GUILD_NOTEXIST_GUILDINFO, 0);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendGuildWarWinSkill(ERROR_GUILD_NOTEXIST_GUILDINFO, 0);
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif
			// 우승길드가 맞는지 확인.
			if( !g_pGuildWarManager->GetPreWinGuildUID().IsSet() && g_pGuildWarManager->GetPreWinGuildUID() != GuildUID )
			{
				SendGuildWarWinSkill(ERROR_GUILDWAR_NOT_PRE_WIN, 0);
				return ERROR_GUILDWAR_NOT_PRE_WIN;
			}
			// 쿨타임이 맞는지 확인
			DWORD dwCurTick = timeGetTime();
			if( g_pGuildWarManager->GetPreWinSKillCoolTime()+GUILDWAR_PRE_WIN_SKILL_COOLTIME > dwCurTick )
			{
				SendGuildWarWinSkill(ERROR_GUILDWAR_SKILL_COOLTIME, g_pGuildWarManager->GetPreWinSKillCoolTime()+GUILDWAR_PRE_WIN_SKILL_COOLTIME - dwCurTick);
				return ERROR_GUILDWAR_SKILL_COOLTIME;
			}
			// nExp를 잠시 사용..UINT라서
			g_pGuildWarManager->SetPreWinSKillCoolTime(dwCurTick);
			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
			{
				g_pMasterConnection->SendSetGuilldWarPreWinSkillCoolTime(g_Config.nManagedID, dwCurTick);
			}
			SendGuildWarWinSkill(ERROR_NONE, 0);
			m_ParamData.nExp = GetGuildUID().nDBID;
			m_ParamData.nIndex = pPacket->nSkillID;
			SendFieldMessage(FM_GUILDWARWINSKILL);
		}
		break;
		case eGuild::CS_GUILD_BUY_REWARDITEM:
		{
			//길드 구매
			if (sizeof(CSBuyGuildRewardItem) != nLen)
				return ERROR_INVALIDPACKET;

			CSBuyGuildRewardItem *pPacket = reinterpret_cast<CSBuyGuildRewardItem*>(pData);

			g_pGuildManager->BuyGuildRewardItem( this, pPacket->nItemID );
			return ERROR_NONE;
		}
		break;
		case eGuild::CS_PLAYER_REQUEST_GUILDINFO:
		{
			const CSPlayerGuildInfo *pPacket = reinterpret_cast<CSPlayerGuildInfo*>(pData);

			if (sizeof(CSPlayerGuildInfo) != nLen)
				return ERROR_INVALIDPACKET;

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSession(pPacket->nSessionID);

			if( !pUserObj )
			{
				SendPlayerGuildInfo( ERROR_GENERIC_INVALIDREQUEST, 0, NULL, NULL, 0, 0, 0, NULL );
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			const TGuildUID GuildUID = pUserObj->GetGuildUID();
			if (!GuildUID.IsSet()) 
			{
				SendPlayerGuildInfo( ERROR_NONE, pUserObj->GetSessionID(), NULL, NULL, 0, 0, 0, NULL );
				return ERROR_NONE;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			if (!pGuild) 
			{
				SendPlayerGuildInfo( ERROR_NONE, pUserObj->GetSessionID(), NULL, NULL, 0, 0, 0, NULL );
				return ERROR_NONE;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendPlayerGuildInfo( ERROR_NONE, pUserObj->GetSessionID(), NULL, NULL, 0, 0, 0, NULL );
				return ERROR_NONE;
			}
#endif

			TGuildMember *GuildMaster = pGuild->GetGuildMaster();
			if( GuildMaster )
				SendPlayerGuildInfo( ERROR_NONE, pUserObj->GetSessionID(), pGuild->GetInfo()->GuildView.wszGuildName, GuildMaster->wszCharacterName, pGuild->GetLevel(), pGuild->GetMemberCount(), pGuild->GetGuildMemberSize(), pGuild->GetGuildRewardItem() );
			else
				SendPlayerGuildInfo( ERROR_NONE, pUserObj->GetSessionID(), pGuild->GetInfo()->GuildView.wszGuildName, NULL, pGuild->GetLevel(), pGuild->GetMemberCount(), pGuild->GetGuildMemberSize(), pGuild->GetGuildRewardItem() );

			return ERROR_NONE;
		}
		break;
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
		case eGuild::CS_GUILD_CONTRIBUTION_RANK:
		{
			m_pDBCon->QueryGetWeeklyGuildContributionPointRanking( m_cDBThreadID, g_Config.nWorldSetID, GetAccountDBID(), GetGuildUID().nDBID );

			return ERROR_NONE;
		}
		break;
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	default:
		DN_BREAK;
	}

	return ERROR_UNKNOWN_HEADER;
}

bool CDNUserSession::_CheckPvPGameModeInventory()
{
	CDNPvPRoom* pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( GetChannelID(), GetPvPIndex() );
	if( pPvPRoom == NULL )
	{
		_DANGER_POINT();
		return false;
	}
	const TPvPGameModeTable* pPvPGameMode = g_pDataManager->GetPvPGameModeTable( pPvPRoom->GetGameModeTableID() );
	if( pPvPGameMode == NULL )
	{
		_DANGER_POINT();
		return false;
	}
	if( pPvPGameMode->iNeedInven > GetItem()->FindBlankInventorySlotCount() )
		return false;

	return true;
}

bool CDNUserSession::_CheckPvPGameModeInventory(UINT uiPvPIndex)
{
	CDNPvPRoom* pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( GetChannelID(), uiPvPIndex );
	if( pPvPRoom == NULL )
	{
		_DANGER_POINT();
		return false;
	}

	if (pPvPRoom->GetEventRoomIndex() <= 0)
		return true;

	const TPvPGameModeTable* pPvPGameMode = g_pDataManager->GetPvPGameModeTable( pPvPRoom->GetGameModeTableID() );
	if( pPvPGameMode == NULL )
	{
		_DANGER_POINT();
		return false;
	}
	if( pPvPGameMode->iNeedInven > GetItem()->FindBlankInventorySlotCount() )
		return false;

	return true;
}

int CDNUserSession::OnRecvPvPMessage(int nSubCmd, char * pData, int nLen)
{
	bool bCheckPvPLobby = true;
	
#if defined(PRE_ADD_QUICK_PVP)
	// 2개를 위해서 따로 빼기도 머하고 PVP라 다른 패킷에 넣기도 머하고..
	if( nSubCmd == ePvP::CS_QUICKPVP_INVITE || nSubCmd == ePvP::CS_QUICKPVP_RESULT)
		bCheckPvPLobby = false;
#endif
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	if( bCheckPvPLobby && (nSubCmd == ePvP::CS_ROOMLIST || nSubCmd == ePvP::CS_JOINROOM || nSubCmd == ePvP::CS_CREATEROOM || nSubCmd == ePvP::CS_PVP_LIST_OPEN_UI || nSubCmd == ePvP::CS_ENTER_LADDERCHANNEL || nSubCmd == ePvP::CS_PVP_CHANGECHANNEL))
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	if( bCheckPvPLobby && (nSubCmd == ePvP::CS_ROOMLIST || nSubCmd == ePvP::CS_JOINROOM || nSubCmd == ePvP::CS_CREATEROOM || nSubCmd == ePvP::CS_PVP_LIST_OPEN_UI || nSubCmd == ePvP::CS_ENTER_LADDERCHANNEL))
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		bCheckPvPLobby = false;
#endif
	// PvP로비가 아니라면 에러처리
	if( !m_pField || (bCheckPvPLobby && !m_pField->bIsPvPLobby()) )
	{
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	switch(nSubCmd)
	{
		// PvPRoom 생성
		case ePvP::CS_CREATEROOM:
		{
			CSPVP_CREATEROOM * pPacket = reinterpret_cast<CSPVP_CREATEROOM*>(pData);

			if(sizeof(CSPVP_CREATEROOM) - sizeof(pPacket->wszBuf) +((pPacket->cRoomNameLen + pPacket->cRoomPWLen) * sizeof(WCHAR)) != nLen)
				return ERROR_INVALIDPACKET;

			const short nRetCode = _RecvPvPCreateRoom( pPacket );
			if( nRetCode != ERROR_NONE )
			{
				SendPvPCreateRoom( nRetCode );
			}
			break;
		}
		// PvPRoom 변경
		case ePvP::CS_MODIFYROOM:
		{
			CSPVP_MODIFYROOM * pPacket = reinterpret_cast<CSPVP_MODIFYROOM*>(pData);

			if(sizeof(CSPVP_MODIFYROOM) - sizeof(pPacket->sCSPVP_CREATEROOM.wszBuf) + \
				((pPacket->sCSPVP_CREATEROOM.cRoomNameLen + pPacket->sCSPVP_CREATEROOM.cRoomPWLen) * sizeof(WCHAR)) != nLen)
				return ERROR_INVALIDPACKET;

			const short nRetCode = _RecvPvPModifyRoom( pPacket );

			if( nRetCode != ERROR_NONE )
			{
				SendPvPModifyRoom( nRetCode );
			}
			break;
		}
		// PvPRoom 나가기
		case ePvP::CS_LEAVEROOM:
		{
			if(nLen != 0)
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendPvPLeaveRoom( GetAccountDBID() );
			else
				_DANGER_POINT();
			break;
		}
		// PvPRoom 방장 변경
		case ePvP::CS_CHANGECAPTAIN:
		{
			CSPVP_CHANGECAPTAIN* pPacket = reinterpret_cast<CSPVP_CHANGECAPTAIN*>(pData);

			if(sizeof(CSPVP_CHANGECAPTAIN) != nLen)
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendPvPChangeCaptain( GetAccountDBID(), pPacket );
			else
				_DANGER_POINT();
			break;
		}
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		case ePvP::CS_PVP_LIST_OPEN_UI : 
		{
			// 레벨 제한 체크
			//RLKT DISABLED
		 //const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr(GlobalEnum::CHANNEL_ATT_PVP);
			//if( pChannelInfo )
			//{
				// 내 레벨을 확인
				if( GetLevel() <  10 )//pChannelInfo->nLimitLevel )	//RLKT			
					SendPvPListOpenUI(ERROR_PVP_LIST_LEVEL_LIMIT, 10);
				else
					SendPvPListOpenUI(ERROR_NONE, 10);
			//}
			break;
		}		
#endif
		// PvPRoom 방리스트 요청
		case ePvP::CS_ROOMLIST:
		{
			CSPVP_ROOMLIST* pPacket = reinterpret_cast<CSPVP_ROOMLIST*>(pData);

			if(sizeof(CSPVP_ROOMLIST) != nLen)
				return ERROR_INVALIDPACKET;

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
			pPacket->cChannelType = GetPvPChannelType();
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendPvPRoomList( GetAccountDBID(), GetChannelID(), pPacket );
			else
				SendPvPRoomList( ERROR_GENERIC_MASTERCON_NOT_FOUND );
			break;
		}
		case ePvP::CS_WAITUSERLIST:
		{
			CSPVP_WAITUSERLIST* pPacket = reinterpret_cast<CSPVP_WAITUSERLIST*>(pData);

			if( sizeof(CSPVP_WAITUSERLIST) != nLen )
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendPvPWaitUserList( GetAccountDBID(), GetChannelID(), pPacket );
			else
				SendPvPWaitUserList( ERROR_GENERIC_MASTERCON_NOT_FOUND );

			break;
		}
		// PvPRoom 들어가기
		case ePvP::CS_JOINROOM:
		{
			CSPVP_JOINROOM* pPacket = reinterpret_cast<CSPVP_JOINROOM*>(pData);

			if(sizeof(CSPVP_JOINROOM) - sizeof(pPacket->wszRoomPW) +(pPacket->cRoomPWLen * sizeof(WCHAR)) != nLen)
				return ERROR_INVALIDPACKET;
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
			if( m_pField->bIsPvPLobby() ) // 일반 PvP로비 일때만 검사..
			{			
				if (_CheckPvPGameModeInventory(pPacket->uiPvPIndex) == false)
					break;
			}
#endif //#if defined(PRE_ADD_PVP_VILLAGE_ACCESS) 
#if defined( PRE_PVP_GAMBLEROOM )				
			int nPvPLobbyChannelID = CDNPvPRoomManager::GetInstance().GetPvPLobbyChannelID();
			CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( nPvPLobbyChannelID, pPacket->uiPvPIndex );
			if( pPvPRoom)
			{
				int nRetCode = pPvPRoom->CheckGambleRoomJoin(this) ;
				if( nRetCode != ERROR_NONE )
				{
					SendPvPJoinRoom( nRetCode );
					break;
				}					
			}
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
			if( pPacket->uiPvPIndex > PvPCommon::Common::MaxRoomCount)
			{
				if( GetPvPIndex() > 0)
					break;

				TWorldPvPRoomDBData* pWorldPvPRoomData = CDNPvPRoomManager::GetInstance().GetWorldPvPRoom( pPacket->uiPvPIndex );
				int nWorldPvPRoomChannelID = CDNPvPRoomManager::GetInstance().GetWorldPvPRoomChannelID();
				if( pWorldPvPRoomData )
				{
					CDNPvPRoom*	pPvPRoom = CDNPvPRoomManager::GetInstance().GetPvPRoom( nWorldPvPRoomChannelID, pPacket->uiPvPIndex );
					if( g_pMasterConnection && pPvPRoom )
					{
						bool bLobby = false;
						if( GetField() && GetField()->bIsPvPLobby())
						{
							bLobby = true;
						}	

						UINT uiTeam = -1;
						if( pPacket->bIsObserver )
							uiTeam = PvPCommon::Team::Observer;
						else
						{
							if( !pPvPRoom->CheckLevel(GetLevel()) )
								SendPvPJoinRoom( ERROR_PVP_JOINROOM_LEVELLIMIT );
							if( pPvPRoom->GetWorldPvPRoomPw() )
							{
								int nRoomPw = _wtoi(pPacket->wszRoomPW);
								if( pPvPRoom->GetWorldPvPRoomPw() != nRoomPw )
								{
									SendPvPJoinRoom( ERROR_PVP_JOINROOM_INVALIDPW );
									break;
								}
							}							
						}

						SetPvPIndex(pPacket->uiPvPIndex);
						SendPvPJoinRoom(ERROR_NONE, pPvPRoom);
							
						g_pMasterConnection->SendWorldPvPJoinRoom( GetAccountDBID(), GetCharacterDBID(), pPacket->uiPvPIndex, pWorldPvPRoomData, uiTeam, bLobby );
					}
				}
				break;
			}
#endif
			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
			{
				g_pMasterConnection->SendPvPJoinRoom( GetAccountDBID(), GetLevel(), pPacket, this );
			}
			else
				SendPvPJoinRoom( ERROR_GENERIC_MASTERCON_NOT_FOUND );
			break;
		}
		// PvPRoom 레디
		case ePvP::CS_READY:
		{
			CSPVP_READY* pPacket = reinterpret_cast<CSPVP_READY*>(pData);

			if(sizeof(CSPVP_READY) != nLen)
				return ERROR_INVALIDPACKET;

			if( _CheckPvPGameModeInventory() == false )
				break;

			if( g_pMasterConnection )
				g_pMasterConnection->SendPvPReady( GetAccountDBID(), pPacket );
			else
				_DANGER_POINT();
			break;
		}
		// PvPRoom 시작
		case ePvP::CS_START:
		{
			CSPVP_START* pPacket= reinterpret_cast<CSPVP_START*>(pData);

			if(sizeof(CSPVP_START) != nLen)
				return ERROR_INVALIDPACKET;

			if( _CheckPvPGameModeInventory() == false )
				break;

			if( g_pMasterConnection )
				g_pMasterConnection->SendPvPStart( GetAccountDBID(), pPacket );
			else
				_DANGER_POINT();
			break;
		}
		// PvP방 랜덤 들어가기
		case ePvP::CS_RANDOMJOINROOM:
		{
			if(nLen != 0)
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				g_pMasterConnection->SendPvPRandomJoinRoom( GetAccountDBID(), GetLevel() );
			else
				SendPvPJoinRoom( ERROR_GENERIC_MASTERCON_NOT_FOUND );
			break;
		}
		// PvP 팀 변경
		case ePvP::CS_CHANGETEAM:
		{
			CSPVP_CHANGETEAM* pPacket = reinterpret_cast<CSPVP_CHANGETEAM*>(pData);

			if(sizeof(CSPVP_CHANGETEAM) != nLen)
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection )
				g_pMasterConnection->SendPvPChangeTeam( GetAccountDBID(), GetLevel(), pPacket );
			else
				_DANGER_POINT();
			break;
		}
		// 추방
		case ePvP::CS_BAN:
		{
			if(sizeof(CSPVP_BAN) != nLen)
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection )
				g_pMasterConnection->SendPvPBan( GetAccountDBID(), reinterpret_cast<CSPVP_BAN*>(pData) );
			else
				_DANGER_POINT();
			break;
		}

		//####################################################################################################
		// LadderSystem 관련 Protocol
		//####################################################################################################
		case ePvP::CS_ENTER_LADDERCHANNEL:
		{
			if(sizeof(LadderSystem::CS_ENTER_CHANNEL) != nLen )
				return ERROR_INVALIDPACKET;
			LadderSystem::CS_ENTER_CHANNEL* pPacket = reinterpret_cast<LadderSystem::CS_ENTER_CHANNEL*>(pData);
#if defined( PRE_FIX_TEAM_LADDER_3vs3 )
			switch( pPacket->MatchType )
			{
				case LadderSystem::MatchType::_1vs1:
				case LadderSystem::MatchType::_3vs3:
					break;
				default:
					return ERROR_INVALIDPACKET;
			}
#endif // #if defined( PRE_FIX_TEAM_LADDER_3vs3 )
			if( g_pMasterConnection && g_pMasterConnection->GetActive() == true )
				g_pMasterConnection->SendPvPLadderEnterChannel( GetAccountDBID(), pPacket->MatchType );
			else
				SendLadderEnterChannel( ERROR_GENERIC_MASTERCON_NOT_FOUND, pPacket->MatchType );
			break;
		}
		case ePvP::CS_LEAVE_LADDERCHANNEL:
		{
			if( nLen != 0 )
				return ERROR_INVALIDPACKET;
			LadderSystem::CManager::GetInstance().OnLeaveChannel( this );
			break;
		}
		case ePvP::CS_LADDER_MATCHING:
		{
			if(sizeof(LadderSystem::CS_LADDER_MATCHING) != nLen )
				return ERROR_INVALIDPACKET;
			LadderSystem::CS_LADDER_MATCHING* pPacket = reinterpret_cast<LadderSystem::CS_LADDER_MATCHING*>(pData);
			LadderSystem::CManager::GetInstance().OnMatching( this, pPacket->bIsCancel );
			break;
		}
		case ePvP::CS_LADDER_PLAYING_ROOMLIST:
		{
			if(sizeof(LadderSystem::CS_PLAYING_ROOMLIST) != nLen )
				return ERROR_INVALIDPACKET;
			LadderSystem::CS_PLAYING_ROOMLIST* pPacket = reinterpret_cast<LadderSystem::CS_PLAYING_ROOMLIST*>(pData);
			LadderSystem::CRoom* pRoom = LadderSystem::CManager::GetInstance().GetRoomPtr( GetCharacterName() );
			if( pRoom == NULL )
			{
				SendLadderPlayingRoomList( ERROR_INVALIDUSER_LADDERROOM );
				break;
			}
			LadderSystem::CManager::GetInstance().GetRoomRepositoryPtr()->SendPlayingRoomList( this, pRoom->GetMatchType(), pPacket->uiReqPage );
			break;
		}
		case ePvP::CS_LADDER_OBSERVER:
		{
			if(sizeof(LadderSystem::CS_OBSERVER) != nLen )
				return ERROR_INVALIDPACKET;
			int iRet = LadderSystem::CManager::GetInstance().OnEnterObserver( this, reinterpret_cast<LadderSystem::CS_OBSERVER*>(pData) );
			SendLadderObserver( iRet );
			
			break;
		}
		case ePvP::CS_LADDER_INVITE:
		{
			if( sizeof(LadderSystem::CS_INVITE) != nLen )
				return ERROR_INVALIDPACKET;
			if( g_pMasterConnection && g_pMasterConnection->GetActive() == true )
				g_pMasterConnection->SendPvPLadderInviteUser( GetAccountDBID(), reinterpret_cast<LadderSystem::CS_INVITE*>(pData)->wszCharName );
			else
				SendPvPLadderInviteUser( ERROR_GENERIC_MASTERCON_NOT_FOUND, reinterpret_cast<LadderSystem::CS_INVITE*>(pData)->wszCharName );
			break;
		}
		case ePvP::CS_LADDER_INVITE_CONFIRM:
		{
			if( sizeof(LadderSystem::CS_INVITE_CONFIRM) != nLen )
				return ERROR_INVALIDPACKET;
			if( g_pMasterConnection && g_pMasterConnection->GetActive() == true )
				g_pMasterConnection->SendPvPLadderInviteConfirm( GetAccountDBID(), GetCharacterName(), reinterpret_cast<LadderSystem::CS_INVITE_CONFIRM*>(pData) );
			else
				SendPvPLadderInviteConfirm( ERROR_GENERIC_MASTERCON_NOT_FOUND, reinterpret_cast<LadderSystem::CS_INVITE_CONFIRM*>(pData)->wszCharName  );
			break;
		}
		case ePvP::CS_LADDER_KICKOUT:
		{
			if( sizeof(LadderSystem::CS_KICKOUT) != nLen )
				return ERROR_INVALIDPACKET;
			int iRet = LadderSystem::CManager::GetInstance().OnKickOut( this, reinterpret_cast<LadderSystem::CS_KICKOUT*>(pData)->biCharacterDBID );
			SendPvPLadderKickOut( iRet );
			break;
		}
		case ePvP::CS_FATIGUE_OPTION:
			{				
				if(sizeof(CSPVP_FATIGUE_OPTION) != nLen )
					return ERROR_INVALIDPACKET;
#if defined(PRE_ADD_DWC)
				if(IsDWCCharacter())
					return ERROR_DWC_FAIL;
#endif
				CSPVP_FATIGUE_OPTION* pPacket = reinterpret_cast<CSPVP_FATIGUE_OPTION*>(pData);
				// 그냥 마스터에 전달 하면 땡.
				if( g_pMasterConnection && g_pMasterConnection->GetActive() )
					g_pMasterConnection->SendPvPFatigueOption( GetAccountDBID(), pPacket->bOption );
				break;
			}
		case ePvP::CS_SWAPMEMBERINDEX:
			{
				CSPvPTeamSwapMemberIndex * pPacket = (CSPvPTeamSwapMemberIndex*)pData;
				if(sizeof(CSPvPTeamSwapMemberIndex) - sizeof(pPacket->Index) +(sizeof(TSwapMemberIndex) * pPacket->cCount) != nLen)
					return ERROR_INVALIDPACKET;

				if(g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendPvPSwapMemberIndex(GetAccountDBID(), pPacket->cCount, pPacket->Index);
				else
					SendPvPMemberIndex(0, 0, NULL, ERROR_PVP_SWAPTEAM_MEMBERINDEX_FAIL);
				break;
			}

		case ePvP::CS_GUILDWAR_CHANGEMEMBER_GRADE:
			{
				CSPvPGuildWarChangeMemberGrade * pPacket = (CSPvPGuildWarChangeMemberGrade*)pData;

				if(sizeof(CSPvPGuildWarChangeMemberGrade) != nLen)
					return ERROR_INVALIDPACKET;

				if(g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendPvPChangeMemberGrade(GetAccountDBID(), pPacket->nType, pPacket->nSessionID, pPacket->bAsign);
				else
					SendPvPGuildWarMemberGrade(0, 0, ERROR_PVP_CHANGEMEMBERGRADE_FAIL);
				break;
			}
#if defined(_VILLAGESERVER)

#if defined(PRE_ADD_QUICK_PVP)
		case ePvP::CS_QUICKPVP_INVITE :
			{
				CSQuickPvPInvite* pPacket = (CSQuickPvPInvite*)pData;
				if(sizeof(CSQuickPvPInvite) != nLen )
					return ERROR_INVALIDPACKET;

				CDNUserSession *pUserSession = g_pUserSessionManager->FindUserSession(pPacket->nReceiverSessionID);
				if (!pUserSession){
					SendQuickPvPResult(ERROR_PVP_QUICK_USERNOTFOUND);
					return ERROR_NONE;
				}
#if defined(PRE_ADD_DWC)
				if( IsDWCCharacter() )
				{
					SendQuickPvPResult(ERROR_DWC_FAIL);
					return ERROR_NONE;
				}
				if( pUserSession->IsDWCCharacter() )
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_USERNOTFOUND);
					return ERROR_NONE;
				}
#endif
				const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr(GlobalEnum::CHANNEL_ATT_PVP);
				if( pChannelInfo )
				{
					// 내 레벨을 확인
					if( GetLevel() < pChannelInfo->nLimitLevel )
					{
						SendQuickPvPResult(ERROR_PVP_QUICK_MY_LEVEL);
						return ERROR_NONE;
					}
					// 상대방의 레벨을 확인
					if( pUserSession->GetLevel() < pChannelInfo->nLimitLevel )
					{
						SendQuickPvPResult(ERROR_PVP_QUICK_OTHER_LEVEL);
						return ERROR_NONE;
					}
				}
				else
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
					return ERROR_NONE;
				}			

				// 상태 검사 시작..
				if( !IsNoneWindowState() )
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
					return ERROR_NONE;
				}
				if( !pUserSession->IsNoneWindowState())
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_CANTINVITE);
					return ERROR_NONE;
				}				
				if( pUserSession->m_eUserState != STATE_NONE )
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_CANTINVITE);
					return ERROR_NONE;
				}
				//상대방의 게임옵션을 확인합니다.
				if (pUserSession->IsAcceptAbleOption(GetCharacterDBID(), GetAccountDBID(), _ACCEPTABEL_CHECKTYPE_QUICKPVPINVITE) == false)
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_CANTINVITE);
					return ERROR_NONE;
				}
				// 나랑 상대방이 파티 상태인지 검사
				if( pUserSession->GetPartyID() > 0 || GetPartyID() > 0)
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_CANTINVITE);
					return ERROR_NONE;
				}
				pUserSession->SetWindowState(WINDOW_ISACCEPT);
				pUserSession->SendQuickPvPInvite(GetSessionID());
			}
			break;
		case ePvP::CS_QUICKPVP_RESULT :
			{
				CSQuickPvPResult* pPacket = (CSQuickPvPResult*)pData;
				if(sizeof(CSQuickPvPResult) != nLen)
					return ERROR_INVALIDPACKET;
				IsWindowStateNoneSet(WINDOW_ISACCEPT);
				CDNUserSession *pUserSession = g_pUserSessionManager->FindUserSession(pPacket->nSenderSessionID);
				if (!pUserSession){
					SendQuickPvPResult(ERROR_PVP_QUICK_USERNOTFOUND);
					return ERROR_NONE;
				}

				// 상태 검사 시작..
				if( !IsNoneWindowState() )
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
					pUserSession->SendQuickPvPResult(ERROR_PVP_QUICK_DENY);
					return ERROR_NONE;
				}
				if( !pUserSession->IsNoneWindowState() )
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
					pUserSession->SendQuickPvPResult(ERROR_PVP_QUICK_DENY);
					return ERROR_NONE;
				}				
				if( pUserSession->m_eUserState != STATE_NONE )
				{
					SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
					pUserSession->SendQuickPvPResult(ERROR_PVP_QUICK_DENY);
					return ERROR_NONE;
				}
				if( pPacket->bAccept )
				{
					if( g_pMasterConnection && g_pMasterConnection->GetActive() )
						g_pMasterConnection->SendMakeQuickPvPRoom(pUserSession->GetAccountDBID(), GetAccountDBID());
					else
					{
						SendQuickPvPResult(ERROR_PVP_QUICK_FAIL);
						pUserSession->SendQuickPvPResult(ERROR_PVP_QUICK_DENY);
						return ERROR_NONE;
					}
				}
				else
					pUserSession->SendQuickPvPResult(ERROR_PVP_QUICK_DENY);
			}
			break;
#endif //#if defined(PRE_ADD_QUICK_PVP)

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		case ePvP::CS_PVP_CHANGECHANNEL:
			{
				if (sizeof(CSPVP_CHANGECHANNEL) != nLen)
					return ERROR_INVALIDPACKET;

				CSPVP_CHANGECHANNEL * pPacket = (CSPVP_CHANGECHANNEL*)pData;
				if (pPacket->cType >= PvPCommon::RoomType::max)
				{
					//에러처리 해야함
					_DANGER_POINT();
					SendPvPChangeChannelResult(static_cast<BYTE>(m_ePvPChannel), ERROR_PVP_CHANGELEVEL_FAIL);
					return ERROR_NONE;
				}

				if (g_pMasterConnection)
					g_pMasterConnection->SendPvPChangeChannel(GetAccountDBID(), pPacket->cType, GetPvPData()->cLevel, static_cast<BYTE>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Colosseum_Limit_Rank)));
				else
				{
					_DANGER_POINT();
					SendPvPChangeChannelResult(static_cast<BYTE>(m_ePvPChannel), ERROR_PVP_CHANGELEVEL_FAIL);
				}			
			}
			break;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		
#endif //#if defined(_VILLAGESERVER)

#if defined(PRE_ADD_PVP_TOURNAMENT)
		case ePvP::CS_PVP_SWAP_TOURNAMENT_INDEX:
			{
				if(sizeof(CSPvPSwapTournamentIndex) != nLen )
					return ERROR_INVALIDPACKET;

				CSPvPSwapTournamentIndex* pPacket = reinterpret_cast<CSPvPSwapTournamentIndex*>(pData);

				if(g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendPvPSwapTournamentIndex(GetAccountDBID(), pPacket->cSourceIndex, pPacket->cDestIndex);
				else
					SendPvPMemberIndex(0, 0, NULL, ERROR_PVP_SWAP_TOURNAMENT_INDEX_FAIL);
				break;
			}
			break;
#endif
		default:
		{
			return ERROR_UNKNOWN_HEADER;
		}
	}

	return ERROR_NONE;
}


int CDNUserSession::OnRecvFarmMessage(int nSubCmd, char * pData, int nLen)
{
	switch(nSubCmd)
	{
		case eFarm::CS_START:
		{
			if(g_pMasterConnection && g_pMasterConnection->GetActive() )
			{
				CSFarmEnter * pPacket = (CSFarmEnter*)pData;

#if defined( PRE_ADD_FARM_DOWNSCALE ) && defined( PRE_ADD_VIP_FARM )
				int iAttr = Farm::Attr::None;
				if( g_pFarm->GetFarmAttr( pPacket->iFarmDBID, iAttr ) == false )
				{
					_DANGER_POINT();
					break;
				}

				if( iAttr&Farm::Attr::Vip )
				{
					if( bIsFarmVip() == false )
					{
						SendFarmStart( ERROR_FARM_INVALID_VIP );
						return ERROR_NONE;
					}					
				}
#elif defined( PRE_ADD_VIP_FARM )
				Farm::Attr::eType Attr = Farm::Attr::None;
				if( g_pFarm->GetFarmAttr( pPacket->iFarmDBID, Attr ) == false )
				{
					_DANGER_POINT();
					break;
				}

				switch( Attr )
				{
					case Farm::Attr::Vip:
					{
						if( bIsFarmVip() == false )
						{
							SendFarmStart( ERROR_FARM_INVALID_VIP );
							return ERROR_NONE;
						}
						break;
					}
				}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE ) && defined( PRE_ADD_VIP_FARM )
				//rlkt 2016
				g_pMasterConnection->SendReqGameID( GameTaskType::Farm, REQINFO_TYPE_PVP_BREAKINTO, m_nAccountDBID, GetChannelID(), timeGetTime(), 15025, 0, CONVERT_TO_DUNGEONDIFFICULTY(0), true, NULL, pPacket->iFarmDBID );
		
				//g_pMasterConnection->SendReqGameID( GameTaskType::Farm, REQINFO_TYPE_FARM, m_nAccountDBID, GetChannelID(), timeGetTime(), 15025, 0, CONVERT_TO_DUNGEONDIFFICULTY(0), true, NULL, pPacket->iFarmDBID );
			}
			else
			{
				_DANGER_POINT();
			}

			break;
		}

		case eFarm::CS_FARMINFO:
		{
			ReqFarmInfo(false);
			break;
		}
	}

	return ERROR_NONE;
}



int CDNUserSession::OnRecvIsolateMessage(int nSubCmd, char * pData, int nLen)
{
	return CDNUserBase::OnRecvIsolateMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvAppellationMessage(int nSubCmd, char * pData, int nLen)
{
	switch( nSubCmd ) {
		case eAppellation::CS_SELECT_APPELLATION:
			{
				CSSelectAppellation *pPacket = (CSSelectAppellation *)pData;

				if(sizeof(CSSelectAppellation) != nLen)
					return ERROR_INVALIDPACKET;

				if( m_pAppellation->OnRecvSelectAppellation( pPacket->nArrayIndex, pPacket->nCoverArrayIndex ) == false )
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
#else
		case eAppellation::CS_COLLECTIONBOOK:
			{
				CSCollectionBook *pPacket = (CSCollectionBook*)pData;

				if(sizeof(CSCollectionBook) != nLen)
					return ERROR_INVALIDPACKET;

				m_pAppellation->OnRecvCollectionBook(pPacket->nCollectionID);
				return ERROR_NONE;
			}
			break;
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	}
	return ERROR_UNKNOWN_HEADER;
}

int CDNUserSession::OnRecvGameOptionMessage(int nSubCmd, char * pData, int nLen)
{
	return CDNUserBase::OnRecvGameOptionMessage(nSubCmd, pData, nLen);
}

int CDNUserSession::OnRecvRadioMessage(int nSubCmd, char * pData, int nLen)
{
	switch(nSubCmd)
	{
	case eRadio::CS_USERADIO:
		{
			CSUseRadio * pPacket = (CSUseRadio*)pData;

			if(sizeof(CSUseRadio) != nLen)
				return ERROR_INVALIDPACKET;

			if(GetPartyID() <= 0)
			{	//파티상태에서만 사용 가능 하데요.
				_DANGER_POINT();
				return ERROR_NONE;
			}

			g_pPartyManager->SendPartyUseRadio(GetPartyID(), GetSessionID(), pPacket->nID);
			return ERROR_NONE;
		}
		break;
	}
	return ERROR_UNKNOWN_HEADER;
}

#ifdef _USE_VOICECHAT
int CDNUserSession::OnRecvVoiceChatMessage(int nSubCmd, char * pData, int nLen)
{
	switch(nSubCmd)
	{
	case eVoiceChat::CS_VOICEAVAILABLE:
		{
			CSVoiceChatAvailable * pPacket = (CSVoiceChatAvailable*)pData;

			if(sizeof(CSVoiceChatAvailable) != nLen)
				return ERROR_INVALIDPACKET;

			m_bVoiceAvailable = pPacket->cAvailable > 0 ? true : false;
			
			CDNParty * pParty = NULL;
			if(GetPartyID() > 0)
				pParty = g_pPartyManager->GetParty(GetPartyID());
			
			if(m_bVoiceAvailable)
			{
				if(m_nVoiceChannelID > 0 || pParty == NULL) return ERROR_NONE;
				if(pParty->m_nVoiceChannelID > 0)
					JoinVoiceChannel(pParty->m_nVoiceChannelID, GetIp(), &GetTargetPos(), (int)m_nVoiceRotate);
			}
			else
				LeaveVoiceChannel();

			if(pParty)
				pParty->SendRefreshVoiceInfo();
			return ERROR_NONE;
		}
		break;

	case eVoiceChat::CS_VOICEMUTE:
		{
			CSVoiceMute * pPacket = (CSVoiceMute*)pData;

			if(sizeof(CSVoiceMute) != nLen)
				return ERROR_INVALIDPACKET;

			if(GetPartyID() <= 0 || m_nVoiceChannelID <= 0)
			{
				_DANGER_POINT();
				return ERROR_GENERIC_INVALIDREQUEST;
			}
			
			CDNUserSession * pSession = g_pUserSessionManager->FindUserSession(pPacket->nSessionID);
			if(pSession)
				VoiceMuteOnetoOne(pSession->GetAccountDBID(), pPacket->cMute == 0 ? false : true);
			else
				_DANGER_POINT();
			return ERROR_NONE;
		}
		break;

	case eVoiceChat::CS_VOICECOMPLAINTREQ:
		{
			CSVoiceComplaintReq * pPacket = (CSVoiceComplaintReq*)pData;

			if(sizeof(CSVoiceComplaintReq) != nLen)
				return ERROR_INVALIDPACKET;

			if(GetPartyID() <= 0 || m_nVoiceChannelID <= 0)
			{
				_DANGER_POINT();
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			CDNUserSession * pSession = g_pUserSessionManager->FindUserSession(pPacket->nSessionID);
			if(pSession)
				VoiceComplaintRequest(pSession->GetAccountDBID(), pPacket->szCategory, pPacket->szSubject, pPacket->szMsg);
			else
				_DANGER_POINT();
			return ERROR_NONE;
		}
		break;
	}
	return ERROR_UNKNOWN_HEADER;
}
#endif

int CDNUserSession::OnRecvChatRoomMessage( int nSubCmd, char * pData, int nLen )
{
	switch( nSubCmd )
	{
	case eChatRoom::CS_CREATECHATROOM:
		{
			CSCreateChatRoom * pCreateChatRoom = (CSCreateChatRoom *)pData;
			if(sizeof(CSCreateChatRoom) != nLen)	return ERROR_INVALIDPACKET;


			// #23581 채널이동요청 중 채팅방 생성을 막는다.
			if( m_eUserState != STATE_NONE )
				return ERROR_NONE;

			TChatRoomView ChatRoomView;

			// 0. 다른 채팅방에 참여중인가?
			if( m_nChatRoomID > 0 )
			{
				SendChatRoomCreateChatRoom( ERROR_CHATROOM_ANOTHERROOM, ChatRoomView );
				return ERROR_NONE;
			}

			int nChatRoomID = 0;
			// 1. 방 생성
			nChatRoomID = g_pChatRoomManager->CreateChatRoom( GetAccountDBID(), pCreateChatRoom->wszName, pCreateChatRoom->nRoomType,
																				pCreateChatRoom->nRoomAllow, pCreateChatRoom->wszPassword,
																				pCreateChatRoom->PRLine1, pCreateChatRoom->PRLine2, pCreateChatRoom->PRLine3 );

			if( nChatRoomID == 0 )
			{
				// 방생성 실패 - ERROR_CHATROOM_FAILEDMAKEROOM
				SendChatRoomCreateChatRoom( ERROR_CHATROOM_FAILEDMAKEROOM, ChatRoomView );
				return ERROR_NONE;
			}

			// 방찾기
			CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( nChatRoomID );
			if( pChatRoom == NULL )
			{
				// 방생성 실패 - ERROR_CHATROOM_FAILEDMAKEROOM
				SendChatRoomCreateChatRoom( ERROR_CHATROOM_FAILEDMAKEROOM, ChatRoomView );
				return ERROR_NONE;
			}

			// 방 디스플레이 정보
			ChatRoomView.Set( pChatRoom->GetChatRoomID(), pChatRoom->GetChatRoomName(), pChatRoom->GetChatRoomType(), pChatRoom->UsePassword(),
							  pChatRoom->GetChatRoomPRLine1(), pChatRoom->GetChatRoomPRLine2(), pChatRoom->GetChatRoomPRLine3() );

			// 2. 리더 입장
			int nRet = ERROR_NONE;
			nRet = pChatRoom->EnterRoom( GetAccountDBID(), pCreateChatRoom->wszPassword, true );

			if( nRet != ERROR_NONE )
			{
				// 리더가 입장을 못하였므로 방을 파괴한다.
				if( pChatRoom->GetUserCount() <= 0 )
				{
					g_pChatRoomManager->DestroyChatRoom( pChatRoom->GetChatRoomID() );
				}
			}

			// 결과전송 - 방을 생성한 캐릭터에게 보내진다.
			SendChatRoomCreateChatRoom( nRet, ChatRoomView );

			// 주변 캐릭터에게 전송
			BroadcastingChatRoom( eChatRoom::SC_CHATROOMVIEW, pChatRoom->GetChatRoomID() );

			return ERROR_NONE;
		}
		break;
	case eChatRoom::CS_ENTERCHATROOM:
		{
			CSChatRoomEnterRoom * pEnterRoom = (CSChatRoomEnterRoom *)pData;
			if(sizeof(CSChatRoomEnterRoom) != nLen)	return ERROR_INVALIDPACKET;

			int nRet = ERROR_NONE;
			int UserSessionIDs[CHATROOMMAX];
			int nUserCount = 0;
			int LeaderSID = 0;
			memset( UserSessionIDs, 0, sizeof(UserSessionIDs) );

			// 방찾기
			CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( pEnterRoom->nChatRoomID );
			if( pChatRoom == NULL )
			{
				SendChatRoomEnterRoom( ERROR_CHATROOM_INVALIDCHATROOM, LeaderSID, UserSessionIDs, nUserCount );
				return ERROR_NONE;
			}

			// 일반 유저 입장
			nRet = pChatRoom->EnterRoom( GetAccountDBID(), pEnterRoom->wszPassword, false );
			if( nRet == ERROR_NONE )
			{
				// 방장의 세션 ID
				if( pChatRoom->GetLeaderAID() > 0 )
				{
					CDNUserSession * pLeaderSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pChatRoom->GetLeaderAID() );
					if( pLeaderSession )	LeaderSID = pLeaderSession->GetSessionID();
				}

				// 참여자들의 세션 ID
				for( int i=0; i<CHATROOMMAX; i++ )
				{
					if( pChatRoom->GetMemberAIDFromIndex(i) <= 0 )	 continue;
					CDNUserSession * pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( pChatRoom->GetMemberAIDFromIndex(i) );
					if( pUserSession == NULL )						 continue;

					UserSessionIDs[ nUserCount ] = pUserSession->GetSessionID();
					nUserCount++;
				}
			}

			// 결과전송
			SendChatRoomEnterRoom( nRet, LeaderSID, UserSessionIDs, nUserCount );

			if( nRet == ERROR_NONE )
			{
				// 주변 캐릭터에게 전송
				BroadcastingChatRoom( eChatRoom::SC_ENTERUSERCHATROOM, pChatRoom->GetChatRoomID() );
				// 프로필 전송
				if( pChatRoom->GetChatRoomType() == CHATROOMTYPE_PR && GetProfile()->bOpenPublic )
					pChatRoom->SendProfileToAll( GetSessionID(), *GetProfile() );
			}

			return ERROR_NONE;
		}
		break;
	case eChatRoom::CS_CHANGEROOMOPTION:
		{
			CSChatRoomChangeRoomOption * pChangeOption = (CSChatRoomChangeRoomOption*)pData;
			if(sizeof(CSChatRoomChangeRoomOption) != nLen)	return ERROR_INVALIDPACKET;

			// 방찾기
			CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( m_nChatRoomID );
			if( pChatRoom == NULL )
			{
				SendChatRoomChangeRoomOption( ERROR_CHATROOM_INVALIDCHATROOM, pChangeOption->ChatRoomView );
				return ERROR_NONE;
			}

			// 방장이 아니면 방옵션을 바꿀 수 없다.
			if( pChatRoom->IsLeader( GetAccountDBID() ) == false )
			{
				SendChatRoomChangeRoomOption( ERROR_CHATROOM_ONLYLEADER, pChangeOption->ChatRoomView );
				return ERROR_NONE;
			}

			pChatRoom->ChangeRoomOption( pChangeOption->ChatRoomView.m_wszChatRoomName, pChangeOption->ChatRoomView.m_nChatRoomType, 
										 pChangeOption->nRoomAllow, pChangeOption->wszPassword,
										 pChangeOption->ChatRoomView.m_wszChatRoomPRLine1,
										 pChangeOption->ChatRoomView.m_wszChatRoomPRLine2,
										 pChangeOption->ChatRoomView.m_wszChatRoomPRLine3 );

			TChatRoomView RoomView;
			RoomView.Set( pChatRoom->GetChatRoomID(), pChatRoom->GetChatRoomName(), pChatRoom->GetChatRoomType(), pChatRoom->UsePassword(),
						  pChatRoom->GetChatRoomPRLine1(), pChatRoom->GetChatRoomPRLine2(), pChatRoom->GetChatRoomPRLine3() );

			SendChatRoomChangeRoomOption( ERROR_NONE, RoomView );

			// 주변 캐릭터에게 전송
			BroadcastingChatRoom( eChatRoom::SC_CHATROOMVIEW, pChatRoom->GetChatRoomID() );

			return ERROR_NONE;
		}
		break;
	case eChatRoom::CS_KICKUSER:
		{
			CSChatRoomKickUser * pKickUser = (CSChatRoomKickUser *)pData;
			if(sizeof(CSChatRoomKickUser) != nLen)	return ERROR_INVALIDPACKET;

			int nRet = ERROR_NONE;

			// 방찾기
			CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( m_nChatRoomID );
			if( pChatRoom == NULL )
			{
				SendChatRoomKickUser( ERROR_CHATROOM_INVALIDCHATROOM, pKickUser->nKickUserSessionID	);
				return ERROR_NONE;
			}

			// 방장이 아니면 강퇴명령을 사용할 수 없다.
			if( pChatRoom->IsLeader( GetAccountDBID() ) == false )
			{
				SendChatRoomKickUser( ERROR_CHATROOM_ONLYLEADER, pKickUser->nKickUserSessionID	);
				return ERROR_NONE;
			}

			// 자기 자신은 강퇴 시킬수 없다.
			if( GetSessionID() == pKickUser->nKickUserSessionID )
			{
				return ERROR_NONE;
			}

			CDNUserSession *pUserSession = g_pUserSessionManager->FindUserSession( pKickUser->nKickUserSessionID );
			if( pUserSession == NULL )
			{
				return ERROR_NONE;
			}

			// 유저 삭제
			nRet = pChatRoom->LeaveUser( pUserSession->GetAccountDBID(), pKickUser->nKickReason );
			if( nRet == ERROR_NONE )
			{
				// 강퇴된 캐릭터에게 메시지 전달
				pUserSession->SendChatRoomLeaveUser( pUserSession->GetSessionID(), pKickUser->nKickReason );

				// 주변 캐릭터에게 전송
				pUserSession->BroadcastingChatRoom( eChatRoom::SC_LEAVEUSER, -pKickUser->nKickReason );
			}

			return ERROR_NONE;
		}
		break;

	case eChatRoom::CS_LEAVEROOM:
		{
			if(0 != nLen)	return ERROR_INVALIDPACKET;

			int nRet = ERROR_NONE;

			CDNChatRoom * pChatRoom = g_pChatRoomManager->GetChatRoom( m_nChatRoomID );
			if( pChatRoom == NULL )
			{
				return ERROR_NONE;
			}

			if( pChatRoom->IsLeader(GetAccountDBID()) )
			{
				// 리더가 방에서 나가는 경우 방이 삭제된다.
				while( pChatRoom->GetUserCount() > 0 )
				{
					UINT UserAID = pChatRoom->GetMemberAIDFromIndex( 0 );
					if( UserAID <= 0 )	break;	// 무한루프 방지를 위해 루프를 탈출하도록 한다.

					CDNUserSession * pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( UserAID );
					if( pUserSession == NULL )	break;	// 무한루프 방지를 위해 루프를 탈출하도록 한다.

					nRet = pChatRoom->LeaveUser( pUserSession->GetAccountDBID(), CHATROOMLEAVE_DESTROYROOM );
					if( nRet == ERROR_NONE )
					{
						// 방에서 나가게 되는 캐릭터와 주변 캐릭터에게 메시지를 전송한다.
						if( pChatRoom->IsLeader( GetAccountDBID() ) )
						{
							TChatRoomView RoomView;
							RoomView.Set( 0, L"", CHATROOMTYPE_NONE, false, L"", L"", L"" );
							pUserSession->SendChatRoomLeaveUser( pUserSession->GetSessionID(), CHATROOMLEAVE_DESTROYROOM );
							pUserSession->BroadcastingChatRoom( eChatRoom::SC_CHATROOMVIEW, 0 );	// 음수 파라미터 전송
						}
						else
						{
							pUserSession->SendChatRoomLeaveUser( pUserSession->GetSessionID(), CHATROOMLEAVE_DESTROYROOM );
							pUserSession->BroadcastingChatRoom( eChatRoom::SC_LEAVEUSER, -CHATROOMLEAVE_DESTROYROOM );	// 음수 파라미터 전송
						}
					}
				}
				// 방폭~!
				g_pChatRoomManager->DestroyChatRoom( pChatRoom->GetChatRoomID() );
			}
			else
			{
				// 유저가 방에서 나감
				nRet = pChatRoom->LeaveUser( GetAccountDBID(), CHATROOMLEAVE_LEAVE );
				if( nRet == ERROR_NONE )
				{
					// 방을 나간 캐릭터에게 메시지 전달
					SendChatRoomLeaveUser( GetSessionID(), CHATROOMLEAVE_LEAVE );

					// 주변 캐릭터에게 전송
					BroadcastingChatRoom( eChatRoom::SC_LEAVEUSER, -CHATROOMLEAVE_LEAVE );	// 음수 파라미터 전송
				}
			}

			return ERROR_NONE;
		}
		break;
	}

	return ERROR_UNKNOWN_HEADER;
}

int CDNUserSession::OnRecvMasterSystemMessage( int nSubCmd, char* pData, int nLen )
{
	switch( nSubCmd )
	{
		case eMasterSystem::CS_MASTERLIST:
		{
			if( sizeof(MasterSystem::CSMasterList) != nLen )
				return ERROR_INVALIDPACKET;

			// 스승목록 요청할 수 있는 조건 체크( 제자레벨,최대스승수 )
			if( GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_PupilMaxLevel )) || m_MasterSystemData.SimpleInfo.iMasterCount >= MasterSystem::Max::MasterCount )
			{
				SendMasterSystemMasterList( ERROR_GENERIC_INVALIDREQUEST, std::vector<TMasterInfo>() );
				return ERROR_NONE;
			}

			MasterSystem::CSMasterList* pPacket = reinterpret_cast<MasterSystem::CSMasterList*>(pData);
			MasterSystem::CCacheRepository::GetInstance().GetMasterList( this, pPacket->uiPage, pPacket->cJob, pPacket->cGender );
			return ERROR_NONE;
		}
		case eMasterSystem::CS_INTRODUCTION_ONOFF:
		{
			MasterSystem::CSIntroduction* pPacket = reinterpret_cast<MasterSystem::CSIntroduction*>(pData);

			int iNeedSize = pPacket->bRegister ? sizeof(MasterSystem::CSIntroduction) : sizeof(MasterSystem::CSIntroduction)-sizeof(pPacket->wszSelfIntroduction);
			if( iNeedSize != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_MasterMinLevel )) )
			{
				SendMasterSystemIntroductionOnOff( ERROR_GENERIC_INVALIDREQUEST, pPacket->bRegister, NULL );
				return ERROR_NONE;
			}

			if( CheckDBConnection() == false )
			{
				SendMasterSystemIntroductionOnOff( ERROR_GENERIC_DBCON_NOT_FOUND, pPacket->bRegister, NULL );
				return ERROR_NONE;
			}

			if( pPacket->bRegister )
			{
				size_t len = wcslen(pPacket->wszSelfIntroduction);
				if( len == 0 || len >= MasterSystem::Max::SelfIntrotuctionLen )
				{
					SendMasterSystemIntroductionOnOff( ERROR_GENERIC_INVALIDREQUEST, pPacket->bRegister, NULL );
					return ERROR_NONE;
				}
				GetDBConnection()->QueryRegisterMasterCharacter( this, pPacket->wszSelfIntroduction );
			}
			else
			{
				GetDBConnection()->QueryRegisterCancelMasterCharacter( this );
			}
			return ERROR_NONE;
		}
		case eMasterSystem::CS_MASTER_APPLICATION:
		{
			MasterSystem::CSMasterApplication* pPacket = reinterpret_cast<MasterSystem::CSMasterApplication*>(pData);

			if( sizeof(MasterSystem::CSMasterApplication) != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_PupilMaxLevel )) )
			{
				SendMasterSystemMasterApplication( ERROR_GENERIC_INVALIDREQUEST );
				return ERROR_NONE;
			}

			if( bIsMasterApplicationPenalty() )
			{
				SendMasterSystemMasterApplication( ERROR_MASTERSYSTEM_MASTERAPPLICATION_PENALTY );
			}
			else
			{
				if( g_pMasterConnection && g_pMasterConnection->GetActive() )
				{
					g_pMasterConnection->SendCheckMasterApplication( pPacket->biCharacterDBID, GetCharacterDBID() );
				}
				else
				{
					SendMasterSystemMasterApplication( ERROR_GENERIC_MASTERCON_NOT_FOUND );
				}
			}

			return ERROR_NONE;
		}
		case eMasterSystem::CS_JOIN:
		{
			MasterSystem::CSJoin* pPacket = reinterpret_cast<MasterSystem::CSJoin*>(pData);

			if( sizeof(MasterSystem::CSJoin) != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_PupilMaxLevel )) )
			{
				SendMasterSystemJoin( ERROR_GENERIC_INVALIDREQUEST, false, false, true );
				return ERROR_NONE;
			}

			if( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::LimitLevel_MasterSystem )) )
			{
				SendMasterSystemJoin( ERROR_GENERIC_LEVELLIMIT, false, false, true );
				return ERROR_NONE;
			}

			if( CheckDBConnection() == false )
			{
				SendMasterSystemJoin( ERROR_GENERIC_DBCON_NOT_FOUND, false, false, true );
				return ERROR_NONE;
			}
			// 우선 나랑 같은 빌리지에 있는지 찾아보자.
			CDNUserSession* pMasterSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszMasterCharName );
			if( !pMasterSession  )
			{
				// 마스터 한테 보내기..
				if( g_pMasterConnection == NULL || g_pMasterConnection->GetActive() == false )
				{
					SendMasterSystemJoin( ERROR_GENERIC_MASTERCON_NOT_FOUND, false, false, true );
					return ERROR_NONE;
				}
				g_pMasterConnection->SendMasterSystemJoinConfirm(pPacket->biMasterCharacterDBID, GetLevel(), GetUserJob(), GetCharacterName());
			}
			else
			{
				// 스승이 캐시샵에 있는지 검사
				if( !pMasterSession->IsNoneWindowState() )
				{
					SendMasterSystemJoin( ERROR_MASTERSYSTEM_CANT_JOINSTATE, false, false, true );
					return ERROR_NONE;
				}
				// 스승한테 메세지 보내기.
				pMasterSession->SendMasterSystemJoinComfirm(GetLevel(), GetUserJob(), GetCharacterName());
			}
			return ERROR_NONE;
		}
		case eMasterSystem::CS_JOIN_CONFIRM : 
		{
			MasterSystem::CSJoinComfirm* pPacket = reinterpret_cast<MasterSystem::CSJoinComfirm*>(pData);
			// 우선 나랑 같은 빌리지에 있는지 찾아보자.
			CDNUserSession* pPupilSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszPupilCharName);
			if( !pPupilSession  )
			{
				// 마스터 한테 보내기..
				if( g_pMasterConnection == NULL || g_pMasterConnection->GetActive() == false )									
					return ERROR_NONE;
				g_pMasterConnection->SendMasterSystemJoinConfirmResult(ERROR_NONE, pPacket->bIsAccept, GetCharacterDBID(), pPacket->wszPupilCharName);
			}
			else
			{
				if( pPacket->bIsAccept )
				{
#if defined( _FINAL_BUILD )
					GetDBConnection()->QueryJoinMasterSystem( this, GetCharacterDBID(), pPupilSession->GetCharacterDBID(), false, DBDNWorldDef::TransactorCode::Master );
#else
					GetDBConnection()->QueryJoinMasterSystem( this, GetCharacterDBID(), pPupilSession->GetCharacterDBID(), false, DBDNWorldDef::TransactorCode::Master, m_bIsMasterSystemSkipDate );
#endif // #if defined( _FINAL_BUILD )
				}
				else
					pPupilSession->SendMasterSystemJoin( ERROR_MASTERSYSTEM_JOIN_DENY, false, false, true );
			}
			return ERROR_NONE;
		}		
		case eMasterSystem::CS_CLASSMATEINFO:
		{
			MasterSystem::CSClassmateInfo* pPacket = reinterpret_cast<MasterSystem::CSClassmateInfo*>(pData);

			if( sizeof(MasterSystem::CSClassmateInfo) != nLen )
				return ERROR_INVALIDPACKET;

			if( CheckDBConnection() == false )
			{
				SendMasterSystemClassmateInfo( ERROR_GENERIC_DBCON_NOT_FOUND );
				return ERROR_NONE;
			}

			MasterSystem::CCacheRepository::GetInstance().GetClassmateInfo( this, pPacket->biClassmateCharacterDBID, true );
			return ERROR_NONE;
		}
		case eMasterSystem::CS_MYMASTERINFO:
		{
			MasterSystem::CSMyMasterInfo* pPacket = reinterpret_cast<MasterSystem::CSMyMasterInfo*>(pData);

			if( sizeof(MasterSystem::CSMyMasterInfo) != nLen )
				return ERROR_INVALIDPACKET;

			MasterSystem::CCacheRepository::GetInstance().GetMyMasterInfo( this, GetCharacterDBID(), pPacket->biMasterCharacterDBID, true );
			return ERROR_NONE;
		}
		case eMasterSystem::CS_LEAVE:
		{
			MasterSystem::CSLeave* pPacket = reinterpret_cast<MasterSystem::CSLeave*>(pData);

			if( sizeof(MasterSystem::CSLeave) != nLen )
				return ERROR_INVALIDPACKET;

			if( g_pMasterConnection == NULL || g_pMasterConnection->GetActive() == false )
			{
				SendMasterSystemLeave( ERROR_GENERIC_MASTERCON_NOT_FOUND );
				return ERROR_NONE;
			}

			g_pMasterConnection->SendMasterSystemCheckLeave( GetAccountDBID(), pPacket->biDestCharacterDBID, pPacket->bIsMaster );
			return ERROR_NONE;
		}
		case eMasterSystem::CS_INVITE_PUPIL:
		{
			MasterSystem::CSInvitePupil* pPacket = reinterpret_cast<MasterSystem::CSInvitePupil*>(pData);

			if( sizeof(MasterSystem::CSInvitePupil) != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_MasterMinLevel )) || m_MasterSystemData.SimpleInfo.iPupilCount >= MasterSystem::Max::PupilCount || m_MasterSystemData.SimpleInfo.iMasterCount > 0 )
			{
				SendMasterSystemInvitePupil( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszCharName );
				return ERROR_NONE;
			}

			CDNUserSession* pPupilSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszCharName );
			if( pPupilSession == NULL )
			{
				SendMasterSystemInvitePupil( ERROR_GENERIC_USER_NOT_FOUND, pPacket->wszCharName );
				return ERROR_NONE;
			}

			if( pPupilSession->GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::LimitLevel_MasterSystem ))  )
			{
				SendMasterSystemInvitePupil( ERROR_GENERIC_LEVELLIMIT, pPacket->wszCharName );
				return ERROR_NONE;
			}

			// 같은 곳에 있는지 확인
			if( GetMapIndex() != pPupilSession->GetMapIndex() || GetChannelID() != pPupilSession->GetChannelID() )
			{
				SendMasterSystemInvitePupil( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszCharName );
				return ERROR_NONE;
			}

			// 제자가 캐시샵에 있는지 검사
			if( !pPupilSession->IsNoneWindowState() )
			{
				SendMasterSystemInvitePupil( ERROR_MASTERSYSTEM_CANT_JOINSTATE, pPacket->wszCharName );
				return ERROR_NONE;
			}

			if( pPupilSession->GetMasterSystemData()->SimpleInfo.iMasterCount >= MasterSystem::Max::MasterCount )
			{
				SendMasterSystemInvitePupil( ERROR_MASTERSYSTEM_CANT_JOINSTATE, pPacket->wszCharName );
				return ERROR_NONE;
			}
#if defined(PRE_FIX_MASTERSYSTEM_CHECK_PUPILBLOCKDATE)
			if ( pPupilSession->bIsMasterApplicationPenalty() )
			{
				SendMasterSystemInvitePupil( ERROR_MASTERSYSTEM_CANT_JOINSTATE, pPacket->wszCharName );
				return ERROR_NONE;
			}
#endif	// #if defined(PRE_FIX_MASTERSYSTEM_CHECK_PUPILBLOCKDATE)

			SendMasterSystemInvitePupil( ERROR_NONE, pPacket->wszCharName );
			pPupilSession->SendMasterSystemInvitePupilConfirm( ERROR_NONE, GetCharacterName() );
			//제자 상태변경			
			pPupilSession->SetWindowState(WINDOW_ISACCEPT);
			return ERROR_NONE;
		}
		case eMasterSystem::CS_INVITE_PUPIL_CONFIRM:
		{
			MasterSystem::CSInvitePupilConfirm* pPacket = reinterpret_cast<MasterSystem::CSInvitePupilConfirm*>(pData);

			if( sizeof(MasterSystem::CSInvitePupilConfirm) != nLen )
				return ERROR_INVALIDPACKET;
			IsWindowStateNoneSet(WINDOW_ISACCEPT);
			CDNUserSession* pMasterSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszMasterCharName );
			if( pMasterSession == NULL )
			{
				SendMasterSystemInvitePupilConfirm( ERROR_GENERIC_USER_NOT_FOUND, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}
			// 체크
			if( GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_PupilMaxLevel )) )
			{
				SendMasterSystemInvitePupilConfirm( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			if( CheckDBConnection() == false )
			{
				SendMasterSystemInvitePupilConfirm( ERROR_GENERIC_DBCON_NOT_FOUND, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			if( pPacket->bIsAccept == false )
			{
				pMasterSession->SendMasterSystemInvitePupil( ERROR_MASTERSYSTEM_JOIN_DENY, GetCharacterName() );
				return ERROR_NONE;
			}

#if defined( _FINAL_BUILD )
			GetDBConnection()->QueryJoinMasterSystem( this, pMasterSession->GetCharacterDBID(), GetCharacterDBID(), true, DBDNWorldDef::TransactorCode::Pupil );
#else
			GetDBConnection()->QueryJoinMasterSystem( this, pMasterSession->GetCharacterDBID(), GetCharacterDBID(), true, DBDNWorldDef::TransactorCode::Pupil, m_bIsMasterSystemSkipDate );
#endif // #if defined( _FINAL_BUILD )
			return ERROR_NONE;
		}
		case eMasterSystem::CS_JOIN_DIRECT:
		{
			MasterSystem::CSJoinDirect* pPacket = reinterpret_cast<MasterSystem::CSJoinDirect*>(pData);

			if( sizeof(MasterSystem::CSJoinDirect) != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_PupilMaxLevel )) || m_MasterSystemData.SimpleInfo.iMasterCount >= MasterSystem::Max::MasterCount )
			{
				SendMasterSystemJoinDirect( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			if( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::LimitLevel_MasterSystem ))  )
			{
				SendMasterSystemJoinDirect( ERROR_GENERIC_LEVELLIMIT, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			CDNUserSession* pMasterSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszMasterCharName );
			if( pMasterSession == NULL )
			{
				SendMasterSystemJoinDirect( ERROR_GENERIC_USER_NOT_FOUND, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			// 같은 곳에 있는지 확인
			if( GetMapIndex() != pMasterSession->GetMapIndex() || GetChannelID() != pMasterSession->GetChannelID() )
			{
				SendMasterSystemJoinDirect( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			// 스승이 캐시샵에 있는지 검사
			if( !pMasterSession->IsNoneWindowState() )
			{
				SendMasterSystemJoinDirect( ERROR_MASTERSYSTEM_CANT_JOINSTATE, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			if( pMasterSession->GetMasterSystemData()->SimpleInfo.iPupilCount >= MasterSystem::Max::PupilCount )
			{
				SendMasterSystemJoinDirect( ERROR_MASTERSYSTEM_CANT_JOINSTATE, pPacket->wszMasterCharName );
				return ERROR_NONE;
			}

			SendMasterSystemJoinDirect( ERROR_NONE, pPacket->wszMasterCharName );
			pMasterSession->SendMasterSystemJoinDirectConfirm( ERROR_NONE, GetCharacterName() );
			//스승 상태변경			
			pMasterSession->SetWindowState(WINDOW_ISACCEPT);
			return ERROR_NONE;
		}
		case eMasterSystem::CS_JOIN_DIRECT_CONFIRM:
		{
			MasterSystem::CSJoinDirectConfirm* pPacket = reinterpret_cast<MasterSystem::CSJoinDirectConfirm*>(pData);

			if( sizeof(MasterSystem::CSJoinDirectConfirm) != nLen )
				return ERROR_INVALIDPACKET;
			IsWindowStateNoneSet(WINDOW_ISACCEPT);
			CDNUserSession* pPupilSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszPupilCharName );
			if( pPupilSession == NULL )
			{
				SendMasterSystemInvitePupilConfirm( ERROR_GENERIC_USER_NOT_FOUND, pPacket->wszPupilCharName );
				return ERROR_NONE;
			}

			// 체크
			if( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_MasterMinLevel )) )
			{
				SendMasterSystemInvitePupilConfirm( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszPupilCharName );
				return ERROR_NONE;
			}

			if( CheckDBConnection() == false )
			{
				SendMasterSystemInvitePupilConfirm( ERROR_GENERIC_DBCON_NOT_FOUND, pPacket->wszPupilCharName );
				return ERROR_NONE;
			}

			// 거절
			if( pPacket->bIsAccept == false )
			{
				pPupilSession->SendMasterSystemJoinDirect( ERROR_MASTERSYSTEM_JOIN_DENY, GetCharacterName() );
				return ERROR_NONE;
			}

#if defined( _FINAL_BUILD )
			GetDBConnection()->QueryJoinMasterSystem( this, GetCharacterDBID(), pPupilSession->GetCharacterDBID(), true, DBDNWorldDef::TransactorCode::Master );
#else
			GetDBConnection()->QueryJoinMasterSystem( this, GetCharacterDBID(), pPupilSession->GetCharacterDBID(), true, DBDNWorldDef::TransactorCode::Master, m_bIsMasterSystemSkipDate );
#endif // #if defined( _FINAL_BUILD )
			return ERROR_NONE;
		}
		case eMasterSystem::CS_RECALL_MASTER:
		{
			MasterSystem::CSRecallMaster* pPacket = reinterpret_cast<MasterSystem::CSRecallMaster*>(pData);

			if( sizeof(MasterSystem::CSRecallMaster) != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_PupilMaxLevel )) || GetMasterSystemData()->SimpleInfo.iMasterCount <= 0 )
			{
				SendMasterSystemRecallMaster( ERROR_GENERIC_INVALIDREQUEST, pPacket->wszMasterCharName, false );
				return ERROR_NONE;
			}

			CDNUserSession* pMasterSession = g_pUserSessionManager->FindUserSessionByName( pPacket->wszMasterCharName );
			if( pMasterSession )
			{
				// 같은 채널 검사
				if( GetChannelID() == pMasterSession->GetChannelID() )
				{
					SendMasterSystemRecallMaster( ERROR_MASTERSYSTEM_RECALL_SAMECHANNEL, pPacket->wszMasterCharName, false );
					return ERROR_NONE;
				}
				else
				{
					// 소환 대상자 상태 검사
					if( pMasterSession->m_eUserState != STATE_NONE || pMasterSession->GetPartyID() > 0 || !pMasterSession->IsNoneWindowState() )
					{
						SendMasterSystemRecallMaster( ERROR_MASTERSYSTEM_RECALL_CANTSTATUS, pPacket->wszMasterCharName, false );
						return ERROR_NONE;
					}

					// 사제 관계 검사
					bool bCheck = false;
					for( int i=0 ; i<GetMasterSystemData()->SimpleInfo.iMasterCount ; ++i )
					{
						if( pMasterSession->GetCharacterDBID() == GetMasterSystemData()->SimpleInfo.OppositeInfo[i].CharacterDBID )
						{
							bCheck = true;
							break;
						}
					}

					if( bCheck == false )
					{
						SendMasterSystemRecallMaster( ERROR_MASTERSYSTEM_RECALL_FAILED, pPacket->wszMasterCharName, false );
						return ERROR_NONE;
					}
					// 스승 상태변경..
					pMasterSession->SetWindowState(WINDOW_ISACCEPT);
					SendMasterSystemRecallMaster( ERROR_NONE, pPacket->wszMasterCharName, false );
					pMasterSession->SendMasterSystemRecallMaster( ERROR_NONE, GetCharacterName(), true );
					return ERROR_NONE;
				}
			}

			if( g_pMasterConnection == NULL || g_pMasterConnection->GetActive() == false )
			{
				SendMasterSystemRecallMaster( ERROR_GENERIC_MASTERCON_NOT_FOUND, pPacket->wszMasterCharName, false );
				return ERROR_NONE;
			}

			g_pMasterConnection->SendMasterSystemRecallMaster( GetAccountDBID(), GetCharacterName(), pPacket->wszMasterCharName, GetMasterSystemData() );
			return ERROR_NONE;
		}
		case eMasterSystem::CS_BREAKINTO_PUPIL:
		{
			MasterSystem::CSBreakInto* pPacket = reinterpret_cast<MasterSystem::CSBreakInto*>(pData);

			if( sizeof(MasterSystem::CSBreakInto) != nLen )
				return ERROR_INVALIDPACKET;

			// 체크
			if( GetLevel() < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_MasterMinLevel )) || GetMasterSystemData()->SimpleInfo.iPupilCount <= 0 )
			{
				_DANGER_POINT();
				return ERROR_NONE;
			}

			int iRet = pPacket->iRet;

			if( pPacket->iRet == ERROR_NONE )
			{				
				if( m_eUserState != STATE_NONE || GetPartyID() > 0 )					
					iRet = ERROR_MASTERSYSTEM_RECALL_CANTSTATUS;
			}
			SetWindowState(WINDOW_NONE); // 스승 상태 풀어주기
			if( g_pMasterConnection == NULL || g_pMasterConnection->GetActive() == false )
				return ERROR_NONE;

			g_pMasterConnection->SendMasterSystemBreakInto( iRet, GetAccountDBID(), pPacket->wszPupilCharName, GetMasterSystemData() );
			return ERROR_NONE;
		}
	}

	return CDNUserBase::OnRecvMasterSystemMessage( nSubCmd, pData, nLen );
}

#if defined( PRE_ADD_SECONDARY_SKILL )

int CDNUserSession::OnRecvSecondarySkillMessage( int nSubCmd, char * pData, int nLen )
{
	return CDNUserBase::OnRecvSecondarySkillMessage( nSubCmd, pData, nLen );
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

void CDNUserSession::OnDBRecvMsgadjustment(int nSubCmd, char * pData)
{
	switch(nSubCmd)
	{
		case QUERY_MSGADJUST:
			{
				TAMsgAdjust * pPacket = (TAMsgAdjust*)pData;

				if(g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE_SAMEDEST, GetAccountDBID(), pPacket->nVerifyMapIndex, pPacket->cGateNo, pPacket->nChannelID );
				else
					SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);
				m_eUserState = STATE_NONE;
			}
	}
}

int CDNUserSession::OnRecvGuildRecruitMessage( int nSubCmd, char* pData, int nLen )
{
	switch( nSubCmd )
	{
	case eGuildRecruit::CS_GUILDRECRUIT_REGISTER:
		{
			GuildRecruitSystem::CSGuildRecruitRegister* pPacket = reinterpret_cast<GuildRecruitSystem::CSGuildRecruitRegister*>(pData);

			if( sizeof(GuildRecruitSystem::CSGuildRecruitRegister) != nLen )
				return ERROR_INVALIDPACKET;

			const TGuildUID GuildUID = GetGuildUID();
			if( !GuildUID.IsSet())
			{
				SendRegisterGuildRecruitList( ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, (GuildRecruitSystem::RegisterType::eType)pPacket->cRegisterType );
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}

			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			if (!pGuild) 
			{
				SendRegisterGuildRecruitList( ERROR_GUILD_NOTEXIST_GUILDINFO, (GuildRecruitSystem::RegisterType::eType)pPacket->cRegisterType );
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendRegisterGuildRecruitList( ERROR_GUILD_NOTEXIST_GUILDINFO, (GuildRecruitSystem::RegisterType::eType)pPacket->cRegisterType );
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 권한 체크(초대)
			DN_ASSERT(CHECK_LIMIT(GetGuildSelfView().btGuildRole, GUILDROLE_TYPE_CNT),	"Check!");

			//마스터만 사용가능
			if ( GetGuildSelfView().btGuildRole != GUILDROLE_TYPE_MASTER ) 
			{
				SendRegisterGuildRecruitList( ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER, (GuildRecruitSystem::RegisterType::eType)pPacket->cRegisterType );
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}
					
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterOn)	//등록
				GetDBConnection()->QueryRegisterOnGuildRecruit( this, pPacket->wszGuildIntroduction, pPacket->cClassGrade, pPacket->nMinLevel, pPacket->nMaxLevel, pPacket->cPurposeCode, pPacket->bCheckHomePage );
			else if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterMod)	//수정
				GetDBConnection()->QueryRegisterModGuildRecruit( this, pPacket->wszGuildIntroduction, pPacket->cClassGrade, pPacket->nMinLevel, pPacket->nMaxLevel, pPacket->cPurposeCode, pPacket->bCheckHomePage );
			else if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterOff)	//삭제
				GetDBConnection()->QueryRegisterOffGuildRecruit( this );
#else	//	#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterOn)	//등록
				GetDBConnection()->QueryRegisterOnGuildRecruit( this, pPacket->wszGuildIntroduction, pPacket->cClassGrade, pPacket->nMinLevel, pPacket->nMaxLevel, 0, false );
			else if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterMod)	//수정
				GetDBConnection()->QueryRegisterModGuildRecruit( this, pPacket->wszGuildIntroduction, pPacket->cClassGrade, pPacket->nMinLevel, pPacket->nMaxLevel, 0, false );
			else if( pPacket->cRegisterType == GuildRecruitSystem::RegisterType::RegisterOff)	//삭제
				GetDBConnection()->QueryRegisterOffGuildRecruit( this );
#endif	//	#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			return ERROR_NONE;
		}
		break;
	case eGuildRecruit::CS_GUILDRECRUIT_REQUEST:
		{
			GuildRecruitSystem::CSGuildRecruitRequest* pPacket = reinterpret_cast<GuildRecruitSystem::CSGuildRecruitRequest*>(pData);

			if( sizeof(GuildRecruitSystem::CSGuildRecruitRequest) != nLen )
				return ERROR_INVALIDPACKET;

			if( GetGuildUID().IsSet())
			{
				SendGuildRecruitRequest( ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD, (GuildRecruitSystem::RequestType::eType)pPacket->cRequestType );
				return ERROR_GUILD_YOUR_ALREADY_BELONGTOGUILD;
			}

			if( pPacket->cRequestType == GuildRecruitSystem::RequestType::RequestOn )
				GetDBConnection()->QueryRequestOnGuildRecruit( this, pPacket->GuildUID.nDBID );
			else if( pPacket->cRequestType == GuildRecruitSystem::RequestType::RequestOff )
				GetDBConnection()->QueryRequestOffGuildRecruit( this, pPacket->GuildUID.nDBID );
			return ERROR_NONE;
		}
		break;
	case eGuildRecruit::CS_GUILDRECRUIT_ACCEPT:
		{
			GuildRecruitSystem::CSGuildRecruitAccept* pPacket = reinterpret_cast<GuildRecruitSystem::CSGuildRecruitAccept*>(pData);

			if( sizeof(GuildRecruitSystem::CSGuildRecruitAccept) != nLen )
				return ERROR_INVALIDPACKET;

			if (0 >= ::wcslen(pPacket->wszToCharacterName)) 
			{
				SendGuildRecruitAccept(ERROR_GENERIC_INVALIDREQUEST, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			const TGuildUID GuildUID = GetGuildUID();
			if( !GuildUID.IsSet())
			{
				SendGuildRecruitAccept( ERROR_GUILD_YOUR_NOT_BELONGANYGUILD, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
				return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
			}
			
			CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);

			if (!pGuild) 
			{
				SendGuildRecruitAccept( ERROR_GUILD_NOTEXIST_GUILDINFO, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}

#if !defined( PRE_ADD_NODELETEGUILD )
			CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
			if (FALSE == pGuild->IsEnable())
			{
				SendGuildRecruitAccept( ERROR_GUILD_NOTEXIST_GUILDINFO, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
				return ERROR_GUILD_NOTEXIST_GUILDINFO;
			}
#endif

			// 권한 체크(초대)
			DN_ASSERT(CHECK_LIMIT(GetGuildSelfView().btGuildRole, GUILDROLE_TYPE_CNT),	"Check!");

			if (!pGuild->CheckGuildInfoAuth(static_cast<eGuildRoleType>(GetGuildSelfView().btGuildRole), static_cast<eGuildAuthType>(GUILDAUTH_TYPE_INVITE))) 
			{
				SendGuildRecruitAccept( ERROR_GUILD_HAS_NO_AUTHORITY, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
				return ERROR_GUILD_HAS_NO_AUTHORITY;
			}

			if( pPacket->cAcceptType == GuildRecruitSystem::AcceptType::AcceptOn )
			{	
				// 인원추가 가능여부 체크
				if (!pGuild->IsMemberAddable()) 
				{
					SendGuildRecruitAccept( ERROR_GUILD_CANTADD_GUILDMEMBER, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
					return ERROR_GUILD_CANTADD_GUILDMEMBER;
				}

				// 길드전 예선/본선 기간에는 가입을 못시킨다. 
				if (g_pGuildWarManager && pGuild->IsEnrollGuildWar() && g_pGuildWarManager->GetRestriction( eGuild::CS_INVITEGUILDMEMBREQ, pGuild->GetUID()) )
				{
					SendGuildRecruitAccept( ERROR_GUILDWAR_CANTADD_GUILDMEMBER, pPacket->biAcceptCharacterDBID, false, (GuildRecruitSystem::AcceptType::eType)pPacket->cAcceptType );
					return ERROR_GUILDWAR_CANTADD_GUILDMEMBER;
				}
				
				GetDBConnection()->QueryAcceptOnGuildRecruit( this, pPacket->GuildUID.nDBID, pPacket->biAcceptCharacterDBID, pGuild->GetInfo()->wGuildSize, pPacket->wszToCharacterName );
			}
			else if( pPacket->cAcceptType == GuildRecruitSystem::AcceptType::AcceptOff )
			{
				GetDBConnection()->QueryAcceptOffGuildRecruit( this, pPacket->GuildUID.nDBID, pPacket->biAcceptCharacterDBID, pPacket->wszToCharacterName );
			}
			return ERROR_NONE;
		}
		break;
	}
	return CDNUserBase::OnRecvGuildRecruitMessage( nSubCmd, pData, nLen );
}

#if defined (PRE_ADD_BESTFRIEND)
int CDNUserSession::OnRecvBestFriendMessage(int nSubCmd, char *pData, int nLen)
{
	switch( nSubCmd )
	{	
	case eBestFriend::CS_SEARCH:
		{
			const BestFriend::CSSearch *pPacket = reinterpret_cast<BestFriend::CSSearch*>(pData);

			if (sizeof(BestFriend::CSSearch) != nLen)
				return ERROR_INVALIDPACKET;

			if (0 >= ::wcslen(pPacket->wszName)) 
			{
				SendSearchBestFriendResult(ERROR_GENERIC_INVALIDREQUEST, 0, 0, NULL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (wcscmp(GetCharacterName(), pPacket->wszName) == 0)
			{
				SendSearchBestFriendResult(ERROR_BESTFRIEND_SELFSEARCH, 0, 0, NULL);
				return ERROR_BESTFRIEND_SELFSEARCH;
			}

			if (m_pBestFriend->IsRegistered())
			{
				SendSearchBestFriendResult(ERROR_BESTFRIEND_ALREADY_REGISTERED, 0, 0, NULL);
				return ERROR_BESTFRIEND_ALREADY_REGISTERED;
			}

			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pPacket->wszName);
			if (!pUserObj)
			{
				// 마스터 서버에 요청
				g_pMasterConnection->SendSearchBestFriend(GetAccountDBID(), ERROR_NONE, false, pPacket->wszName);
				return ERROR_NONE;
			}

			SendSearchBestFriendResult(ERROR_NONE, pUserObj->GetLevel(), pUserObj->GetStatusData()->cJob, pUserObj->GetCharacterName());
			return ERROR_NONE;
		}
		break;
	case eBestFriend::CS_REGIST:
		{
			const BestFriend::CSRegist *pPacket = reinterpret_cast<BestFriend::CSRegist*>(pData);

			if (sizeof(BestFriend::CSRegist) != nLen)
				return ERROR_INVALIDPACKET;

			if (0 >= ::wcslen(pPacket->wszBestFriendName)) 
			{
				SendRegistBestFriendReq(ERROR_GENERIC_INVALIDREQUEST, 0, 0, NULL, 0, 0, NULL);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			if (wcscmp(GetCharacterName(), pPacket->wszBestFriendName) == 0)
			{
				SendRegistBestFriendReq(ERROR_BESTFRIEND_SELFREGIST, 0, 0, NULL, 0, 0, NULL);
				return ERROR_BESTFRIEND_SELFREGIST;
			}

			if (m_pBestFriend->IsRegistered())
			{
				SendRegistBestFriendReq(ERROR_BESTFRIEND_ALREADY_REGISTERED, 0, 0, NULL, 0, 0, NULL);
				return ERROR_BESTFRIEND_ALREADY_REGISTERED;
			}

			const TItem* pRegistItem = m_pItem->GetCashInventory(pPacket->biInvenSerial);
			if (!pRegistItem)
			{
				SendRegistBestFriendReq(ERROR_BESTFRIEND_NOTHAVE_REGISTITEM, 0, 0, NULL, 0, 0, NULL);
				return ERROR_BESTFRIEND_NOTHAVE_REGISTITEM;
			}
	
			m_pBestFriend->SetRegistSerial(pPacket->biInvenSerial);
			CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pPacket->wszBestFriendName);
			if (!pUserObj)
			{
				// 마스터 서버에 요청
				g_pMasterConnection->SendRegistBestFriend(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), pPacket->wszBestFriendName, pRegistItem->nItemID);
				return ERROR_NONE;
			}

			// 조건체크
			if (pUserObj->GetBestFriend()->IsRegistered())
			{
				SendRegistBestFriendReq(ERROR_BESTFRIEND_REGISTFAIL_DESTUSER_SITUATION_NOTALLOWED, 0, 0, NULL, 0, 0, NULL);
				return ERROR_BESTFRIEND_REGISTFAIL_DESTUSER_SITUATION_NOTALLOWED;
			}

			// 절친 요청
			pUserObj->SendRegistBestFriendReq(ERROR_NONE, GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), pUserObj->GetAccountDBID(), pUserObj->GetCharacterDBID(), pUserObj->GetCharacterName());
			
			return ERROR_NONE;
		}
		break;

	case eBestFriend::CS_ACCEPT:
		{
			const BestFriend::CSAccept *pPacket = reinterpret_cast<BestFriend::CSAccept*>(pData);

			if (sizeof(BestFriend::CSAccept) != nLen)
				return ERROR_INVALIDPACKET;

			if (!pPacket->bAccept) // 거절
			{
				CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pPacket->wszFromName);
				if (pUserObj)
					pUserObj->SendRegistBestFriendAck(ERROR_BESTFRIEND_REFUESED_REGISTRATION, GetAccountDBID(), GetCharacterName(), pUserObj->GetAccountDBID(), pUserObj->GetCharacterName());
				else
					g_pMasterConnection->SendRegistBestFriendResult(pPacket->nFromAccountDBID, ERROR_BESTFRIEND_REFUESED_REGISTRATION, true, 0, pPacket->wszFromName, GetAccountDBID(), GetCharacterDBID(), GetCharacterName());
				return ERROR_NONE;
			}

			g_pMasterConnection->SendRegistBestFriendResult(pPacket->nFromAccountDBID, ERROR_NONE, true, pPacket->biFromCharacterDBID, pPacket->wszFromName, GetAccountDBID(), GetCharacterDBID(), GetCharacterName());
			return ERROR_NONE;

		}
		break;

	case eBestFriend::CS_CANCELBF:
		{
			const BestFriend::CSCancel *pPacket = reinterpret_cast<BestFriend::CSCancel*>(pData);
			if (sizeof(BestFriend::CSCancel) != nLen)
				return ERROR_INVALIDPACKET;

			if (false == m_pBestFriend->IsRegistered())
			{
				SendCancelBestFriend(ERROR_BESTFRIEND_NOT_REGISTERED, NULL);
				return ERROR_BESTFRIEND_NOT_REGISTERED;
			}

			// DB 처리
			m_pDBCon->QueryCancelBestFriend(this, pPacket->bCancel);
			return ERROR_NONE;
		}
		break;


	case eBestFriend::CS_EDITMEMO:
		{
			const BestFriend::CSEditMemo *pPacket = reinterpret_cast<BestFriend::CSEditMemo*>(pData);
			if (sizeof(BestFriend::CSEditMemo) != nLen)
				return ERROR_INVALIDPACKET;

			// 조건 처리
			if (false == m_pBestFriend->IsRegistered())
			{
				SendEditBestFriendMemo(ERROR_BESTFRIEND_NOT_REGISTERED, true, NULL);
				return ERROR_BESTFRIEND_NOT_REGISTERED;
			}

			// DB 처리
			m_pDBCon->QueryEditBestFriendMemo(this, pPacket->wszMemo);
			return ERROR_NONE;
		}
		break;
	}

	return CDNUserBase::OnRecvBestFriendMessage( nSubCmd, pData, nLen );
}
#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined (PRE_PRIVATECHAT_CHANNEL)
int CDNUserSession::OnRecvPrivateChatChannelMessage(int nSubCmd, char *pData, int nLen)
{		
	return CDNUserBase::OnRecvPrivateChatChannelMessage( nSubCmd, pData, nLen );
}
#endif // #if defined (PRE_PRIVATECHAT_CHANNEL)

#if defined( PRE_ALTEIAWORLD_EXPLORE )
int CDNUserSession::OnRecvWorldAlteiaMessage(int nSubCmd, char *pData, int nLen)
{		
	switch( nSubCmd )
	{
		case eAlteiaWorld::CS_ALTEIAWORLD_INFO:
		{
			AlteiaWorld::CSAlteiaWorldInfo* pPacket = reinterpret_cast<AlteiaWorld::CSAlteiaWorldInfo*>(pData);

			if( sizeof(AlteiaWorld::CSAlteiaWorldInfo) != nLen )
				return ERROR_INVALIDPACKET;

			AlteiaWorldInfo( pPacket );
		}
		break;
		case eAlteiaWorld::CS_ALTEIAWORLD_JOIN:
		{
			int nRet = AlteiaWorldJoin();
			if(nRet != ERROR_NONE)
			{
				SendAlteiaWorldJoinResult(nRet);
			}
		}
		break;
		case eAlteiaWorld::CS_ALTEIAWORLD_SENDTICKET:
			{
				AlteiaWorld::CSAlteiaWorldSendTicket* pPacket = reinterpret_cast<AlteiaWorld::CSAlteiaWorldSendTicket*>(pData);

				if( sizeof(AlteiaWorld::CSAlteiaWorldSendTicket) != nLen )
					return ERROR_INVALIDPACKET;
				
				if(m_cSendTicketCount <= 0)
				{
					SendAlteiaWorldSendTicketResult(ERROR_INVALIDPACKET, GetAlteiaSendTicketCount() );
					break;
				}

				CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(pPacket->wszCharacterName);

				if(pSession)
				{	
					if( GetSessionID() == pSession->GetSessionID() )
					{
						SendAlteiaWorldSendTicketResult(ERROR_INVALIDPACKET, GetAlteiaSendTicketCount() );
						break;
					}

					BYTE cMaxSendTicketCount = static_cast<BYTE>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldSendTicketCount ));
					pSession->GetDBConnection()->QueryAddAlteiaWorldSendTicketList(pSession, GetCharacterDBID(), GetCharacterName(), cMaxSendTicketCount);
					return ERROR_NONE;
				}	
				else
				{
					if( g_pMasterConnection )
						g_pMasterConnection->SendAddAlteiaWorldSendTicket( GetWorldSetID(), pPacket->wszCharacterName, GetCharacterDBID(), GetCharacterName() );
				}
			}
			break;
		case eAlteiaWorld::CS_ALTEIAWORLD_QUITINFO:
			{				
				SetWindowState(WINDOW_NONE);
			}
			break;
	}			
	return CDNUserBase::OnRecvWorldAlteiaMessage( nSubCmd, pData, nLen );
}
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined(PRE_ADD_DWC)
int CDNUserSession::_RecvInviteDWCTeamMembReq(LPCWSTR pwszToCharacterName)
{
	if (0 >= ::wcslen(pwszToCharacterName)) 
		return ERROR_GENERIC_INVALIDREQUEST;

	if( !IsDWCCharacter() )
		return ERROR_DWC_FAIL;

	if (GetDWCTeamID() <= 0) 
		return ERROR_DWC_HAVE_NOT_TEAM;

	CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->GetDWCTeam(GetDWCTeamID());
	if (!pDWCTeam) 
		return ERROR_DWC_NOTEXIST_TEAMINFO;

	if (pDWCTeam->GetLeaderCharacterDBID() != GetCharacterDBID())	//팀장만 초대가능
		return ERROR_DWC_HAS_NO_AUTHORITY;

	if (!pDWCTeam->IsMemberAddable())	// 인원추가 가능여부 체크
		return ERROR_DWC_CANTADD_TEAMMEMBER;

	CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByName(pwszToCharacterName);

	// 현재서버에 유저를 찾지 못하면 마스터서버에 요청하여 찾아본다.
	if (!pUserObj)
	{
		g_pMasterConnection->SendInviteDWCTeamMember(GetDWCTeamID(), GetAccountDBID(), GetCharacterName(), pwszToCharacterName, pDWCTeam->GetTeamInfo()->wszTeamName);
		return ERROR_NONE;
	}

	// 현재서버에 유저가 존재하는 경우

	// 로딩상태 확인
	if(!pUserObj->m_bLoadUserData)
		return ERROR_CHARACTER_DATA_NOT_LOADED_OPPOSITE;

	if( !pUserObj->IsDWCCharacter() )
		return ERROR_DWC_NOT_DWC_CHARACTER;

	if(pUserObj->GetDWCTeamID() > 0) 
		return ERROR_DWC_ALREADY_HASTEAM;

	if(GetAccountDBID() == pUserObj->GetAccountDBID()) 
		return ERROR_DWC_CANT_INVITE_YOURESELF;

	if(!pDWCTeam->CheckDuplicationJob(pUserObj->GetUserJob()))
		return ERROR_DWC_DUPLICATION_JOB;

	// 유저 상태 검사 및 변경.
	if ( !pUserObj->IsNoneWindowState() )
		return ERROR_DWC_CANTACCEPT_TEAMINVITATION;

	pUserObj->SetWindowState(WINDOW_ISACCEPT);
	pUserObj->SendInviteDWCTeamMemberReq(GetDWCTeamID(), GetAccountDBID(), GetCharacterName(), pDWCTeam->GetTeamInfo()->wszTeamName);
	return ERROR_NONE;
}

int CDNUserSession::_RecvInviteDWCTeamMembAck(bool bAccept, UINT nFromAccountDBID, UINT nTeamID)
{	
	if( !bAccept )
	{	//초대한 유저에게 결과 알림
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(nFromAccountDBID);

		if (pUserObj) 
			pUserObj->SendInviteDWCTeamMemberAck(GetAccountDBID(), GetCharacterDBID(), GetCharacterName(), ERROR_DWC_REFUESED_TEAMINVITATION, pUserObj->GetAccountDBID(), pUserObj->GetCharacterDBID(), 0,NULL, NULL);
		else
			g_pMasterConnection->SendInviteDWCTeamMemberACK(nFromAccountDBID, ERROR_DWC_REFUESED_TEAMINVITATION, false, GetCharacterName());
		
		return ERROR_NONE;
	}

	if( !IsDWCCharacter() )
		return ERROR_DWC_PERMIT_FAIL;

	if (GetDWCTeamID() > 0) 
		return ERROR_DWC_ALREADY_HASTEAM;

	CDnDWCTeam* pDWCTeam = g_pDWCTeamManager->GetDWCTeam(nTeamID);
	if (!pDWCTeam)	// 서버상에 팀정보가 존재하지 않으므로 DB조회를 하여 처리하도록 한다.
	{
		m_pDBCon->QueryDWCInviteMember(m_cDBThreadID, m_nAccountDBID, m_biCharacterDBID, nTeamID, g_Config.nWorldSetID, true);
		return ERROR_NONE;
	}

	// 인원추가 가능여부 체크
	if (!pDWCTeam->IsMemberAddable()) 
		return ERROR_DWC_CANTADD_TEAMMEMBER;

	if(!pDWCTeam->CheckDuplicationJob(GetUserJob()))
		return ERROR_DWC_DUPLICATION_JOB;

	m_pDBCon->QueryAddDWCTeamMember(m_cDBThreadID, m_nAccountDBID, m_biCharacterDBID, nTeamID, g_Config.nWorldSetID);
	return ERROR_NONE;
}


int CDNUserSession::OnRecvDWCMessage( int nSubCmd, char* pData, int nLen )
{
	switch( nSubCmd )
	{
	case eDWC::CS_CREATE_DWCTEAM:
		{
			CSCreateDWCTeam* pPacket = (CSCreateDWCTeam*)pData;

			if( sizeof(CSCreateDWCTeam) != nLen )
				return ERROR_INVALIDPACKET;

			// 팀명 사이즈 검사
			if (0 >= ::wcslen(pPacket->wszTeamName)) 
			{
				SendCreateDWCTeam(ERROR_GENERIC_INVALIDREQUEST, GetSessionID(), L"", 0);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			// 20100629 길드명 작성 시 국가별 유니코드 문자대역 확인하도록 기능 추가
			if (!g_CountryUnicodeSet.Check(pPacket->wszTeamName)) 
			{	
				SendCreateDWCTeam(ERROR_GENERIC_INVALIDREQUEST, GetSessionID(), L"", 0);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

#if defined(PRE_ADD_MULTILANGUAGE)
			if (g_pDataManager->CheckProhibitWord(static_cast<int>(m_eSelectedLanguage), pPacket->wszTeamName))
			{
				SendCreateDWCTeam(ERROR_DWC_PROHIBITWORD, GetSessionID(), L"", 0);
				return ERROR_DWC_PROHIBITWORD;
			}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
			if (g_pDataManager->CheckProhibitWord(pPacket->wszTeamName))
			{
				SendCreateDWCTeam(ERROR_DWC_PROHIBITWORD, GetSessionID(), L"", 0);
				return ERROR_DWC_PROHIBITWORD;
			}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

			if( !IsDWCCharacter() )
			{
				SendCreateDWCTeam(ERROR_DWC_PERMIT_FAIL, GetSessionID(), L"", 0);
				return ERROR_DWC_PERMIT_FAIL;
			}

			if( GetDWCTeamID() > 0 )
			{
				SendCreateDWCTeam(ERROR_DWC_ALREADY_HASTEAM, GetSessionID(), L"", 0);
				return ERROR_DWC_ALREADY_HASTEAM;
			}

			// DB 데이터 전송
			m_pDBCon->QueryCreateDWCTeam(m_cDBThreadID, m_nAccountDBID, m_biCharacterDBID, pPacket->wszTeamName, g_Config.nWorldSetID);
			return ERROR_NONE;
		}
		break;
	case eDWC::CS_LEAVE_DWCTEAM_MEMB:
		{
			if( !IsDWCCharacter() )
			{
				SendCreateDWCTeam(ERROR_DWC_PERMIT_FAIL, GetSessionID(), L"", 0);
				return ERROR_DWC_PERMIT_FAIL;
			}

			if( GetDWCTeamID() <= 0 )
			{
				SendCreateDWCTeam(ERROR_DWC_ALREADY_HASTEAM, GetSessionID(), L"", 0);
				return ERROR_DWC_HAVE_NOT_TEAM;
			}

			if(!g_pDWCTeamManager)
			{
				SendCreateDWCTeam(ERROR_DWC_NOTEXIST_TEAMINFO, GetSessionID(), L"", 0);
				return ERROR_DWC_NOTEXIST_TEAMINFO;
			}

			CDnDWCTeam *pDWCTeam = g_pDWCTeamManager->GetDWCTeam(GetDWCTeamID());
			if(!pDWCTeam)
			{
				SendCreateDWCTeam(ERROR_DWC_NOTEXIST_TEAMINFO, GetSessionID(), L"", 0);
				return ERROR_DWC_NOTEXIST_TEAMINFO;
			}

			if(!pDWCTeam->CheckCanLeaveTeam())
			{
				SendCreateDWCTeam(ERROR_DWC_CANT_LEAVE_TEAM, GetSessionID(), L"", 0);
				return ERROR_DWC_CANT_LEAVE_TEAM;
			}

			m_pDBCon->QuerLeaveDWCTeam(m_cDBThreadID, m_nAccountDBID, m_biCharacterDBID, GetDWCTeamID(), g_Config.nWorldSetID);
			return ERROR_NONE;
		}
		break;
	case eDWC::CS_INVITE_DWCTEAM_MEMBREQ:	// CS_INVITE_DWCTEAM_MEMBREQ 요청의 모든 상황은 SC_INVITE_DWCTEAM_MEMBACK 로 돌려보낸다.
		{
			const CSInviteDWCTeamMemberReq *pPacket = reinterpret_cast<CSInviteDWCTeamMemberReq*>(pData);

			if (sizeof(CSInviteDWCTeamMemberReq) != nLen)
				return ERROR_INVALIDPACKET;

			int nRet = _RecvInviteDWCTeamMembReq(pPacket->wszToCharacterName);
			if( nRet != ERROR_NONE )				
				SendInviteDWCTeamMemberAck(nRet);

			return ERROR_NONE;
		}
		break;
	case eDWC::CS_INVITE_DWCTEAM_MEMBACK:
		{
			const CSInviteDWCTeamMemberAck *pPacket = reinterpret_cast<CSInviteDWCTeamMemberAck*>(pData);

			if (sizeof(CSInviteDWCTeamMemberAck) != nLen)
				return ERROR_INVALIDPACKET;

			//상태 해제
			IsWindowStateNoneSet(WINDOW_ISACCEPT);

			int nRet = _RecvInviteDWCTeamMembAck( pPacket->bAccept, pPacket->nFromAccountDBID, pPacket->nTeamID );
			if( nRet != ERROR_NONE )
				SendInviteDWCTeamMemberAckResult(nRet);

			return ERROR_NONE;
		}
		break;
	case eDWC::CS_GET_DWCTEAM_INFO:
		{
			CSGetDWCTeamInfo* pPacket = (CSGetDWCTeamInfo*)pData;
			if (sizeof(CSGetDWCTeamInfo) != nLen)
				return ERROR_INVALIDPACKET;

			if(!IsDWCCharacter())
			{
				SendGetDWCTeamInfo(NULL, ERROR_DWC_PERMIT_FAIL);
				return ERROR_NONE;
			}

			if( GetDWCTeamID() <= 0 )
			{
				SendGetDWCTeamInfo(NULL, ERROR_DWC_HAVE_NOT_TEAM);
				return ERROR_NONE;
			}

			CDnDWCTeam* pDWCTeam =  g_pDWCTeamManager->GetDWCTeam(GetDWCTeamID());
			if(pDWCTeam)
			{
				SendGetDWCTeamInfo(pDWCTeam, ERROR_NONE);
				if(pPacket->bNeedMembList)
					SendGetDWCTeamMember(pDWCTeam, ERROR_NONE);
			}
			else
			{
				SendGetDWCTeamInfo(NULL, ERROR_DWC_NOTEXIST_TEAMINFO);
				if (m_pDBCon && m_pDBCon->GetActive())
					m_pDBCon->QueryGetDWCTeamInfo(this, true);
			}
			
			return ERROR_NONE;
		}
		break;

	case eDWC::CS_GET_DWC_RANKPAGE:
		{
			CSGetDWCRankPage * pPacket = (CSGetDWCRankPage*)pData;
			if (sizeof(CSGetDWCRankPage) != nLen)
				return ERROR_INVALIDPACKET;

			if (!IsDWCCharacter())
			{
				SendDWCRankResult(ERROR_DWC_PERMIT_FAIL, 0, 0, 0, NULL);
				return ERROR_NONE;
			}

			if (pPacket->nPageSize > DWC::Common::RankPageMaxSize)
			{
				SendDWCRankResult(ERROR_DWC_FAIL, 0, 0, 0, NULL);
				return ERROR_NONE;
			}

			if (GetDBConnection())
			{
				GetDBConnection()->QueryGetDWCRank(this, pPacket->nPageNum, pPacket->nPageSize);
				return ERROR_NONE;
			}

			SendDWCRankResult(ERROR_DWC_FAIL, 0, 0, 0, NULL);
			return ERROR_NONE;
		}
		break;

	case eDWC::CS_GET_DWC_FINDRANK:
		{
			CSGetDWCFindRank * pPacket = (CSGetDWCFindRank*)pData;

			if (sizeof(CSGetDWCFindRank) != nLen)
				return ERROR_INVALIDPACKET;

			if (!IsDWCCharacter())
			{
				SendDWCFindRankResult(ERROR_DWC_PERMIT_FAIL, NULL);
				return ERROR_NONE;
			}

			if (wcslen(pPacket->wszFindKey) <= 0)
			{
				SendDWCFindRankResult(ERROR_DWC_FAIL, NULL);
				return ERROR_NONE;
			}

			if (GetDBConnection())
			{
				GetDBConnection()->QueryGetDWCFindRank(this, pPacket->cType, pPacket->wszFindKey);
				return ERROR_NONE;
			}

			SendDWCFindRankResult(ERROR_DWC_FAIL, NULL);
			return ERROR_NONE;
		}
		break;

	default:
		return ERROR_UNKNOWN_HEADER;
	}

	return ERROR_NONE;
}
#endif

int CDNUserSession::_MakeEmblemErrorCode( CDnItemCompounder::S_OUTPUT &Output )
{
	int iResult = ERROR_ITEM_EMBLEM_COMPOUND_FAIL;

	if( Output.eResultCode == CDnItemCompounder::R_FAIL )
	{
		// 알 수 없는 오류로 인해 조합 실패.
		iResult = ERROR_ITEM_EMBLEM_COMPOUND_FAIL;
	}
	else
	//if( Output.eResultCode == CDnItemCompounder::R_POSSIBILITY_FAIL )
	//{
	//	// 확률상 실패함. 에러는 아님.
	//	iResult = ERROR_ITEM_EMBLEM_COMPOUND_POSSIBILITY_FAIL;
	//}
	//else
	{
		// 이것은 에러를 가리킴. 애초부터 패킷이 서버까지 오지 않았어야 하는 비정상적인 상황.
		switch( Output.eErrorCode )
		{
			case CDnItemCompounder::E_NOT_ENOUGH_MONEY:			// 돈이 모자름
				iResult = ERROR_ITEM_EMBLEM_COMPOUND_NOT_ENOUGH_MONEY;
				break;

			case CDnItemCompounder::E_NOT_ENOUGH_ITEM:			// 재료 아이템 부족
				iResult = ERROR_ITEM_EMBLEM_COMPOUND_NOT_ENOUGH_ITEM;
				break;

			case CDnItemCompounder::E_NOT_MATCH_SLOT:				// 슬롯에 맞지 않는 아이템임
				iResult = ERROR_ITEM_EMBLEM_COMPOUND_NOT_MATCH_SLOT;
				break;

			case CDnItemCompounder::E_NOT_ENOUGH_ITEM_COUNT:		// 슬롯엔 맞았으나 갯수가 모자람
				iResult = ERROR_ITEM_EMBLEM_COMPOUND_NOT_ENOUGH_ITEM_COUNT;
				break;

			case CDnItemCompounder::E_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND:		// 플레이트가 수행하는 아이템 조합 리스트에 입력받은 아이템 조합 인덱스 없음.
				iResult = ERROR_ITEM_EMBLEM_COMPOUND_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND_INDEX;
				break;
		}
	}	

	return iResult;
}


int CDNUserSession::_RecvMovePvPVillageToLobby()
{
	if( !m_pField || !g_pMasterConnection )
	{
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	// 현재 맵이 PvP마을이 아니라면..
	if( !m_pField->bIsPvPVillage() )
	{
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if(g_pMasterConnection && g_pMasterConnection->GetActive())
		g_pMasterConnection->SendVillageToVillage( REQINFO_TYPE_PVP, m_nAccountDBID, m_pField->GetMapIndex(), PvPCommon::Common::PvPVillageToLobbyGateNo );
	else
		_DANGER_POINT();
	return ERROR_NONE;
}

int CDNUserSession::_RecvMovePvPLobbyToPvPVillage()
{
	if( !m_pField || !g_pMasterConnection )
	{
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	// 현재 맵이 PvP로비가 아니라면..
	if( !m_pField->bIsPvPLobby() )
	{
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	g_pMasterConnection->SendMovePvPLobbyToPvPVillage( m_nAccountDBID );
	return ERROR_NONE;
}

bool CDNUserSession::bIsCheckPvPCreateRoom( CDNUserSession* pSession, const CSPVP_CREATEROOM* pPacket, const UINT uiMapIndex, UINT& uiPvPGameMode )
{
	if( pPacket->cRoomNameLen == 0 || pPacket->cRoomNameLen > PvPCommon::TxtMax::RoomName || pPacket->cRoomPWLen > PvPCommon::TxtMax::RoomPW )
		return false;

	// MapTable 검사
	const TPvPMapTable* pPvPMapTable = g_pDataManager->GetPvPMapTable( uiMapIndex );
	if( !pPvPMapTable )
		return false;

	// 래더맵 검사
	if( pPvPMapTable->MatchType != LadderSystem::MatchType::None )
		return false;
	// MaxUser 검사
	if( !pPvPMapTable->CheckMaxUser( pPacket->cMaxUser ) )
		return false;
	// 게임모드 검사
	if( !pPvPMapTable->CheckGameModeTableID( pPacket->uiGameModeTableID ) )
		return false;
	// 난입옵션 검사
	if( !pPvPMapTable->CheckBreakIntoOption( pPacket->unRoomOptionBit&PvPCommon::RoomOption::BreakInto ) )
		return false;

	const TPvPGameModeTable* pPvPGameModeTable = g_pDataManager->GetPvPGameModeTable( pPacket->uiGameModeTableID );
	if( !pPvPGameModeTable )
		return false;

	if(pPvPGameModeTable->bAllowedUserCreateMode == false)		//유저생성 불가 모드
		return false;

	// 승리조건 검사
	if( !pPvPGameModeTable->CheckWinCondition( pPacket->uiSelectWinCondition ) )
		return false;
	// 플레이타임 검사
	if( !pPvPGameModeTable->CheckPlayTimeSec( pPacket->uiSelectPlayTimeSec ) )
		return false;
	if( !pPvPGameModeTable->CheckMaxUser( pPacket->cMaxUser) )
		return false;

	if( pPvPGameModeTable->bIsSelectableRegulation == false )
	{
		if( pPvPGameModeTable->bIsDefaultRegulation == true )
		{
			if( pPacket->unRoomOptionBit&PvPCommon::RoomOption::NoRegulation )
				return false;
		}
		else
		{
			if( !(pPacket->unRoomOptionBit&PvPCommon::RoomOption::NoRegulation) )
				return false;
		}
	}

	// 레벨제한 유효성 검사
	if( pPacket->cMinLevel < 1 || pPacket->cMaxLevel > CHARLEVELMAX || pPacket->cMinLevel > pPacket->cMaxLevel )
		return false;
	if( pSession && (pSession->GetLevel() < pPacket->cMinLevel || pSession->GetLevel() > pPacket->cMaxLevel) )
		return false;

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	//초보타입 유효성 검사
	if (pSession && pSession->GetPvPChannelType() != pPacket->cRoomType)
	{
		_DANGER_POINT();
		return false;
	}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	switch( pPvPGameModeTable->uiGameMode )
	{
		// 대장전에서는 ShowHP 옵션을 사용하지 않는다.
		case PvPCommon::GameMode::PvP_Captain:
		{
			const_cast<CSPVP_CREATEROOM*>(pPacket)->unRoomOptionBit &= ~(PvPCommon::RoomOption::ShowHP);
			break;
		}
		case PvPCommon::GameMode::PvP_AllKill:
#if defined( PRE_ADD_PVP_TOURNAMENT)
		case PvPCommon::GameMode::PvP_Tournament:
#endif //#if defined( PRE_ADD_PVP_TOURNAMENT)
		{
			// 올킬전에서는 난입/아이템 옵션을 사용하지 않는다.
			const_cast<CSPVP_CREATEROOM*>(pPacket)->unRoomOptionBit &= ~(PvPCommon::RoomOption::BreakInto|PvPCommon::RoomOption::DropItem);
			
			if( pPacket->unRoomOptionBit&PvPCommon::RoomOption::RandomTeam )
			{
				if( !(pPacket->unRoomOptionBit&PvPCommon::RoomOption::AllKill_RandomOrder) )
					return false;
			}
			break;
		}
	}

	if( pPvPGameModeTable->uiGameMode != PvPCommon::GameMode::PvP_AllKill )
	{
		const_cast<CSPVP_CREATEROOM*>(pPacket)->unRoomOptionBit &= ~(PvPCommon::RoomOption::AllKill_RandomOrder);
	}

#if defined(PRE_ADD_DWC)
	//DWC 케릭터는 올킬전만 가능
	if( pSession && (pSession->IsDWCCharacter() && pPvPGameModeTable->uiGameMode != PvPCommon::GameMode::PvP_AllKill) )
		return false;
#endif

	uiPvPGameMode = pPvPGameModeTable->uiGameMode;
	return true;
}

short CDNUserSession::_RecvPvPCreateRoom( const CSPVP_CREATEROOM* pPacket )
{
	if( !g_pMasterConnection || !g_pMasterConnection->GetActive() )
		return ERROR_GENERIC_MASTERCON_NOT_FOUND;

	UINT uiPvPGameMode;
	if( !bIsCheckPvPCreateRoom( this, pPacket, pPacket->uiMapIndex, uiPvPGameMode ) )
		return ERROR_PVP_CREATEROOM_FAILED;
	
	if( !m_pField )
	{
		_DANGER_POINT();
		return ERROR_PVP_CREATEROOM_FAILED;
	}

#if defined( PRE_WORLDCOMBINE_PVP )
	if( pPacket->nWorldPvPRoomType )
	{
		if( (GetAccountLevel() >= AccountLevel_New && GetAccountLevel() <= AccountLevel_Developer) && g_Config.nCombinePartyWorld == g_Config.nWorldSetID )
		{
			const TPvPGameModeTable* pGameModeTable = g_pDataManager->GetPvPGameModeTable( pPacket->uiGameModeTableID );
			if(pGameModeTable)
			{
				if( !g_pDataManager->bIsWorldPvPRoomAllowMode( pGameModeTable->uiGameMode ) )
					return ERROR_PVP_CREATEROOM_FAILED;
			}
			else 
				return ERROR_PVP_CREATEROOM_FAILED;
			
			//운영자만 방 생성 가능( 로비이고 기준월드만 룸 생성가능)
			TWorldPvPMissionRoom MissonRoomData;
			memset(&MissonRoomData,0,sizeof(MissonRoomData));
			MissonRoomData.bDropItem = false;
			MissonRoomData.cMaxPlayers = pPacket->cMaxUser;
			MissonRoomData.nStartPlayers = pPacket->cMinUser;
			MissonRoomData.cModeID = pPacket->uiGameModeTableID;
			MissonRoomData.eWorldReqType = WorldPvPMissionRoom::Common::GMRoom;
#if defined( PRE_ADD_COLOSSEUM_BEGINNER )
			MissonRoomData.nChannelType = GetPvPChannelType();
#endif
			MissonRoomData.nItemID = CDNPvPRoomManager::GetInstance().GetGMWorldPvPRoomIndex();
			MissonRoomData.nMapID = pPacket->uiMapIndex;
			MissonRoomData.nMinLevel = pPacket->cMinLevel;
			MissonRoomData.nMaxLevel = pPacket->cMaxLevel;
			MissonRoomData.nPlayTime = pPacket->uiSelectPlayTimeSec;
			MissonRoomData.nWinCondition = pPacket->uiSelectWinCondition;
			MissonRoomData.unRoomOptionBit = pPacket->unRoomOptionBit;
			MissonRoomData.unRoomOptionBit |= PvPCommon::RoomOption::RandomTeam;
			if( pGameModeTable->uiGameMode == PvPCommon::GameMode::PvP_AllKill )
				MissonRoomData.unRoomOptionBit |= PvPCommon::RoomOption::AllKill_RandomOrder;

			_wcscpy( MissonRoomData.wszRoomName, _countof(MissonRoomData.wszRoomName), pPacket->wszBuf, pPacket->cRoomNameLen );

			if( pPacket->cRoomPWLen > 0 )
			{
				WCHAR wszBuf[PASSWORDLENMAX];
				memset( &wszBuf, 0, sizeof(WCHAR)*PASSWORDLENMAX );
				_wcscpy( wszBuf, _countof(wszBuf), pPacket->wszBuf+pPacket->cRoomNameLen, pPacket->cRoomPWLen );
				MissonRoomData.nRoomPW = _wtoi(wszBuf);
			}

			g_pMasterConnection->SendWorldPvPCreateRoom( MissonRoomData.nItemID, MissonRoomData, GetAccountDBID() );
			return ERROR_NONE;
		}
		return ERROR_PVP_CREATEROOM_FAILED;
	}
#endif

#ifdef PRE_MOD_PVPOBSERVER
	if (pPacket->bExtendObserver == true)
		return ERROR_PVP_CREATEROOM_FAILED;
#endif		//#ifdef PRE_MOD_PVPOBSERVER

#if defined(PRE_ADD_PVP_TOURNAMENT)
	g_pMasterConnection->SendPvPCreateRoom( m_nAccountDBID, static_cast<BYTE>(uiPvPGameMode), GetChannelID(), m_pField->GetMapIndex(), pPacket, NULL, GetUserJob() );
#else
	g_pMasterConnection->SendPvPCreateRoom( m_nAccountDBID, static_cast<BYTE>(uiPvPGameMode), GetChannelID(), m_pField->GetMapIndex(), pPacket );
#endif

	return ERROR_NONE;
}

short CDNUserSession::_RecvPvPModifyRoom( const CSPVP_MODIFYROOM* pPacket )
{
	if( !g_pMasterConnection || !g_pMasterConnection->GetActive() )
		return ERROR_GENERIC_MASTERCON_NOT_FOUND;

	// 현재 PvPMapIndex 얻어오기
	UINT		uiPvPIndex		= GetPvPIndex();
	CDNPvPRoom* pPvPRoom		= CDNPvPRoomManager::GetInstance().GetPvPRoom( GetChannelID(), uiPvPIndex );
	if( !pPvPRoom )
	{
		_DANGER_POINT();
		return ERROR_PVP_MODIFYROOM_FAILED;
	}
	// 방인원은 바뀔 수 없다.
	if( pPvPRoom->GetMaxUser() != pPacket->sCSPVP_CREATEROOM.cMaxUser )
	{
		_DANGER_POINT();
		return ERROR_PVP_MODIFYROOM_FAILED;
	}

	//UINT uiPvPMapIndex = pPvPRoom->GetMapIndex();
	UINT uiPvPMapIndex = pPacket->sCSPVP_CREATEROOM.uiMapIndex;
	UINT uiPvPGameMode;
	if( !bIsCheckPvPCreateRoom( this, &pPacket->sCSPVP_CREATEROOM, uiPvPMapIndex, uiPvPGameMode ) )
		return ERROR_PVP_MODIFYROOM_FAILED;

	g_pMasterConnection->SendPvPModifyRoom( m_nAccountDBID, static_cast<BYTE>(uiPvPGameMode), pPacket );

	return ERROR_NONE;
}

void CDNUserSession::SetPartyData(TPARTYID PartyID)
{
	CDNParty* pParty = g_pPartyManager->GetParty(PartyID);
	if (!pParty) 
		return;

	pParty->SetCompleteMember(false);		// 마을로 다시 들어왔기 때문에 풀어준다

	bool boLeader = false;
	if (pParty->GetLeaderAccountDBID() == GetAccountDBID())
	{
		boLeader = true;
		m_boPartyLeader = true;
		pParty->SetLeaderSessionID(GetSessionID());
		pParty->SetLeaderAccountDBID(GetAccountDBID());
	}

	int nMemberIndex = pParty->GetPartyMemberIdx(m_nAccountDBID);
	
	if (pParty->CheckCompleteParty())
	{
#if defined( PRE_PARTY_DB )
#else
		g_pPartyManager->PushWaitPartyList(pParty);
#endif // #if defined( PRE_PARTY_DB )
	}

	m_PartyID = PartyID;
	m_boPartyLeader = boLeader;
	m_nPartyMemberIndex = nMemberIndex;

#ifdef _USE_VOICECHAT
	if (pParty->m_nVoiceChannelID > 0)
	{
		TMemberVoiceInfo * pVoice = pParty->GetInitVoiceInfo(GetAccountDBID());
		if (pVoice)
		{
			memcpy(m_nVoiceMutedList, pVoice->nMutedList, sizeof(UINT[PARTYCOUNTMAX]));
			JoinVoiceChannel(pParty->m_nVoiceChannelID, GetIp(), &GetTargetPos(), (int)m_nVoiceRotate, pVoice->cVoiceAvailable == 0 ? false : true);
		}
	}
#endif
}

void CDNUserSession::SetTargetMapIndexByStartStage()
{
	if (m_cGateNo <= 0) return;

	if (g_pDataManager->GetMapType(GetMapIndex()) == GlobalEnum::MAP_VILLAGE)
	{
		if (GetChannelAttribute()&GlobalEnum::CHANNEL_ATT_DARKLAIR){
			SetLastSubVillageMapIndex(GetMapIndex());
		}
		else{
			// GM??
			if ( !(GetChannelAttribute()&(GlobalEnum::CHANNEL_ATT_GM|GlobalEnum::CHANNEL_ATT_DARKLAIR|GlobalEnum::CHANNEL_ATT_FARMTOWN)) )
				SetLastMapIndex( GetMapIndex() );
		}
	}

	SetLastVillageGateNo(m_cGateNo);

	int nTargetMap = GetTargetMapIndex();

	m_bCharOutLog = false;

	if (nTargetMap > 0)
		SetMapIndex(nTargetMap);

	m_cGateNo = -1;
	m_cGateSelect = -1;
}

int CDNUserSession::GetTargetMapIndex()
{
	if (m_cGateNo <= 0) return GetMapIndex();
	return g_pDataManager->GetMapIndexByGateNo(GetMapIndex(), m_cGateNo, m_cGateSelect);
}

void CDNUserSession::ClearPartyInfo()
{
	m_PartyID = 0;
	m_boPartyLeader = false;
	m_nPartyMemberIndex = -1;
}

// PvP
void CDNUserSession::CreatePvPRoom( const MAVIPVP_CREATEROOM* pPacket )
{
}

void CDNUserSession::NextTalk(UINT nNpcObjectID, WCHAR* wszTalkIndex, WCHAR* wszTarget, std::vector<TalkParam>& talkParam )
{
	SendNextTalk(nNpcObjectID, wszTalkIndex, wszTarget, talkParam);
}

void CDNUserSession::ChangeMap(BYTE cVillageID, BYTE cTargetGateNo, int nTargetChannelID, int nTargetMapIndex, char *pIp, USHORT nPort)
{
	if ( (nTargetChannelID <= 0) ||(nTargetMapIndex <= 0)) 
		return;

	m_bCharOutLog = false;

	// _SetLastVillageMapIndex(GetMapIndex());

	LeaveWorld();
	if (m_bIsChannelCounted)
	{
		if (GetChannelID() <= 0) _DANGER_POINT();
		g_pUserSessionManager->DecreaseChannelUserCount(GetChannelID());
		m_bIsChannelCounted = false;
	}

	const sChannelInfo* pTargetChannelInfo	= g_pMasterConnection->GetChannelInfo( nTargetChannelID );
	const sChannelInfo* pChannelInfo		= g_pMasterConnection->GetChannelInfo( GetChannelID() );
	if( pTargetChannelInfo && pChannelInfo )
	{
		bool bUpdate = true;
		if( pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX && pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX )
			bUpdate = false;

		if (pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DARKLAIR)
			SetLastSubVillageMapIndex(nTargetMapIndex);
		else{
			SetLastSubVillageMapIndex(-1);
			if( bUpdate ){
				SetLastMapIndex( GetMapIndex() );

				GlobalEnum::eMapTypeEnum MapType = g_pDataManager->GetMapType(nTargetMapIndex);
				if (MapType == GlobalEnum::MAP_VILLAGE)
					SetLastMapIndex(nTargetMapIndex);
			}
		}
	}

	SetChannelID(nTargetChannelID);
	SetMapIndex(nTargetMapIndex);

#if defined( PRE_FIX_67546 )
	m_pDBCon = g_pDBConnectionManager->GetVillageDBConnection(nTargetChannelID, m_cDBThreadID);
#else
	m_pDBCon = g_pDBConnectionManager->GetDBConnection(nTargetChannelID, m_cDBThreadID);
#endif
	if (m_pDBCon == NULL)
	{
		//이 타이밍에 디비컨이 없으면 괴롭다.....끊어 버린다.
		DetachConnection(L"ChangeMap DBCon NotFound");
		return;
	}

	TPosition Pos = { 0, };
	g_pFieldDataManager->GetStartPosition(nTargetMapIndex, cTargetGateNo, Pos);

	m_BaseData.CurPos = Pos;
	m_BaseData.TargetPos = Pos;

	if (cVillageID == g_Config.nVillageID){
		m_bChangeSameServer = true;
		m_cGateNo = -1;
		m_cGateSelect = -1;
		SendVillageInfo(pIp, nPort, ERROR_NONE, GetAccountDBID(), GetCertifyingKey());	// 장비가 같으므로 바로 정보 날려주면 된다	// 추가 구현 필요 ???(같은 장비의 마을일 경우 재접속 ??? 그렇다면 인증체크 다시 필요 !!!)
	}
	else{
		m_bChangeSameServer = false;

		// 이동할 정보 저장
		_strcpy(m_MoveToVillageInfo.szIP, _countof(m_MoveToVillageInfo.szIP), pIp, (int)strlen(pIp));
		m_MoveToVillageInfo.nPort		= nPort;

		m_eUserState = STATE_READYTOVILLAGE;

		m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();
		DN_ASSERT(0 != m_biCertifyingKey,	"Invalid!");	// 인증키가 0 이 생성되면 않됨 !!!(없음 의미)

		g_pAuthManager->QueryStoreAuth(SERVERTYPE_VILLAGE, this);
	}
}

void CDNUserSession::ChangeMap(BYTE cVillageID, int nTargetChannelID, int nTargetMapIndex, char *pIp, USHORT nPort, bool bPartyinto)
{
	if ((nTargetChannelID <= 0) ||(nTargetMapIndex <= 0)) return;

	m_bCharOutLog = false;

	const sChannelInfo* pTargetChannelInfo = g_pMasterConnection->GetChannelInfo( nTargetChannelID );
	if( !pTargetChannelInfo )
	{
		_DANGER_POINT();
		return;
	}

	TPosition Pos = { 0, };
	if (bPartyinto == false)
	{
#if defined(PRE_ADD_DWC)
		if( pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DWC )
#else
		if( pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP )
#endif
			g_pFieldDataManager->GetStartPosition( nTargetMapIndex, PvPCommon::Common::PvPVillageStartPositionGateNo, Pos );
		else if (pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_FARMTOWN || pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DARKLAIR)
			g_pFieldDataManager->GetStartPosition( nTargetMapIndex, GlobalEnum::DEFAULT_STARTPOSITION_GATENO, Pos );
		else
			g_pFieldDataManager->GetRandomStartPosition(nTargetMapIndex, Pos);
		if (Pos.nX == 0) return;
	}
	else
		m_bPartyInto = true;

	// _SetLastVillageMapIndex(GetMapIndex());

	LeaveWorld();
	if (m_bIsChannelCounted)
	{
		if (GetChannelID() <= 0) _DANGER_POINT();
		g_pUserSessionManager->DecreaseChannelUserCount(GetChannelID());
		m_bIsChannelCounted = false;
	}

	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfo( GetChannelID() );
	if( pTargetChannelInfo && pChannelInfo )
	{
		bool bUpdate = true;
		if( pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX && pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX )
			bUpdate = false;

		if (pTargetChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_DARKLAIR)
			SetLastSubVillageMapIndex(nTargetMapIndex);
		else{
			SetLastSubVillageMapIndex(-1);
			if( bUpdate ){
				SetLastMapIndex( GetMapIndex() );

				GlobalEnum::eMapTypeEnum MapType = g_pDataManager->GetMapType(nTargetMapIndex);
				if (MapType == GlobalEnum::MAP_VILLAGE)
					SetLastMapIndex(nTargetMapIndex);
			}
		}
	}

	SetChannelID(nTargetChannelID);
	SetMapIndex(nTargetMapIndex);

#if defined( PRE_FIX_67546 )
	m_pDBCon = g_pDBConnectionManager->GetVillageDBConnection(nTargetChannelID, m_cDBThreadID);
#else
	m_pDBCon = g_pDBConnectionManager->GetDBConnection(nTargetChannelID, m_cDBThreadID);
#endif
	if (m_pDBCon == NULL)
	{
		//이 타이밍에 디비컨이 없으면 괴롭다.....끊어 버린다.
		DetachConnection(L"ChangeMap DBCon NotFound");
		return;
	}

	if (bPartyinto == false)
	{
		m_BaseData.CurPos = Pos;
		m_BaseData.TargetPos = Pos;
	}

	if (cVillageID == g_Config.nVillageID)
	{
		m_bChangeSameServer = true;
		SendVillageInfo(pIp, nPort, ERROR_NONE, GetAccountDBID(), GetCertifyingKey());	// 장비가 같으므로 바로 정보 날려주면 된다	// 추가 구현 필요 ???(같은 장비의 마을일 경우 재접속 ??? 그렇다면 인증체크 다시 필요 !!!)
	}
	else
	{
		m_bChangeSameServer = false;

		// 이동할 정보 저장
		_strcpy(m_MoveToVillageInfo.szIP, _countof(m_MoveToVillageInfo.szIP), pIp, (int)strlen(pIp));
		m_MoveToVillageInfo.nPort = nPort;

		m_eUserState = STATE_READYTOVILLAGE;

		m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();
		DN_ASSERT(0 != m_biCertifyingKey,	"Invalid!");	// 인증키가 0 이 생성되면 않됨 !!!(없음 의미)

		g_pAuthManager->QueryStoreAuth(SERVERTYPE_VILLAGE, this);
	}
}

void CDNUserSession::ChangePos(int nX, int nY, int nZ, float fLookX, float fLookZ)
{
	LeaveWorld();

	if (m_bIsChannelCounted)
	{
		if (GetChannelID() <= 0) _DANGER_POINT();
		g_pUserSessionManager->DecreaseChannelUserCount(GetChannelID());
		m_bIsChannelCounted = false;
	}

	m_BaseData.CurPos.nX = nX;
	m_BaseData.CurPos.nY = nY;
	m_BaseData.CurPos.nZ = nZ;

	EnterWorld();

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	EtVector3 vPos( GetCurrentPos().nX/1000.f, GetCurrentPos().nY/1000.f, GetCurrentPos().nZ/1000.f );
	EtVector2 vLook( fLookX, fLookZ );

	Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

	SendActorMsg( GetSessionID(), eActor::SC_CMDWARP, Stream.Tell(), Stream.GetBuffer() );
}

void CDNUserSession::ReconnectLogin(short nRet)
{
	if (nRet == ERROR_NONE){
		m_eUserState = STATE_MOVETOLOGIN;
	}

	SendReconnectLogin(nRet, GetAccountDBID(), GetCertifyingKey());
}

//Farm
bool CDNUserSession::ReqFarmInfo(bool bRefreshGate /*=true*/)
{
	if (g_pFarm->GetInitFlag() == false)
		return false;		//로드되어진 농장정보가 없습니다.

	//디비에 농장정보를 요청합니다.
	m_pDBCon->QueryGetListFieldByCharacter( this, bRefreshGate );
	return true;
}

void CDNUserSession::SetLevel(BYTE cLevel, int nLogCode, bool bDBSave)
{
	if (GetLevel() == cLevel) return;	// 같으면 나가고

	if (cLevel > CHARLEVELMAX) cLevel = CHARLEVELMAX;	// 맥스치를 넘어가면 안됨

	if (GetLevel() < cLevel){		// 레벨이 바뀌었다면
		int nSkillPoint = m_pSkill->GetLevelUpSkillPoint( GetLevel(), cLevel );

		// 백섭이 일어나서 레벨에 내려간 경우엔.. 아무 영향도 주지 않도록 해야할까나.
		if( nSkillPoint > 0 ){
			ChangeSkillPoint(nSkillPoint, 0, true, DBDNWorldDef::SkillPointCode::LevelUp, DualSkill::Type::Primary);
			ChangeSkillPoint(nSkillPoint, 0, true, DBDNWorldDef::SkillPointCode::LevelUp, DualSkill::Type::Secondary);
		}

		m_ParamData.cLevel = cLevel;
		m_ParamData.nExp = GetExp();

		SendUserLocalMessage(0, FM_LEVELUP);
		NotifyGuildMemberLevelUp(cLevel);
	}
	BYTE cPreLevel = GetLevel();
	m_UserData.Status.cLevel = cLevel;
	GetEventSystem()->OnEvent( EventSystem::OnLevelUp );

	if (bDBSave){
		m_pDBCon->QueryLevel(this, nLogCode);	// level db저장 20100128
	}	
	// 사제 졸업
	if( m_MasterSystemData.SimpleInfo.iMasterCount > 0 && cLevel >= static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_GraduateLevel )) )
	{
		if( m_pDBCon && m_pDBCon->GetActive() )
			m_pDBCon->QueryMasterSystemGraduate( this );
	}

	if( cPreLevel < cLevel)
	{
		for( BYTE bCurLevel=cPreLevel+1; bCurLevel<=cLevel; ++bCurLevel) //한번에 여러 레벨 건너뛸때도 레벨업 이벤트에 걸리게..
		{
			SendLevelupEventMail(bCurLevel, GetClassID(), GetUserJob());
		}
	}	

#if defined( PRE_ADD_BESTFRIEND )
	if( GetBestFriend()->IsRegistered() )
	{
		sWorldUserState State;
		if (g_pWorldUserState->GetUserState(GetBestFriend()->GetInfo().wszName, GetBestFriend()->GetInfo().biCharacterDBID, &State))
		{
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName(GetBestFriend()->GetInfo().wszName);
			if(pSession)
			{
				pSession->GetBestFriend()->ChangeLevel(cLevel);
			}
			else
			{
				if (g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendLevelUpBestFriend(cLevel, m_pBestFriend->GetInfo().wszName);
			}
		}
	}
#endif
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	if (cPreLevel < cLevel)
	{
		AddTotalLevelSkillLevel(cLevel - cPreLevel);
		SendTotalLevel( m_pSession->GetSessionID(), GetTotalLevelSkillLevel() );
	}
#endif
	const TGuildUID GuildUID = GetGuildUID();
	if (GuildUID.IsSet())
	{
		CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
		if (pGuild) 
		{
#ifdef PRE_ADD_BEGINNERGUILD
			if (cLevel >=(int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::BeginnerGuild_GraduateLevel))
			{
#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if (TRUE == pGuild->IsEnable())
				{
					if (pGuild->GetInfo()->cGuildType == BeginnerGuild::Type::Beginner)
					{
						if (m_pDBCon)
							m_pDBCon->QueryDelGuildMember(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), GetAccountDBID(), GetCharacterDBID(), GuildUID.nDBID, GetLevel(), g_Config.nWorldSetID, false, true);
					}
				}
#else		//#if !defined( PRE_ADD_NODELETEGUILD )
				if (pGuild->GetInfo()->cGuildType == BeginnerGuild::Type::Beginner)
				{
					if (m_pDBCon)
						m_pDBCon->QueryDelGuildMember(m_cDBThreadID, GetAccountDBID(), GetCharacterDBID(), GetAccountDBID(), GetCharacterDBID(), GuildUID.nDBID, GetLevel(), g_Config.nWorldSetID, false, true);
				}
#endif		//#if !defined( PRE_ADD_NODELETEGUILD )
			}
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined(PRE_ADD_JOINGUILD_SUPPORT)
			if (GetDBConnection() && bDBSave)
				GetDBConnection()->QueryGuildSupportRewardInfo(this);
#endif		//#if defined(PRE_ADD_JOINGUILD_SUPPORT)
		}
		else
			_DANGER_POINT();
	}

#if defined(_KRAZ)
	m_pDBCon->QueryActozUpdateCharacterInfo(m_pSession, ActozCommon::UpdateType::Levelup);
#endif	// #if defined(_KRAZ)
}

#if defined(PRE_ADD_EXPUP_ITEM)
void CDNUserSession::ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey, bool bAbsolute)
{
	CDNUserBase::ChangeExp(nChangeExp, nLogCode, biFKey, bAbsolute);

	if (GetPartyID() > 0)
		g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
}
#else
void CDNUserSession::ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey)
{
	CDNUserBase::ChangeExp(nChangeExp, nLogCode, biFKey);

	if (GetPartyID() > 0)
		g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
}
#endif

void CDNUserSession::SetExp(UINT nExp, int nLogCode, INT64 biFKey, bool bDBSave)
{
	CDNUserBase::SetExp(nExp, nLogCode, biFKey, bDBSave);

	BYTE cLevel = g_pDataManager->GetLevel(GetClassID(), GetExp());
	if (cLevel <= 0) cLevel = 1;
	if (cLevel > CHARLEVELMAX) cLevel = CHARLEVELMAX;

	if (cLevel != GetLevel()){		// 현재 레벨보다 올랐다면
		int nLevelLog = DBDNWorldDef::CharacterLevelChangeCode::Normal;
		switch(nLogCode)
		{
		case DBDNWorldDef::CharacterExpChangeCode::Cheat: nLevelLog = DBDNWorldDef::CharacterLevelChangeCode::Cheat; break;
		case DBDNWorldDef::CharacterExpChangeCode::Admin: nLevelLog = DBDNWorldDef::CharacterLevelChangeCode::Admin; break;
		}

		SetLevel(cLevel, nLevelLog, bDBSave);	// 여기서 db저장
	}
	if (GetPartyID() > 0)
		g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
}

void CDNUserSession::QueryGetMasterSystemCountInfo( bool bClientSend )
{
	if( CheckDBConnection() == false)
		return;

	CDNParty* pParty = g_pPartyManager->GetParty( GetPartyID() );
	if( pParty == NULL )
		return;

	std::vector<INT64> vPartyListExceptMe;
	pParty->GetMemberCharacterDBIDInfo( vPartyListExceptMe, GetCharacterDBID() );

	GetDBConnection()->QueryGetMasterSystemCountInfo( m_cDBThreadID, this, bClientSend, vPartyListExceptMe );
}

// PvP
bool CDNUserSession::bIsPvPRoomListRefreshTime()
{
	if( timeGetTime()-m_dwPvPRoomListRefreshTime >= PvPCommon::Common::RoomListRefreshGapTime )
		return true;

	return false;
}

bool CDNUserSession::bIsLadderUser()
{
#if defined( _WORK )
	LadderSystem::CRoom* pRoom = LadderSystem::CManager::GetInstance().GetRoomPtr( GetCharacterName() );
	_ASSERT( pRoom ? m_bIsLadderUser == true : m_bIsLadderUser == false );
#endif // #if defined( _WORK )
	return m_bIsLadderUser;
}

void CDNUserSession::SetUserJob(BYTE cJob)
{
	CDNUserBase::SetUserJob(cJob);

	if (GetPartyID() > 0)
		g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);

	m_ParamData.nJob = cJob;
	SendUserLocalMessage(0, FM_CHANGEJOB);
}

void CDNUserSession::SetHide(bool bHide)
{
	m_bHide = bHide;
	m_ParamData.bHide = m_bHide;
	SendUserLocalMessage(0, FM_CHANGEHIDE);
}

void CDNUserSession::SetDefaultMaxFatigue(bool bSend)
{
	CDNUserBase::SetDefaultMaxFatigue(bSend);

	if (GetPartyID() > 0)
		g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
}

void CDNUserSession::SetDefaultMaxWeeklyFatigue(bool bSend)
{
	CDNUserBase::SetDefaultMaxWeeklyFatigue(bSend);

	if (GetPartyID() > 0)
		g_pPartyManager->SendPartyMemberPart(GetPartyID(), this);
}

void CDNUserSession::SetDefaultMaxRebirthCoin(bool bSend)
{
	CDNUserBase::SetDefaultMaxRebirthCoin(bSend);

	if (bSend)
		SendRebirthCoin(ERROR_NONE, 0, _REBIRTH_SELF, GetSessionID());
}

void CDNUserSession::IncreaseFatigue(int nGap)
{
	CDNUserBase::IncreaseFatigue(nGap);

	SendFatigue(GetSessionID(), GetFatigue(), GetWeeklyFatigue(), GetPCBangFatigue(), GetEventFatigue(), GetVIPFatigue());
}

// Guild
void CDNUserSession::RefreshGuildSelfView()
{
	SendUserLocalMessage(0, FM_CHANGEGUILDSELFVIEW);
}

int CDNUserSession::CmdSelectChannel( const int iChannelID, bool bSkipVerifyChannelID/*=false*/ )
{
	if( !m_pField || !g_pMasterConnection )	
		return ERROR_NONE;

	//이동할 채널을 골랐씨유 이동할 채널이 가능한 채널인지 확인해 봅쉬다
	if( iChannelID != m_pField->GetChnnelID())
	{
		// 이동할 채널 정보 얻어온다.
		const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfo( iChannelID );
		if( !pChannelInfo )
		{
			_DANGER_POINT();
			SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);
			return ERROR_NONE;
		}

		int nVerifyMapIndex = m_pField->GetMapIndex();
		//if( m_pField->bIsPvPVillage() || m_pField->bIsGMVillage() || pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_GM )
		if( m_pField->bIsUseLastMapIndex() || pChannelInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_GM )
			nVerifyMapIndex = pChannelInfo->nMapIdx;

		// PvP마을,다크레어마을, 농장마을로 이동하는거면 VerifyChannelID Skip 하고 PvP마을 MapIdx 설정해준다.
#if defined(PRE_ADD_DWC)
		if( bSkipVerifyChannelID || pChannelInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_DARKLAIR|GlobalEnum::CHANNEL_ATT_FARMTOWN|GlobalEnum::CHANNEL_ATT_DWC) )
#else
		if( bSkipVerifyChannelID || pChannelInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_DARKLAIR|GlobalEnum::CHANNEL_ATT_FARMTOWN) )
#endif
		{
			bSkipVerifyChannelID	= true;
			nVerifyMapIndex			= pChannelInfo->nMapIdx;
		}

		//다른 채널로 가는거 맞습니다아~ 그런채널이 있는지 맵인덱스가 같은지 확인 먼저 고고싱
		if( bSkipVerifyChannelID || g_pMasterConnection->VerifyChannelID( iChannelID, nVerifyMapIndex ) )
		{
			if (GetPartyID() > 0)
			{
#if defined( PRE_PARTY_DB )
				return ERROR_NONE;
#else
				CDNParty * pParty = g_pPartyManager->GetParty(GetPartyID());
				if (!pParty)
				{
					_DANGER_POINT();
					SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);
					return ERROR_NONE;
				}

				if( g_pPartyManager->DelPartyMember( pParty, this ) != ERROR_NONE )
				{
					_DANGER_POINT();
					SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);
					return ERROR_NONE;
				}
#endif // #if defined( PRE_PARTY_DB )
			}

			if (pChannelInfo->cVillageID == g_Config.nVillageID && pChannelInfo->nMapIdx == GetMapIndex() )
			{
				//새로운 미들웨어에서는 변경 시점에서 DbCon과 threadid를 다시 받아와야 합니다.
				//이때 타 프로세스(빌리지)로 이동시에는 상관이 없지만 같은 프로세스 인경우 디비 미들웨어 처리가 다 끝나고 나서
				//dbcon을 바꿔야 하므로...
				if (m_pDBCon && m_pDBCon->GetActive())
				{
					m_pDBCon->QueryMsgAdj(this, iChannelID, nVerifyMapIndex, m_cGateNo);
					m_eUserState = STATE_MOVESAMECHANNEL;
				}
				else
					SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);

			}
			else
			{
				if (g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE, GetAccountDBID(), nVerifyMapIndex, m_cGateNo, iChannelID );
				else
					SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);
			}
			return ERROR_NONE;
		}
		else
			_DANGER_POINT();
	}
	else	
		_DANGER_POINT();

	SendMoveChannelFail(ERROR_PARTY_MOVECHANNELFAIL);
	return ERROR_NONE;
}

void CDNUserSession::BroadcastingEffect(char cType, char cState)
{
	m_ParamData.cType = cType;
	m_ParamData.cState = cState;

	SendUserLocalMessage(0, FM_EFFECT);
}

void CDNUserSession::BroadcastingChatRoom( int nSubCmd, UINT nChatRoomParam )
{
	switch( nSubCmd )
	{
	case eChatRoom::SC_CHATROOMVIEW:
		{
			m_ParamData.nChatRoomParam = nChatRoomParam;	// Param값은 방번호(Param==0 은 방 삭제)
			SendUserLocalMessage( 0, FM_CHATROOMVIEW );
		}
		break;
	case eChatRoom::SC_ENTERUSERCHATROOM:
		{
			m_ParamData.nChatRoomParam = nChatRoomParam;
			SendUserLocalMessage( 0, FM_CHATROOMUSER );			// Param값은 방번호
		}
		break;
	case eChatRoom::SC_LEAVEUSER:
		{
			m_ParamData.nChatRoomParam = nChatRoomParam;	// Param값은 음수로 퇴장 사유
			SendUserLocalMessage( 0, FM_CHATROOMUSER );
		}
		break;
	}
}

void CDNUserSession::StartCostumeMix(int nInvenType, INT64 biInvenSerial)
{
	if( !IsNoneWindowState())
	{
		SendCosMixOpen(ERROR_ITEM_COSMIX_OPEN_FAIL);
		return;
	}

	const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
	if(mgr.IsEnableCostumeMix() == false)
	{
		SendCosMixOpen(ERROR_ITEM_COSMIX_OPEN_FAIL);
		return;
	}

	if(m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
	{
		SendCosMixOpen(ERROR_ITEM_COSMIX_OPEN_FAIL);
		return;
	}
	SetWindowState(WINDOW_COSMIX);
	SendCosMixOpen(ERROR_NONE);

	// m_nCosMixInvenTypeCache = nInvenType;
	m_biCosMixSerialCache = biInvenSerial;
	return;
}

int CDNUserSession::OnEndCostumeMix()
{
	IsWindowStateNoneSet(WINDOW_COSMIX);

	m_biCosMixSerialCache = 0;
	// m_nCosMixInvenTypeCache = 0;

	return ERROR_NONE;
}

void CDNUserSession::StartCostumeDesignMix(int nInvenType, int nInvenIndex, INT64 biInvenSerial)
{
	if( !IsNoneWindowState() )
	{
		SendCosDesignMixOpen(ERROR_ITEM_DESIGNMIX_OPEN_FAIL);
		return;
	}

	const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
	if(mgr.IsEnableCostumeDesignMix() == false)
	{
		SendCosDesignMixOpen(ERROR_ITEM_DESIGNMIX_OPEN_FAIL);
		return;
	}

	if(m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
	{
		SendCosDesignMixOpen(ERROR_ITEM_DESIGNMIX_OPEN_FAIL);
		return;
	}
	SetWindowState(WINDOW_DESIGNMIX);

	m_biCosDesignMixSerialCache = biInvenSerial;

	SendCosDesignMixOpen(ERROR_NONE);
	return;
}

void CDNUserSession::ResetCostumeDesignCache()
{
	m_biCosDesignMixSerialCache = 0;
}

int CDNUserSession::OnEndCostumeDesignMix()
{
	IsWindowStateNoneSet(WINDOW_DESIGNMIX);
	ResetCostumeDesignCache();

	return ERROR_NONE;
}

#ifdef PRE_ADD_COSRANDMIX

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
void CDNUserSession::StartCostumeRandomMix(CostumeMix::RandomMix::eOpenType openType, int nInvenType, INT64 biInvenSerial)
#else
void CDNUserSession::StartCostumeRandomMix(int nInvenType, INT64 biInvenSerial)
#endif
{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	bool bInvalidState = false;
	if (openType == CostumeMix::RandomMix::OpenByItem)
	{
		if (!IsNoneWindowState())
			bInvalidState = true;
	}
	else
	if (openType == CostumeMix::RandomMix::OpenByNpc)
	{
		if (!IsWindowState(WINDOW_BLIND))
			bInvalidState = true;
	}

	if (bInvalidState)
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL, CostumeMix::RandomMix::OpenFail);
		return;
	}

	const CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
	if (mgr.IsEnableCosRandomMix() == false)
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL, CostumeMix::RandomMix::OpenFail);
		return;
	}
#else
	if( !IsNoneWindowState() )
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL);
		return;
	}

	const CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
	if (mgr.IsEnableCosRandomMix() == false)
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL);
		return;
	}
#endif // PRE_ADD_COSRANDMIX_ACCESSORY

	if (m_pRestraint->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
		return;

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	if (openType == CostumeMix::RandomMix::OpenFail)
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL, openType);
		return;
	}

	m_CosRandMixOpenType = openType;
#endif

#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	if (openType == CostumeMix::RandomMix::OpenByItem)
	{
		const TItem *pItem = m_pItem->GetCashInventory(biInvenSerial);
		if (pItem == NULL)
		{
			SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL, openType);
			return;
		}

		const TItemData *pCurItemData = g_pDataManager->GetItemData(pItem->nItemID);
		if (pCurItemData == NULL)
		{
			SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL, openType);
			return;
		}

		m_nCosRandMixerEnablePartsType = pCurItemData->nTypeParam[0];
	}
	#else // PRE_ADD_COSRANDMIX_ACCESSORY
	const TItem *pItem = m_pItem->GetCashInventory(biInvenSerial);
	if (pItem == NULL)
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL);
		return;
	}

	const TItemData *pCurItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (pCurItemData == NULL)
	{
		SendCosRandomMixOpen(ERROR_ITEM_RANDOMMIX_OPEN_FAIL);
		return;
	}

	m_nCosRandMixerEnablePartsType = pCurItemData->nTypeParam[0];
	#endif // PRE_ADD_COSRANDMIX_ACCESSORY
#endif

	SetWindowState(WINDOW_RANDOMMIX);

	m_biCosRandomMixSerialCache = biInvenSerial;

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	SendCosRandomMixOpen(ERROR_NONE, openType);
#else
	SendCosRandomMixOpen(ERROR_NONE);
#endif
	return;
}

int CDNUserSession::OnEndCostumeRandomMix()
{
	IsWindowStateNoneSet(WINDOW_RANDOMMIX);
	ResetCostumeRandomMixCache();

	return ERROR_NONE;
}

void CDNUserSession::ResetCostumeRandomMixCache()
{
	m_biCosRandomMixSerialCache = 0;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	m_CosRandMixOpenType = CostumeMix::RandomMix::OpenFail;
#endif
#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	m_nCosRandMixerEnablePartsType = CostumeMix::RandomMix::MixerCanMixPartsAll;
#endif
}
#endif // PRE_ADD_COSRANDMIX

void CDNUserSession::CheckValidCollisionHeight()
{
	if( m_pField == NULL )
		return;

	CDNFieldData* pFieldData = m_pField->GetFieldData();
	if(!pFieldData)
		return;

	if(GetMapIndex() != pFieldData->GetMapIndex())
	{
		if(m_nAbuseLogSendCount % 10 == 0)
			g_Log.Log(LogType::_ERROR, L"CheckValidCollisionHeight - Wrong MapIndex [ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [Map:%d<>%d][Channel:%d<>%d]", 
						GetAccountDBID(), GetCharacterDBID(), GetSessionID(), m_BaseData.nObjectID, GetMapIndex(), pFieldData->GetMapIndex(), m_BaseData.nChannelID, m_pField->GetChnnelID());
	
		m_nAbuseLogSendCount++;
		return;
	}

	DWORD dwTileSize = pFieldData->GetCurrentTileSize();

	float fX = (float)GetTargetPos().nX / 1000.0f;
	float fY = (float)GetTargetPos().nY / 1000.0f;
	float fZ = (float)GetTargetPos().nZ / 1000.0f;

	int nTileMax = 3;
	int nTileGap = 2;
	int nMaxJump = 1000;

	float fOffSetX = fX -(dwTileSize * nTileGap);
	float fOffSetZ = fZ -(dwTileSize * nTileGap);

	float fPositionX = 0;
	float fPositionZ = 0;

	float fMaxHeight = FLT_MIN;
	for (int i=0; i<nTileMax; i++)
	{
		fPositionX = fOffSetX +(dwTileSize *i);
		for (int j=0; j<nTileMax; j++)
		{
			fPositionZ = fOffSetZ +(dwTileSize *j);

			float fTemp = pFieldData->GetCollisionHeight(fPositionX, fPositionZ) + nMaxJump;
			if( fTemp > fMaxHeight ) fMaxHeight = fTemp;
		}
	}

	if( fY > fMaxHeight )
	{
		if(m_nAbuseLogSendCount % 10 == 0)
		{
			WCHAR wszBuf[100];
			swprintf( wszBuf, L"ABOVE MAX HEIGHT [Map:%d<>%d][Channel:%d<>%d][X:%0.2f][Y:%0.2f][Z:%0.2f][Max:%0.2f]", GetMapIndex(), pFieldData->GetMapIndex(), m_BaseData.nChannelID, m_pField->GetChnnelID(), fX, fY, fZ, fMaxHeight);
			GetDBConnection()->QueryAddAbuseLog(this, ABUSE_MOVE_INSKY, wszBuf);
			m_nAbuseLogSendCount++;
		}
	}

	//OutputDebug("ABOVE MAX HEIGHT [Map:%d<>%d][Channel:%d<>%d][X:%0.2f][Y:%0.2f][Z:%0.2f][Max:%0.2f]\n", GetMapIndex(), pFieldData->GetMapIndex(), m_BaseData.nChannelID, m_pField->GetChnnelID(), fX, fY, fZ, fMaxHeight);
}

void CDNUserSession::GuildWarReset()
{
	CDNUserBase::GuildWarReset();
	m_nGuildWarPoint = 0;
	m_bGuildWarStats = false;		// 정보 로딩 여부
	m_bGuildWarVote = false;
	m_bGuildWarVoteLoading = false;
	memset(&m_sMyGuildWarMissionRaningInfo, 0, sizeof(m_sMyGuildWarMissionRaningInfo));
	memset(&m_bRewardResults, 1, sizeof(m_bRewardResults));
	m_GuildWarBuyedItem.clear();
}

void CDNUserSession::SetGuildWarMissionRankingInfo(TAGetGuildWarPointPartTotal* pGetGuildWarPointPartTotal)
{
	for (int i=0; i<GUILDWAR_RANKINGTYPE_MAX; ++i)
	{
		m_sMyGuildWarMissionRaningInfo[i].wRanking = pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].wRanking;
		m_sMyGuildWarMissionRaningInfo[i].nTotalPoint = pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].nPoint;
	}
	m_bGuildWarStats = true;
}

void CDNUserSession::SetGuildWarRewardResults(bool* pRewardResults )
{
	for( char i=0; i<GUILDWAR_REWARD_CHAR_MAX; ++i)
	{
		m_bRewardResults[i] = pRewardResults[i];
	}
}

void CDNUserSession::SetGuildWarRewardResultIndex(char cRewardType, bool bResult)
{
	if( cRewardType >= GUILDWAR_REWARD_CHAR_MAX || cRewardType < GUILDWAR_REWARD_CHAR_PRESENT )
		return;
	m_bRewardResults[cRewardType] = bResult;
}

char CDNUserSession::GetGuildWarRewardEnable()
{
	for( char i=0; i<GUILDWAR_REWARD_CHAR_MAX; ++i)
	{
		if( m_bRewardResults[i] == false )		 //보상을 못받은게 하나라도 있으면..
			return i;
	}
	return GUILDWAR_REWARD_CHAR_MAX;
}

bool CDNUserSession::IsGuildWarReward()
{
	for( char i=0; i<GUILDWAR_REWARD_CHAR_MAX; ++i)
	{
		if( m_bRewardResults[i] == false )		 //보상을 못받은게 하나라도 있으면..
			return true;
	}
	return false;
}

void CDNUserSession::AddGuildWarBuyedItem(int itemID, int count)
{
	std::map<int, int>::iterator it = m_GuildWarBuyedItem.find(itemID);
	if (it == m_GuildWarBuyedItem.end())
		m_GuildWarBuyedItem.insert(std::map<int, int>::value_type(itemID, count));
	else
		it->second = it->second + count;
}

int CDNUserSession::GetGuildWarBuyedItem(int itemID) const
{
	std::map<int, int>::const_iterator it = m_GuildWarBuyedItem.find(itemID);
	if (it == m_GuildWarBuyedItem.end())
		return 0;

	return it->second;
}

void CDNUserSession::ResetGuildWarBuyedItems()
{
	m_GuildWarBuyedItem.clear();
}


#ifdef PRE_ADD_CHANGEJOB_CASHITEM
bool CDNUserSession::CheckAndCalcParallelChangeJob( /*IN*/ int iJobID, /*OUT*/ int& iFirstJobID,  /*OUT*/ int& iSecondJobID )
{
#ifdef PRE_ADD_JOBCHANGE_RESTRICT_JOB
	DNTableFileFormat* pJobTable = GetDNTable( CDnTableDB::TJOB );
	CDNTableFile::Cell* pServiceCell = pJobTable->GetFieldFromLablePtr( iJobID, "_Service" );
	CDNTableFile::Cell* pJobChangeAvailCell = pJobTable->GetFieldFromLablePtr( iJobID, "_JobChangeAble" );

	if( pServiceCell == NULL || pJobChangeAvailCell == NULL )
	{
		return false;
	}
	else
	{
		if( pServiceCell->GetInteger() <= 0 || pJobChangeAvailCell->GetInteger() <= 0 )
			return false;
	}
#endif // PRE_ADD_JOBCHANGE_RESTRICT_JOB

	bool bResult = false;

	iFirstJobID = 0;		// 결과값으로 넘겨줄 첫번째 직업 ID
	iSecondJobID = 0;		// 결과값으로 넘겨줄 두번째 직업 ID

	// 현재 직업의 단계값과 루트 직업을 얻어옴.
	int iNowJob = this->GetUserJob();
	int iNowJobDegree = g_pDataManager->GetJobNumber( iNowJob );
	int iBaseClassID = g_pDataManager->GetBaseClass( iNowJob );
	
	int iJobIDToChange = iJobID;

	TJobTableData* pJobDataToChange = g_pDataManager->GetJobTableData( iJobIDToChange );
	if( pJobDataToChange )
	{ 
		// 전직 아이템 사용으로 바꾸려는 직업은 같은 차수의 직업이어야만 한다.
		int iBaseClassIDToChangeJob =  g_pDataManager->GetBaseClass( iJobIDToChange );
		if( iBaseClassID == iBaseClassIDToChangeJob )
		{
			int iJobDeepToChange = g_pDataManager->GetJobNumber( iJobIDToChange );
			if( iJobDeepToChange == iNowJobDegree )
			{
				bResult = true;

				if( 1 == iNowJobDegree )
				{
					iFirstJobID = iJobIDToChange;
				}
				else
				if( 2 == iNowJobDegree )
				{
					iFirstJobID = g_pDataManager->GetParentJob( iJobIDToChange );
					iSecondJobID = iJobIDToChange;
				}
			}
			else
			{
#ifdef _WORK
				// 바꾸고자 하는 직업의 부모 직업이 현재 직업이 아님.
				wstring wszString = FormatW(L"다른 차수의 직업으로 변경하려고 합니다. 불가능!!\r\n");
				this->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif // #ifdef _WORK
			}
		}
		else
		{
#ifdef _WORK
			// 바꾸고자하는 직업이 다른 클래스임. 못바꿈.
			wstring wszString = FormatW(L"다른 클래스의 직업으로 바꿀 수 없습니다!!\r\n");
			this->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif // #ifdef _WORK
		}
	}

	if( false == bResult )
	{
#ifdef _WORK
		wstring wszString = FormatW(L"잘못된 Job ID 입니다..\r\n");
		this->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif // #ifdef _WORK
	}

	return bResult;
}
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM


void CDNUserSession::WritePingLog()
{
	if( m_pairTcpPing.second == 0 )
		return;

	g_Log.Log( LogType::_PING, this, L"[%d] IP=%S TcpPing=%dms(%d/%d)\r\n", g_Config.nManagedID, GetIp(), m_pairTcpPing.first/m_pairTcpPing.second, m_pairTcpPing.first, m_pairTcpPing.second );
}


void CDNUserSession::UseExpandSkillPage()
{
	int nSkillArray[DEFAULTSKILLMAX] = { 0, };
	int nSkillPoint = GetSkill()->GetLevelUpSkillPoint( 1, GetLevel());
	SetSkillPoint( nSkillPoint, DualSkill::Type::Secondary );
	g_pDataManager->GetCreateDefaultSkill( GetClassID(), nSkillArray );

	GetDBConnection()->QueryUseExpandSkillPage(m_pSession, nSkillArray, nSkillPoint );
}

int CDNUserSession::TryWarpVillage(int nMapIndex, INT64 nItemSerial)
{
	if (!g_pMasterConnection || !g_pMasterConnection->GetActive())
		return ERROR_GENERIC_MASTERCON_NOT_FOUND;

	g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE, GetAccountDBID(), nMapIndex, -1, -1, NULL, nItemSerial);

	return ERROR_NONE;
}

void CDNUserSession::WarpVillage(MAVITargetVillageInfo* pPacket)
{
	SetWindowState(WINDOW_NONE);
	if (!DeleteWarpVillageItemByUse(pPacket->nItemSerial))
		return;

	ChangeMap(pPacket->cVillageID, pPacket->nTargetChannelID, pPacket->nTargetMapIdx, pPacket->szIP, pPacket->nPort, pPacket->cPartyInto <= 0 ? false : true);
}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
void CDNUserSession::ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType, bool bSendDB, bool bUseItem )
{
	CDNParty* pParty = g_pPartyManager->GetParty( GetPartyID() );

	bool bBuff = false;
	
	if(pParty && pItemData && pItemData->nTypeParam[0] == EffectSkillNameSpace::BuffType::Partybuff )
	{
		pParty->ApplyPartyEffectSkillItemData( pPacket, pItemData, nUseSessionID, eType, bSendDB, bUseItem );		
		bBuff = true;
	}				
	
	if( !bBuff && GetItem() )
	{
		GetItem()->ApplyPartyEffectSkillItemData( pPacket, pItemData, GetSessionID(), eType, true, true );
	}

	if( pParty )
	{
		pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
	}
}
#endif

#if defined( PRE_FIX_BUFFITEM )
void CDNUserSession::DelPartyEffectSkillItemData()
{
	if( GetPartyID() > 0 )
	{
		CDNParty * pParty = g_pPartyManager->GetParty(GetPartyID());
		if (pParty)
			pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
	}	
}
#endif

#if defined(PRE_FIX_62281)
#if defined(PRE_PARTY_DB)
int CDNUserSession::CheckPartyInvitableState(const WCHAR * pInviterName, int nPartyMinLevel)
#else
int CDNUserSession::CheckPartyInvitableState(const WCHAR * pInviterName, int nPartyMinLevel, int nPartyMaxLevel)
#endif
{
	if( m_eUserState != STATE_NONE )	//대상이 이동중이거나 레디 등등의 상태이면 초대 불가하다
		return ERROR_PARTY_INVITEFAIL_DEST_USER_NOWREADY;
	//상대방의 게임옵션을 확인합니다.
	if( IsAcceptAbleOption(GetCharacterDBID(), GetAccountDBID(), _ACCEPTABLE_CHECKTYPE_PARTYINVITE) == false )
		return ERROR_PARTY_INVITEFAIL;
	//상대방이 나를 차단했는지 알아 봅니다
	if( GetIsolate()->IsIsolateItem(pInviterName) )
		return ERROR_PARTY_INVITEFAIL;
	// GM이면 없는 사람처럼 해야함
	if( (GetAccountLevel() >= AccountLevel_New) && (GetAccountLevel() <= AccountLevel_Master))
		return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;

	const sChannelInfo* pUserChInfo = g_pMasterConnection->GetChannelInfo(GetChannelID());
	if( pUserChInfo == NULL )
		return ERROR_PARTY_INVITEFAIL;

#if defined(PRE_ADD_DWC)
	if( pUserChInfo->nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY|GlobalEnum::CHANNEL_ATT_DWC) )
#else
	if( pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVP || pUserChInfo->nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY)
#endif
		return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;

	if( !IsNoneWindowState())
		return ERROR_PARTY_INVITEFAIL_DESTUSER_SITUATION_NOTALLOWED;

	if( GetPartyID() > 0)
		return ERROR_PARTY_INVITEFAIL_ALREADY_HAS_PARTY;

	if( GetLevel() < nPartyMinLevel)
		return ERROR_PARTY_INVITEFAIL_CHARACTERLEVEL_SHORTAGE;

#if defined( PRE_PARTY_DB )
#else
	if( GetLevel() > nPartyMaxLevel)
		return ERROR_PARTY_JOINFAIL_CHARACTERLEVEL_OVER;
#endif // #if defined( PRE_PARTY_DB )

	return ERROR_NONE;
}
#endif

#if defined( PRE_ADD_NEWCOMEBACK )
void CDNUserSession::CheckComebackRewardItem( TLevelupEvent ComebackReward )
{
	if( ComebackReward.nMailID > 0)
	{
		TMailTableData *pData = g_pDataManager->GetMailTableData( ComebackReward.nMailID );
		if (pData)
		{
			TItem NewItem = {0};
			for( int i=0; i<MAILATTACHITEMMAX; i++ ) 
			{
				if( pData->ItemIDArr[i] > 0 && pData->ItemCountArr[i] > 0 )
				{
					TItemData* pItemData = g_pDataManager->GetItemData(pData->ItemIDArr[i]);
					if( pItemData )
					{
						if( pItemData->IsCash )
						{											
							g_Log.Log(LogType::_COMEBACK, this, L"AccountDBID[%d] ComebackUser!! Item CashType Failed!! ItemID[%d]\n", GetAccountDBID(), pItemData->nItemID);
							break;
						}
						if( GetItem()->CreateInvenItem1( pItemData->nItemID, pData->ItemCountArr[i], -1, -1, DBDNWorldDef::AddMaterializedItem::COMEBACKREWARD, 0 ) )
						{
							g_Log.Log(LogType::_COMEBACK, this, L"AccountDBID[%d] ComebackUser!! Inven Item Failed!! ItemID[%d]\n", GetAccountDBID(), pItemData->nItemID);
						}
					}

				}
			}
		}
	}

	if( ComebackReward.nCashMailID > 0)
	{
		TMailTableData *pData = g_pDataManager->GetMailTableData( ComebackReward.nCashMailID );
		if (pData)
		{
			for( int i=0; i<MAILATTACHITEMMAX; i++ ) 
			{
				if( pData->ItemSNArr[i] > 0 )
				{
					int nItemID = g_pDataManager->GetCashCommodityItem0(pData->ItemSNArr[i]);
					int nItemCount = g_pDataManager->GetCashCommodityCount(pData->ItemSNArr[i]);
					int nPeriod = g_pDataManager->GetCashCommodityPeriod(pData->ItemSNArr[i]);
					TItemData* pItemData = g_pDataManager->GetItemData(nItemID);
					if( pItemData && nItemCount > 0 )
					{
						if( !pItemData->IsCash )
						{
							g_Log.Log(LogType::_COMEBACK, this, L"AccountDBID[%d] ComebackUser!! Item NotCashType Failed!! ItemID[%d]\n", GetAccountDBID(), pItemData->nItemID);
							break;
						}
						if( GetItem()->CreateCashInvenItem( nItemID, nItemCount, DBDNWorldDef::AddMaterializedItem::COMEBACKREWARD, -1, nPeriod, 0, pItemData->nItemID ) )
						{
							g_Log.Log(LogType::_COMEBACK, this, L"AccountDBID[%d] ComebackUser!! CashItem Error!! ItemID[%d]\n", GetAccountDBID(), pItemData->nItemID);
						}
					}									
				}
			}							
		}
	}	
}
void CDNUserSession::SetComebackAppellation( int nAppelationID )
{
	if( m_nComebackAppellation != nAppelationID )
	{
		int nPrevComebackAppellation = m_nComebackAppellation;
		m_nComebackAppellation = nAppelationID;

		GetDBConnection()->QueryModMemberComebackInfo( this, GetPartyID() );

		if( nPrevComebackAppellation > 0 )
		{			
			// 기존 버프 삭제
			if( GetPartyID() > 0 )
			{
				CDNParty* pParty = g_pPartyManager->GetParty( GetPartyID() );
				if( pParty && pParty->bIsComebackParty() )
				{
					pParty->DelPartyMemberAppellation( nPrevComebackAppellation );
				}
			}
			else
			{
				TAppellationData *pAData = g_pDataManager->GetAppellationData( nPrevComebackAppellation );
				if( pAData )
				{
					TItemData* pBuffItemData = g_pDataManager->GetItemData( pAData->nSkillItemID );
					if( pBuffItemData )
					{
						if( GetItem() )
						{

							TEffectSkillData* EffectSkill = GetItem()->GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pBuffItemData->nSkillID );
							if( EffectSkill )
							{
								GetItem()->DelEffectSkillItem( pBuffItemData->nSkillID );
								SendDelEffectSkillItemData( GetSessionID(), pBuffItemData->nItemID );
							}			
						}
					}
				}				
			}
		}		

		if( m_nComebackAppellation > 0 )
		{
			// 버프 적용 및 파티 상태 변경
			TAppellationData *pAData = g_pDataManager->GetAppellationData( m_nComebackAppellation );
			if( pAData )
			{
				if( pAData->nSkillItemID > 0 )
				{
					CSUseItem pPacket;
					memset(&pPacket, 0, sizeof(pPacket));
					pPacket.biInvenSerial = GetItem()->MakeItemSerial();
					TItemData* pBuffItemData = g_pDataManager->GetItemData( pAData->nSkillItemID );
					if( pBuffItemData )
					{
						if( GetPartyID() > 0 )
						{
							CDNParty* pParty = g_pPartyManager->GetParty( GetPartyID() );
							if( pParty )
							{								
								pParty->SetComebackParty(true);
								pParty->ApplyPartyEffectSkillItemData(&pPacket, pBuffItemData, m_pSession->GetSessionID(), EffectSkillNameSpace::ShowEffectType::NONEEFFECT, false, true);
								pParty->SendAllRefreshParty(PARTYREFRESH_NONE);;
							}
						}
						else
						{
							m_pSession->GetItem()->ApplyPartyEffectSkillItemData(&pPacket, pBuffItemData, m_pSession->GetSessionID(), EffectSkillNameSpace::ShowEffectType::NONEEFFECT, false, true);
						}
					}					
				}
			}
		}
		else
		{
			if( GetPartyID() > 0 )
			{
				CDNParty* pParty = g_pPartyManager->GetParty( GetPartyID() );
				if( pParty )
					pParty->SendAllRefreshParty(PARTYREFRESH_NONE);;
			}
		}
	}
}
#endif

#if defined(PRE_ADD_TRANSFORM_POTION)
void CDNUserSession::CalcTransformExpire(DWORD CurTick)
{
	// 변신 해제용..
	if( m_nTransformID > 0 && m_dwTransformTick<=CurTick)
	{
		// 해제
		m_dwTransformTick = 0;
		m_nTransformID = 0;
		SendUserLocalMessage(0, FM_CHANGE_TRANSFORM);
	}
}

void CDNUserSession::SetTransformID(int nTransformID, int nExpireMinute)
{
	m_nTransformID = nTransformID;
	m_dwTransformTick = timeGetTime()+(1000*nExpireMinute);
}
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )

void CDNUserSession::AlteiaWorldDailyResetInfo()
{
	BYTE cTicketCount = (BYTE)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldDailyTicketCount );
	BYTE cSendTicketCount = (BYTE)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldSendTicketCount );
	SetAlteiaDailyPlayCount(0);
	SetAlteiaTicketCount(cTicketCount);
	SetAlteiaSendTicketCount(cSendTicketCount);
	m_AlteiaSendTicketList.clear();
	SendAlteiaWorldJoinInfo( m_cWeeklyPlayCount, m_cDailyPlayCount, m_cTicketCount );
}

void CDNUserSession::AlteiaWorldWeeklyResetInfo()
{
	SetAlteiaWeeklyPlayCount(0);
}

void CDNUserSession::AlteiaWorldInfo( AlteiaWorld::CSAlteiaWorldInfo* pPacket )
{
	if( !IsNoneWindowState() && !IsWindowState(WINDOW_ALTEIAWORLD) )
		return;

	if( GetPartyID() > 0 )
		return;

	switch( pPacket->eType )
	{
	case AlteiaWorld::Info::JoinInfo:
		{
			SendAlteiaWorldJoinInfo( m_cWeeklyPlayCount, m_cDailyPlayCount, m_cTicketCount );
			SetWindowState(WINDOW_ALTEIAWORLD);
		}
		break;
	case AlteiaWorld::Info::PrivateGoldKeyRankInfo:
		{
			GetDBConnection()->QueryGetAlteiaWorldPrivateGoldKeyRank( this );
		}
		break;
	case AlteiaWorld::Info::PrivatePlayTimeRankInfo:
		{
			GetDBConnection()->QueryGetAlteiaWorldPrivatePlayTimeRank( this );
		}
		break;
	case AlteiaWorld::Info::GuildGoldKeyRankInfo:
		{
			GetDBConnection()->QueryGetAlteiaWorldGuildGoldKeyRank( this );
		}
		break;
	case AlteiaWorld::Info::SendTicketInfo:
		{
			AlteiaWorldSendTicketInfo();
		}
		break;
	}
	
	return;
}

int CDNUserSession::AlteiaWorldJoin()
{
	if( !IsWindowState(WINDOW_ALTEIAWORLD) )
		return ERROR_GENERIC_INVALIDREQUEST;

	if( GetPartyID() > 0 )
		return ERROR_GENERIC_INVALIDREQUEST;

	TAlteiaWorldMapInfo* pAlteiaWorldMapInfo = g_pDataManager->GetAlteiaWorldMapInfo(1);

	// 횟수체크
	BYTE cPlayCount = (BYTE)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldDailyPlayCount );
	if(GetAlteiaDailyPlayCount() >= cPlayCount)
		return ERROR_GENERIC_INVALIDREQUEST;
	
	// 입장권체크
	BYTE cTicketCount = (BYTE)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldJoinTicketCount );
	if(m_cTicketCount < cTicketCount )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 입장시간체크

	if( g_pDataManager && !g_pDataManager->bIsAlteiaWorldEventTime() )
		return ERROR_GENERIC_INVALIDREQUEST;

	ScheduleStruct *pSchedule;
	for( DWORD i=0; i<m_VecScheduleList.size(); i++ ) 
	{
		pSchedule = &m_VecScheduleList[i];
		if( pSchedule->EventType == CDNSchedule::ResetWeeklyMission )
		{
			time_t LocalTime;
			tm pLocalTime;

			time(&LocalTime);
			pLocalTime = *localtime(&LocalTime);

			int nWeek = pSchedule->Cycle - CDNSchedule::Sunday;

			if( pLocalTime.tm_wday == nWeek )
			{
				int nEnterLimitStartTime = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldEnterLimitStartTime );
				int nEnterLimitEndTime = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::AlteiaWorldEnterLimitEndTime );
				int nResetTime = pSchedule->cHour * 60 + pSchedule->cMinute;
				int nCurTime = pLocalTime.tm_hour * 60 + pLocalTime.tm_min;
				if( nCurTime >= nResetTime - nEnterLimitStartTime && nCurTime <= nResetTime + nEnterLimitEndTime )
				{
					//진입 불가
					return ERROR_ALTEIAWORLD_ENTERLIMITTIME;
				}
			}
		}
	}


	if( pAlteiaWorldMapInfo )
	{
		int TargetMapIndex = pAlteiaWorldMapInfo->nMapID;
		if( TargetMapIndex <= 0 || m_pField == NULL )
		{			
			m_cGateNo = -1;
			m_cGateSelect = -1;
			return ERROR_PARTY_STARTSTAGE_FAIL;
		}
		if( g_pMasterConnection->GetActive() == false )
			return ERROR_GENERIC_MASTERCON_NOT_FOUND;

		m_nSingleRandomSeed = timeGetTime();//_rand();
		m_cGateNo = 1;
		m_cGateSelect = -1;

		g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_SINGLE, m_nAccountDBID, GetChannelID(), m_nSingleRandomSeed, TargetMapIndex, m_cGateNo, Dungeon::Difficulty::Easy, true );	// GameID를 요청
	}
	else
	{
		g_Log.Log(LogType::_NORMAL, m_pSession, L"AlateiaWorldMapInfo Error!!\r\n");
		return ERROR_GENERIC_INVALIDREQUEST;
	}
	return ERROR_NONE;
}

bool CDNUserSession::bIsAlteiaSendTicketList( INT64 biCharacterDBID )
{
	for( std::list<INT64>::iterator itor = m_AlteiaSendTicketList.begin();itor != m_AlteiaSendTicketList.end();itor++ )
	{
		if( biCharacterDBID == *itor)
			return true;
	}
	return false;
}

void CDNUserSession::AlteiaWorldSendTicketInfo()
{
	INT64 biCharacterDBID[AlteiaWorld::Common::MaxSendCount];
	memset(biCharacterDBID, 0, sizeof(biCharacterDBID));
	int nCount = 0;
	for( std::list<INT64>::iterator itor = m_AlteiaSendTicketList.begin();itor != m_AlteiaSendTicketList.end();itor++ )
	{
		biCharacterDBID[nCount] = *itor;
		nCount++;
		if( nCount >= AlteiaWorld::Common::MaxSendCount )
		{
			g_Log.Log(LogType::_NORMAL, m_pSession, L"AlateiaWorld SendTicketList Error!!\r\n");
			break;
		}
	}	
	SendAlteiaWorldSendTicketInfo( biCharacterDBID, m_cSendTicketCount);
}

void CDNUserSession::AddAlteiaTicket( int nCount/*=1*/ )
{
	m_cTicketCount += nCount;
}

void CDNUserSession::DelAlteiaSendTicket( int nCount/*=1*/ )
{
	m_cSendTicketCount -= nCount;
}
#endif

#if defined( PRE_DRAGONBUFF )
void CDNUserSession::ApplyWorldBuff( WCHAR* wszCharacterName, int nItemID )
{	
	TItemData* pBuffItemData = g_pDataManager->GetItemData(nItemID);
	if (!pBuffItemData)
	{
		_DANGER_POINT();
		g_Log.Log(LogType::_BESTFRIEND, this, L"CDNUserSession::ApplyWorldBuff Invalid BuffItem [%d] \r\n", nItemID);
		return;
	}
	CSUseItem pPacket;
	memset(&pPacket, 0, sizeof(pPacket));
	pPacket.biInvenSerial = GetItem()->MakeItemSerial();
	GetItem()->ApplyPartyEffectSkillItemData(&pPacket, pBuffItemData, GetSessionID(), EffectSkillNameSpace::ShowEffectType::NONEEFFECT, true, false);
	SendWorldBuffMsg(wszCharacterName, nItemID);

	if( GetPartyID() > 0 )
	{
		CDNParty* pParty = g_pPartyManager->GetParty( GetPartyID() );		
		if( pParty )
		{
			pParty->SendAllRefreshParty(PARTYREFRESH_NONE);
		}
	}	
}
#endif

#if defined(PRE_ADD_DWC)
void CDNUserSession::RefresDWCTeamName()
{
	SendUserLocalMessage(0, FM_CHANGEDWCTEAM);
}
#endif