#include "stdafx.h"
#include "DnOccupationTask.h"
#include "DnOccupationZone.h"
#include "DnTableDB.h"
#include "EtWorldEventArea.h"
#include "EtWorldEventControl.h"
#include "EtWorldSector.h"
#include "DnWorld.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"
#include "PvPSendPacket.h"
#include "DnMainMenuDlg.h"
#include "DnCharVehicleDlg.h"
#include "DnStateBlow.h"
#include "PvPOccupationScoreSystem.h"
#include "DnOccupationModeHUD.h"
#include "TaskManager.h"
#include "DnPvPGameTask.h"
#include "DnMutatorOccupationMode.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnOccupationTask::CDnOccupationTask()
: CTaskListener( false )
, m_bProgress( false )
, m_bChangeAction( false )
, m_bBreakInfoUser( false )
, m_nTrySound( -1 )
, m_nTryArea( -1 )
, m_nTableModeID( -1 )
, m_nRedTeamResource( 0 )
, m_nRedTeamTotalResource( )
, m_nBlueTeamResource( 0 )
, m_nBlueTeamTotalResource( 0 )
, m_pScoreSystem( NULL )
{
}

CDnOccupationTask::~CDnOccupationTask()
{
	for( std::vector< CDnOccupationZone *>::iterator itor = m_vFlag.begin(); itor != m_vFlag.end(); ++itor )
		SAFE_DELETE( (*itor) );

	m_vFlag.clear();
}

void CDnOccupationTask::Finalize()
{

}

