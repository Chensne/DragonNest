
#include "stdafx.h"
#include "PvPRoundMode.h"
#include "DNPvPGameRoom.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "DnPlayerActor.h"
#include "DnPvPGameTask.h"
#include "PvPRespawnLogic.h"
#include "DnStateBlow.h"
#include "DNMissionSystem.h"
#include "DNDBConnection.h"

CPvPRoundMode::CPvPRoundMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPGameMode( pGameRoom, pPvPGameModeTable, pPacket ),m_bFinishRoundFlag(false)
{
	m_dwCheckAllDeadTick = 0;
	m_fFinishRoundDelta = 0.f;
#if defined(PRE_ADD_PVP_TOURNAMENT)
	m_cTournamentStep = pPacket->cMaxUser;
	m_cTournamentStepCount = 0 ;
	m_cCurrentTournamentStep = 0;
#endif
}

CPvPRoundMode::~CPvPRoundMode()
{
}

void CPvPRoundMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bFinishRoundFlag && !m_bFinishGameModeFlag )
	{
		bool bCheck = true;
		if( m_fFinishRoundDelta >= PvPCommon::Common::RoundModeFinishDelta )
			bCheck = false;

		m_fFinishRoundDelta -= fDelta;
		if( bCheck && m_fFinishRoundDelta <= 0.f )
			_OnStartRound();
		else
			return;
	}
	
	if( _CanProcess() == false )
		return;

	CPvPGameMode::Process( LocalTime, fDelta );
	_OnAfterProcess();
}

void CPvPRoundMode::_OnAfterProcess()
{
	if( m_bStartGameModeFlag && m_fStartDelta <= 0.f && !m_bFinishRoundFlag )
	{
		if( timeGetTime()-m_dwCheckAllDeadTick >= 500 )
		{
			OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllDead );
			m_dwCheckAllDeadTick = timeGetTime();
		}
	}
}

bool CPvPRoundMode::bIsPlayingUser( DnActorHandle hActor )
{
	if( !hActor || !hActor->GetName() || !hActor->IsPlayerActor() )
		return false;

	if( !CPvPGameMode::bIsPlayingUser( hActor ) )
		return false;

	// SessionState 검사
	CDnPlayerActor* pPlayer			= static_cast<CDnPlayerActor *>(hActor.GetPointer());
	CDNUserSession* pGameSession	= pPlayer->GetUserSession();
	if( !pGameSession )
		return false;
	if( pGameSession->GetState() != SESSION_STATE_GAME_PLAY )
		return false;
	
	return (m_mBreakInto.find( hActor->GetName() ) == m_mBreakInto.end()) ? true : false;
}

bool CPvPRoundMode::bIsFirstRound()
{
	UINT uiATeamScore, uiBTeamScore;
	m_pScoreSystem->GetGameModeScore( uiATeamScore, uiBTeamScore );

	if( uiATeamScore == 0 && uiBTeamScore == 0 )
		return true;

	return false;
}

// RoundMode에서는 부활을 시키지 않는다.
void CPvPRoundMode::OnFinishProcessDie( DnActorHandle hActor )
{

}

void CPvPRoundMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	UINT uiWinTeam = PvPCommon::Team::Max;

	switch( Reason )
	{
		case PvPCommon::FinishReason::OpponentTeamAllGone:
		{
			uiWinTeam = OnCheckZeroUserWinTeam();
			break;
		}
		case PvPCommon::FinishReason::TimeOver:
		{
			UINT uiATeamUser = 0;
			UINT uiBTeamUser = 0;

			for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
			{
				CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
				if( !pPartyStruct->pSession )
					continue;
				DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
				if( !hActor )
					continue;
				if( !bIsPlayingUser( hActor ) )
					continue;
				if( hActor->IsDie() )
					continue;
				
				if( hActor->GetTeam() == PvPCommon::Team::A )
					++uiATeamUser;
				else if( hActor->GetTeam() == PvPCommon::Team::B )
					++uiBTeamUser;
			}

			if( uiATeamUser > uiBTeamUser )
				uiWinTeam = PvPCommon::Team::A;
			else if( uiBTeamUser > uiATeamUser )
				uiWinTeam = PvPCommon::Team::B;
			break;
		}
		case PvPCommon::FinishReason::OpponentTeamAllDead:
		{
			UINT uiCheckBit = 0;
			for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
			{
				CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
				if( !pPartyStruct->pSession )
					continue;
				DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
				if( !hActor )
					continue;
				if( !bIsPlayingUser( hActor ) )
					continue;
				if( hActor->IsDie() )
					continue;
				if( hActor->GetTeam() == PvPCommon::Team::A )
					uiCheckBit |= 1;
				else if( hActor->GetTeam() == PvPCommon::Team::B )
					uiCheckBit |= 2;
				if( uiCheckBit == 3 )
					return;
			}

			if( uiCheckBit )
				uiWinTeam = (uiCheckBit==1) ? PvPCommon::Team::A : PvPCommon::Team::B;
			break;
		}
		case PvPCommon::FinishReason::LadderNoGame:
		{
			break;
		}
		default:
		{
			return;
		}
	}

	_ProcessFinishRound( uiWinTeam, Reason );
}

void CPvPRoundMode::_ProcessFinishRound( const UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason )
{
	// 이미 끝난라운드면 리턴
	if( m_bFinishRoundFlag )
		return;

	// Round종료 플래그 설정
	m_bFinishRoundFlag	= true;
	m_fFinishRoundDelta	= PvPCommon::Common::RoundModeFinishDelta;

	// ResapwnLogic 에 게임모드 종료 알림
	if( m_pGameRoom && m_pGameRoom->GetGameTask() && ((CDnPvPGameTask*)m_pGameRoom->GetGameTask())->GetRespawnLogic() )
		((CDnPvPGameTask*)m_pGameRoom->GetGameTask())->GetRespawnLogic()->FinishRound();

	//=============================================================================================
	// Score계산
	//=============================================================================================

	UINT uiATeamScore, uiBTeamScore;
	GetGameModeScore( uiATeamScore, uiBTeamScore );
	if( uiWinTeam == PvPCommon::Team::A )
		++uiATeamScore;
	else if( uiWinTeam == PvPCommon::Team::B )
		++uiBTeamScore;
	m_pScoreSystem->SetGameModeScore( uiATeamScore, uiBTeamScore );
	m_pScoreSystem->FinishRound( GetGameRoom(), uiWinTeam );

	//=============================================================================================
	// 라운드 결과 정리
	//=============================================================================================

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		// SESSION_STATE_GAME_PLAY 인 유저에게만 보내준다.
		if( pPartyStruct->pSession && pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
		{
			DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
			if( hActor && bIsPlayingUser( hActor ) )
			{
				bool bIsWin = false;
				if( uiWinTeam != PvPCommon::Team::Max )
					bIsWin = (uiWinTeam == hActor->GetTeam() ? true : false);
				
				_ProcessFinishRoundMode( pPartyStruct->pSession, bIsWin, uiWinTeam, Reason );
				m_pScoreSystem->OnFinishRound( hActor, bIsWin );
			}
		}		
	}	

	//=============================================================================================
	// 게임모드 종료 검사
	//=============================================================================================

	// 1.승리조건에 도달했는지 검사해본다.
	UINT uiMaxScore = max( uiATeamScore, uiBTeamScore );

	UINT uiCheckScore = uiMaxScore;
	if( bIsZombieMode() == true )
		uiCheckScore = uiATeamScore+uiBTeamScore;
	if( _CheckFinishGameMode( const_cast<UINT&>(uiWinTeam) ) || (_CheckWinContition()&& uiCheckScore >= m_uiWinCondition ) )
		return FinishGameMode( uiWinTeam, PvPCommon::FinishReason::AchieveWinCondition );
	// 2.상대방이 모두 나갔으면 무조건 남은팀이 이긴다.
	else if( Reason == PvPCommon::FinishReason::OpponentTeamAllGone || Reason == PvPCommon::FinishReason::OpponentCaptainGone )
	{
		bool bFinish = false;
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if (bIsZombieMode() == false && bIsTournamentMode() == false)
#else
		if (bIsZombieMode() == false)
#endif
			bFinish = true;
		else
		{
			//좀비모드라면 다음 라운드를 체크한다. 1명이 남기전까지는 진행!
			if (GetGameRoom()->GetUserCountWithoutGM()-GetGameRoom()->GetUserCount(PvPCommon::Team::Observer) <= 1)
				bFinish = true;
		}

		// 3.다음 라운드를 할 난입 유저가 있는지 검사.
		if (bFinish)
		{
			if( OnCheckZeroUserWinTeam( false ) != PvPCommon::Team::Max )
				return FinishGameMode( uiWinTeam, PvPCommon::FinishReason::OpponentTeamAllGone );
		}
	}
#if defined(PRE_ADD_DWC)
	if( GetGameRoom()->bIsLadderRoom() && !static_cast<CDNPvPGameRoom*>(GetGameRoom())->bIsDWCMatch() )
#else
	if( GetGameRoom()->bIsLadderRoom() )
#endif
	{
		if( Reason == PvPCommon::FinishReason::LadderNoGame )
			return FinishGameMode( uiWinTeam, PvPCommon::FinishReason::LadderNoGame );

		return FinishGameMode( uiWinTeam, PvPCommon::FinishReason::LadderDraw );
	}
	//=============================================================================================
	// 다음 라운드
	//=============================================================================================

	_FinishRound( uiWinTeam, Reason );
}

