
#include "Stdafx.h"
#include "PvPAllKillMode.h"
#include "DNGameRoom.h"
#include "DNUserSession.h"
#include "PvPAllKillModeState.hpp"
#include "PvPScoreSystem.h"
#include "PvPAllKillScoreSystem.h"

CPvPAllKillMode::CPvPAllKillMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPRoundMode( pGameRoom, pPvPGameModeTable, pPacket ),m_pState(NULL)
{
	ChangeState( eState::None );
}

CPvPAllKillMode::~CPvPAllKillMode()
{
	SAFE_DELETE( m_pState );
}

void CPvPAllKillMode::ChangeState( eState State )
{
	IPvPAllKillModeState* pPrevState = m_pState;
	IPvPAllKillModeState* pNewState = NULL;

	switch( State )
	{
		case eState::None:
		{
			pNewState = new CPvPAllKillModeNoneState( this );
			break;
		}
		case eState::SelectPlayer:
		{
			if( static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetRoomOptionBit()&PvPCommon::RoomOption::AllKill_RandomOrder )
			{
				pNewState = new CPvPAllKillModeStartingState( this );
				{
					DnActorHandle hActor = GetActiveActor( PvPCommon::TeamIndex::A );
					if( !hActor || hActor->IsDie() )
						SetActiveActor( PvPCommon::TeamIndex::A, CDnActor::Identity() );
				}
				{
					DnActorHandle hActor = GetActiveActor( PvPCommon::TeamIndex::B );
					if( !hActor || hActor->IsDie() )
						SetActiveActor( PvPCommon::TeamIndex::B, CDnActor::Identity() );
				}
				CalcActiveActor( false );
				SendActiveActor();
			}
			else
				pNewState = new CPvPAllKillModeSelectPlayerState( this );			
			break;
		}
		case eState::Starting:
		{
			pNewState = new CPvPAllKillModeStartingState( this );
			break;
		}
		case eState::Playing:
		{
			pNewState = new CPvPAllKillModePlayingState( this );
			break;
		}
		case eState::FinishRound:
		{
#if defined( PRE_WORLDCOMBINE_PVP )
//			if( GetGameRoom()->GetWorldPvPRoomReqType() == WorldPvPMissionRoom::Common::GMRoom )
//				pNewState = new CPvPAllKillModeNoneState( this );
//			else
				pNewState = new CPvPAllKillModeFinishRoundState( this );
#else
			pNewState = new CPvPAllKillModeFinishRoundState( this );
#endif
			break;
		}
	}

	if( pPrevState )
		pPrevState->EndState();
	pNewState->BeginState();

	SAFE_DELETE( pPrevState );
	m_pState = pNewState;
}

void CPvPAllKillMode::SetActiveActorStartPosition()
{
	_SetRespawnPosition( GetActiveActor( PvPCommon::TeamIndex::A ) );
	_SetRespawnPosition( GetActiveActor( PvPCommon::TeamIndex::B ) );
}

void CPvPAllKillMode::_BeforeProcess( LOCAL_TIME LocalTime, float fDelta )
{
	m_pState->BeforeProcess( fDelta );
	
	if( m_pState->GetState() == eState::None )
		ChangeState( SelectPlayer );
}

void CPvPAllKillMode::_AfterProcess( LOCAL_TIME LocalTime, float fDelta )
{
	m_pState->AfterProcess( fDelta );

	switch( m_pState->GetState() )
	{
		case SelectPlayer:
		{
			if( m_pState->GetElapsedTick() >= PvPCommon::AllKillMode::Time::SelectPlayerTick )
				ChangeState( Starting );
			break;
		}
		case Starting:
		{
			if( m_fStartDelta <= 0.f )
				ChangeState( Playing );
			break;
		}
		case Playing:
		{
			if( m_bFinishRoundFlag == true )
				ChangeState( FinishRound );
			break;
		}
		case FinishRound:
		{
			if( m_fFinishRoundDelta <= 0.f )
				ChangeState( SelectPlayer );
			break;
		}
	}
}

