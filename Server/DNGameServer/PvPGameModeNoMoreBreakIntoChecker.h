
#pragma once

#include "PvPGameModeChecker.h"
#include "DNMasterConnectionManager.h"
#include "DNPvPGameRoom.h"

class CPvPGameModeNoMoreBreakIntoChecker:public IPvPGameModeChecker,public TBoostMemoryPool<CPvPGameModeNoMoreBreakIntoChecker>
{
public:

	CPvPGameModeNoMoreBreakIntoChecker( CPvPGameMode* pGameMode ):IPvPGameModeChecker(pGameMode),m_bSend(false){}
	virtual ~CPvPGameModeNoMoreBreakIntoChecker(){}

	virtual void Check()
	{
		if( m_bSend || !m_pPvPGameMode )
			return;

		const TPvPGameModeTable* pPvPGameModeTable = m_pPvPGameMode->GetPvPGameModeTable();
		if( !pPvPGameModeTable )
			return;

		switch( pPvPGameModeTable->uiGameMode )
		{
			case PvPCommon::GameMode::PvP_Respawn:
			case PvPCommon::GameMode::PvP_IndividualRespawn:
			{
				if( m_pPvPGameMode->GetTimeOver() <= 30.0f )
				{
					CDNGameRoom* pGameRoom = m_pPvPGameMode->GetGameRoom();
					if( g_pMasterConnectionManager && pGameRoom )
					{
						if( pGameRoom->bIsLadderRoom() == false )
							g_pMasterConnectionManager->SendPvPCommand( pGameRoom->m_iWorldID, PvPCommon::GAMA_Command::NoMoreBreakInto, pGameRoom->GetPvPIndex(), pGameRoom->GetRoomID() );
					}
					m_bSend = true;
				}
				break;
			}
		}		
	}

private:

	bool	m_bSend;
};
