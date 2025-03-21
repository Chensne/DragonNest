#include "StdAfx.h"
#include "DNPvP.h"
#include "Log.h"
#include "DNDivisionManager.h"
#include "DNVillageConnection.h"
#include "DnGameConnection.h"
#include "DNExtManager.h"

extern TMasterConfig g_Config;

CDNPvP::CDNPvP()
:m_cVillageID(0),m_unVillageChannelID(0),m_uiVillageMapIndex(0),m_uiIndex(0),m_uiCaptainAccountDBID(0),m_uiRoomState(PvPCommon::RoomState::None)
,m_uiMapIndex(0),m_uiMaxUser(0),m_iRandomSeed(0),m_unGameModeCheck(PvPCommon::Check::AllCheck),m_uiGameMode(PvPCommon::GameMode::Max),m_uiWinCondition(0),m_uiPlayTimeSec(0),m_uiGameModeTableID(0)
,m_biSNMain(0),m_iSNSub(0)
{
	m_cEventRoomIndex		= 0;
	m_bIsCanStartEventRoom	= false;
	memset( &m_EventData, 0, sizeof(m_EventData) );
	m_uiMinUser = 0;
	// PvP방 생성 GameServer 정보
	m_iGameServerID			= 0;
	m_iGameRoomID			= 0;
	m_cMinLevel				= 1;
	m_cMaxLevel				= CHARLEVELMAX;
	memset(m_nVoiceChannelID, 0, sizeof(m_nVoiceChannelID));

	m_unRoomOptionBit	= PvPCommon::RoomOption::None;
	m_bIsGuildWarSystem = false;

	m_uiStartMsgCount = 0;
	m_uiStartMsgTick = 0;
	m_biCreateRoomCharacterDBID = 0;		
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_eRoomType = PvPCommon::RoomType::max;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_dwPlayingStateCheckTick = 0;
#ifdef PRE_MOD_PVPOBSERVER
	m_bExtendObserver = false;
#endif		//#ifdef PRE_MOD_PVPOBSERVER
#if defined(PRE_ADD_PVP_TOURNAMENT)
	memset(m_vTournamentAccoutDBID, 0, sizeof(m_vTournamentAccoutDBID));	
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
#if defined( PRE_PVP_GAMBLEROOM )
	m_cGambleType = 0;
	m_nGamblePrice = 0;
#endif
}

CDNPvP::~CDNPvP()
{
	if( m_uiIndex )
	{
		if( g_pDivisionManager )
			g_pDivisionManager->PushPvPIndex( m_uiIndex );
		else
			_DANGER_POINT();
	}
}

void CDNPvP::GetVoiceChannelID(UINT *pVoiceChannel)
{
	if (pVoiceChannel)
		memcpy(pVoiceChannel, &m_nVoiceChannelID, sizeof(m_nVoiceChannelID));
	else
		_DANGER_POINT();
}

UINT CDNPvP::GetGuildDBIDbyIdx(int nIdx) const
{
	if (nIdx < 0 || nIdx >= (int)m_vGuildDBID.size())
	{
		_DANGER_POINT();
		return 0;
	}

	return m_vGuildDBID[nIdx].nGuildDBID;
}

void CDNPvP::Update( const UINT uiCurTick )
{
	if( m_uiRoomState&PvPCommon::RoomState::Starting )
	{
		if( uiCurTick-m_uiStartMsgTick >= 1000)
		{
			if( m_uiStartMsgCount == 0 )
			{
				_RequestGameRoom();
				return;
			}

			CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
			if( pVillageCon )
				pVillageCon->SendPvPStartMsg( m_unVillageChannelID, m_uiIndex, m_uiStartMsgCount );

			m_uiStartMsgTick = uiCurTick;
			--m_uiStartMsgCount;
		}
	}
#if defined( PRE_PVP_GAMBLEROOM )
	else if( ( GetEventRoomIndex() > 0 || GetGambleRoomType() > 0 ) && m_uiRoomState == PvPCommon::RoomState::None )
#else
	else if( GetEventRoomIndex() > 0 && m_uiRoomState == PvPCommon::RoomState::None )
#endif
	{
		CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
		if (pVillageCon == NULL)
			return;

		VIMAPVP_START TxPacket;
		memset( &TxPacket, 0, sizeof(TxPacket) );
		TxPacket.sCSPVP_START.unCheck = PvPCommon::Check::AllCheck;
		int iRet = StartPvP( pVillageCon, &TxPacket );

		if( iRet == ERROR_NONE )
		{
			if( m_bIsCanStartEventRoom == true )
			{
				if( uiCurTick-m_uiStartMsgTick >= 1000)
				{
					if( m_uiStartMsgCount == 0 )
						return;

					CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
					if( pVillageCon )
					{
						if( m_uiStartMsgCount%10 ==0 || m_uiStartMsgCount <= 5 )
							pVillageCon->SendPvPStartMsg( m_unVillageChannelID, m_uiIndex, m_uiStartMsgCount );
					}

					m_uiStartMsgTick = uiCurTick;
					--m_uiStartMsgCount;
				}
			}
		}
		else
		{
			m_bIsCanStartEventRoom = false;
		}
	}
	else if(GetEventRoomIndex() > 0 && GetRoomState()&PvPCommon::RoomState::Playing)
	{
		if (bIsEmpty())
		{
			if (m_dwPlayingStateCheckTick == 0)
				m_dwPlayingStateCheckTick = timeGetTime();
			else if (PvPCommon::Common::PvPStateAdjustmentTerm + m_dwPlayingStateCheckTick < uiCurTick)
			{
				g_pDivisionManager->SetPvPFinishGameMode(GetIndex(), GetGameServerRoomIndex());
				g_pDivisionManager->SetDestroyPvPGameRoom(GetIndex(), GetGameServerRoomIndex());
				m_dwPlayingStateCheckTick = 0;
			}
		}
	}
}

