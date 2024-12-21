
#include "Stdafx.h"
#include "PvPAllKillScoreSystem.h"
#if defined( _GAMESERVER )
#include "DNPvPGameRoom.h"
#include "PvPAllKillMode.h"
#include "DNUserSession.h"
#endif // #if defined( _GAMESERVER )

#if defined( _GAMESERVER )

void CPvPAllKillModeScoreSystem::_ClearPreWinActor()
{
	m_hPreWinActor = CDnActor::Identity();
	m_uiContinuousCount = 0;
}

void CPvPAllKillModeScoreSystem::OnFinishRound( CDNGameRoom* pGameRoom, UINT uiWinTeam )
{
	if( uiWinTeam == PvPCommon::Team::Max )
	{
		_ClearPreWinActor();
		return;
	}

	CDNPvPGameRoom* pPvPGameRoom = static_cast<CDNPvPGameRoom*>(pGameRoom);

	CPvPGameMode* pPvPGameMode = pPvPGameRoom->GetPvPGameMode();
	if( pPvPGameMode == NULL )
		return;
#if defined(PRE_ADD_PVP_TOURNAMENT)
	if( pPvPGameMode->bIsAllKillMode() == false && pPvPGameMode->bIsTournamentMode() == false)
#else
	if( pPvPGameMode->bIsAllKillMode() == false )
#endif
		return;
	CPvPAllKillMode* pPvPAllKillMode = static_cast<CPvPAllKillMode*>(pPvPGameMode);

	DnActorHandle hWinActor = pPvPAllKillMode->GetActiveActor( PvPCommon::Team2Index( static_cast<PvPCommon::Team::eTeam>(uiWinTeam)) );
	if( !hWinActor )
	{
		_ClearPreWinActor();
		return;
	}

	if( m_hPreWinActor && m_hPreWinActor == hWinActor )
	{
		++m_uiContinuousCount;
	}
	else
	{
		m_hPreWinActor = hWinActor;
		m_uiContinuousCount = 1;
	}

	_SendContinuousInfo( pGameRoom );
	_SendFinishDetailReason( pGameRoom, pPvPAllKillMode->GetFinishDetailReason() );
}

void CPvPAllKillModeScoreSystem::_SendContinuousInfo( CDNGameRoom* pGameRoom )
{
	SCPVP_ALLKILL_CONTINUOUSWIN TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.uiSessionID = m_hPreWinActor->GetUniqueID();
	TxPacket.uiCount = m_uiContinuousCount;

	for( UINT i=0 ; i<pGameRoom->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = pGameRoom->GetUserData(i);
		if( pSession == NULL )
			continue;

		pSession->AddSendData( SC_PVP, ePvP::SC_PVP_ALLKILL_CONTINUOUSWIN, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );

#if defined( _WORK )
		WCHAR wszBuf[MAX_PATH];
		wsprintf( wszBuf, L"[AllKill] %s %d½Â", m_hPreWinActor->GetName(), m_uiContinuousCount );
		pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
	}
}

void CPvPAllKillModeScoreSystem::_SendFinishDetailReason( CDNGameRoom* pGameRoom, PvPCommon::FinishDetailReason::eCode Reason )
{
	SCPVP_ALLKILL_FINISHDETAILREASON TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.Reason = Reason;

	for( UINT i=0 ; i<pGameRoom->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = pGameRoom->GetUserData(i);
		if( pSession == NULL )
			continue;

		pSession->AddSendData( SC_PVP, ePvP::SC_PVP_ALLKILL_FINISH_DETAILREASON, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );

#if defined( _WORK )
		WCHAR wszBuf[MAX_PATH];
		wsprintf( wszBuf, L"[AllKill] DetailReason:%d", Reason );
		pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
	}
}

#endif // #if defined( _GAMESERVER )
