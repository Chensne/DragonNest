
#include "stdafx.h"
#include "PvPCaptainMode.h"
#include "DnPlayerActor.h"
#include "DNUserSession.h"
#include "DNPvPGameRoom.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnStateBlow.h"
#include "DNMissionSystem.h"

CPvPCaptainMode::CPvPCaptainMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPRoundMode( pGameRoom, pPvPGameModeTable, pPacket )
{

}

CPvPCaptainMode::~CPvPCaptainMode()
{

}

void CPvPCaptainMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	_ProcessSelectCaptain();

	CPvPRoundMode::Process( LocalTime, fDelta );
	
	// Playing 중일 때 대장 나갔는지 검사
	if( bIsPlaying() )
		OnCheckFinishRound( PvPCommon::FinishReason::OpponentCaptainGone );
}

void CPvPCaptainMode::OnFinishProcessDie( DnActorHandle hActor )
{
	if( !bIsPlaying() )
		return;

	if( bIsCaptain( hActor ) )
		return;

	if( !bIsPlayingUser( hActor ) )
		return;

	if( hActor )
	{
		// 리스폰위치 설정
		_SetRespawnPosition( hActor );
		// 부활시킴
		UINT uiRespawnHPPercent = GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnHPPercent			: 100;
		UINT uiRespawnMPPercent = GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnMPPercent			: 100;
		UINT uiImmortalTime		= GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnNoDamageTimeSec	: 5;

		char szParam[32];
		sprintf_s( szParam, "Coin/%d/%d/%d", uiRespawnHPPercent, uiRespawnMPPercent, uiImmortalTime );
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_057, uiImmortalTime*1000, szParam );
	}	
}

void CPvPCaptainMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	UINT uiWinTeam = PvPCommon::Team::Max;

	switch( Reason )
	{
		case PvPCommon::FinishReason::TimeOver:
		{
			if( m_hCaptain[PvPCommon::TeamIndex::A] && m_hCaptain[PvPCommon::TeamIndex::B] )
			{
				if( m_hCaptain[PvPCommon::TeamIndex::A]->GetHPPercent() > m_hCaptain[PvPCommon::TeamIndex::B]->GetHPPercent() )
					uiWinTeam = PvPCommon::Team::A;
				else if( m_hCaptain[PvPCommon::TeamIndex::A]->GetHPPercent() < m_hCaptain[PvPCommon::TeamIndex::B]->GetHPPercent() )
					uiWinTeam = PvPCommon::Team::B;
			
				CPvPRoundMode::_ProcessFinishRound( uiWinTeam, Reason );
			}
			return;
		}
		case PvPCommon::FinishReason::OpponentCaptainGone:
		{
			if( m_hCaptain[PvPCommon::TeamIndex::A] && m_hCaptain[PvPCommon::TeamIndex::B] )
				return;
			else if( m_hCaptain[PvPCommon::TeamIndex::A] && !m_hCaptain[PvPCommon::TeamIndex::B] )
				uiWinTeam = PvPCommon::Team::A;
			else if( !m_hCaptain[PvPCommon::TeamIndex::A] && m_hCaptain[PvPCommon::TeamIndex::B] )
				uiWinTeam = PvPCommon::Team::B;

			CPvPRoundMode::_ProcessFinishRound( uiWinTeam, Reason );
			return;
		}
		case PvPCommon::FinishReason::OpponentCaptainDead:
		{
			if( m_hCaptain[PvPCommon::TeamIndex::A] && m_hCaptain[PvPCommon::TeamIndex::A]->IsDie() )
				uiWinTeam = PvPCommon::Team::B;
			else if( m_hCaptain[PvPCommon::TeamIndex::B] && m_hCaptain[PvPCommon::TeamIndex::B]->IsDie() )
				uiWinTeam = PvPCommon::Team::A;
			else
				return;

			// 속도조절
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance(GetGameRoom()).GetTask( "GameTask" );
			if( pGameTask ) 
				pGameTask->RequestChangeGameSpeed( 0.2f, 3000 );

			CPvPRoundMode::_ProcessFinishRound( uiWinTeam, Reason );
			return;
		}
	}

	CPvPRoundMode::OnCheckFinishRound( Reason );
}

void CPvPCaptainMode::OnFinishRound()
{
	for( UINT i=0 ; i<PvPCommon::TeamIndex::Max ; ++i )
		m_hCaptain[i] = CDnActor::Identity();
}

void CPvPCaptainMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	CPvPGameMode::OnDie( hActor, hHitter );

	if( bIsCaptain( hActor ) )
	{
		if( hHitter && hHitter->IsPlayerActor() )
		{
			CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hHitter.GetPointer());

			if( pPlayer->GetUserSession() )
				pPlayer->GetUserSession()->GetEventSystem()->OnEvent( EventSystem::OnPvPKillCaptain );
		}
		OnCheckFinishRound( PvPCommon::FinishReason::OpponentCaptainDead );
	}
}

