#include "stdafx.h"
#include "DnOccupationZone.h"
#include "DnTableDB.h"
#include "DnWorldOperationProp.h"
#include "DnLocalPlayerActor.h"
#include "EtWorldEventArea.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#define COUNT_SHOW_RATE 0.7f

CDnOccupationZone::CDnOccupationZone()
: m_pProp( NULL )
, m_eState( PvPCommon::OccupationState::None )
, m_nOwnedUniqueID( 0 )
, m_nOwnedTemID( 0 )
, m_nTryUniqueID( 0 )
, m_nTryTeamID( 0 )
, m_nMoveWaitUniqueID( 0 )
, m_nMoveWaitTeamID( 0 )
, m_nAreaID( -1 )
, m_nRedClickStringID( 0 )
, m_nBlueClickStringID( 0 )
, m_nRedOwnStringID( 0 )
, m_nBlueOwnStringID( 0 )
, m_nRedDefendStringID( 0 )
, m_nBlueDefendStringID( 0 )
, m_nRedTeamStringID( 0 )
, m_nBlueTeamStringID( 0 )
#ifdef PRE_MOD_PVPOBSERVER
, m_nObserverBlueTeamStringID(0)
, m_nObserverRedTeamStringID(0)
#endif // PRE_MOD_PVPOBSERVER
, m_nTrySound( 0 )
, m_nOwnSound( 0 )
, m_nTableModeID( -1 )
, m_nZoneIndex( 0 )
, m_fElapsedTime( 0.f )
{

}

CDnOccupationZone::~CDnOccupationZone()
{
	CEtSoundEngine::GetInstance().RemoveSound( m_nTrySound );
	CEtSoundEngine::GetInstance().RemoveSound( m_nOwnSound );
}

bool CDnOccupationZone::Initialize( CEtWorldEventArea * pArea, int nTableModeID )
{
	if( !pArea )
		return false;

	m_nTableModeID = nTableModeID;

	std::string					strString(pArea->GetName());
	std::vector<std::string>	vSplit;

	boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(" ") );

	_ASSERT( vSplit.size() == 3 );
	if( vSplit.size() < 3 )
		return false;

	int iFlagAreaIndex = boost::lexical_cast<int>( vSplit[2] );

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDRESWAR );

	for( int itr = 0; itr < pSox->GetItemCount(); ++itr )
	{
		int nItemID = pSox->GetItemID( itr );
		if( iFlagAreaIndex == pSox->GetFieldFromLablePtr( nItemID, "_EventAreaID" )->GetInteger() 
			&& m_nTableModeID == pSox->GetFieldFromLablePtr( nItemID, "_PvPGamemodeId" )->GetInteger() )
		{
			DnPropHandle hProp = CDnWorldProp::FindPropFromUniqueID( pSox->GetFieldFromLablePtr( nItemID, "_PropID" )->GetInteger() );

			if( !hProp )
			{
				OutputDebug("길드전 깃발 생성 오류.");
				return false;
			}

			m_pProp = (CDnWorldOperationProp *)(hProp.GetPointer());
			m_pProp->CmdAction( "Idle" );
			m_pProp->SetCrosshairType( CDnLocalPlayerActor::CrossHairType::FlagTarget );
			m_nRedClickStringID = pSox->GetFieldFromLablePtr( nItemID, "_DefenderClickUIstring" )->GetInteger();
			m_nBlueClickStringID = pSox->GetFieldFromLablePtr( nItemID, "_AttackerClickUIstring" )->GetInteger();
			m_nRedOwnStringID = pSox->GetFieldFromLablePtr( nItemID, "_DefendCaptureUIstring" )->GetInteger();
			m_nBlueOwnStringID = pSox->GetFieldFromLablePtr( nItemID, "_AttackCaptureUIstring" )->GetInteger();
			m_nRedDefendStringID = pSox->GetFieldFromLablePtr( nItemID, "_DefendSuccessUIstring" )->GetInteger();
			m_nBlueDefendStringID = pSox->GetFieldFromLablePtr( nItemID, "_AttackSuccessUIstring" )->GetInteger();
			m_nRedTeamStringID = pSox->GetFieldFromLablePtr( nItemID, "_EnemyCaptureMsg" )->GetInteger();
			m_nBlueTeamStringID = pSox->GetFieldFromLablePtr( nItemID, "_FriendCaptureMsg" )->GetInteger();

#ifdef PRE_MOD_PVPOBSERVER
			m_nObserverRedTeamStringID = pSox->GetFieldFromLablePtr( nItemID, "_ObserverRedTeamMsg" )->GetInteger();
			m_nObserverBlueTeamStringID = pSox->GetFieldFromLablePtr( nItemID, "_ObserverBlueTeamMsg" )->GetInteger();
#endif // #ifdef PRE_MOD_PVPOBSERVER

			m_nZoneIndex = pSox->GetFieldFromLablePtr( nItemID, "_BaseName" )->GetInteger();

			int nStringID = pSox->GetFieldFromLablePtr( nItemID, "_FlagToolTipUIstring" )->GetInteger();
			m_wszName = std::wstring( GetEtUIXML().GetUIString( CEtUIXML::idCategory1,nStringID) );

			m_nTrySound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( pSox->GetFieldFromLablePtr( nItemID, "_Sound_Try" )->GetString() ).c_str(), false, false );
			m_nOwnSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( pSox->GetFieldFromLablePtr( nItemID, "_Sound_Complete" )->GetString() ).c_str(), false, false );
			break;
		}
	}

	m_nAreaID = iFlagAreaIndex;
	m_OBB = *(pArea->GetOBB());

	return true;
}