int CPvPAllKillMode::OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	if (pSession == NULL)
		return ERROR_GENERIC_INVALIDREQUEST;

	switch (nSubCmd)
	{
		case ePvP::CS_PVP_ALLKILL_SELECTPLAYER:
		{
			if( sizeof(CSPVP_ALLKILL_SELECTPLAYER) != nLen )
				return ERROR_INVALIDPACKET;

			CSPVP_ALLKILL_SELECTPLAYER* pPacket = reinterpret_cast<CSPVP_ALLKILL_SELECTPLAYER*>(pData);

			DnActorHandle hActor = pSession->GetActorHandle();
			if( !hActor )
				break;
			if( bIsGroupCaptain( hActor ) == false )
				break;
			CDNUserSession* pSelectSession = GetGameRoom()->GetUserSession( pPacket->uiSelectPlayerSessionID );
			if( pSelectSession == NULL )
				break;
			DnActorHandle hSelectActor = pSelectSession->GetActorHandle();
			if( !hSelectActor )
				break;
			if( hActor->GetTeam() != hSelectActor->GetTeam() )
				break;
			if( m_pState->GetState() != CPvPAllKillMode::SelectPlayer )
				break;
			SetActiveActor( PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(hSelectActor->GetTeam()) ), hSelectActor, true );
			break;
		}
	}

	return ERROR_NONE;
}

void CPvPAllKillMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
#if defined( PRE_WORLDCOMBINE_PVP )
	if( GetGameRoom()->bIsWorldPvPRoom() )
	{
		if( GetGameRoom()->bIsWorldPvPRoomStart() && GetGameRoom()->GetBreakIntoUserCount() == 0 )
		{
			if( !bIsStartSetting() )
			{
				SetWorldPvPRoomStart();
			}
			_BeforeProcess( LocalTime, fDelta );
		}
	}
	else
		_BeforeProcess( LocalTime, fDelta );
	CPvPRoundMode::Process( LocalTime, fDelta );
	_AfterProcess( LocalTime, fDelta );	
#else
	_BeforeProcess( LocalTime, fDelta );
	CPvPRoundMode::Process( LocalTime, fDelta );
	_AfterProcess( LocalTime, fDelta );
#endif
}

void CPvPAllKillMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	UINT uiWinTeam = PvPCommon::Team::Max;
	m_FinishDetailReason = PvPCommon::FinishDetailReason::None;

	switch( Reason )
	{
		case PvPCommon::FinishReason::OpponentTeamAllDead:
		{
			DnActorHandle hATeamActor = GetActiveActor( PvPCommon::TeamIndex::A );
			DnActorHandle hBTeamActor = GetActiveActor( PvPCommon::TeamIndex::B );

			if( !hATeamActor || !hBTeamActor )
				return;

			if( hATeamActor->IsDie() && hBTeamActor->IsDie() )
				return;

			if( !(hATeamActor->IsDie()) && !(hBTeamActor->IsDie()) )
				return;

			uiWinTeam = hATeamActor->IsDie() ? PvPCommon::Team::B : PvPCommon::Team::A;
			break;
		}
		case PvPCommon::FinishReason::OpponentTeamAllGone:
		{
			DnActorHandle hATeamActor = GetActiveActor( PvPCommon::TeamIndex::A );
			DnActorHandle hBTeamActor = GetActiveActor( PvPCommon::TeamIndex::B );

			if( hATeamActor && hBTeamActor )
				return;

			if( hATeamActor && !hBTeamActor )
				uiWinTeam = PvPCommon::Team::A;
			else if( !hATeamActor && hBTeamActor )
				uiWinTeam = PvPCommon::Team::B;
			break;
		}
		case PvPCommon::FinishReason::TimeOver:
		{
			DnActorHandle hATeamActor = GetActiveActor( PvPCommon::TeamIndex::A );
			DnActorHandle hBTeamActor = GetActiveActor( PvPCommon::TeamIndex::B );

			if( hATeamActor && hBTeamActor )
			{
				m_FinishDetailReason = PvPCommon::FinishDetailReason::SuperiorHPWin;

				int iAHPPercent = hATeamActor->GetHPPercent();
				int iBHPPercent = hBTeamActor->GetHPPercent();

				if( iAHPPercent > iBHPPercent )
					uiWinTeam = PvPCommon::Team::A;
				else if( iAHPPercent < iBHPPercent )
					uiWinTeam = PvPCommon::Team::B;
				else
				{
					CPvPScoreSystem* pScoreSystem = static_cast<CPvPScoreSystem*>(GetScoreSystem());
					CPvPScoreSystem::SMyScore* pAScore = pScoreSystem->FindMyScoreData( hATeamActor->GetName() );
					CPvPScoreSystem::SMyScore* pBScore = pScoreSystem->FindMyScoreData( hBTeamActor->GetName() );

					if( pAScore && pBScore )
					{
						if( pAScore->uiWinRound == pBScore->uiWinRound )
						{
							uiWinTeam = PvPCommon::Team::Max;
							m_FinishDetailReason = PvPCommon::FinishDetailReason::HPnContinuousWinDraw;
						}
						else
						{
							m_FinishDetailReason = PvPCommon::FinishDetailReason::SuperiorContinuousWin;
							uiWinTeam = pAScore->uiWinRound>pBScore->uiWinRound ? PvPCommon::Team::A : PvPCommon::Team::B;
						}
					}
				}
			}
			else if( !hATeamActor && !hBTeamActor )
			{
				uiWinTeam = PvPCommon::Team::Max;
			}
			else
			{
				uiWinTeam = hATeamActor ? PvPCommon::Team::A : PvPCommon::Team::B;
			}

			break;
		}
		default:
		{
			return;
		}
	}

	_ProcessFinishRound( uiWinTeam, Reason );
}