bool CDnOccupationTask::Initialize( int nGameMode )
{
	m_nTableModeID = CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID;

	CEtWorldEventControl*	pControl	= NULL;
	CEtWorldSector*			pSector		= NULL;
	CEtWorldEventArea*		pArea		= NULL;

	CDnWorld * pWorld = CDnWorld::GetInstancePtr();

	for( DWORD i=0; i<pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
	{
		pSector	 = pWorld->GetGrid()->GetActiveSector(i);
		pControl = pSector->GetControlFromUniqueID( ETE_GuildWarFlag );
		if( !pControl ) 
			continue;

		for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
		{
			pArea = pControl->GetAreaFromIndex(j);

			if( !pArea ) continue;

			CDnOccupationZone * pOccupationZone = new CDnOccupationZone;
			if( pOccupationZone->Initialize( pArea, m_nTableModeID ) )
				m_vFlag.push_back( pOccupationZone );
			else
				SAFE_DELETE( pOccupationZone );
		}
	}

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );

	int nBattleGroundID = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "_BattleGroundID" )->GetInteger();;

	pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDMODE );

	m_BlueAniCmd[STRING_IDLE_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Idle_Blue" )->GetString() );
	m_BlueAniCmd[STRING_TRY_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Try_Blue" )->GetString() );
	m_BlueAniCmd[STRING_MOVEWAIT_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Standby_Blue" )->GetString() );
	m_BlueAniCmd[STRING_OWNED_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Conquered_Blue" )->GetString() );

	m_RedAniCmd[STRING_IDLE_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Idle_Red" )->GetString() );
	m_RedAniCmd[STRING_TRY_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Try_Red" )->GetString() );
	m_RedAniCmd[STRING_MOVEWAIT_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Standby_Red" )->GetString() );
	m_RedAniCmd[STRING_OWNED_CMD] = std::string( pSox->GetFieldFromLablePtr( nBattleGroundID, "_Conquered_Red" )->GetString() );

	return true;
}

void CDnOccupationTask::TryAcquirePoint( int nAreaID )
{
	CDnOccupationZone * pOccupation = GetFlag( nAreaID );

	if( !pOccupation || !pOccupation->m_pProp )
		return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
	if( !localActor ) return;

	localActor->CmdAction( "Stand" );
	localActor->ProcessFlushPacketQueue();
	localActor->CmdToggleBattle( false );
	localActor->CmdAction( "GuildWar_Switch" );

	WCHAR wszString[256];
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121122 ) );	// 점령중...

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDRESWAR );
	if( !pSox ) return;

	int nProgress = 0;
	for( int itr = 0 ; itr < pSox->GetItemCount(); ++itr )
	{
		int nItemID = pSox->GetItemID( itr );

		if( pOccupation->m_nAreaID == pSox->GetFieldFromLablePtr( nItemID, "_EventAreaID" )->GetInteger() 
			&& m_nTableModeID == pSox->GetFieldFromLablePtr( nItemID, "_PvPGamemodeId" )->GetInteger() )
		{
			nProgress = pSox->GetFieldFromLablePtr( nItemID, "_ClickKeepTime" )->GetInteger() / 1000;

			if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
			{
				CDnPvPGameTask * pGameTask = (CDnPvPGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( pGameTask )
				{
					CDnMutatorOccupationMode * pMutator = dynamic_cast<CDnMutatorOccupationMode *>( pGameTask->GetMutator() );

					if( pMutator && pMutator->IsClimaxMode() )
						nProgress = pSox->GetFieldFromLablePtr( nItemID, "_Cl_ClickKeepTime" )->GetInteger() / 1000;
				}
			}			
			m_nTrySound = pOccupation->m_nTrySound;
			break;
		}
	}

	GetInterface().OpenMovieProcessDlg( NULL, (float)nProgress, wszString, ACCEPT_REQUEST_DIALOG, this, false );

	CDnMouseCursor::GetInstance().ShowCursor( false );

	m_bProgress = true;
	m_nTryArea = nAreaID;
}

void CDnOccupationTask::OccupationPoint( SCPvPOccupationState * pData )
{
	if( !m_pScoreSystem )
		return;

	for( BYTE itr = 0; itr < pData->cCount; ++itr )
	{
		CDnOccupationZone * pOccupation = GetFlag( pData->Info[itr].nAreaID );

		if( !pOccupation )
		{
			ASSERT("Flag Area Not Found!!");
			continue;
		}

		if( PvPCommon::OccupationState::None == pData->Info[itr].nOccupationState )	//아무 소유도 아님
		{
			pOccupation->Clear();
			pOccupation->m_pProp->CmdAction( m_BlueAniCmd[STRING_IDLE_CMD].c_str() );

			if( m_bProgress && pData->Info[itr].nAreaID == m_nTryArea )
			{
				m_bProgress = false;
				GetInterface().CloseMovieProcessDlg();							
			}
		}
		else if( PvPCommon::OccupationState::Try & pData->Info[itr].nOccupationState )	//점령 시도중...
		{
			pOccupation->m_eState = pData->Info[itr].nOccupationState;
			pOccupation->m_nTryTeamID = pData->Info[itr].nTryTeamID;
			pOccupation->m_nTryUniqueID = pData->Info[itr].nTryUniqueID;

			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pData->Info[itr].nTryUniqueID );

			if( PvPCommon::Team::A == pOccupation->m_nTryTeamID )
			{
				if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) && !m_bBreakInfoUser )
				{
					GetInterface().ShowOccupationWarnning( hActor, pOccupation->m_nRedClickStringID, false, false, true, false );
					CEtSoundEngine::GetInstance().PlaySound( "2D", pOccupation->m_nTrySound, false );
				}
			}
			else if( PvPCommon::Team::B == pOccupation->m_nTryTeamID )
			{
				if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) && !m_bBreakInfoUser )
				{
					GetInterface().ShowOccupationWarnning( hActor, pOccupation->m_nBlueClickStringID, false, false, true, false );
					CEtSoundEngine::GetInstance().PlaySound( "2D", pOccupation->m_nTrySound, false );
				}
			}

			pOccupation->m_pProp->CmdAction( GetAnimationName( STRING_TRY_CMD, pOccupation->m_nTryTeamID ) );
		}
		else if( PvPCommon::OccupationState::MoveWait & pData->Info[itr].nOccupationState )	//소유 이동 대기
		{
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pData->Info[itr].nMoveWaitUniqueID );

			if( PvPCommon::Team::A == pData->Info[itr].nMoveWaitTeamID )
			{
				if( pOccupation->m_nMoveWaitTeamID != pData->Info[itr].nMoveWaitTeamID 
					&& !m_bBreakInfoUser 
					&& CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar )
					)
					GetInterface().ShowOccupationWarnning( hActor, pOccupation->m_nRedClickStringID, false, false, true, false );
			}
			else if( PvPCommon::Team::B == pData->Info[itr].nMoveWaitTeamID )
			{
				if( pOccupation->m_nMoveWaitTeamID != pData->Info[itr].nMoveWaitTeamID 
					&& !m_bBreakInfoUser 
					&& CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar )
					)
					GetInterface().ShowOccupationWarnning( hActor, pOccupation->m_nBlueClickStringID, false, false, true, false );
			}

			pOccupation->m_pProp->CmdAction( GetAnimationName( STRING_MOVEWAIT_CMD, pData->Info[itr].nMoveWaitTeamID ) );

			pOccupation->m_eState = pData->Info[itr].nOccupationState;
			pOccupation->m_nMoveWaitTeamID = pData->Info[itr].nMoveWaitTeamID;
			pOccupation->m_nMoveWaitUniqueID = pData->Info[itr].nMoveWaitUniqueID;

			if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
			{
				pOccupation->m_nOwnedTemID = 0;
				pOccupation->m_nOwnedUniqueID = 0;
			}
			CEtSoundEngine::GetInstance().PlaySound( "2D", pOccupation->m_nTrySound, false );
		}
		else if( PvPCommon::OccupationState::Own & pData->Info[itr].nOccupationState )	//소유중
		{
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pData->Info[itr].nOwnedUniqueID );

			if( PvPCommon::Team::A == pData->Info[itr].nOwnedTemID )
			{
				if( pOccupation->m_nOwnedTemID != pData->Info[itr].nOwnedTemID && !m_bBreakInfoUser )	//소유가 변경 되었을 경우
				{
					CDnPartyTask::PartyStruct * pStruct = GetPartyTask().GetPartyDataFromSessionID( pData->Info[itr].nOwnedUniqueID, true );

					if( !pStruct )
						return;

					bool bSnatched = pOccupation->m_nOwnedTemID != 0;
					m_pScoreSystem->OnAcquirePoint( pStruct->hActor->GetTeam(), pStruct->hActor->GetName(), bSnatched );
					GetInterface().ShowOccupationWarnning( hActor, pOccupation->m_nRedOwnStringID, false, false, true, false );					

					if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
					{
						CDnOccupationModeHUD * pHud = dynamic_cast<CDnOccupationModeHUD *>( GetInterface().GetHUD() );

#ifdef PRE_MOD_PVPOBSERVER
						pHud->ShowOccupationInfo( pStruct->hActor, 
							pOccupation->m_wszName,
							pOccupation->m_nBlueTeamStringID, 
							pOccupation->m_nRedTeamStringID,
							pOccupation->m_nObserverBlueTeamStringID,
							pOccupation->m_nObserverRedTeamStringID );
#else
						pHud->ShowOccupationInfo( pStruct->hActor, 
												pOccupation->m_wszName,
												pOccupation->m_nBlueTeamStringID, 
												pOccupation->m_nRedTeamStringID );
#endif // PRE_MOD_PVPOBSERVER

					}
				}
			}
			else if( PvPCommon::Team::B == pData->Info[itr].nOwnedTemID )
			{
				if( pOccupation->m_nOwnedTemID != pData->Info[itr].nOwnedTemID && !m_bBreakInfoUser )
				{
					CDnPartyTask::PartyStruct * pStruct = GetPartyTask().GetPartyDataFromSessionID( pData->Info[itr].nOwnedUniqueID, true );

					if( !pStruct )
					{
						ASSERT("파티원 정보가 없어서 소유를 못합니다.");
						return;
					}
					bool bSnatched = pOccupation->m_nOwnedTemID != 0;
					m_pScoreSystem->OnAcquirePoint( pStruct->hActor->GetTeam(), pStruct->hActor->GetName(), bSnatched );

					GetInterface().ShowOccupationWarnning( hActor, pOccupation->m_nBlueOwnStringID, false, false, true, false );

					if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
					{
						CDnOccupationModeHUD * pHud = dynamic_cast<CDnOccupationModeHUD *>( GetInterface().GetHUD() );

#ifdef PRE_MOD_PVPOBSERVER
						pHud->ShowOccupationInfo( pStruct->hActor, 
							pOccupation->m_wszName,
							pOccupation->m_nBlueTeamStringID, 
							pOccupation->m_nRedTeamStringID, 
							pOccupation->m_nObserverBlueTeamStringID,
							pOccupation->m_nObserverRedTeamStringID );
#else
						pHud->ShowOccupationInfo( pStruct->hActor, 
							pOccupation->m_wszName,
							pOccupation->m_nBlueTeamStringID, 
							pOccupation->m_nRedTeamStringID );
#endif // PRE_MOD_PVPOBSERVER
					}
				}
			}

			pOccupation->m_pProp->CmdAction( GetAnimationName( STRING_OWNED_CMD, pData->Info[itr].nOwnedTemID ) );

			if( m_bProgress && pData->Info[itr].nAreaID == m_nTryArea )
			{
				m_bProgress = false;
				GetInterface().CloseMovieProcessDlg();							
			}

			pOccupation->m_eState = pData->Info[itr].nOccupationState;
			pOccupation->m_nOwnedTemID = pData->Info[itr].nOwnedTemID;
			pOccupation->m_nOwnedUniqueID = pData->Info[itr].nOwnedUniqueID;
			pOccupation->m_nMoveWaitTeamID = 0;
			pOccupation->m_nMoveWaitUniqueID = 0;

			CEtSoundEngine::GetInstance().PlaySound( "2D", pOccupation->m_nOwnSound, false );
		}
	}

	if( m_bBreakInfoUser )
		m_bBreakInfoUser = false;
}
void CDnOccupationTask::SetResource( SCPvPOccupationTeamState * pData )
{
	if( PvPCommon::Team::A == pData->nTeam )
	{
		m_nBlueTeamResource = pData->nResource;
		m_nBlueTeamTotalResource = pData->nResourceSum;
	}
	else if( PvPCommon::Team::B == pData->nTeam )
	{
		m_nRedTeamResource = pData->nResource;
		m_nRedTeamTotalResource = pData->nResourceSum;
	}
}

