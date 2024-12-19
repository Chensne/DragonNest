
#include "Stdafx.h"
#include "GrowingAreaStateNone.h"
#include "GrowingArea.h"
#include "DNFarmGameRoom.h"
#include "DNUserSession.h"


bool CFarmAreaStateNone::CanStartPlantSeed()
{
	if( m_pArea->GetPlantSeed() )
		return false;

	return true;	
}

void CFarmAreaStateNone::BeginProcess()
{
	m_pArea->DestroySeed();
	SendBeginAreaState();
}

void CFarmAreaStateNone::Process( float fDelta )
{

}

void CFarmAreaStateNone::EndProcess()
{
	if( m_pArea->GetState() == Farm::AreaState::PLANTING )
	{
		CPlantSeed* pSeed = m_pArea->GetPlantSeed();

		CDNUserSession* pSession = m_pArea->GetUserSession( m_pArea->GetOwnerCharacterDBID() );
		if( pSession )
			pSession->SendFarmPlantSeed( ERROR_NONE, Farm::ActionType::START, m_pArea->GetIndex(), pSeed->GetSeedItemID(), pSeed->GetFirstAttachItemID() );
	}
}