bool CDNPvP::bCreate( const BYTE cVillageID, const UINT uiPvPIndex, const VIMAPVP_CREATEROOM* pPacket, const INT64 biCreateRoomCharacterDBID, bool bExtendObserver )
{
	m_cEventRoomIndex = pPacket->nEventID;
	if( m_cEventRoomIndex > 0 )
		memcpy( &m_EventData, &pPacket->EventData, sizeof(m_EventData) );

	m_uiMinUser				= pPacket->sCSPVP_CREATEROOM.cMinUser;

	// PvP방 만든 Village 정보
	m_cVillageID			= cVillageID;
	m_unVillageChannelID	= pPacket->unVillageChannelID;
	m_uiVillageMapIndex		= pPacket->uiVillageMapIndex;

	// PvP룸 정보
	m_uiMapIndex			= pPacket->sCSPVP_CREATEROOM.uiMapIndex;
	m_uiGameModeTableID		= pPacket->sCSPVP_CREATEROOM.uiGameModeTableID;
	m_uiGameMode			= pPacket->cGameMode;
	m_uiWinCondition		= pPacket->sCSPVP_CREATEROOM.uiSelectWinCondition;
	m_uiPlayTimeSec			= pPacket->sCSPVP_CREATEROOM.uiSelectPlayTimeSec;
	m_uiMaxUser				= pPacket->sCSPVP_CREATEROOM.cMaxUser;
	m_uiCaptainAccountDBID	= pPacket->uiCreateAccountDBID;
	m_cMinLevel				= pPacket->sCSPVP_CREATEROOM.cMinLevel;
	m_cMaxLevel				= pPacket->sCSPVP_CREATEROOM.cMaxLevel;
	m_unRoomOptionBit		= pPacket->sCSPVP_CREATEROOM.unRoomOptionBit;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_eRoomType = static_cast<PvPCommon::RoomType::eRoomType>(pPacket->sCSPVP_CREATEROOM.cRoomType);
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#ifdef PRE_MOD_PVPOBSERVER
	m_bExtendObserver = bExtendObserver;
#endif		//#ifdef PRE_MOD_PVPOBSERVER

#if defined( PRE_PVP_GAMBLEROOM )	
	m_cGambleType = pPacket->cGambleType;
	m_nGamblePrice = pPacket->nPrice;
#endif

	WCHAR wszBuf[MAX_PATH];
	memset( &wszBuf, 0, sizeof(WCHAR)*MAX_PATH );
	wcsncpy( wszBuf, pPacket->sCSPVP_CREATEROOM.wszBuf, pPacket->sCSPVP_CREATEROOM.cRoomNameLen );
	m_wstrRoomName	= wszBuf;

	if( pPacket->sCSPVP_CREATEROOM.cRoomPWLen > 0 )
	{
		memset( &wszBuf, 0, sizeof(WCHAR)*MAX_PATH );
		wcsncpy( wszBuf, pPacket->sCSPVP_CREATEROOM.wszBuf+pPacket->sCSPVP_CREATEROOM.cRoomNameLen, pPacket->sCSPVP_CREATEROOM.cRoomPWLen );
		m_wstrRoomPW = wszBuf;
	}

	if ( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
	{
		if (_InitGuildWar(PvPCommon::Common::DefaultGuildCount, pPacket))
		{
			m_bIsGuildWarSystem = m_vGuildDBID.empty() ? false : true;
		}
	}
	m_biCreateRoomCharacterDBID = biCreateRoomCharacterDBID;
	m_uiIndex = uiPvPIndex;
	return true;
}

void CDNPvP::ModifyRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_MODIFYROOM* pPacket )
{
	UINT uiPrevGameMode = m_uiGameMode;

	// PvP룸 정보
	m_uiMapIndex			= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiMapIndex;
	m_uiGameModeTableID		= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiGameModeTableID;
	m_uiGameMode			= pPacket->cGameMode;
	m_uiWinCondition		= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiSelectWinCondition;
	m_uiPlayTimeSec			= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiSelectPlayTimeSec;
	m_cMinLevel				= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cMinLevel;
	m_cMaxLevel				= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cMaxLevel;
	m_unRoomOptionBit		= pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.unRoomOptionBit;

	WCHAR wszBuf[MAX_PATH];
	memset( &wszBuf, 0, sizeof(WCHAR)*MAX_PATH );
	wcsncpy( wszBuf, pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.wszBuf, pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomNameLen );
	m_wstrRoomName = wszBuf;

	if( pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomPWLen > 0 )
	{
		memset( &wszBuf, 0, sizeof(WCHAR)*MAX_PATH );
		wcsncpy( wszBuf, pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.wszBuf+pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomNameLen, pPacket->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomPWLen );
		m_wstrRoomPW = wszBuf;
	}
	else
		m_wstrRoomPW.clear();

#ifdef PRE_MOD_PVPOBSERVER
	if (bIsAllowObserver() == false)
	{
		LeaveObserver(pVillageCon);
	}
#endif		//#ifdef PRE_MOD_PVPOBSERVER

	// 방정보 변경 알림
	pVillageCon->SendPvPModifyRoom( ERROR_NONE, this, pPacket );
	// RoomState 변경 검사
	if( m_uiRoomState&PvPCommon::RoomState::Password )
	{
		if( m_wstrRoomPW.empty() )
			SetRoomState( m_uiRoomState&~PvPCommon::RoomState::Password );
	}
	else
	{
		if( !m_wstrRoomPW.empty() )
			SetRoomState( m_uiRoomState|PvPCommon::RoomState::Password );
	}
	// UserState 변경(Ready풀기)
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
		if( pUser )
		{
			UINT uiState = pUser->GetPvPUserState();
			if( uiState&PvPCommon::UserState::Ready )
				uiState &= ~PvPCommon::UserState::Ready;

			if( uiPrevGameMode == PvPCommon::GameMode::PvP_AllKill && IsMode( PvPCommon::GameMode::PvP_AllKill ) == false )
				uiState &= ~PvPCommon::UserState::GroupCaptain;

			if( uiState != pUser->GetPvPUserState() )
				SetPvPUserState( pUser, uiState );
		}
	}
	if( uiPrevGameMode != PvPCommon::GameMode::PvP_AllKill && IsMode( PvPCommon::GameMode::PvP_AllKill ) == true )
		CheckAndSetGroupCaptain();
}

bool CDNPvP::bIsGuildWarGrade(UINT nAccountDBID, int nGrade)
{
	std::list<UINT>::iterator itor = std::find( m_listAccountDBID.begin(), m_listAccountDBID.end(), nAccountDBID );
	if (itor != m_listAccountDBID.end())
	{
		if (g_pDivisionManager)
		{		
			CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID((*itor));
			if (pUser)
			{
				if (pUser->GetPvPUserState()&nGrade)
					return true;
			}
		}
	}
	return false;
}

bool CDNPvP::bIsCheckUser( const UINT uiAccountDBID )
{
	std::list<UINT>::iterator itor = std::find( m_listAccountDBID.begin(), m_listAccountDBID.end(), uiAccountDBID );
	
	return (itor!=m_listAccountDBID.end()) ? true : false;
}

void CDNPvP::AddBanUser( UINT uiSessionID )
{
	m_mBanUser.insert( std::make_pair(uiSessionID,GetTickCount()) );
}

void CDNPvP::ClearBanUser()
{
	m_mBanUser.clear();
}

bool CDNPvP::bIsBanUser( const UINT uiSessionID )
{
	std::map<UINT,DWORD>::iterator itor = m_mBanUser.find( uiSessionID );

	return (itor==m_mBanUser.end()) ? false : true;
}

bool CDNPvP::bIsEmpty( bool bExecptObserver/*=false*/ )
{ 
	if( bExecptObserver )
	{
		for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
			if( pUser )
			{
				if( !pUser->bIsObserver() )
					return false;
#ifdef PRE_MOD_PVPOBSERVER
				if (bIsExtendObserver() && pUser->bIsObserver() && pUser->GetPvPUserState()&(PvPCommon::UserState::Captain))
					return false;
#endif		//#ifdef PRE_MOD_PVPOBSERVER
			}
		}

		return true;
	}
	
	return m_listAccountDBID.empty(); 
}

bool CDNPvP::bIsReady( CDNUser* pUser )
{
	if( pUser->GetPvPUserState()&(PvPCommon::UserState::Ready|PvPCommon::UserState::Captain) )
		return true;

	if( pUser->bIsObserver() )
		return true;

	return false;
}

bool CDNPvP::bIsAllowLevel( const BYTE cUserLevel )
{
	if( cUserLevel >= m_cMinLevel && cUserLevel <= m_cMaxLevel )
		return true;

	g_Log.Log( LogType::_ERROR, L"[PvP] LevelLimit!!! PvPRoomIndex:%d Level:%d~%d ReqLv:%d 이상\r\n", m_uiIndex, (int)m_cMinLevel, (int)m_cMaxLevel, (int)cUserLevel );
	return false;
}

bool CDNPvP::bLeave( CDNVillageConnection* pVillageCon, CDNUser* pLeaveUser, const bool bDisconnectVillageServer/*=false*/, PvPCommon::LeaveType::eLeaveType Type/*=PvPCommon::LeaveType::Normal*/ )
{
	UINT uiLeaveAccountDBID	= pLeaveUser->GetAccountDBID();
	USHORT nLeaveTeam = pLeaveUser->GetPvPTeam();
	UINT uiLeavePvPUserState = pLeaveUser->GetPvPUserState();

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; )
	{
		if( (*itor) == uiLeaveAccountDBID )
		{
			if( pVillageCon )
				pVillageCon->SendPvPLeaveRoom( ERROR_NONE, m_unVillageChannelID, m_uiIndex, uiLeaveAccountDBID, Type );

			pLeaveUser->SetPvPIndex( 0 );
			pLeaveUser->SetPvPTeam( PvPCommon::Team::Max );
			pLeaveUser->SetPvPUserState( PvPCommon::UserState::None );
			itor = m_listAccountDBID.erase( itor );
			g_Log.Log( LogType::_NORMAL, pLeaveUser, L"[PvP] Leave PvP Room!!! PvPRoomIndex:%d\r\n", m_uiIndex );
			break;
		}
		else
		{
			++itor;
		}
	}

	if( bDisconnectVillageServer )
		return true;

	//길드전일경우 길드마스터가 나갈경우 세팅해줘야한다.
	if ( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
	{
		SetPvPMemberIndex(nLeaveTeam, uiLeaveAccountDBID);
		if (GetUserCountByState(nLeaveTeam, PvPCommon::UserState::GuildWarCaptain) <= 0 && GetUserCountByTeam(nLeaveTeam) > 0)
		{
			UINT nNewGuildWarCaptain = SelectNewGuildWarCaptain(nLeaveTeam);
			SetPvPGuildWarMemberGrade(nLeaveTeam, true, PvPCommon::UserState::GuildWarCaptain, nNewGuildWarCaptain);

			if (g_pDivisionManager && m_iGameServerID > 0 && (m_uiRoomState&PvPCommon::RoomState::Playing))
			{
				CDNUser * pNewGuildWarCaptain = g_pDivisionManager->GetUserBySessionID(nNewGuildWarCaptain);
				if (pNewGuildWarCaptain)
				{
					CDNGameConnection * pGameCon = g_pDivisionManager->GetGameConnectionByGameID(pNewGuildWarCaptain->GetGameID());
					if (pGameCon)
					{
						//변경을 알린다.
						pGameCon->SendPvPMemberGrade(pNewGuildWarCaptain->GetAccountDBID(), pNewGuildWarCaptain->GetPvPTeam(), pNewGuildWarCaptain->GetPvPUserState(), nNewGuildWarCaptain, ERROR_NONE);
					}
					else
						_DANGER_POINT();
				}
				else
					_DANGER_POINT();
			}
		}
	}
#if defined(PRE_ADD_PVP_TOURNAMENT)
	if( IsMode(PvPCommon::GameMode::PvP_Tournament) == true)
	{
		DelTournamentIndex(uiLeaveAccountDBID);
	}
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)

	if( !bIsEmpty(true) )
	{
		// 방장변경
		if( m_uiCaptainAccountDBID == uiLeaveAccountDBID )
		{
#ifdef PRE_MOD_PVPOBSERVER
			if (bIsExtendObserver())
			{
				return bLeaveAll(pVillageCon, PvPCommon::LeaveType::DestroyRoom);
			}
#endif		//#ifdef PRE_MOD_PVPOBSERVER
			UINT uiNewCaptain = FindNewCaptain();
			ChangeCaptain( pVillageCon, uiNewCaptain );
		}
		if( IsMode( PvPCommon::GameMode::PvP_AllKill) == true )
		{
			if( uiLeavePvPUserState&PvPCommon::UserState::GroupCaptain )
			{
				UINT uiNewGroupCaptaion = FindNewGroupCaptain( nLeaveTeam );
				ChangeGroupCaptain( uiNewGroupCaptaion, nLeaveTeam );
			}
		}
		// STATE_GAME 이면 게임방에 나갔음을 알려줘야한다.
		if( pLeaveUser->GetUserState() == STATE_GAME )
		{
			CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pLeaveUser->GetGameID() );
			if( pGameCon )
			{
				CDNUser* pCaptainUser = GetUserByAccountDBID( m_uiCaptainAccountDBID );
				if( pCaptainUser )
					pGameCon->SendDetachUser(uiLeaveAccountDBID);
			}
		}
	}

	return true;
}

