
#include "Stdafx.h"
#include "GrowingAreaStateAddWater.h"
#include "DNUserSession.h"
#include "GrowingArea.h"


void CFarmAreaStateAddWater::BeginProcess()
{
	_ASSERT( m_biCharacterDBID > 0 );
	if( m_biCharacterDBID > 0 )
	{
		CDNUserSession* pSession = m_pArea->GetUserSession( m_biCharacterDBID );
		if( pSession )
		{
			pSession->SendFarmAddWater( ERROR_NONE, Farm::ActionType::START, m_pArea->GetIndex() );
			pSession->BroadcastingEffect( EffectType_Water, EffectState_Start );
		}
	}
}

void CFarmAreaStateAddWater::Process( float fDelta )
{
	if( m_bIsQuery )
		return;

	m_fElapsedDelta += fDelta;
	if( GetElapsedTick() >= Farm::Max::ADDWATERING_TICK+Farm::Max::EXTRA_SERVER_TICK )
	{
		m_bIsFinish = true;
		m_bIsCancel	= true;
	}
}

void CFarmAreaStateAddWater::EndProcess()
{
	Farm::ActionType::eType ActionType = m_bIsCancel ? Farm::ActionType::CANCEL : Farm::ActionType::COMPLETE;

	CDNUserSession* pSession = m_pArea->GetUserSession( m_biCharacterDBID );
	if( pSession )
	{
		pSession->SendFarmAddWater( ERROR_NONE, ActionType, m_pArea->GetIndex() );
		pSession->BroadcastingEffect( EffectType_Water, EffectState_Cancel );
	}
}

void CFarmAreaStateAddWater::SetUserInfo( CDNUserSession* pSession )
{
	m_biCharacterDBID = pSession->GetCharacterDBID();	
}