void CPvPAllKillMode::OnFinishRound()
{

}

void CPvPAllKillMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	m_pState->OnDie( hActor, hHitter );
}

void CPvPAllKillMode::OnLeaveUser( DnActorHandle hActor )
{
	CPvPRoundMode::OnLeaveUser( hActor );
	m_pState->OnLeaveUser( hActor );
}

void CPvPAllKillMode::OnInitializeActor( CDNUserSession* pSession )
{
	CPvPGameMode::OnInitializeActor( pSession );

	if( pSession->GetPvPUserState()&PvPCommon::UserState::GroupCaptain )
	{
		PvPCommon::TeamIndex::eCode TeamIndex = PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(pSession->GetTeam()) );
		
		if( TeamIndex != PvPCommon::TeamIndex::Max )
			SetGroupCaptainActor( TeamIndex, pSession->GetActorHandle() );
	}
}

UINT CPvPAllKillMode::GetPvPStartStateEffectDurationTick()
{
	UINT uiTick = CPvPGameMode::GetPvPStartStateEffectDurationTick();
	if( uiTick > 0 )
	{
		return uiTick+m_pState->GetRemainStateTick();
	}

	return 0;
}

void CPvPAllKillMode::OnCmdPvPStartAddStateEffect( DnActorHandle hActor )
{
	hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_230, -1, NULL, true );	// 230) 올킬모드에 사용하는 Spectator 상태 
	hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_099, -1, "-1", true );	// 99) 프로그램쪽에서만 쓰는 무적 상태효과
}

void CPvPAllKillMode::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	CPvPRoundMode::OnSuccessBreakInto( pGameSession );
#if defined(PRE_ADD_PVP_DUAL_INFO)
	// 난입한 경우에 현재 싸우는 유저정보 보내줌
	SendBattleActor(pGameSession);
#endif // #if defined(PRE_ADD_PVP_DUAL_INFO)
}

void CPvPAllKillMode::_OnRefreshHPSP( DnActorHandle hActor )
{
}

void CPvPAllKillMode::_OnAfterProcess()
{

}

bool CPvPAllKillMode::_CanProcess()
{
	switch( m_pState->GetState() )
	{
		case SelectPlayer:
		case FinishRound:
		{
			return false;
		}
	}

	return true;
}