bool CDNPvP::bLeaveAll( CDNVillageConnection* pVillageCon, PvPCommon::LeaveType::eLeaveType Type/*=PvPCommon::LeaveType::Normal*/ )
{
	std::list<UINT> tempList = m_listAccountDBID;
	for( std::list<UINT>::iterator itor=tempList.begin() ; itor!=tempList.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser )
			{
				bLeave( pVillageCon, pUser, true, Type );
			}
		}
	}
	return true;
}

#ifdef PRE_MOD_PVPOBSERVER
bool CDNPvP::LeaveObserver(CDNVillageConnection* pVillageCon, PvPCommon::LeaveType::eLeaveType Type)
{
	std::list<UINT> tempList = m_listAccountDBID;
	for( std::list<UINT>::iterator itor=tempList.begin() ; itor!=tempList.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser && pUser->bIsObserver() && !(pUser->GetPvPUserState()&(PvPCommon::UserState::Captain)))
			{
				bLeave( pVillageCon, pUser, true, Type );
			}
		}
	}
	return true;
}
#endif		//#ifdef PRE_MOD_PVPOBSERVER

short CDNPvP::Join( CDNVillageConnection* pVillageCon, CDNUser* pJoinUser, const UINT UserState, bool bIsObserver/*=false*/, UINT nGuildDBID/* = 0*/ )
{
	USHORT usTeam = PvPCommon::Team::Max;
	if( bIsObserver )
		usTeam = PvPCommon::Team::Observer;
	else
		usTeam = _SelectRandomTeam();

	char cIndex = -1;
	UINT uiUserState = UserState;
	if( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
	{
		if (bIsObserver == false)
		{
			usTeam = _SelectTeamGuild(nGuildDBID);			
			
			if (usTeam != PvPCommon::Team::A && usTeam != PvPCommon::Team::B)
			{
				_DANGER_POINT();
				return ERROR_GENERIC_INVALIDREQUEST;
			}
			else
			{
				if (GetUserCountByTeam(usTeam) >= (int)m_uiMaxUser/2)
					return ERROR_PVP_JOINROOM_MAXPLAYER;
			}

			GetAndSetRemainTeamIndex(usTeam, pJoinUser->GetAccountDBID(), cIndex);
			_GetJoinUserGrade(usTeam, uiUserState);
		}
		else if (GetIsGuildWarSystem() && pJoinUser->GetAccountLevel() <= 0)		//일반유저는 옵저버로 진입불가
			return ERROR_PVP_JOINROOM_FAILED;
	}
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	if (m_eRoomType != pJoinUser->GetPvPChannelType())
		return ERROR_PVP_JOINFAIL_MISMATCHCHANNEL;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	if( IsMode( PvPCommon::GameMode::PvP_AllKill) == true )
	{
		switch( usTeam )
		{
			case PvPCommon::Team::A:
			case PvPCommon::Team::B:
			{
				CDNUser* pUser = GetGroupCaptain( usTeam );
				if( pUser == NULL )
					uiUserState |= PvPCommon::UserState::GroupCaptain;
				break;
			}
		}
	}
#if defined(PRE_ADD_PVP_TOURNAMENT)
	if( IsMode(PvPCommon::GameMode::PvP_Tournament) == true )
	{
		if (bIsObserver == false)
		{
			cIndex = GetTournamentIndex(pJoinUser->GetAccountDBID());
			if( cIndex == -1)
				return ERROR_PVP_JOINROOM_FAILED;
			usTeam = cIndex%2==0 ? PvPCommon::Team::A : PvPCommon::Team::B;
		}
	}
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)

	m_listAccountDBID.push_back( pJoinUser->GetAccountDBID() );
	pJoinUser->SetPvPIndex( m_uiIndex );
	pJoinUser->SetPvPTeam( usTeam );
	pJoinUser->SetPvPUserState( uiUserState );
	if( pVillageCon )
	{		
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		if( pVillageCon->GetVillageID() != pJoinUser->GetVillageID() )
			pVillageCon = pJoinUser->GetCurrentVillageConnection();

		if (pVillageCon == NULL)
		{
			_DANGER_POINT();
			return ERROR_NONE;
		}
#endif
		pVillageCon->SendPvPJoinRoom( ERROR_NONE, pJoinUser->GetAccountDBID(), GetVillageChannelID(), m_uiIndex, usTeam, uiUserState, cIndex );
	}

	return ERROR_NONE;
}

bool CDNPvP::bIsEmptySlot( bool bIsObserver/*=false*/ )
{
	if( bIsObserver )
	{
#ifdef PRE_MOD_PVPOBSERVER
		bool bRet = bIsEmptyTeamSlot( PvPCommon::Team::Observer );
		if (bRet == false)
			g_Log.Log(LogType::_NORMAL, 0, 0, 0, 0, L"ObserverUser Overflow pvpid[%d] MaxObserverCount[%d]", m_uiIndex, bIsExtendObserver() ? PvPCommon::Common::ExtendMaxObserverPlayer : PvPCommon::Common::MaxObserverPlayer);
		return bRet;
#else		//#ifdef PRE_MOD_PVPOBSERVER
		return bIsEmptyTeamSlot( PvPCommon::Team::Observer );
#endif		//#ifdef PRE_MOD_PVPOBSERVER
	}
	else
	{
		return (bIsEmptyTeamSlot( PvPCommon::Team::A ) || bIsEmptyTeamSlot( PvPCommon::Team::B ));
	}
}

bool CDNPvP::bIsEmptyTeamSlot( const USHORT usTeam )
{
	UINT uiCount = 0;

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
		if( pUser ) 
		{
			if( pUser->GetPvPTeam() == usTeam )
				++uiCount;
		}
		else
		{
			g_Log.Log( LogType::_ERROR, L"[PvP] bIsEmptyTeamSlot!!! PvPRoomIndex:%d, AccountDBID:%d \r\n", m_uiIndex, *itor );
			_DANGER_POINT();
			return false;
		}
	}

	if( usTeam == PvPCommon::Team::Observer )
	{
#ifdef PRE_MOD_SYNCPACKET
		if (m_bExtendObserver)
			return (uiCount < PvPCommon::Common::ExtendMaxObserverPlayer);
#endif		//#ifdef PRE_MOD_SYNCPACKET
		return (uiCount < PvPCommon::Common::MaxObserverPlayer);
	}
	if( m_uiMaxUser/2 <= uiCount )
		return false;

	return true;
}

bool CDNPvP::bIsCheckPW( const WCHAR* pwszPW )
{
	return wcscmp( m_wstrRoomPW.c_str(), pwszPW ) ? false : true;
}

CDNUser* CDNPvP::GetUserBySessionID( const UINT uiSessionID )
{
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
		if( pUser && pUser->GetSessionID() == uiSessionID )
			return pUser;
	}

	return NULL;
}

CDNUser* CDNPvP::GetUserByAccountDBID( const UINT uiAccountDBID )
{
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( uiAccountDBID == (*itor) )
			return g_pDivisionManager->GetUserByAccountDBID( *itor );
	}

	return NULL;
}