int CDnOccupationTask::GetResource( PvPCommon::Team::eTeam eTeam )
{
	if( PvPCommon::Team::A == eTeam )
		return m_nBlueTeamResource;
	else if( PvPCommon::Team::B == eTeam )
		return m_nRedTeamResource;

	return 0;
}

int CDnOccupationTask::GetTotalResource( PvPCommon::Team::eTeam eTeam )
{
	if( PvPCommon::Team::A == eTeam )
		return m_nBlueTeamTotalResource;
	else if( PvPCommon::Team::B == eTeam )
		return m_nRedTeamTotalResource;

	return 0;
}
void CDnOccupationTask::FlagOperation( DnPropHandle hProp )
{
	CDnOccupationZone * pOccupation = GetFlag( hProp );

	if( pOccupation )
	{
		if( !CDnActor::s_hLocalActor ) return;
		CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
		if( !localActor ) return;

		bool bState = localActor->GetStateBlow()->IsExistStateBlowFromBlowIndex( STATE_BLOW::BLOW_099 );

		if( bState )
			return;

		if( localActor->IsVehicleMode() )
		{
			CDnCharStatusDlg * pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			CDnCharVehicleDlg * pCharVehicleDlg = pCharStatusDlg->GetVehicleDlg();

			if( pCharVehicleDlg )
				pCharVehicleDlg->ForceUnRideVehicle();
		}

		SendTryAcquirePoint( pOccupation->m_nAreaID );
	}
}

