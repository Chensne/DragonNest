
#include "Stdafx.h"
#include "GrowingAreaStatePlanting.h"
#include "GrowingArea.h"
#include "DNFarmGameRoom.h"
#include "DNUserSession.h"
#include "PlantSeed.h"
#include "DNDBConnection.h"
#include "DNGameDataManager.h"


void CFarmAreaStatePlanting::BeginProcess()
{
	INT64 biCharacterDBID = m_pArea->GetOwnerCharacterDBID();
	if( biCharacterDBID > 0 )
	{
		CDNUserSession* pSession = m_pArea->GetUserSession( biCharacterDBID );
		if( pSession )
		{
			pSession->BroadcastingEffect( EffectType_Plant, EffectState_Start );

			TFarmCultivateTableData* pData = g_pDataManager->GetFarmCultivateTableData( m_pArea->GetPlantSeed()->GetSeedItemID() );
			if( pData )
				m_bCheckOverlapCount = pData->iOverlap <= 0 ? true : false;
			else
				_ASSERT(0);

			if( m_bCheckOverlapCount == false )
			{
				pSession->GetDBConnection()->QueryGetFieldItemCount( pSession->GetDBThreadID(), pSession->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), m_pArea->GetPlantSeed()->GetSeedItemID(), pSession->GetRoomID(), m_pArea->GetIndex(), pSession->GetSessionID() );
			}
		}
	}

	SendBeginAreaState();
}

void CFarmAreaStatePlanting::Process( float fDelta )
{
	if( GetQueryFlag() )
		return;

	if( m_pArea->GetElapsedStateTick() >= Farm::Max::PLANTING_TICK+Farm::Max::EXTRA_SERVER_TICK )
		m_pArea->ChangeState( Farm::AreaState::NONE );
}

void CFarmAreaStatePlanting::EndProcess()
{
	CDNUserSession* pSession = NULL;

	INT64 biCharacterDBID = m_pArea->GetOwnerCharacterDBID();
	if( biCharacterDBID > 0 )
	{
		pSession = m_pArea->GetFarmGameRoom()->GetUserSessionByCharDBID( biCharacterDBID );
	}

	_ASSERT( m_pArea->GetPlantSeed() );

	if( pSession )
	{
		int iRet = ERROR_NONE;
		Farm::ActionType::eType	ActionType = Farm::ActionType::CANCEL;

		// GROWING 상태로 바뀐 경우
		if( m_pArea->GetState() == Farm::AreaState::GROWING )
		{
			// 쿼리 날리기
			pSession->GetDBConnection()->QueryGetFieldCountByCharacter( pSession->GetDBThreadID(), pSession->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), true );
			ActionType = Farm::ActionType::COMPLETE;
		}
		else
		{
			// DB 쿼리 날린 경우라면 COMPLETE 실패한 경우이다.
			if( GetQueryFlag() )
			{
				iRet		= (m_iLastError == 103284) ? ERROR_FARM_CANT_COMPLETE_PLANTSTATE_MAXFIELDCOUNT : ERROR_FARM_CANT_COMPLETE_PLANTSTATE;
				ActionType	= Farm::ActionType::COMPLETE;
			}
			else
			{
				iRet = m_iLastError ? m_iLastError : ERROR_NONE;
			}
		}

		CPlantSeed* pSeed = m_pArea->GetPlantSeed();
		pSession->SendFarmPlantSeed( iRet, ActionType, m_pArea->GetIndex(), pSeed->GetSeedItemID(), pSeed->GetFirstAttachItemID() );
		pSession->BroadcastingEffect( EffectType_Plant, EffectState_Cancel );
	}
}

bool CFarmAreaStatePlanting::CanCompletePlantSeed()
{
	// 시간 검사
	if( m_pArea->GetElapsedStateTick() < static_cast<DWORD>(Farm::Max::PLANTING_TICK*Farm::ElapsedSyncPer) )
		return false;

	// OverlapCount 쿼리가 안왔으면 캔슬시켜야한다.
	if( m_bCheckOverlapCount == false )
		return false;

	return true;
}

