#include "stdafx.h"
#include "PvPRacingMode.h"
#include "DNGameRoom.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "PvPScoreSystem.h"
#include "DnPlayerActor.h"

#if defined( PRE_ADD_RACING_MODE )

CPvPRacingMode::CPvPRacingMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPGameMode( pGameRoom, pPvPGameModeTable, pPacket )
{
	m_bStartRacing  = false;
	m_bEndRacing = false;
	m_mStartTick.clear();
	m_mEndTick.clear();	
	memset(m_nRacingRanking, 0, sizeof(m_nRacingRanking));
	m_bFirstEnd = false;
	m_nRacingIndex = 0;	
}

CPvPRacingMode::~CPvPRacingMode()
{
}

void CPvPRacingMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CPvPGameMode::Process( LocalTime, fDelta );	
}

void CPvPRacingMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{	
	UINT uiWinTeam = PvPCommon::Team::Max;	
	m_bEndRacing = true;
	
	FinishGameMode( uiWinTeam, Reason );
	//랩타임 보내기.
	SendRacingRapTime();
}

void CPvPRacingMode::SetRacingStart()
{
	DWORD NowTick = timeGetTime();
	CPvPScoreSystem* pScoreSystem = static_cast<CPvPScoreSystem*>(GetScoreSystem());
	for( DWORD i=0; i<GetGameRoom()->GetUserCount(); ++i)
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( !pSession )
			continue;
		DnActorHandle hActor = pSession->GetActorHandle();
		if( !hActor )
			continue;
		if( bIsPlayingUser( hActor ) == false )
			continue;
		if( i>= PvPCommon::Common::MaxPlayer )
			continue;

		m_mStartTick[pSession->GetSessionID()] = NowTick;
		if( pScoreSystem )
			pScoreSystem->FindMyScoreDataIfnotInsert(hActor);
	}
	m_bStartRacing = true;
}

void CPvPRacingMode::SetRacingEnd(UINT nSessionID)
{
	CDNUserSession* pSession = GetGameRoom()->GetUserSession(nSessionID);
	if( !pSession )
		return;

	DnActorHandle hActor = pSession->GetActorHandle();
	if( !hActor )
		return;
	if( bIsPlayingUser( hActor ) == false )
		return;

	DWORD NowTick = timeGetTime();
	if( !m_bFirstEnd )
	{
		m_bFirstEnd = true;
		SendRacingFirstEnd(pSession->GetSessionID());
	}
	m_mEndTick[pSession->GetSessionID()] = NowTick;
	m_nRacingRanking[m_nRacingIndex++] = pSession->GetSessionID();

	if( CheckRacingEnd(CDnActor::Identity()) )
	{
		OnCheckFinishRound( PvPCommon::FinishReason::RacingAllFinish );
		m_bEndRacing = true;
	}
}

BYTE CPvPRacingMode::GetRacingRanking(UINT uiSessionID)
{
	for( int i=0; i<PvPCommon::Common::MaxPlayer; ++i)
	{
		if( m_nRacingRanking[i] == 0)
			break;

		if( m_nRacingRanking[i] == uiSessionID )
			return i+1;		
	}
	return 0;
}

void CPvPRacingMode::OnLeaveUser( DnActorHandle hActor )
{
	if( !m_bEndRacing && CheckRacingEnd(hActor) )
	{
		OnCheckFinishRound( PvPCommon::FinishReason::RacingAllFinish );
		m_bEndRacing = true;
	}
}

bool CPvPRacingMode::CheckRacingEnd( DnActorHandle hActor )
{
	for( DWORD i=0; i<GetGameRoom()->GetUserCount(); ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( !pSession )
			continue;
		DnActorHandle hTempActor = pSession->GetActorHandle();
		if( !hTempActor )
			continue;
		if( hTempActor == hActor)
			continue;
		if( bIsPlayingUser( hTempActor ) == false )
			continue;

		std::map<UINT,DWORD>::iterator itor = m_mEndTick.find( pSession->GetSessionID() );
		if( itor == m_mEndTick.end() )
			return false;
	}
	return true;
}

void CPvPRacingMode::SendRacingRapTime()
{
	SCPVP_RACING_RAPTIME sRacingRapTime = {0,};
	for( int i=0; i<PvPCommon::Common::MaxPlayer; ++i)
	{
		if( m_nRacingRanking[i] == 0)
			break;;

		CDNUserSession* pSession = GetGameRoom()->GetUserSession(m_nRacingRanking[i]);		
		if( pSession )
		{
			sRacingRapTime.sRapTimeArr[sRacingRapTime.cCount].uiSessionID = pSession->GetSessionID();
			sRacingRapTime.sRapTimeArr[sRacingRapTime.cCount].dwLapTime = m_mEndTick[pSession->GetSessionID()] - m_mStartTick[pSession->GetSessionID()];
			++sRacingRapTime.cCount;
		}
	}
	int iSize = static_cast<int>(sizeof(SCPVP_RACING_RAPTIME)-sizeof(sRacingRapTime.sRapTimeArr)+sRacingRapTime.cCount*sizeof(TPvPUserRapTime));
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		// SESSION_STATE_GAME_PLAY 인 유저에게만 보내준다.
		if( pSession && pSession->GetState() == SESSION_STATE_GAME_PLAY )
		{
			pSession->AddSendData( SC_PVP, ePvP::SC_PVP_RACING_RAPTIME, reinterpret_cast<char*>(&sRacingRapTime), iSize );
			pSession->GetItem()->RemoveInstantEquipVehicleData(true);
			pSession->GetItem()->RemoveInstantVehicleItemData(true);				
		}		
	}	
}
void CPvPRacingMode::SendRacingFirstEnd(UINT uiSessionID)
{
	SCPVP_RACING_FIRST_END sRacingFirstEnd = {0,};
	sRacingFirstEnd.uiSessionID = uiSessionID;
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		// SESSION_STATE_GAME_PLAY 인 유저에게만 보내준다.
		if( pSession && pSession->GetState() == SESSION_STATE_GAME_PLAY )
		{
			pSession->AddSendData( SC_PVP, ePvP::SC_PVP_RACING_FIRST_END, reinterpret_cast<char*>(&sRacingFirstEnd), sizeof(sRacingFirstEnd) );
		}
	}
}

#endif //#if defined( PRE_ADD_RACING_MODE )