bool CDnOccupationTask::IsFlagOperation( DnPropHandle hProp )
{
	CDnOccupationZone * pOccupation = GetFlag( hProp );

	if( pOccupation && pOccupation->IsFlagOperation(hProp) )
		return true;

	return false;
}

CDnOccupationZone * CDnOccupationTask::GetFlag( int nAreaID )
{
	for( int itr = 0; itr < (int)m_vFlag.size(); ++itr )
	{
		if( m_vFlag[itr]->IsFlag( nAreaID ) )
			return  m_vFlag[itr];
	}

	return NULL;
}

CDnOccupationZone * CDnOccupationTask::GetFlag( DnPropHandle hProp )
{
	for( int itr = 0; itr < (int)m_vFlag.size(); ++itr )
	{
		if( m_vFlag[itr]->IsFlag( hProp ) )
			return  m_vFlag[itr];
	}

	return NULL;
}

void CDnOccupationTask::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	switch( nID ) 
	{
	case ACCEPT_REQUEST_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( strcmp( pControl->GetControlName(), "ID_PROCESS_COMPLETE") == 0 )
				{
					if( -1 != m_nTrySound )
					{
						CEtSoundEngine::GetInstance().PlaySound( "2D", m_nTrySound, false );
						m_nTrySound = -1;
					}
				}
				else if( strcmp( pControl->GetControlName(), "ID_BUTTON_CANCEL") == 0 )
				{
					SendTryAcquirePointCancle();
					if( m_bProgress )
					{
						m_bProgress = false;
						GetInterface().CloseMovieProcessDlg( true );
					}
				}

				if( !CDnActor::s_hLocalActor ) return;
				CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
				if( !localActor) return;

				if( localActor->IsCanBattleMode() )
					localActor->CmdToggleBattle( true );

				if( m_bChangeAction )
					localActor->CmdAction("Jump");

				m_bChangeAction = false;

				m_bProgress = false;
				m_nTryArea = -1;
			}
		}
	}
}

void CDnOccupationTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bProgress )
	{
		if( !CDnActor::s_hLocalActor ) return;
		CDnPlayerActor * pPlayerActor = (CDnPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
		if( !pPlayerActor ) return;

		m_bChangeAction = pPlayerActor->IsJump();

		if( pPlayerActor->IsMove() || pPlayerActor->IsHit() || pPlayerActor->IsBattleMode() || m_bChangeAction )
		{
			m_bProgress = false;
			GetInterface().CloseMovieProcessDlg();
		}
	}

	for( int itr = 0; itr < (int)m_vFlag.size(); ++itr )
		m_vFlag[itr]->Process( fDelta );
}

void CDnOccupationTask::SetScoreSystem( IScoreSystem * pScoreSystem )
{
	m_pScoreSystem = pScoreSystem;
}

const char * CDnOccupationTask::GetAnimationName( int eString, int eTeam )
{
	if( eString < 0 || eString >= STRING_MAX_CMD )
		return NULL;

	if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
	{
		if( PvPCommon::Team::A == eTeam )
			return m_BlueAniCmd[ eString ].c_str();
		else if( PvPCommon::Team::B == eTeam )
			return m_RedAniCmd[ eString ].c_str();
	}
	if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
	{
		if( !CDnActor::s_hLocalActor ) return NULL;
		CDnLocalPlayerActor * pLocalActor = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
		if( !pLocalActor ) return NULL;

		if( PvPCommon::Team::Observer == pLocalActor->GetTeam() )
		{
			if( PvPCommon::Team::A == eTeam )
				return m_BlueAniCmd[ eString ].c_str();
			else if( PvPCommon::Team::B == eTeam )
				return m_RedAniCmd[ eString ].c_str();
		}
		else
		{
			if( pLocalActor->GetTeam() == eTeam )
				return m_BlueAniCmd[ eString ].c_str();
			else
				return m_RedAniCmd[ eString ].c_str();
		}
	}
	return NULL;
}