void CDNPvP::ChangeCaptain( CDNVillageConnection* pVillageCon, const UINT32 uiCaptainAccountDBID )
{
	// 이벤트방에는 방장이 존재하지 않는다.
	if( GetEventRoomIndex() > 0 )
		return;

#if defined( PRE_PVP_GAMBLEROOM )
	if( GetGambleRoomType() > 0 )
		return;
#endif

	// 길드전시스템도 방장이 존재하지 않는다
	if (GetIsGuildWarSystem())
		return;

	// FindNewCaptain 에서 새로운 방장을 찾지 못한 경우 0 으로 넘어온다.
	if( uiCaptainAccountDBID == 0 )
		return;

	// 이전 방장 UserState 설정
	CDNUser* pPrevCaptain = GetUserByAccountDBID( m_uiCaptainAccountDBID );
	if( pPrevCaptain )
	{
		UINT uiUserState = pPrevCaptain->GetPvPUserState();
		// 이전방장은 레뒤와 방장 풀어준다.
		uiUserState &= ~(PvPCommon::UserState::Ready|PvPCommon::UserState::Captain);
		SetPvPUserState( pPrevCaptain, uiUserState );
	}

	// 방장이양
	m_uiCaptainAccountDBID = uiCaptainAccountDBID;

	// 새로운 방장 UserState 설정
	CDNUser* pNewCaptain = GetUserByAccountDBID( m_uiCaptainAccountDBID );
	if( pNewCaptain )
	{
		UINT uiUserState = pNewCaptain->GetPvPUserState();
		
		// 새방장은 레뒤풀보 방장 달아준다.
		uiUserState &= ~PvPCommon::UserState::Ready;
		uiUserState |= PvPCommon::UserState::Captain;
		SetPvPUserState( pNewCaptain, uiUserState );

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		CDNGameConnection * pGameConnection = g_pDivisionManager->GetGameConnectionByGameID( pNewCaptain->GetGameID() );
		if( pGameConnection && PvPCommon::RoomState::Playing == m_uiRoomState )
		{
			pGameConnection->SendChangeRoomMaster( m_iGameRoomID, pNewCaptain->GetCharacterDBID(), pNewCaptain->GetSessionID() );
		}
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
	}
	else
		_DANGER_POINT();

	// 방장이 바뀌면 Ban 정보 초기화
	ClearBanUser();
}

bool CDNPvP::bIsGroupCaptain( UINT uiAccountDBID )
{
	CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( uiAccountDBID );
	if( pUser == NULL )
		return false;
	if( pUser->GetPvPUserState()&PvPCommon::UserState::GroupCaptain )
		return true;
	return false;
}

CDNUser* CDNPvP::GetGroupCaptain( USHORT nTeam )
{
	if( IsMode( PvPCommon::GameMode::PvP_AllKill) == false )
		return NULL;

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
		if( pUser == NULL )
			continue;

		if( pUser->GetPvPTeam() == nTeam )
		{
			if( pUser->GetPvPUserState()&PvPCommon::UserState::GroupCaptain )
				return pUser;
		}
	}

	return NULL;
}

void CDNPvP::ChangeGroupCaptain( UINT uiGroupCaptainAccountDBID, USHORT nTeam )
{
	if( IsMode( PvPCommon::GameMode::PvP_AllKill) == false )
		return;

	// FindNewGroupCaptain 에서 새로운 방장을 찾지 못한 경우 0 으로 넘어온다.
	if( uiGroupCaptainAccountDBID == 0 )
		return;

	CDNUser* pPrevGroupCaptain = GetGroupCaptain( nTeam );
	if( pPrevGroupCaptain )
	{
		UINT uiUserState = pPrevGroupCaptain->GetPvPUserState();
		uiUserState &= ~(PvPCommon::UserState::GroupCaptain);
		SetPvPUserState( pPrevGroupCaptain, uiUserState );
	}

	CDNUser* pNewGroupCaptain = GetUserByAccountDBID( uiGroupCaptainAccountDBID );
	if( pNewGroupCaptain )
	{
		UINT uiUserState = pNewGroupCaptain->GetPvPUserState();

		uiUserState |= PvPCommon::UserState::GroupCaptain;
		SetPvPUserState( pNewGroupCaptain, uiUserState );
	}
}

void CDNPvP::CheckAndSetGroupCaptain()
{
	if( IsMode( PvPCommon::GameMode::PvP_AllKill) == false )
		return;

	CDNUser* pUser = GetGroupCaptain( PvPCommon::Team::A );
	if( pUser == NULL )
		ChangeGroupCaptain( FindNewGroupCaptain( PvPCommon::Team::A ), PvPCommon::Team::A );

	pUser = GetGroupCaptain( PvPCommon::Team::B );
	if( pUser == NULL )
		ChangeGroupCaptain( FindNewGroupCaptain( PvPCommon::Team::B ), PvPCommon::Team::B );
}

void CDNPvP::BanPvPRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_BAN* pPacket )
{
	// 방장검사
	if( !bIsCaptain( pPacket->uiAccountDBID ) )
	{
		_DANGER_POINT();
		return;
	}

	// 강퇴할 수 없는 RoomState 인지 검사
	if( GetRoomState()&PvPCommon::RoomState::CantBanMask )
	{
		_DANGER_POINT();
		return;
	}

	// 강퇴 당 할 유저 존재 검사
	CDNUser* pBanUser = g_pDivisionManager->GetUserBySessionID( pPacket->sCSPvPBan.uiSessionID );
	if( !pBanUser )
	{
		_DANGER_POINT();
		return;
	}

	if( !bIsCheckUser( pBanUser->GetAccountDBID() ) )
	{
		_DANGER_POINT();
		return;
	}

	// 자기자신검사
	if( pBanUser->GetAccountDBID() == pPacket->uiAccountDBID )
	{
		_DANGER_POINT();
		return;
	}

	// 강퇴할 수 없는 UserState 인지 검사
	if( pBanUser->GetPvPUserState()&PvPCommon::UserState::CantBanMask )
	{
		_DANGER_POINT();
		return;
	}

	PvPCommon::LeaveType::eLeaveType Type = (pPacket->sCSPvPBan.eType == PvPCommon::BanType::Permanence) ? PvPCommon::LeaveType::PermanenceBan : PvPCommon::LeaveType::Ban;
	bLeave( pVillageCon, pBanUser, false, Type );

	// 영구추방일 경우 기억
	if( Type == PvPCommon::LeaveType::PermanenceBan )
	{
		AddBanUser( pBanUser->GetSessionID() );
	}
}

void CDNPvP::ReadyUser( CDNVillageConnection* pVillageCon, const VIMAPVP_READY* pPacket )
{
	// 이벤트방은 User에 의해 래디할수없당.
	if( GetEventRoomIndex() > 0 )
		return;

#if defined( PRE_PVP_GAMBLEROOM )
	if( GetGambleRoomType() > 0 )
		return;
#endif

	// 길드전시스템도 User에 의해 뤠뒤불가
	if (GetIsGuildWarSystem())
		return;

	// RoomState 검사
	UINT uiRoomState = m_uiRoomState&~PvPCommon::RoomState::Password;
	if( uiRoomState != PvPCommon::RoomState::None )
		return;

	if( bIsCaptain( pPacket->uiAccountDBID ) )
		return;

	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		if( pUser->bIsObserver() )
			return;

		UINT uiUserState = pUser->GetPvPUserState();
		if( pPacket->sCSPVP_READY.cReady )
			uiUserState |= PvPCommon::UserState::Ready;
		else
			uiUserState &= ~PvPCommon::UserState::Ready;

		SetPvPUserState( pUser, uiUserState );
	}
}

void CDNPvP::SetVoiceChannelID(UINT *pChannelID)
{
	if (pChannelID)
	{
		for (int i = 0; i < PvPCommon::TeamIndex::Max; i++)
		{
			if (pChannelID[i] <= 0) continue;
			m_nVoiceChannelID[i] = pChannelID[i];
		}
	}
	else
		_DANGER_POINT();
}

bool CDNPvP::GetGuildWarInfo(UINT * pGuildDBID, int * pScore)
{
	if (pGuildDBID == NULL || pScore == NULL)
		return false;

	int nCount = 0;
	std::vector <PvPCommon::Team::TPvPGuildWarInfo>::iterator ii;
	for (ii = m_vGuildDBID.begin(); ii != m_vGuildDBID.end(); ii++)
	{
		pGuildDBID[nCount] = (*ii).nGuildDBID;
		pScore[nCount] = (*ii).nQualifyingScore;
		nCount++;

		if (nCount >= PvPCommon::Common::DefaultGuildCount)
			return true;
	}
	return false;
}

