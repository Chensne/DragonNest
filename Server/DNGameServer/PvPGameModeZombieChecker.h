
#pragma once

#include "PvPGameModeChecker.h"

class CPvPGameModeZombieChecker:public IPvPGameModeChecker,public TBoostMemoryPool<CPvPGameModeZombieChecker>
{
public:

	CPvPGameModeZombieChecker( CPvPGameMode* pGameMode ):IPvPGameModeChecker(pGameMode),m_dwFinishTick(0)
	{
		m_listZombie.clear();
	}
	virtual ~CPvPGameModeZombieChecker(){}

	virtual void Check()
	{
		if( !m_pPvPGameMode )
			return;

		if( !(m_pPvPGameMode->GetGameModeCheck()&PvPCommon::Check::CheckTimeOver) )
			return;

		if( m_pPvPGameMode->CPvPGameMode::bIsFinishFlag() )
		{
			if( m_dwFinishTick == 0 )
				m_dwFinishTick = GetTickCount();

			// PvPCommon::Common::PvPFinishAutoClose + 5초
			if( GetTickCount()-m_dwFinishTick < ((PvPCommon::Common::PvPFinishAutoClose+5)*1000) )
				return;

			CDNGameRoom* pGameRoom = m_pPvPGameMode->GetGameRoom();

			// m_VecMember 에 들어오기 전 난입 유저 처리
			if( GetTickCount()-m_dwFinishTick >= ((PvPCommon::Common::PvPFinishAutoClose+15)*1000) )
			{
				const std::list<CDNUserSession*>& rList = pGameRoom->GetBreakIntoUserList();

				for( std::list<CDNUserSession*>::const_iterator itor=rList.begin() ; itor!=rList.end() ; ++ itor )
				{
					CDNUserSession* pSession = (*itor);
					if( pSession == NULL )
						continue;

					if( pGameRoom->GetUserSession( pSession->GetSessionID() ) == NULL )
					{
						g_Log.Log(LogType::_PVPROOM, pSession, L"PVP 난입유저 Session 정리\r\n" );
						pSession->DetachConnection(L"CPvPGameModeZombieChecker");
					}
				}
			}

			for( UINT i=0 ; i<pGameRoom->GetUserCount() ; ++i )
			{
				CDNGameRoom::PartyStruct* pPartyStruct = pGameRoom->GetPartyData(i);
				if( pPartyStruct->pSession )
				{
					if( std::find( m_listZombie.begin(), m_listZombie.end(), pPartyStruct->pSession->GetAccountDBID() ) == m_listZombie.end() )
					{
						pPartyStruct->pSession->SendPvPGameToPvPLobby();
						m_listZombie.push_back( pPartyStruct->pSession->GetAccountDBID() );
					}
					else
					{
						// PvPCommon::Common::PvPFinishAutoClose + 15초가 지나면 서버에서 강제로 끊어버린다.
						if( GetTickCount()-m_dwFinishTick >= ((PvPCommon::Common::PvPFinishAutoClose+15)*1000) )
						{
							pPartyStruct->pSession->DetachConnection(L"CPvPGameModeZombieChecker");
						}
					}
				}
			}
		}

	}

private:

	DWORD			m_dwFinishTick;
	std::list<UINT>	m_listZombie;
};