bool CPvPAllKillMode::_CheckFinishGameMode( UINT& uiWinTeam )
{
	bool ATeam = false;
	bool BTeam = false;

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( pSession == NULL )
			continue;
		DnActorHandle hActor = pSession->GetActorHandle();
		if( !hActor )
			continue;
		if( bIsPlayingUser( hActor ) == false )
			continue;
		if( hActor->IsDie() == false )
		{
			switch( hActor->GetTeam() )
			{
				case PvPCommon::Team::A:
				{
					ATeam = true;
					break;
				}
				case PvPCommon::Team::B:
				{
					BTeam = true;
					break;
				}
			}

			if( ATeam && BTeam )
				return false;
		}
	}

	if( ATeam == false && BTeam == false )
		uiWinTeam = PvPCommon::Team::Max;
	else if( ATeam == true && BTeam == false )
		uiWinTeam = PvPCommon::Team::A;
	else
		uiWinTeam = PvPCommon::Team::B;
	return true;
}

UINT CPvPAllKillMode::_GetPvPRoundStartStateEffectDurationTick()
{
	if( static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetRoomOptionBit()&PvPCommon::RoomOption::AllKill_RandomOrder )
		return PvPCommon::Common::GameModeStartDelta*1000;
	else
		return PvPCommon::Common::GameModeStartDelta*1000+PvPCommon::AllKillMode::Time::SelectPlayerTick;
}

void CPvPAllKillMode::_ProcessFinishRoundMode( CDNUserSession* pSession, bool bIsWin, UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason )
{
	DnActorHandle hActor = pSession->GetActorHandle();
	if( !hActor )
		return;

	if( bIsActiveActor( hActor) == false )
		return;

	if( bIsWin == true )
	{
		INT64 iHP = hActor->GetHP();
		iHP += (hActor->GetMaxHP()*PvPCommon::AllKillMode::WinBonus::HPPercent/100);
		int iSP = hActor->GetSP();
		iSP += (hActor->GetMaxSP()*PvPCommon::AllKillMode::WinBonus::SPPercent/100);
		hActor->CmdRefreshHPSP( iHP, iSP );
	}
	else
	{
		hActor->CmdRefreshHPSP( 0,0 );
	}
}

DnActorHandle CPvPAllKillMode::GetPreWinActor()
{
	return static_cast<const CPvPAllKillModeScoreSystem*>(GetScoreSystem()->GetPvPGameModeScoreSystem())->GetPreWinActor();
}

void CPvPAllKillMode::SetGroupCaptainActor( PvPCommon::TeamIndex::eCode Team, DnActorHandle hActor, bool bSend/*=false*/ )
{
	if( Team >= PvPCommon::TeamIndex::Max )
		return;
	m_hGroupCaptainActor[Team] = hActor;
	if( bSend == true )
		_SendGroupCaptain( Team );
}

DnActorHandle CPvPAllKillMode::GetGroupCaptainActor( PvPCommon::TeamIndex::eCode Team )
{
	if( Team >= PvPCommon::TeamIndex::Max )
		return CDnActor::Identity();

	return m_hGroupCaptainActor[Team];
}

void CPvPAllKillMode::ClearGroupCaptain( DnActorHandle hActor )
{
	if( !hActor )
		return;

	PvPCommon::TeamIndex::eCode TeamIndex = PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(hActor->GetTeam()) );
	if( TeamIndex == PvPCommon::TeamIndex::Max )
		return;

	if( m_hGroupCaptainActor[TeamIndex] == hActor )
		m_hGroupCaptainActor[TeamIndex] = CDnActor::Identity();
}

void CPvPAllKillMode::CalcGroupCaptain( bool bSend, DnActorHandle hIgnoreActor/*=CDnActor::Identity()*/ )
{
	for( UINT i=0 ; i<PvPCommon::TeamIndex::Max ; ++i )
	{
		if( m_hGroupCaptainActor[i] )
			continue;
		
		PvPCommon::Team::eTeam Team = PvPCommon::TeamIndex2Team( static_cast<PvPCommon::TeamIndex::eCode>(i) );
		if( Team == PvPCommon::Team::Max )
			continue;

		for( UINT j=0 ; j<GetGameRoom()->GetUserCount() ; ++j )
		{
			CDNUserSession* pSession = GetGameRoom()->GetUserData(j);
			if( pSession == NULL )
				continue;
			DnActorHandle hActor = pSession->GetActorHandle();
			if( !hActor )
				continue;
			if( hActor->GetTeam() != Team )
				continue;
			if( bIsPlayingUser( hActor) == false )
				continue;
			if( hIgnoreActor && hIgnoreActor == hActor )
				continue;

			SetGroupCaptainActor( static_cast<PvPCommon::TeamIndex::eCode>(i), hActor, bSend );
			break;
		}
	}
}