void CPvPCaptainMode::OnLeaveUser( DnActorHandle hActor )
{
	CPvPRoundMode::OnLeaveUser( hActor );

	if( hActor )
		m_mSelectCaptainCount.erase( hActor->GetSessionID() );

	OnCheckFinishRound( PvPCommon::FinishReason::OpponentCaptainGone );
}

void CPvPCaptainMode::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	CPvPRoundMode::OnSuccessBreakInto( pGameSession );

	// 난입 유저에게 대장 정보 알려준다.
	if( m_hCaptain[PvPCommon::TeamIndex::A] )
		pGameSession->SendPvPSelectCaptain( m_hCaptain[PvPCommon::TeamIndex::A]->GetSessionID(), m_hCaptain[PvPCommon::TeamIndex::A]->GetName() );
	if( m_hCaptain[PvPCommon::TeamIndex::B] )
		pGameSession->SendPvPSelectCaptain( m_hCaptain[PvPCommon::TeamIndex::B]->GetSessionID(), m_hCaptain[PvPCommon::TeamIndex::B]->GetName() );
}

void CPvPCaptainMode::_ProcessSelectCaptain()
{
	// Starting 단계에서만 대장이 설정된다.
	if( !bIsStarting() )
		return;

	for( UINT i=0 ; i<PvPCommon::TeamIndex::Max ; ++i )
	{
		// 이미 대장이 설정되어있으면 설정할 필요가 없다.
		if( m_hCaptain[i] )
			continue;

		std::vector<DnActorHandle> vResult;
		_GetCaptainCandidate( static_cast<PvPCommon::Team::eTeam>(PvPCommon::Team::A+i), vResult );
		if( vResult.empty() )
			continue;

		m_hCaptain[i] = vResult[_rand(GetGameRoom())%vResult.size()];
		if( m_hCaptain[i] )
		{
			_AddCaptainBlow( m_hCaptain[i] );
			_UpdateSelectCaptainCount( m_hCaptain[i]->GetSessionID() );
			_ASSERT( GetGameRoom()->bIsPvPRoom() );
			static_cast<CDNPvPGameRoom*>(GetGameRoom())->SendSelectCaptain( m_hCaptain[i] );
		}
	}
}

void CPvPCaptainMode::_AddCaptainBlow( DnActorHandle hActor )
{
	// 대장에게 대장 전용 상태효과를 걸어준다.
	// 라운드가 끝나면 상태효과를 해제해주기 때문에 여기서는 시간은 무한으로 걸어준다.
	//hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_002, -1, "1.0", true );	// 2) 물리 공격력 비율
	hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_050, -1, "1.0", true );	// 50) 파이널 데미지 비율 변경.
	//hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_029, -1, "1.0", true );	// 29) 마법 공격 비율
	hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_058, -1, "0.5", true );	// 58) 최대 HP 비율
	//hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_059, -1, "0.5", true );	// 59) 최대 SP 비율

	hActor->GetStateBlow()->Process( 0, 0.f );
	hActor->CmdRefreshHPSP( hActor->GetMaxHP(), hActor->GetMaxSP() );
}

bool CPvPCaptainMode::bIsCaptain( DnActorHandle hActor )
{
	if( !hActor )
		return false;

	for( UINT i=0 ; i<PvPCommon::TeamIndex::Max ; ++i )
	{
		if( !m_hCaptain[i] )
			continue;

		if( m_hCaptain[i] == hActor )
			return true;
	}

	return false;
}

void CPvPCaptainMode::_UpdateSelectCaptainCount( UINT uiSessionID )
{
	std::map<UINT,UINT>::iterator itor = m_mSelectCaptainCount.find( uiSessionID );
	if( itor != m_mSelectCaptainCount.end() )
	{
		++(*itor).second;
	}
	else
	{
		m_mSelectCaptainCount.insert( std::make_pair(uiSessionID,1) );
	}
}

UINT CPvPCaptainMode::_GetSelectCaptainCount( UINT uiSessionID )
{
	std::map<UINT,UINT>::iterator itor = m_mSelectCaptainCount.find( uiSessionID );
	if( itor != m_mSelectCaptainCount.end() )
		return (*itor).second;

	return 0;
}

void CPvPCaptainMode::_GetCaptainCandidate( PvPCommon::Team::eTeam Team, std::vector<DnActorHandle>& vResult )
{
	UINT uiMinValue = UINT_MAX;

	std::vector<DnActorHandle> vCandidate;
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( !pSession || !pSession->GetActorHandle() )
			continue;
		if( pSession->GetTeam() != Team )
			continue;
		if( !bIsPlayingUser( pSession->GetActorHandle() ) )
			continue;
		UINT uiCount = _GetSelectCaptainCount( pSession->GetSessionID() );
		if(  uiCount < uiMinValue )	
			uiMinValue = uiCount;

		vCandidate.push_back( pSession->GetActorHandle() );
	}

	for( UINT i=0 ; i<vCandidate.size() ; ++i )
	{
		if( _GetSelectCaptainCount( vCandidate[i]->GetSessionID() ) == uiMinValue )
			vResult.push_back( vCandidate[i] );
	}
}