void CPvPRoundMode::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	CPvPGameMode::OnSuccessBreakInto( pGameSession );

	// 1.게임모드가 종료된 경우면 PvPRound모드라고 따로 해줄것이없다.
	if( m_bFinishGameModeFlag )
		return;

	// 2.PvPRound모드가 종료된 경우면 무적 상태효과 부여
	if( m_bFinishRoundFlag )
	{
		DnActorHandle hActor = pGameSession->GetActorHandle();
		if( hActor )
		{
			int iDurationTime = static_cast<int>(m_fFinishRoundDelta*1000);
			if( iDurationTime > 0 )
				hActor->CDnActor::AddStateBlow( STATE_BLOW::BLOW_099, NULL, iDurationTime, "-1" );
		}
	}
	// 3.PvPRound모드가 진행중이라면 이번라운드는 유령상태로 있는다.
	else if( GetRemainStartTick() == 0 )
	{
		// 4.첫라운드이고 일정시간(10sec) 이내의 난입이라면 플레이 가능하게 한다.
		if( bIsFirstRound() && bIsInPlayTime( 10 ) )
			return;

		// 4.가 아니라면 캐릭터 유령상태로~~
		DnActorHandle hActor = pGameSession->GetActorHandle();
		if( hActor )
		{
			m_mBreakInto.insert( std::make_pair( hActor->GetName(), pGameSession) );

			CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
			pPlayer->CmdRefreshHPSP( 0,0 );
		}
	}
}

void CPvPRoundMode::OnRebirth( DnActorHandle hActor, bool bForce/*=false*/ )
{
	if( m_bFinishRoundFlag )
		CPvPGameMode::OnRebirth( hActor, true );
}

void CPvPRoundMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	CPvPGameMode::OnDie( hActor, hHitter );

	OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllDead );
}

void CPvPRoundMode::OnLeaveUser( DnActorHandle hActor )
{
	CPvPGameMode::OnLeaveUser( hActor );

	if( hActor )
	{
		wchar_t* pwName = hActor->GetName();
		if( pwName )
			m_mBreakInto.erase( pwName );
	}
}

#if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )
void CPvPRoundMode::GetFinishRoundPenalty( const UINT uiWinTeam, OUT UINT& uiPenaltyPercent )
{
	uiPenaltyPercent = 0;

	UINT uiWinConditionScore = GetSelectWinCondition();
	UINT uiATeamScore = 0;
	UINT uiBTeamScore = 0;
	
	GetGameModeScore(uiATeamScore, uiBTeamScore);

	UINT uiWinTeamScore = 0;
	if( PvPCommon::Team::A == uiWinTeam )
		uiWinTeamScore = uiATeamScore;
	if( PvPCommon::Team::B == uiWinTeam )
		uiWinTeamScore = uiBTeamScore;

	UINT uiDefaultPercent = 100;
	if(0 < uiWinConditionScore)
	{
		uiDefaultPercent = uiWinTeamScore * 100 / uiWinConditionScore;
		uiDefaultPercent = min(uiDefaultPercent, 100);
	}

	uiPenaltyPercent = 100 - uiDefaultPercent;
}
#endif // #if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )

void CPvPRoundMode::_OnStartRound()
{
	m_bFinishRoundFlag		= false;
	m_bStartGameModeFlag	= false;

	m_mBreakInto.clear();

	m_pScoreSystem->OnStartRound();

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		if( pPartyStruct->pSession )
		{
			DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();

			// 1. 모든 상태효과 없애준다.
			if( hActor )
				hActor->OnInitializePVPRoundRestart();
			
			// 2. 새로운 라운드 시작을 알린다.
			pPartyStruct->pSession->SendPvPRoundStart();

			if( hActor && pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
			{
				DN_ASSERT( m_pGameRoom->bIsPvPRoom() == true, "CPvPRoundMode::_OnStartRound() m_pGameRoom->bIsPvPRoom() == true" );

				// 프로젝타일 없앤다
				CDnWeapon::ReleaseClass( GetGameRoom(), CDnWeapon::Projectile );

				// InstantItem 날린다.
				pPartyStruct->pSession->GetItem()->RemoveInstantItemData( true );
				
				CDNPvPGameRoom* pPvPGameRoom = static_cast<CDNPvPGameRoom*>(m_pGameRoom);
				pPvPGameRoom->CmdPvPStartAddStateEffect( hActor, _GetPvPRoundStartStateEffectDurationTick(), true );

				// 4.Respawn 포인트로 이동
				_OnStartRoundStartPosition( hActor );

				hActor->GetStateBlow()->Process( 0, 0.f, true );

				// 5.HP/SP 설정
				_OnRefreshHPSP( hActor );
			}
		}
	}
}

void CPvPRoundMode::_OnStartRoundStartPosition( DnActorHandle hActor )
{
	_SetRespawnPosition( hActor, true );
}

void CPvPRoundMode::_FinishRound( const UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason )
{	
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		if( pPartyStruct->pSession )
		{
			if( pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
			{
				// Observer 는 ActorHandle 이 없기 때문에 상위에서 처리한다. - 김밥 -
				// 1. PvPRoundMode 결과를 알려준다.
#if defined(PRE_ADD_PVP_TOURNAMENT)
				pPartyStruct->pSession->SendPvPRoundFinish( uiWinTeam, Reason, this, m_uiWinSessionID, m_cCurrentTournamentStep );
#else
				pPartyStruct->pSession->SendPvPRoundFinish( uiWinTeam, Reason, this );
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
				DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
				if( !hActor )
					continue;

				INT64 iHP = hActor->GetHP();

				// 2. 모든 상태효과 없애준다.
				// 클라이언트 SC_FINISH_PVPROUND 쪽 처리도 같이 바꿔준다.
				hActor->OnInitializePVPRoundRestart();

				INT64 iHP2 = hActor->GetHP();

				// 3. 죽지 않은 캐릭터에게만
				if( !hActor->IsDie() )
				{
					// 3-1. 다음라운드 대기시간만큼 무적Blow 걸어준다.
					int iDurationTime = static_cast<int>(m_fFinishRoundDelta*1000);
					hActor->CDnActor::AddStateBlow( STATE_BLOW::BLOW_099, NULL, iDurationTime, "-1" );
#if defined(PRE_ADD_PVP_TOURNAMENT)
					if( bIsAllKillMode() || bIsTournamentMode() )
#else
					if( bIsAllKillMode() )					
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
						static_cast<CDNPvPGameRoom*>(m_pGameRoom)->CmdPvPStartAddStateEffect( hActor, -1, true );					
					else
					{
						// 3-2. 옵져버 Blow 걸어준다.
						static_cast<CDNPvPGameRoom*>(m_pGameRoom)->CmdObserverAddStateEffect( hActor );
					}
				}
			}
		}
	}

	OnFinishRound();
}