// 난입
short CDNPvP::_StartBreakInto( const VIMAPVP_START* pPacket )
{
	// 난입가능한 방인지 검사
	CDNUser* pUser = NULL;
	if( g_pDivisionManager )
	{
		pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->uiAccountDBID );
		if( bIsBreakIntoRoom() == false )
		{
			if( pUser == NULL || pUser->bIsObserver() == false )
				return ERROR_PVP_STARTPVP_FAILED;
		}
	}
	else
	{
		_DANGER_POINT();
		return ERROR_PVP_STARTPVP_FAILED;
	}

	// 난입가능시점이 지났거나 게임모드가 종료되었으면 더이상 난입이 되지 않는다.
	if( m_uiRoomState&(PvPCommon::RoomState::NoMoreBreakInto|PvPCommon::RoomState::Finished) )
		return ERROR_PVP_STARTPVP_FAILED;

	if( g_pDivisionManager )
	{
		if( pUser ) 
		{
			UINT uiUserState = pUser->GetPvPUserState()&~PvPCommon::UserState::Captain;
			if( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
			{
				uiUserState &= ~PvPCommon::UserState::GuildWarCaptain;
				uiUserState &= ~PvPCommon::UserState::GuildWarSedcondCaptain;
			}
			if( uiUserState == PvPCommon::UserState::None )
			{
				CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( m_iGameServerID );
				if( pGameCon )
				{
					// 난입시 Population 검사해서 못하게 한다.
					if( pGameCon->GetZeroPopulation() )
						return ERROR_PVP_STARTPVP_FAILED;

					char cTeamSlotIndex = GetPvPTeamIndex(pUser);
					pGameCon->SendBreakintoRoom( m_iGameRoomID, pUser, BreakInto::Type::None, cTeamSlotIndex );
					SetPvPUserState( pUser, pUser->GetPvPUserState()|PvPCommon::UserState::Syncing );

					return ERROR_NONE;
				}
			}
		}
	}

	return ERROR_PVP_STARTPVP_FAILED;
}

short CDNPvP::StartPvP( CDNVillageConnection* pVillageCon, const VIMAPVP_START* pPacket )
{
	UINT uiRoomState = m_uiRoomState&~PvPCommon::RoomState::Password;
	// 난입
	if( uiRoomState&PvPCommon::RoomState::Playing )
	{
		return _StartBreakInto( pPacket );
	}
	// RoomState 검사
	else if( uiRoomState != PvPCommon::RoomState::None )
	{
		_DANGER_POINT();
		return ERROR_PVP_STARTPVP_FAILED;
	}

	// 방장검사	
#if defined( PRE_PVP_GAMBLEROOM )
	if (m_bIsGuildWarSystem == false && m_cEventRoomIndex <= 0 && m_cGambleType <= 0)
#else
	if (m_bIsGuildWarSystem == false && m_cEventRoomIndex <= 0)
#endif
	{
#ifdef _WORK
		//todo
#else
		if ( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
		{
			if (_CheckReadyGuildWarMaster() == false)
				return ERROR_PVP_STARTPVP_FAIL_READYMASTER;
		}
#endif
		if (!bIsCaptain(pPacket->uiAccountDBID))
			return ERROR_PVP_STARTPVP_FAILED;
	}

	// 시작조건 테이블 얻기
	TPvPGameStartConditionTable* pGameStartConditionTable = g_pExtManager->GetPvPGameStartConditionTable( m_uiMaxUser );
	if( !pGameStartConditionTable )
		return ERROR_PVP_STARTPVP_FAILED;
	if( !m_bIsGuildWarSystem && pPacket->sCSPVP_START.unCheck&PvPCommon::Check::CheckStartConditionTable)
	{
		UINT uiATeamReadyCount,uiBTeamReadyCount;
#if defined( PRE_PVP_GAMBLEROOM )
		if( m_cEventRoomIndex > 0 || m_cGambleType > 0 )
#else
		if( m_cEventRoomIndex > 0 )
#endif
			_GetTeamUserCount( uiATeamReadyCount, uiBTeamReadyCount );
		else
			_GetReadyUserCount( uiATeamReadyCount, uiBTeamReadyCount );

		// 이벤트방일 경우 별도의 최소인원수로 검사한다.
#if defined( PRE_PVP_GAMBLEROOM )
		if (m_cEventRoomIndex > 0 || m_cGambleType > 0)
#else
		if (m_cEventRoomIndex > 0)
#endif
		{
			if (m_uiMinUser > (uiATeamReadyCount+uiBTeamReadyCount))
				return ERROR_PVP_CANTSTART_LESS_MINTEAMPLAYER;
		}
		else if (GetIsGuildWarSystem() == false)		//시스템에서 생성되어진 길드전이 아닌경우 검사한다
		{
			// 개인전 예외 처리 작업
			if( IsMode( PvPCommon::GameMode::PvP_IndividualRespawn ) == true 
				|| IsMode( PvPCommon::GameMode::PvP_Zombie_Survival ) == true 
#if defined(PRE_ADD_RACING_MODE)
				|| IsMode( PvPCommon::GameMode::PvP_Racing) == true
#endif // #if defined(PRE_ADD_RACING_MODE)
#if defined(PRE_ADD_PVP_TOURNAMENT)
				|| IsMode( PvPCommon::GameMode::PvP_Tournament) == true 
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
				)
			{
				if( uiATeamReadyCount+uiBTeamReadyCount < pGameStartConditionTable->uiMinTeamPlayerNum*2 )
					return ERROR_PVP_CANTSTART_LESS_MINTEAMPLAYER;
			}
			else
			{
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
				if( false == IsMode( PvPCommon::GameMode::PvP_ComboExercise ) )
#endif	// #if defined( PRE_ADD_PVP_COMBOEXERCISE )
				{
					short nRet = pGameStartConditionTable->Check( uiATeamReadyCount, uiBTeamReadyCount, m_unRoomOptionBit & PvPCommon::RoomOption::RandomTeam ? true : false );
					if( nRet != ERROR_NONE )
						return nRet;
				}
			}
		}
	}

#if defined( PRE_PVP_GAMBLEROOM )
	if( m_cEventRoomIndex > 0 || m_cGambleType > 0 )
#else
	if( m_cEventRoomIndex > 0 )
#endif
	{
		if( m_bIsCanStartEventRoom == false )
		{
			m_bIsCanStartEventRoom	= true;
			m_uiStartMsgCount		= 20;		// 게임 시작 메세지
			m_uiStartMsgTick		= 0;
			return ERROR_NONE;
		}
		else 
		{
			if( m_uiStartMsgCount > 0 )
				return ERROR_NONE;
		}
	}

	m_unGameModeCheck = pPacket->sCSPVP_START.unCheck;

	pVillageCon->SendPvPStart( ERROR_NONE, GetVillageChannelID(), m_uiIndex, pPacket->uiAccountDBID );

	// RoomState 변경
	SetRoomState( m_uiRoomState|PvPCommon::RoomState::Starting );
	
	return ERROR_NONE;
}

bool CDNPvP::_InitGuildWar(BYTE cCount, const VIMAPVP_CREATEROOM * pPacket)
{
	if( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
	{
		if (!m_vTeamSlotIndex.empty() || !m_vGuildDBID.empty())
			return false;

		PvPCommon::Team::TPvPTeamIndex TeamIndex;
		PvPCommon::Team::TPvPGuildWarInfo GuildInfo;
		int nIndex = 0;
		for (int i = PvPCommon::Team::StartIndex; i <= PvPCommon::Team::EndIndex; i++)
		{
			memset(&TeamIndex, 0, sizeof(TeamIndex));

			TeamIndex.nTeam = i;
			m_vTeamSlotIndex.push_back(TeamIndex);

			if (pPacket->nGuildDBID[nIndex] > 0)
			{
				memset(&GuildInfo, 0, sizeof(PvPCommon::Team::TPvPGuildWarInfo));

				GuildInfo.nTeam = TeamIndex.nTeam;
				GuildInfo.nGuildDBID = pPacket->nGuildDBID[nIndex];
				GuildInfo.nQualifyingScore = pPacket->nGuildQualifyingScore[nIndex];

				m_vGuildDBID.push_back(GuildInfo);
			}
			nIndex++;
		}
			
		return true;
	}
	return false;
}

USHORT CDNPvP::_SelectTeamGuild(UINT nGuildDBID)
{
	if (m_vGuildDBID.empty())			//길드가 세팅되어 있지 않으면 랜덤으로 고른다.
		return _SelectRandomTeam();

	std::vector <PvPCommon::Team::TPvPGuildWarInfo>::iterator ii;
	for (ii = m_vGuildDBID.begin(); ii != m_vGuildDBID.end(); ii++)
	{
		if ((*ii).nGuildDBID == nGuildDBID)
			return (*ii).nTeam;
	}
	return PvPCommon::Team::Observer;
}

void CDNPvP::_GetJoinUserGrade(USHORT nTeam, UINT &uiUserState)
{
	int nTeamUserCount = GetUserCountByTeam(nTeam);
	if (nTeamUserCount == 0)
		uiUserState |= PvPCommon::UserState::GuildWarCaptain;
}

int CDNPvP::_GetUserCountByTeamIndex(PvPCommon::Team::TPvPTeamIndex &Team)
{
	int nCount = 0;
	for (int i = 0; i < PARTYMAX; i++)
	{
		if (Team.nAccountDBID[i] <= 0) continue;
		nCount++;
	}
	return nCount;
}

bool CDNPvP::_CheckReadyGuildWarMaster()
{
	bool bCheckA, bCheckB;
	bCheckA = bCheckB = false;

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser && pUser->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain )
			{
				if( bIsReady( pUser ) )
				{
					if( pUser->GetPvPTeam() == PvPCommon::Team::A )
						bCheckA = true;
					else if( pUser->GetPvPTeam() == PvPCommon::Team::B )
						bCheckB = true;
				}
			}
		}
	}
	return (bCheckA && bCheckB) ? true : false;
}

