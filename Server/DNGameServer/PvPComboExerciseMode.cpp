
#include "stdafx.h"
#include "PvPComboExerciseMode.h"
#include "DNGameRoom.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "PvPScoreSystem.h"
#include "DnGameTask.h"
#include "DNPvPGameRoom.h"
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
#include "DnPvPGameTask.h"
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

#if defined( PRE_ADD_PVP_COMBOEXERCISE )

CPvPComboExerciseMode::CPvPComboExerciseMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPGameMode( pGameRoom, pPvPGameModeTable, pPacket )
{
	m_nLastRecallMonsterTick = timeGetTime();
}

CPvPComboExerciseMode::~CPvPComboExerciseMode()
{
}

void CPvPComboExerciseMode::OnInitializeActor( CDNUserSession* pSession )
{
	DnActorHandle hActor = pSession->GetActorHandle();
	if( !hActor )
		return;

	int iTeam = PvPCommon::Team::A;
	hActor->CmdChangeTeam( iTeam );
	pSession->SetTeam( iTeam );

	CDNPvPGameRoom* pPvPGameRoom = static_cast<CDNPvPGameRoom*>(GetGameRoom());
	pSession->SendPvPComboExerciseRoomMasterInfo( pPvPGameRoom->GetRoomMasterSessionID() );
}

void CPvPComboExerciseMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	const UINT uiWinTeam = PvPCommon::Team::A;

	FinishGameMode( uiWinTeam, Reason );
}

int CPvPComboExerciseMode::OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	if ( NULL == pSession || NULL == pSession->GetActorHandle() )
		return ERROR_GENERIC_INVALIDREQUEST;

	switch (nSubCmd)
	{
		case ePvP::CS_PVP_COMBOEXERCISE_RECALLMONSTER:
		{
			if( false == GetGameRoom()->bIsPvPRoom() || GameTaskType::PvP != GetGameRoom()->GetGameTaskType() )
			{
				return ERROR_NONE;
			}

			if( 0 < m_fStartDelta )
			{
				return ERROR_NONE;
			}

			CDNPvPGameRoom* pPvPGameRoom = static_cast<CDNPvPGameRoom*>( GetGameRoom() );
			CDnPvPGameTask * pPvPGameTask = static_cast<CDnPvPGameTask*>( pPvPGameRoom->GetGameTask() );

			// 방장 체크
			if( pSession->GetCharacterDBID() != pPvPGameRoom->GetRoomMasterCharacterDBID() )
			{
				return ERROR_NONE;
			}

			// 액션 체크
			if( CDnActorState::ActorStateEnum::Stay != pSession->GetActorHandle()->GetState() )
			{
				return ERROR_NONE;
			}

			// 시간 체크
			const UINT nCurrentTick = timeGetTime();
			if( nCurrentTick < m_nLastRecallMonsterTick + 1000 ) // 몬스터 소환 쿨타임은 1초
			{
				return ERROR_NONE;
			}
			
			m_nLastRecallMonsterTick = timeGetTime();

			EtVector3 vPos = *pSession->GetActorHandle()->GetPosition();
			EtVector2 vLook = EtVec3toVec2( *pSession->GetActorHandle()->GetLookDir() );
				
			pPvPGameTask->RequestRecallAllMonster( vPos, vLook );

			pSession->SendPvPComboExerciseRecallMonsterResult( ERROR_NONE );
		}
		break;
	}

	return ERROR_NONE;
}

#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
