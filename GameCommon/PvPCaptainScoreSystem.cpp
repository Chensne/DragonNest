
#include "Stdafx.h"
#include "PvPCaptainScoreSystem.h"
#include "DnActor.h"
#include "DnPlayerActor.h"

#if defined( _GAMESERVER )
#include "DNUserSession.h"
#include "PvPGameMode.h"
#include "DNGameRoom.h"
#include "PvPCaptainMode.h"
#endif // #if defined( _GAMESERVER )

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
void CPvPCaptainScoreSystem::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	bool bCaptainKill = false;

#if defined( _GAMESERVER )
	if( hActor && hActor->GetGameRoom() )
	{
		CDNGameRoom*	pGameRoom	 = hActor->GetGameRoom();
		CPvPGameMode*	pPvPGameMode = pGameRoom->GetPvPGameMode();

		if( pPvPGameMode )
		{
			_ASSERT( pPvPGameMode->bIsCaptainMode() );
			CPvPCaptainMode* pCaptainMode = static_cast<CPvPCaptainMode*>(pPvPGameMode);

			bCaptainKill = pCaptainMode->bIsCaptain( hActor );
		}
	}
#else 

	if( !hActor || !hActor->IsPlayerActor())
		return;

	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
	if(pPlayer && pPlayer->IsCaptainMode())
		bCaptainKill = true;

	// 클라이언트에서 대장 검사 여기서 할 것!!!

#endif // #if defined( _GAMESERVER )
	
	if( bCaptainKill )
	{
		_UpdateCaptainKillCount( hHitter );
	}

	CPvPScoreSystem::OnDie( hActor, hHitter );
}

void CPvPCaptainScoreSystem::_UpdateCaptainKillCount( DnActorHandle hActor, UINT uiCount/*=0*/ )
{
	if( !hActor )
		return;

	std::map<std::wstring,UINT>::iterator itor = m_mCaptainKillCount.find( hActor->GetName() );
	if( itor == m_mCaptainKillCount.end() )
	{
		m_mCaptainKillCount.insert( std::make_pair(hActor->GetName(),uiCount ? uiCount : 1) );
	}
	else
	{
		if( uiCount )
		{
			(*itor).second = uiCount;
		}
		else
		{
			++(*itor).second;
		}
	}
}

void CPvPCaptainScoreSystem::OnLeaveUser( DnActorHandle hActor )
{	
	if( hActor )
		m_mCaptainKillCount.erase( hActor->GetName() );

	CPvPScoreSystem::OnLeaveUser( hActor );
}

UINT CPvPCaptainScoreSystem::GetCaptainKillCount( DnActorHandle hActor )
{
	if( !hActor )	
		return 0;

	std::map<std::wstring,UINT>::iterator itor = m_mCaptainKillCount.find( hActor->GetName() );
	if( itor == m_mCaptainKillCount.end() )
		return 0;

	return (*itor).second;
}

#if defined( _GAMESERVER )

void CPvPCaptainScoreSystem::SendScore( CDNUserSession* pGameSession )
{
	CPvPScoreSystem::SendScore( pGameSession );

	// 대장전 모드에서는 추가로 대장킬 정보를 보내준다.
	_SendCaptainKillCount( pGameSession );
}

void CPvPCaptainScoreSystem::_SendCaptainKillCount( CDNUserSession* pSession )
{
	if( !pSession )
		return;

	CDNGameRoom* pGameRoom = pSession->GetGameRoom();
	if( !pGameRoom )
		return;

	if( m_mCaptainKillCount.size() > PvPCommon::Common::MaxPlayer )
		return;

	SCPVP_CAPTAINKILL_COUNT TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	char* pStartBuffer	= reinterpret_cast<char*>(TxPacket.sCaptainKillCount);
	char* pBuffer		= pStartBuffer;

	for( std::map<std::wstring,UINT>::iterator itor=m_mCaptainKillCount.begin() ; itor!=m_mCaptainKillCount.end() ; ++itor )
	{
		TPvPCaptainKillCount* pCount = reinterpret_cast<TPvPCaptainKillCount*>(pBuffer);

		CDNGameRoom::PartyStruct* pStruct =  pGameRoom->GetPartyData( const_cast<WCHAR*>((*itor).first.c_str()) );
		if( !pStruct || !pStruct->pSession )
			continue;
		
		pCount->uiSessionID			= pStruct->pSession->GetSessionID();
		pCount->uiCaptainKillCount	= (*itor).second;
		
		++TxPacket.cCount;
		pBuffer += sizeof(TPvPCaptainKillCount);
	}

	int iSize = static_cast<int>(sizeof(SCPVP_CAPTAINKILL_COUNT)-sizeof(TxPacket.sCaptainKillCount)+(pBuffer-pStartBuffer));

	pSession->AddSendData( SC_PVP, ePvP::SC_CAPTAINKILL_COUNT, reinterpret_cast<char*>(&TxPacket), iSize );
}

#else

bool CPvPCaptainScoreSystem::SetCaptainKillCount( DnActorHandle hActor, const void* pData )
{
	const TPvPCaptainKillCount* pCount = static_cast<const TPvPCaptainKillCount*>(pData);
	_UpdateCaptainKillCount( hActor, pCount->uiCaptainKillCount );
	return true;
}

#endif // #if defined( _GAMESERVER )