int CDNPvP::SetPvPGuildWarMemberGrade(USHORT nTeam, bool bAsign, USHORT nType, UINT nSessionID)
{
	if( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == false )
	{
		_DANGER_POINT();
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	if (bAsign == false && nType == PvPCommon::UserState::GuildWarCaptain)
	{
		_DANGER_POINT();		//길드대장은 해임이 불가능하다!
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	if (nType != PvPCommon::UserState::GuildWarSedcondCaptain && nType != PvPCommon::UserState::GuildWarCaptain)
	{
		_DANGER_POINT();
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	CDNUser * pUser = NULL;
	if (nType == PvPCommon::UserState::GuildWarSedcondCaptain && bAsign)
	{
		if (GetUserCountByState(nTeam, PvPCommon::UserState::GuildWarSedcondCaptain) >= PvPCommon::Common::MaximumGuildWarSecondCaptain)
			return ERROR_PVP_GUILDWAR_SECONDARYMASTER_ASIGNFAIL;
	}

	int nRet = ERROR_INVALIDPACKET;
	
	CDNUser * pSecondaryCaptainUser = GetUserBySessionID(nSessionID);
	if (pSecondaryCaptainUser)		//바꿀유저가 PvP방에 있는지 확인
	{
		for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
		{
			pUser = g_pDivisionManager->GetUserByAccountDBID((*itor));
			if (pUser == NULL) continue;

			UINT nUserState = pUser->GetPvPUserState();
			if (pSecondaryCaptainUser->GetAccountDBID() == pUser->GetAccountDBID())
			{
				if (bAsign)
				{
					if (nUserState&PvPCommon::UserState::GuildWarSedcondCaptain)				
						nUserState &= ~PvPCommon::UserState::GuildWarSedcondCaptain;
					nUserState |= nType;
				}
				else
				{
					nUserState &= ~nType;
				}
				
				nRet = ERROR_NONE;
			}
			else if (pSecondaryCaptainUser->GetPvPTeam() == pUser->GetPvPTeam())	//같은 팀의 상태만 변경한다
			{
				if (nType == PvPCommon::UserState::GuildWarCaptain)
					nUserState &= ~nType;
			}
			SetPvPUserState(pUser, nUserState);
		}
	}
	return nRet;
}

bool CDNPvP::SetPvPMemberIndex(int nTeam, UINT nAccountDBID)
{
	std::vector <PvPCommon::Team::TPvPTeamIndex>::iterator ii;
	for (ii = m_vTeamSlotIndex.begin(); ii != m_vTeamSlotIndex.end(); ii++)
	{
		if ((*ii).nTeam == nTeam)
		{
			for (int i = 0; i < PARTYMAX; i++)
			{
				if ((*ii).nAccountDBID[i] == nAccountDBID)
				{
					(*ii).nAccountDBID[i] = 0;
					return true;
				}
			}
		}
	}
	return false;
}

bool CDNPvP::SwapPvPMemberIndex(USHORT nTeam, BYTE cCount, const TSwapMemberIndex * pIndex)
{
	//멤버인덱스는 현재 길드전에만 사용하는 개념
	if( IsMode( PvPCommon::GameMode::PvP_GuildWar ) == false )
	{
		_DANGER_POINT();
		return false;
	}

	if (pIndex == NULL)
	{
		_DANGER_POINT();
		return false;
	}

	//verify index range
	int nOffset = m_uiMaxUser/2;
	std::vector <BYTE> vDuplicate;
	std::vector <BYTE>::iterator iDuplicater;
	for (int i = 0; i < cCount; i++)
	{
		if (pIndex[i].cIndex > ((nTeam == PvPCommon::Team::A ? nOffset : m_uiMaxUser) - 1) || pIndex[i].cIndex < (nTeam == PvPCommon::Team::A ? 0 : nOffset))
		{
			_DANGER_POINT();		//범위오버
			return false;
		}

		iDuplicater = std::find(vDuplicate.begin(), vDuplicate.end(), pIndex[i].cIndex);
		if (vDuplicate.end() != iDuplicater)
		{
			_DANGER_POINT();		//중복인덱스
			return false;			//중복인덱스가 있으면 안데자나
		}

		vDuplicate.push_back(pIndex[i].cIndex);
	}

	std::vector <PvPCommon::Team::TPvPTeamIndex>::iterator ii;
	for (ii = m_vTeamSlotIndex.begin(); ii != m_vTeamSlotIndex.end(); ii++)
	{
		if ((*ii).nTeam == nTeam)
			break;
	}

	if (ii == m_vTeamSlotIndex.end())
	{
		_DANGER_POINT();
		return false;
	}

	//해당유저들이 같은 피비피방에 있는 인원인지 확인
	CDNUser * pUser;
	PvPCommon::Team::TPvPTeamIndex TeamIndex = (*ii);	
	for (int i = 0; i < cCount; i++)
	{
		if (pIndex[i].nSessionID <= 0)
		{	
			TeamIndex.nAccountDBID[pIndex[i].cIndex] = 0;
		}
		else
		{
			pUser = g_pDivisionManager->GetUserBySessionID(pIndex[i].nSessionID);
			if (pUser == NULL)
			{
				_DANGER_POINT();
				return false;
			}
			TeamIndex.nAccountDBID[pIndex[i].cIndex] = pUser->GetAccountDBID();
		}
	}

	//verify
	int nPreCount = _GetUserCountByTeamIndex((*ii));
	int nPostCount = _GetUserCountByTeamIndex(TeamIndex);
	if (nPreCount != nPreCount)
	{
		_DANGER_POINT();
		return false;
	}

	//Reset!!
	(*ii) = TeamIndex;
	return true;
}

int CDNPvP::GetUserCountByTeam(USHORT nTeam)
{
	int outiTeamCount = 0;
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser )
			{
				if( pUser->GetPvPTeam() == nTeam )
					++outiTeamCount;
			}
		}
	}
	return outiTeamCount;
}

int CDNPvP::GetUserCountByState(USHORT nTeam, USHORT nState)
{
	int outnUserCnt = 0;
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser && pUser->GetPvPTeam() == nTeam )
			{
				if( pUser->GetPvPUserState()&nState )
					++outnUserCnt;
			}
		}
	}
	return outnUserCnt;
}

bool CDNPvP::GetAndSetRemainTeamIndex(int nTeam, UINT nAccountDBID, char &cIndex)
{
	int nOffset = m_uiMaxUser/2;
	std::vector <PvPCommon::Team::TPvPTeamIndex>::iterator ii;
	for (ii = m_vTeamSlotIndex.begin(); ii != m_vTeamSlotIndex.end(); ii++)
	{
		if ((*ii).nTeam == nTeam)
		{
			//for (int i = 0; i < PARTYMAX; i++)
			for (int i = (nTeam == PvPCommon::Team::A ? 0 : nOffset); i < PARTYMAX; i++)
			{
				if ((*ii).nAccountDBID[i] == 0)
				{
					(*ii).nAccountDBID[i] = nAccountDBID;
					cIndex = (char)i;// + (nTeam == PvPCommon::Team::A ? 0 : nOffset);
					if (nTeam == PvPCommon::Team::A && (cIndex >= nOffset || cIndex < 0))
					{
						_DANGER_POINT();
					}
					else if (nTeam == PvPCommon::Team::B && (cIndex < nOffset || cIndex < 0))
					{
						_DANGER_POINT();
					}
					return true;
				}
			}
		}
	}
	
	return false;
}

char CDNPvP::GetPvPTeamIndex(CDNUser * pUser)
{
	if (pUser == NULL)
	{
		_DANGER_POINT();
		return -1;
	}

	int nOffset = m_uiMaxUser/2;
	std::vector <PvPCommon::Team::TPvPTeamIndex>::iterator ii;
	for (ii = m_vTeamSlotIndex.begin(); ii != m_vTeamSlotIndex.end(); ii++)
	{
		if ((*ii).nTeam == pUser->GetPvPTeam())
		{
			for (int i = (pUser->GetPvPTeam() == PvPCommon::Team::A ? 0 : nOffset); i < PARTYMAX; i++)
			{
				if ((*ii).nAccountDBID[i] == pUser->GetAccountDBID())
					return (char)i;// + (pUser->GetPvPTeam() == PvPCommon::Team::A ? 0 : nOffset);
			}
		}
	}	
	return -1;
}

