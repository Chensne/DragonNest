
#include "Stdafx.h"
#include "GrowingAreaStateHarvested.h"
#include "DNUserSession.h"
#include "GrowingArea.h"
#include "DNGameDataManager.h"
#include "DNMasterConnectionManager.h"
#include "DNFarmGameRoom.h"


void CFarmAreaStateHarvested::BeginProcess()
{
	if( m_pArea->GetPlantSeed() )
	{
		// 수확 되었기 때문에 DB 에서 제거됨.
		m_pArea->GetPlantSeed()->SetDBCreateFlag( false );

		// 수확창고 아이템 개수 동기화 작업
		g_pMasterConnectionManager->SendFarmSync( m_pArea->GetFarmGameRoom()->GetWorldSetID(), m_pArea->GetPlantSeed()->GetOwnerCharacterDBID(), Farm::ServerSyncType::WAREHOUSE_ITEMCOUNT );
	}

	SendBeginAreaState();
}

void CFarmAreaStateHarvested::Process( float fDelta )
{
	m_pArea->ChangeState( Farm::AreaState::NONE );
}

void CFarmAreaStateHarvested::EndProcess()
{
	
}

// HarvestedInfo
void CFarmAreaStateHarvested::SetHarvestedInfo( CDNUserSession* pSession )
{
	if( pSession )
		m_biHarvestedCharacterDBID = pSession->GetCharacterDBID();
}

