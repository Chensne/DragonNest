
#include "Stdafx.h"
#include "PvPIndividualRespawnMode.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "DNGameRoom.h"

CPvPIndividualRespawnMode::CPvPIndividualRespawnMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPRespawnMode( pGameRoom, pPvPGameModeTable, pPacket )
{
	for( UINT i=0 ; i<PvPCommon::Common::MaxPVPPlayer ; ++i )
	{
		m_UsableTeamIndex.push_back( PvPCommon::Team::StartIndex+i );
	}

	m_lDieCount = 0;
}

CPvPIndividualRespawnMode::~CPvPIndividualRespawnMode()
{
}

void CPvPIndividualRespawnMode::OnInitializeActor( CDNUserSession* pSession )
{
	DnActorHandle hActor = pSession->GetActorHandle();
	if( !hActor )
		return;

	_ASSERT( m_UsableTeamIndex.empty() == false );

	// 1.팀설정
	int iTeam = m_UsableTeamIndex.front();
	m_UsableTeamIndex.pop_front();
	hActor->CmdChangeTeam( iTeam );
	pSession->SetTeam( iTeam );

	// 2.MaxDieDelta 값 설정
	hActor->SetMaxDieDelta( 10.f );
}

UINT CPvPIndividualRespawnMode::OnCheckZeroUserWinTeam( const bool bCheckBreakIntoUser/*=true*/ )
{
	UINT uiWinTeam = PvPCommon::Team::Max;

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);

		if( pPartyStruct->pSession )
		{
			// 플레이중인 유저 검사
			if( bCheckBreakIntoUser )
			{
				if( !bIsPlayingUser( pPartyStruct->pSession->GetActorHandle() ) )
					continue;
			}
			else
			{
				if( pPartyStruct->pSession->GetState() != SESSION_STATE_GAME_PLAY )
					continue;
			}

			if( uiWinTeam != PvPCommon::Team::Max )
				return PvPCommon::Team::Max;

			DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
			if( hActor )
				uiWinTeam = hActor->GetTeam();
		}
	}

	return uiWinTeam;
}

UINT CPvPIndividualRespawnMode::OnCheckFinishWinTeam()
{
	UINT uiWinTeam			= PvPCommon::Team::Max;
	UINT uiMaxKillCount		= 0;
	UINT uiMaxKillUserCount	= 0;

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);

		if( pPartyStruct->pSession )
		{
			DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
			if( !hActor )
				continue;

			if( pPartyStruct->pSession->GetState() != SESSION_STATE_GAME_PLAY )
				continue;

			UINT uiKillCount = m_pScoreSystem->GetKillCount( pPartyStruct->pSession->GetActorHandle() );
			if( uiKillCount > uiMaxKillCount )
			{
				uiWinTeam			= hActor->GetTeam();
				uiMaxKillUserCount	= 1;
				uiMaxKillCount		= uiKillCount;
			}
			else if( uiKillCount == uiMaxKillCount )
			{
				++uiMaxKillUserCount;
			}
		}
	}

	if( uiMaxKillUserCount > 1 )
		return PvPCommon::Team::Max;

	return uiWinTeam;
}

void CPvPIndividualRespawnMode::OnLeaveUser( DnActorHandle hActor )
{
	// 1. 팀값 반환
	if( hActor )
	{
		int iTeam = hActor->GetTeam();
		m_UsableTeamIndex.push_back( iTeam );
	}

	CPvPGameMode::OnLeaveUser( hActor );
}

void CPvPIndividualRespawnMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	CPvPGameMode::OnDie( hActor, hHitter );

	if( !(GetGameModeCheck()&PvPCommon::Check::CheckScore) )
		return;

	UINT uiKillCount = m_pScoreSystem->GetKillCount( hHitter );
	// 스코어체크
	if( !bIsFinishFlag() && uiKillCount >= GetSelectWinCondition() )
		OnCheckFinishRound( PvPCommon::FinishReason::AchieveWinCondition );	
}

void CPvPIndividualRespawnMode::PushUsedRespawnArea( const std::string& name )
{
	m_UsedRespawnAreaList.push_back( name );
}

bool CPvPIndividualRespawnMode::bIsUsedRespawnArea( const std::string& name )
{
	std::list<std::string>::iterator itor = std::find( m_UsedRespawnAreaList.begin(), m_UsedRespawnAreaList.end(), name );
	if( itor == m_UsedRespawnAreaList.end() )
		return false;
	return true;
}