UINT CDNPvP::SelectNewGuildWarCaptain(USHORT nTeam, UINT nExceptSessionID)
{
	CDNUser * pUser;
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		pUser = g_pDivisionManager->GetUserByAccountDBID((*itor));
		if (pUser && pUser->GetPvPTeam() == nTeam)
		{
			if (!(pUser->GetPvPUserState()&PvPCommon::UserState::GuildWarSedcondCaptain) && pUser->GetSessionID() != nExceptSessionID)
				return pUser->GetSessionID();
		}
	}

	//어랏? 없다면 인원이 적고 나머지 인원은 부대방으로 세팅되어 있는 경우이다. 한명을 승격시킨다.
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		pUser = g_pDivisionManager->GetUserByAccountDBID((*itor));
		if (pUser && pUser->GetSessionID() != nExceptSessionID)
			return pUser->GetSessionID();
	}
	return 0;
}

#if defined(PRE_ADD_PVP_TOURNAMENT)
char CDNPvP::FindTournamentIndex(UINT uiAccountDBID)
{
	for( char i=0; i<PvPCommon::Common::PvPTournamentUserMax; ++i)
	{
		if( m_vTournamentAccoutDBID[i] == uiAccountDBID)
			return i;
	}
	return -1;
}

char CDNPvP::GetTournamentIndex(UINT uiAccountDBID)
{
	for( char i=0 ; i<m_uiMaxUser; ++i )
	{
		if( m_vTournamentAccoutDBID[i] == 0 )
		{
			m_vTournamentAccoutDBID[i] = uiAccountDBID;		
			return i;
		}
	}
	return -1;
}

bool CDNPvP::DelTournamentIndex(UINT uiAccountDBID)
{
	for( char i=0; i<PvPCommon::Common::PvPTournamentUserMax; ++i)
	{
		if( m_vTournamentAccoutDBID[i] == uiAccountDBID)
		{
			m_vTournamentAccoutDBID[i] = 0;
			return true;
		}

	}
	return false;
}

bool CDNPvP::SwapPvPTournamentIndex(char cSourceIndex, char cDestIndex)
{
	if( cSourceIndex < 0 || cSourceIndex >= m_uiMaxUser || cDestIndex < 0 || cDestIndex >= m_uiMaxUser )
		return false;

	if( m_vTournamentAccoutDBID[cSourceIndex] == 0 || m_vTournamentAccoutDBID[cDestIndex] == 0)
		return false;

	UINT nTempAccountDBID = m_vTournamentAccoutDBID[cDestIndex];
	m_vTournamentAccoutDBID[cDestIndex] = m_vTournamentAccoutDBID[cSourceIndex];
	m_vTournamentAccoutDBID[cSourceIndex] = nTempAccountDBID;
	return true;
}

UINT CDNPvP::GetPvPTournamentAccountDBID(char cIndex)
{
	if( cIndex < 0 || cIndex >= m_uiMaxUser )
		return 0;

	return m_vTournamentAccoutDBID[cIndex];
}

void CDNPvP::SetTournamentShuffleIndex()
{
	// Max 유저를 기반으로 Random
	std::vector<UINT> vTournamentIndex;
	vTournamentIndex.reserve( m_uiMaxUser );

	for( UINT i=0 ; i<m_uiMaxUser ; ++i )
		vTournamentIndex.push_back( GetPvPTournamentIndex(i) );

	srand (unsigned (time (NULL)));
	random_shuffle (vTournamentIndex.begin(), vTournamentIndex.end());	// Random은 섞는다.			

	for( UINT i=0; i<m_uiMaxUser; ++i )
	{
		m_vTournamentAccoutDBID[i] = vTournamentIndex[i];
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID(m_vTournamentAccoutDBID[i]);
		if( pUser )		
			pUser->SetPvPTeam(i%2==0 ? PvPCommon::Team::A : PvPCommon::Team::B);
	}

	CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
	if( pVillageCon )
		pVillageCon->SendPvPShuffleTournamentIndex( this );
}
#endif

void CDNPvP::SetRoomState( const UINT uiRoomState )
{
	if (m_cEventRoomIndex > 0)
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"PVP State [Index:%d][Room:%d][Event:%d][State:%d] \r\n", m_uiIndex, m_iGameRoomID, m_cEventRoomIndex, uiRoomState);

	m_uiRoomState = uiRoomState;

	CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
	if( pVillageCon )
		pVillageCon->SendPvPRoomState( GetVillageChannelID(), m_uiIndex, m_uiRoomState );

	if( (uiRoomState&~PvPCommon::RoomState::Password) == PvPCommon::RoomState::None )	
	{
		m_iGameServerID			= 0;
		m_iGameRoomID			= 0;
		m_iRandomSeed			= timeGetTime();	// 랜덤시드재발급
		m_bIsCanStartEventRoom	= false;

		// S/N 재발급
		SYSTEMTIME st;
		GetLocalTime( &st );
		char szSN[MAX_PATH];
		sprintf( szSN, "%.4d%.2d%.2d%.2d%.2d%.2d%.3d%", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
		m_biSNMain = _atoi64( szSN );
		
		sprintf( szSN, "%.2d%.2d%.4d", g_Config.nWorldSetID, m_cVillageID, m_uiIndex );
		m_iSNSub = atoi( szSN );
		
		// UserState 초기화
		for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser )
			{
				UINT uiUserState	= pUser->GetPvPUserState();
				UINT uiCheckState	= PvPCommon::UserState::None;
				if( uiUserState&PvPCommon::UserState::Captain )
					uiCheckState = PvPCommon::UserState::Captain;

				if(uiUserState&PvPCommon::UserState::GuildWarCaptain)
					uiCheckState |=PvPCommon::UserState::GuildWarCaptain;
				if( IsMode( PvPCommon::GameMode::PvP_AllKill) )
				{
					if(uiUserState&PvPCommon::UserState::GroupCaptain)
						uiCheckState |=PvPCommon::UserState::GroupCaptain;
				}
				if( uiUserState != uiCheckState )
					SetPvPUserState( pUser, uiCheckState );
			}
		}
		if( IsMode( PvPCommon::GameMode::PvP_AllKill) )
		{
			CheckAndSetGroupCaptain();
		}
	}
	else if( uiRoomState&PvPCommon::RoomState::Starting )
	{
#if defined( PRE_PVP_GAMBLEROOM )
		if( m_cEventRoomIndex > 0 || m_cGambleType > 0 )
#else
		if( m_cEventRoomIndex > 0 )
#endif
		{
			m_uiStartMsgCount	= 0;
			m_uiStartMsgTick	= 0;
		}
		else
		{
			m_uiStartMsgCount	= 5;
			m_uiStartMsgTick	= 0;
		}

		if( g_pDivisionManager )
		{
			for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
			{
				CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
				if( pUser )
				{
					if( bIsReady( pUser ) )
					{
						UINT uiUserState = pUser->GetPvPUserState();
						uiUserState |= PvPCommon::UserState::Starting;

						SetPvPUserState( pUser, uiUserState );
					}
					else if( pUser->bIsObserver() )
					{
						UINT uiUserState = pUser->GetPvPUserState();
						uiUserState |= PvPCommon::UserState::Starting;

						SetPvPUserState( pUser, uiUserState );
					}
					// 이벤트방은 Starting 되는 시점에 모든 유저를 강제로 시작시킨다.
#if defined( PRE_PVP_GAMBLEROOM )
					else if( m_cEventRoomIndex > 0 || m_cGambleType > 0 )
#else
					else if( m_cEventRoomIndex > 0 )
#endif
					{
						UINT uiUserState = pUser->GetPvPUserState();
						uiUserState |= (PvPCommon::UserState::Starting|PvPCommon::UserState::Ready);

						SetPvPUserState( pUser, uiUserState );
					}
				}
				else
					_DANGER_POINT();
			}
		}
		else
			_DANGER_POINT();
	}
	else if( uiRoomState&PvPCommon::RoomState::Playing )
	{
		if( g_pDivisionManager )
		{
			for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
			{
				CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
				if( pUser )
					AddPvPUserState( pUser, PvPCommon::UserState::Playing );
				else
					_DANGER_POINT();
			}
		}
		else
			_DANGER_POINT();
	}
}

void CDNPvP::AddPvPUserState( CDNUser* pUser, const UINT uiAddUserSate )
{
	switch( uiAddUserSate )
	{
		case PvPCommon::UserState::Playing:
		{
			UINT uiUserState = pUser->GetPvPUserState();
			if( uiUserState&PvPCommon::UserState::Syncing )
			{
				uiUserState &= ~(PvPCommon::UserState::Syncing|PvPCommon::UserState::Ready);
				uiUserState |= PvPCommon::UserState::Playing;

				SetPvPUserState( pUser, uiUserState );
			}

			break;
		}
	}
}

