
#include "stdafx.h"
#include "PvPRespawnMode.h"
#include "DNGameRoom.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "PvPScoreSystem.h"

CPvPRespawnMode::CPvPRespawnMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPGameMode( pGameRoom, pPvPGameModeTable, pPacket )
{
}

CPvPRespawnMode::~CPvPRespawnMode()
{
}

// RespawnMode 는 1라운드만 진행되기 때문에 GameMode 종료
void CPvPRespawnMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	UINT uiWinTeam = PvPCommon::Team::Max;

	switch( Reason )
	{
		case PvPCommon::FinishReason::OpponentTeamAllGone:
		{
			uiWinTeam = OnCheckZeroUserWinTeam();
			if( uiWinTeam == PvPCommon::Team::Max )
				return;
			break;
		}
		case PvPCommon::FinishReason::AchieveWinCondition:
		case PvPCommon::FinishReason::TimeOver:
		{
			uiWinTeam = OnCheckFinishWinTeam();
			break;
		}
		default:
		{
			return;
		}
	}

	// 라운드 결과 정리
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		// SESSION_STATE_GAME_PLAY 인 유저에게만 보내준다.
		if( pPartyStruct->pSession && pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
		{
			DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
			if( hActor )
				m_pScoreSystem->OnFinishRound( hActor, (uiWinTeam == hActor->GetTeam() ? true : false) );
		}		
	}	

	FinishGameMode( uiWinTeam, Reason );
}

void CPvPRespawnMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	CPvPGameMode::OnDie( hActor, hHitter );

	if( !(GetGameModeCheck()&PvPCommon::Check::CheckScore) )
		return;

	UINT uiATeamScore,uiBTeamScore;
	GetGameModeScore( uiATeamScore, uiBTeamScore );

	UINT uiMaxScore = max( uiATeamScore, uiBTeamScore );

	// 스코어체크
	if( !bIsFinishFlag() && uiMaxScore >= GetSelectWinCondition() )
		OnCheckFinishRound( PvPCommon::FinishReason::AchieveWinCondition );
}

// ProcessDie() 종료
void CPvPRespawnMode::OnFinishProcessDie( DnActorHandle hActor )
{
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