void CDnOccupationZone::Process( float fElapsedTime )
{
	m_fElapsedTime += fElapsedTime;
}

void CDnOccupationZone::Clear()
{
	m_eState = PvPCommon::OccupationState::None;
	m_nOwnedUniqueID = 0;
	m_nOwnedTemID = 0;
	m_nTryUniqueID = 0;
	m_nTryTeamID = 0;
	m_nMoveWaitUniqueID = 0;
	m_nMoveWaitTeamID = 0;
}

bool CDnOccupationZone::IsFlag( DnPropHandle hProp )
{
	if( !m_pProp ) return false;

	if( m_pProp->GetMySmartPtr().GetHandle() == hProp->GetMySmartPtr().GetHandle() )
		return true;

	return false;
}

bool CDnOccupationZone::IsFlag( int nAreaID )
{
	if( nAreaID == m_nAreaID )
		return true;

	return false;
}

bool CDnOccupationZone::IsFlagOperation( DnPropHandle hProp )
{
	if( !m_pProp )
		return false;

	// 누군가 시도중일 경우는 무조건 점령 아이콘 뜨지 않는다.
	if( m_eState & PvPCommon::OccupationState::Try )
		return false;

	bool bState = false;

	if( m_eState == PvPCommon::OccupationState::None )	// 아무 소유도 아닐경우
		bState = true;
	else if( m_eState & PvPCommon::OccupationState::MoveWait && 
		!(m_eState & PvPCommon::OccupationState::Own) &&
		CDnActor::s_hLocalActor->GetTeam() != m_nMoveWaitTeamID )	//그냥 적군이 점령 대기 중일 때
		bState = true;
	else if( m_eState & PvPCommon::OccupationState::MoveWait && 
		m_eState & PvPCommon::OccupationState::Own && 
		CDnActor::s_hLocalActor->GetTeam() == m_nOwnedTemID &&
		CDnActor::s_hLocalActor->GetTeam() != m_nMoveWaitTeamID )	//현재 아군이 점령 중이었는데 적군이 점령 대기중일 경우
		bState = true;
	else if( m_eState & PvPCommon::OccupationState::Own && 
		!(m_eState & PvPCommon::OccupationState::MoveWait) &&
		CDnActor::s_hLocalActor->GetTeam() != m_nOwnedTemID )	//적군점령 상태일 경우
		bState = true;


	if( bState )
	{
		if( !CDnActor::s_hLocalActor ) return false;

		if( m_OBB.IsInside( *(CDnActor::s_hLocalActor->GetPosition() ) ) )
			return true;
	}

	return false;
}

EtVector3 CDnOccupationZone::GetPropPosition()
{
	return m_OBB.Center;
}

int CDnOccupationZone::GetImageIndex()
{
	if( !CDnActor::s_hLocalActor ) return 0;
	CDnLocalPlayerActor * pLocalPlayer = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor).GetPointer();
	if( !pLocalPlayer ) return 0;

	int nImageIndex = 0;

	if( ZONE_B == m_nZoneIndex )
		nImageIndex = 1;
	else if( ZONE_C == m_nZoneIndex )
		nImageIndex = 2;

	if( PvPCommon::OccupationState::Try & m_eState )
	{
		if( pLocalPlayer->GetTeam() == PvPCommon::Team::Observer )
		{
			if( PvPCommon::Team::A == m_nTryTeamID )
				nImageIndex += COUNT_INDEX + COUNT_INDEX;
			else if( PvPCommon::Team::B == m_nTryTeamID )
				nImageIndex += COUNT_INDEX;
		}
		else
		{
			if( pLocalPlayer->GetTeam() == m_nTryTeamID )
				nImageIndex += COUNT_INDEX + COUNT_INDEX;
			else if( pLocalPlayer->GetTeam() != m_nTryTeamID )
				nImageIndex += COUNT_INDEX;
		}
	}
	else if( PvPCommon::OccupationState::Own & m_eState )
	{
		if( pLocalPlayer->GetTeam() == PvPCommon::Team::Observer )
		{
			if( PvPCommon::Team::A == m_nOwnedTemID )
				nImageIndex += COUNT_INDEX + COUNT_INDEX;
			else if( PvPCommon::Team::B == m_nOwnedTemID )
				nImageIndex += COUNT_INDEX;
		}
		else
		{
			if( pLocalPlayer->GetTeam() == m_nOwnedTemID )
				nImageIndex += COUNT_INDEX + COUNT_INDEX;
			else if( pLocalPlayer->GetTeam() != m_nOwnedTemID )
				nImageIndex += COUNT_INDEX;
		}
	}

	return nImageIndex;
}

DWORD CDnOccupationZone::GetImageColor()
{
	if( PvPCommon::OccupationState::Try & m_eState )
	{
		if( m_fElapsedTime > COUNT_SHOW_RATE )
			m_fElapsedTime = 0.f;

		int nAlpha = (int)( m_fElapsedTime * (float)(255.0f / COUNT_SHOW_RATE) );
		nAlpha = min( nAlpha, 255 );

		return D3DCOLOR_ARGB( nAlpha, 255, 255, 255 );
	}

	return 0xffffffff;
}