void CDNPvP::SetPvPUserState( CDNUser* pUser, const UINT uiUserState )
{
	pUser->SetPvPUserState( uiUserState );

	CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
	if( pVillageCon )
		pVillageCon->SendPvPChangeUserState( GetVillageChannelID(), m_uiIndex, pUser->GetAccountDBID(), uiUserState );
}

void CDNPvP::SetAllGameID( USHORT wGameID, int nRandomSeed, int nMapIdx, BYTE cGateNum )
{
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
		if( pUser )
		{
			if( pUser->GetPvPUserState()&PvPCommon::UserState::Starting )
			{
				UINT uiUserState = pUser->GetPvPUserState()&~PvPCommon::UserState::Starting;
				uiUserState	   |= PvPCommon::UserState::Syncing;

				if (pUser->SetCheckGameInfo( wGameID, -1 ) == false)
					continue;

				pUser->m_nRandomSeed	= nRandomSeed;
				pUser->m_nMapIndex		= nMapIdx;
				pUser->m_cGateNo		= cGateNum;

				SetPvPUserState( pUser, uiUserState );

				if (m_cEventRoomIndex > 0)
					g_Log.Log(LogType::_PVPROOM, pUser, L"SetAllGameID Event [Index:%d][Room:%d][Event:%d][CHRID:%d]\r\n", m_uiIndex, m_iGameRoomID, m_cEventRoomIndex, pUser->GetCharacterDBID());
			}
		}
		else
			_DANGER_POINT();
	}
}

// 게임서버에서 정상적으로 PvP방이 만들어짐.
void CDNPvP::SetAllRoomID( CDNVillageConnection* pVillageCon, CDNGameConnection* pGameCon, int nRoomID, int nServerIdx )
{
	if( !g_pDivisionManager )
	{
		_DANGER_POINT();
		return;
	}

	m_iGameServerID			= pGameCon->GetGameID();
	m_iGameRoomID			= nRoomID;

	pGameCon->SendPvPGameMode( this );
#if defined(PRE_ADD_PVP_TOURNAMENT)
	if( IsMode(PvPCommon::GameMode::PvP_Tournament) == true)	
	{
		//랜덤팀이면 한번 섞는다.
		if( m_unRoomOptionBit & PvPCommon::RoomOption::RandomTeam )		
			SetTournamentShuffleIndex();
		pGameCon->SendPvPTournamentUserInfo( this ); // 대진표 정보 게임서버에 보내주기.		
	}
#endif

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
		// 동기화중 유저만...
		if( pUser && pUser->GetPvPUserState()&PvPCommon::UserState::Syncing )
		{
			if( g_pDivisionManager->VillageToGame( pVillageCon, (*itor), m_iGameServerID, nRoomID, nServerIdx ) == false )
				_DANGER_POINT();
		}
	}
}

void CDNPvP::GetMembersByAccountDBID( std::vector<UINT>& rvAccountDBID )
{
	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
		rvAccountDBID.push_back( *itor );
}

// Retrun
// True : PvP방 파괴
// Flag : PvP방 파괴 안됨
bool CDNPvP::bDisconnectServer( const int iConnectionKey )
{
	switch( iConnectionKey )
	{
		case CONNECTIONKEY_VILLAGE:
		{
			// 리스트 백업본 생성
			std::list<UINT> listBackUp = m_listAccountDBID;

			for( std::list<UINT>::iterator itor=listBackUp.begin() ; itor!=listBackUp.end() ; ++itor )
			{
				if( g_pDivisionManager )
				{
					CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
					if( pUser )
						bLeave( NULL, pUser, true );
				}
			}
			return true;
		}
		case CONNECTIONKEY_GAME:
		{
			CDNVillageConnection* pVillageCon = NULL;
			if( g_pDivisionManager )
				pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );

			if( !pVillageCon )
			{
				_DANGER_POINT();
				return false;
			}

			// 리스트 백업본 생성
			std::list<UINT> listBackUp = m_listAccountDBID;

			for( std::list<UINT>::iterator itor=listBackUp.begin() ; itor!=listBackUp.end() ; ++itor )
			{
				CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
				if( pUser )
				{
					// PvPCommon::UserState::Syncing|PvPCommon::UserStaet::Playing 상태라면 게임서버 다운시 PvP방에서 내쫒는다.
					if( pUser->GetPvPUserState()&(PvPCommon::UserState::Syncing|PvPCommon::UserState::Playing))
						bLeave( pVillageCon, pUser );
				}
			}

			break;
		}
	}

	return bIsEmpty() ? true : false;
}

void CDNPvP::_RequestGameRoom()
{
	UINT uiRoomState = m_uiRoomState&~PvPCommon::RoomState::Starting;

	if( g_pDivisionManager->RequestGameRoom( REQINFO_TYPE_PVP, m_uiIndex, m_iRandomSeed, m_uiMapIndex, 0, 0, true, false, 0) )
	{
		SetRoomState( uiRoomState|PvPCommon::RoomState::Syncing );
		return;
	}
	else
	{
		SetRoomState( uiRoomState );

		CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( m_cVillageID );
		if( pVillageCon )
			pVillageCon->SendPvPStart( ERROR_GENERIC_GAMECON_NOT_FOUND, GetVillageChannelID(), m_uiIndex, 0 );
	}
}

USHORT CDNPvP::_SelectRandomTeam()
{
	UINT	uiATeam		= 0;
	UINT	uiBTeam		= 0;

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( *itor );
			if( pUser )
			{
				if( pUser->GetPvPTeam() == PvPCommon::Team::A )
					++uiATeam;
				else if( pUser->GetPvPTeam() == PvPCommon::Team::B )
					++uiBTeam;
				else if (pUser->GetPvPTeam() != PvPCommon::Team::Observer)
					_DANGER_POINT();
			}
		}
		else
			_DANGER_POINT();
	}

	return (uiATeam <= uiBTeam) ? PvPCommon::Team::A : PvPCommon::Team::B;
}

void CDNPvP::_GetReadyUserCount( UINT& ruiATeam, UINT& ruiBTeam )
{
	ruiATeam = 0;
	ruiBTeam = 0;

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser )
			{
				if( bIsReady( pUser ) )
				{
					if( pUser->GetPvPTeam() == PvPCommon::Team::A )
						++ruiATeam;
					else if( pUser->GetPvPTeam() == PvPCommon::Team::B )
						++ruiBTeam;
				}
			}
		}
	}
}

void CDNPvP::_GetTeamUserCount( UINT& ruiATeam, UINT& ruiBTeam )
{
	ruiATeam = 0;
	ruiBTeam = 0;

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		if( g_pDivisionManager )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser )
			{
				if( pUser->GetPvPTeam() == PvPCommon::Team::A )
					++ruiATeam;
				else if( pUser->GetPvPTeam() == PvPCommon::Team::B )
					++ruiBTeam;
			}
		}
	}
}

UINT CDNPvP::FindNewCaptain()
{
	// 게임이 플레이중일때는 플레이중인 유저중 방장을 선출한다.
	if( m_uiRoomState&PvPCommon::RoomState::Playing )
	{
		for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser && !pUser->bIsObserver() && !bIsCaptain( *itor ) )
			{
				if( pUser->GetPvPUserState()&PvPCommon::UserState::Playing )
					return (*itor);
			}
		}
	}

	for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
	{
		CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
		if( pUser && !pUser->bIsObserver() && !bIsCaptain(*itor) )
			return (*itor);
	}

	return 0;
}

UINT CDNPvP::FindNewGroupCaptain( UINT uiTeam )
{
	if( IsMode( PvPCommon::GameMode::PvP_AllKill) == true )
	{
		// 게임이 플레이중일때는 플레이중인 유저중 선출한다.
		if( m_uiRoomState&(PvPCommon::RoomState::Playing|PvPCommon::RoomState::Syncing) )
		{
			for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
			{
				CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
				if( pUser && pUser->GetPvPTeam() == uiTeam && !pUser->bIsPvPGroupCaptain() )
				{
					if( pUser->GetPvPUserState()&PvPCommon::UserState::Playing )
						return (*itor);
				}
			}
		}

		for( std::list<UINT>::iterator itor=m_listAccountDBID.begin() ; itor!=m_listAccountDBID.end() ; ++itor )
		{
			CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( (*itor) );
			if( pUser && pUser->GetPvPTeam() == uiTeam && !pUser->bIsPvPGroupCaptain() )
				return (*itor);
		}
	}

	return 0;
}
#if defined(PRE_ADD_QUICK_PVP)
void CDNPvP::AddListAccountDBID(UINT uiAddAccountDBID)
{
	m_listAccountDBID.push_back(uiAddAccountDBID);
}
#endif //#if defined(PRE_ADD_QUICK_PVP)
