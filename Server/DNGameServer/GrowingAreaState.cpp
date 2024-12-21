
#include "stdafx.h"
#include "GrowingAreaState.h"
#include "GrowingArea.h"
#include "DnWorldProp.h"
#include "PlantSeed.h"
#include "DNFarmGameRoom.h"
#include "DNUserSession.h"


void IFarmAreaState::SendBeginAreaState()
{
	SCFarmBeginAreaState TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iAreaIndex = m_pArea->GetIndex();
	TxPacket.State		= GetState();

	if( m_pArea->bIsPrivateArea() )
	{
		CDNUserSession* pSession = m_pArea->GetUserSession( m_pArea->GetOwnerCharacterDBID() );
		if( pSession )
			pSession->AddSendData( SC_FARM, eFarm::SC_BEGIN_AREASTATE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
	}
	else
		m_pArea->GetFarmGameRoom()->BroadCast( SC_FARM, eFarm::SC_BEGIN_AREASTATE, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