void CPvPAllKillMode::SendGroupCaptain()
{
	_SendGroupCaptain( PvPCommon::TeamIndex::A );
	_SendGroupCaptain( PvPCommon::TeamIndex::B );
}

void CPvPAllKillMode::_SendGroupCaptain( PvPCommon::TeamIndex::eCode TeamIndex )
{
	DnActorHandle hActor = GetGroupCaptainActor( TeamIndex );
	UINT uiGroupCaptainSessionID = hActor ? hActor->GetUniqueID() : 0;

	int iTeam = PvPCommon::TeamIndex2Team( TeamIndex );

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( pSession == NULL )
			continue;
#ifdef PRE_MOD_PVPOBSERVER
		if(pSession->GetTeam() == iTeam || pSession->GetTeam() == PvPCommon::Team::Observer)
#else		//#ifdef PRE_MOD_PVPOBSERVER
		if( pSession->GetTeam() == iTeam )
#endif		//#ifdef PRE_MOD_PVPOBSERVER
			pSession->SendPvPAllKillGroupCaptain( uiGroupCaptainSessionID );
	}
}

bool CPvPAllKillMode::bIsGroupCaptain( DnActorHandle hActor )
{
	if( !hActor )
		return false;

	PvPCommon::TeamIndex::eCode TeamIndex = PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(hActor->GetTeam()) );
	if( TeamIndex == PvPCommon::TeamIndex::Max )
		return false;
	
	return (hActor == m_hGroupCaptainActor[TeamIndex]);
}

void CPvPAllKillMode::SetActiveActor( PvPCommon::TeamIndex::eCode Team, DnActorHandle hActor, bool bSend/*=false*/ )
{
	if( Team >= PvPCommon::TeamIndex::Max )
		return;
	m_hActiveActor[Team] = hActor;
	if( bSend == true )
		_SendActiveActor( Team );
}

DnActorHandle CPvPAllKillMode::GetActiveActor( PvPCommon::TeamIndex::eCode Team )
{
	if( Team >= PvPCommon::TeamIndex::Max )
		return CDnActor::Identity();

	return m_hActiveActor[Team];
}

void CPvPAllKillMode::ClearActiveActor( DnActorHandle hActor )
{
	if( !hActor )
		return;

	PvPCommon::TeamIndex::eCode TeamIndex = PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(hActor->GetTeam()) );
	if( TeamIndex == PvPCommon::TeamIndex::Max )
		return;

	if( m_hActiveActor[TeamIndex] == hActor )
		m_hActiveActor[TeamIndex] = CDnActor::Identity();
}

void CPvPAllKillMode::CalcActiveActor( bool bSend, DnActorHandle hIgnoreActor/*=CDnActor::Identity()*/ )
{
	for( UINT i=0 ; i<PvPCommon::TeamIndex::Max ; ++i )
	{
		if( m_hActiveActor[i] )
			continue;

		PvPCommon::Team::eTeam Team = PvPCommon::TeamIndex2Team( static_cast<PvPCommon::TeamIndex::eCode>(i) );
		if( Team == PvPCommon::Team::Max )
			continue;

		std::vector<DnActorHandle> vList;

		for( UINT j=0 ; j<GetGameRoom()->GetUserCount() ; ++j )
		{
			CDNUserSession* pSession = GetGameRoom()->GetUserData(j);
			if( pSession == NULL )
				continue;
			DnActorHandle hActor = pSession->GetActorHandle();
			if( !hActor )
				continue;
			if( hActor->IsDie() )
				continue;
			if( hActor->GetTeam() != Team )
				continue;
			if( bIsPlayingUser( hActor) == false )
				continue;
			if( hIgnoreActor && hIgnoreActor == hActor )
				continue;

			vList.push_back( hActor );
		}

		if( vList.empty() == false )
		{
			SetActiveActor( static_cast<PvPCommon::TeamIndex::eCode>(i), vList[_roomrand(GetGameRoom())%vList.size()], bSend );
		}
	}
}

void CPvPAllKillMode::SendActiveActor()
{
	_SendActiveActor( PvPCommon::TeamIndex::A );
	_SendActiveActor( PvPCommon::TeamIndex::B );
}

void CPvPAllKillMode::SendBattleActor(CDNUserSession* pBreakIntoUserSession)
{
	DnActorHandle hATeamActor = GetActiveActor( PvPCommon::TeamIndex::A );
	DnActorHandle hBTeamActor = GetActiveActor( PvPCommon::TeamIndex::B );

	SCPVP_ALLKILL_BATTLEPLAYER TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiSessionIDArr[0] = hATeamActor ? hATeamActor->GetUniqueID() : 0;
	TxPacket.uiSessionIDArr[1] = hBTeamActor ? hBTeamActor->GetUniqueID() : 0;

	if( pBreakIntoUserSession )
	{
		pBreakIntoUserSession->AddSendData( SC_PVP, ePvP::SC_PVP_ALLKILL_BATTLEPLAYER, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
		return;
	}

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( pSession == NULL )
			continue;

		pSession->AddSendData( SC_PVP, ePvP::SC_PVP_ALLKILL_BATTLEPLAYER, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
	}
}

void CPvPAllKillMode::_SendActiveActor( PvPCommon::TeamIndex::eCode TeamIndex )
{
	DnActorHandle hActor = GetActiveActor( TeamIndex );
	UINT uiActiveActorSessionID = hActor ? hActor->GetUniqueID() : 0;

	int iTeam = PvPCommon::TeamIndex2Team( TeamIndex );

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( pSession == NULL )
			continue;

#ifdef PRE_MOD_PVPOBSERVER
		if(pSession->GetTeam() == iTeam || pSession->GetTeam() == PvPCommon::Team::Observer)
#else		//#ifdef PRE_MOD_PVPOBSERVER
		if( pSession->GetTeam() == iTeam )
#endif		//#ifdef PRE_MOD_PVPOBSERVER
			pSession->SendPvPAllKillActiveActor( uiActiveActorSessionID );
	}
}

bool CPvPAllKillMode::bIsActiveActor( DnActorHandle hActor )
{
	if( !hActor )
		return false;

	PvPCommon::TeamIndex::eCode TeamIndex = PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(hActor->GetTeam()) );
	if( TeamIndex == PvPCommon::TeamIndex::Max )
		return false;

	return (hActor == m_hActiveActor[TeamIndex]);
}

#if defined( PRE_WORLDCOMBINE_PVP )
void CPvPAllKillMode::SetWorldPvPRoomStart()
{
	printf("CPvPAllKillMode::SetWorldPvPRoomStart() \n");
	if( GetGameRoom()->GetBreakIntoUserCount() == 0)
	{
		
		printf("GetGameRoom()->GetBreakIntoUserCount()  \n");
		GetGameRoom()->OnSendTeamData(NULL);
		UINT nSessionID[PARTYMAX] = {0};
		int nTeam[PARTYMAX] = {0};
		int nCount = 0;
		for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
		{
			CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
			if( pSession == NULL )
				continue;
			DnActorHandle hActor = pSession->GetActorHandle();
			if(hActor)
			{			
				_OnStartRoundStartPosition(hActor);
				nSessionID[nCount] = hActor->GetSessionID();
				nTeam[nCount] = hActor->GetTeam();
				nCount++;
			}
		}

		for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
		{
			CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
			if( pSession == NULL )
				continue;
			pSession->SendWorldPvPRoomAllKillTeamInfo( nSessionID, nTeam );
		}

		SetStartSetting(true);
	}	
}
#endif
