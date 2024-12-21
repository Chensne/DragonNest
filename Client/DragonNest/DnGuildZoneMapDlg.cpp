#include "stdafx.h"
#include "DnGuildZoneMapDlg.h"
#include "DnWorldDataManager.h"
#include "DnWorldData.h"
#include "DnLocalPlayerActor.h"
#include "DnPartyTask.h"
#include "TaskManager.h"
#include "DnCommonUtil.h"
#include "DnMutatorGuildWar.h"
#include "DnStateBlow.h"
#include "DnTableDB.h"
#include "PvPOccupationScoreSystem.h"
#include "DnMonsterActor.h"
#include "EtWorld.h"
#include "EtWorldEventControl.h"
#include "EtWorldSector.h"
#include "DnGuildWarSituationMng.h"
#include "PvPSendPacket.h"
#include "DnOccupationTask.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnGuildWarZoneMapDlg::CDnGuildWarZoneMapDlg ( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_bEnemyShow( false )
, m_pTextureMinimap( NULL )
, m_pButtonParty( NULL )
, m_pButtonPartyMember( NULL )
, m_pButtonEnemy( NULL )
, m_pButtonMousePoint( NULL )
, m_pStaticPlayer( NULL )
, m_pStaticBlueDoor( NULL )
, m_pStaticBlueBrokenDoor( NULL )
, m_pStaticRedDoor( NULL )
, m_pStaticRedBrokenDoor( NULL )
, m_pStaticBlueBoss( NULL )
, m_pStaticRedBoss( NULL )
, m_pStaticAlarm( NULL )
, m_pStaticBlueText( NULL )
, m_pStaticRedText( NULL )
{
	m_bAutoCursor = false;
	m_fMousePointTime = 0.f;

	SecureZeroMemory( m_pButtonFlag, sizeof(m_pButtonFlag) );
}

CDnGuildWarZoneMapDlg::~CDnGuildWarZoneMapDlg()
{
	SAFE_RELEASE_SPTR( m_hMinimap );
}

void CDnGuildWarZoneMapDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildZoneMapDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarZoneMapDlg::InitialUpdate()
{
	m_pButtonFlag[NONE_FLAG] = GetControl<CEtUIButton>("ID_BUTTON_PVP_FLAG0");
	m_pButtonFlag[BLUE_PROCESS_FLAG] = GetControl<CEtUIButton>("ID_BUTTON_PVP_FLAG2");
	m_pButtonFlag[BLUE_SUCCESS_FLAG] = GetControl<CEtUIButton>("ID_BUTTON_PVP_FLAG1");
	m_pButtonFlag[RED_PROCESS_FLAG] = GetControl<CEtUIButton>("ID_BUTTON_PVP_FLAG4");
	m_pButtonFlag[RED_SUCCESS_FLAG] = GetControl<CEtUIButton>("ID_BUTTON_PVP_FLAG3");

	m_pButtonParty = GetControl<CEtUIButton>("ID_BUTTON_PARTY");
	m_pButtonPartyMember = GetControl<CEtUIButton>("ID_BUTTON_MEMBER");
	m_pButtonEnemy = GetControl<CEtUIButton>("ID_BUTTON_ENEMY");

	m_pButtonMousePoint = GetControl<CEtUIButton>("ID_BUTTON_MOUSEPOINT");
	m_pButtonMousePoint->Show( false );
	
	m_pStaticPlayer = GetControl<CEtUIStatic>("ID_STATIC_PLAYER");
	m_pStaticPlayer->Show( true );

	m_pStaticBlueDoor = GetControl<CEtUIStatic>("ID_STATIC_BLUEPROP");
	m_pStaticBlueBrokenDoor = GetControl<CEtUIStatic>("ID_STATIC_BLUEBROKEN");
	m_pStaticRedDoor = GetControl<CEtUIStatic>("ID_STATIC_REDPROP");
	m_pStaticRedBrokenDoor = GetControl<CEtUIStatic>("ID_STATIC_REDBROKEN");

	m_pStaticBlueBoss = GetControl<CEtUIStatic>("ID_STATIC_BLUEBOSS");
	m_pStaticRedBoss = GetControl<CEtUIStatic>("ID_STATIC_REDBOSS");

	m_pStaticAlarm = GetControl<CEtUIStatic>("ID_STATIC_ALARM");

	m_pStaticBlueText = GetControl<CEtUIStatic>("ID_TEXT_BLUECOUNT");
	m_pStaticRedText = GetControl<CEtUIStatic>("ID_TEXT_REDCOUNT");

	m_pTextureMinimap = GetControl<CEtUITextureControl>("ID_TEXTUREL_MAP");
}

void CDnGuildWarZoneMapDlg::PostInitialize()
{
	SetMap();
	CreateFlagControl();
	CreateDoorBossControl();

	MoveToTail( m_pStaticPlayer );
}

void CDnGuildWarZoneMapDlg::Process( float fElapsedTime )
{
	if( !m_bShow )	
		return;

	ProcessUser();
	ProcessFlag();
	ProcessBuff();
	ProcessDoor();
	ProcessBoss();

	ProcessPosition( fElapsedTime );
	ProcessAlarm( fElapsedTime );
	ProcessMousePointer( fElapsedTime );

	CDnCustomDlg::Process( fElapsedTime );
}

void CDnGuildWarZoneMapDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render(fElapsedTime);
}

void CDnGuildWarZoneMapDlg::Show( bool bShow )
{
	if( bShow == m_bShow )
		return;

	CEtUIDialog::Show( bShow );
}

bool CDnGuildWarZoneMapDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show( false );
			return true;
		}
	}

	if( uMsg == WM_RBUTTONDOWN )
	{
		if( IsMouseInDlg() && CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
		{
			CDnPartyTask::PartyStruct * pData = GetPartyTask().GetPartyDataFromSessionID( CDnBridgeTask::GetInstance().GetSessionID() );

			if( !pData ) return false;

			if( PvPCommon::UserState::GuildWarCaptain & pData->uiPvPUserState 
				|| PvPCommon::UserState::GuildWarSedcondCaptain & pData->uiPvPUserState )
			{
				POINT MousePoint;
				EtVector3 vPos;

				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, vPos.x, vPos.y );
				SUICoord sUICoord = m_pButtonMousePoint->GetUICoord();

				vPos.x -= (sUICoord.fWidth / 2.f);
				vPos.y -= sUICoord.fHeight;

				SendGuildWarConcentrateOrder( vPos );
			}
		}
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnGuildWarZoneMapDlg::ProcessUser()
{
	CDnGuildWarSituationMng * pMng = GetInterface().GetGuildWarSituation();
	if( !pMng )
		return;

	for( std::list<SZonePartyButtonInfo>::iterator iter = m_listButtonParty.begin(); 
		iter != m_listButtonParty.end(); ++iter )
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( iter->nSessionID );

		if( hActor )
		{
			bool bPartyMember = pMng->IsPartyMember( hActor );

			if( bPartyMember != iter->bPartyMember )
			{
				CEtUIButton *pButton(NULL);
				SUIControlProperty sUIProperty;

				if( bPartyMember )
				{
					m_pButtonPartyMember->GetProperty( sUIProperty );
					iter->bPartyMember = true;
				}
				else if( PvPCommon::Team::A == hActor->GetTeam() )
				{
					m_pButtonParty->GetProperty( sUIProperty );
					iter->bPartyMember = false;
				}
				else if( PvPCommon::Team::B == hActor->GetTeam() )
				{
					m_pButtonEnemy->GetProperty( sUIProperty );
					iter->bPartyMember = false;
				}

				pButton = (CEtUIButton*)CEtUIDialog::CreateControl( &sUIProperty );
				pButton->Show( true );

				CEtUIDialog::DeleteControl( iter->pButton );
				iter->pButton = pButton;
			}
		}
	}
}

void CDnGuildWarZoneMapDlg::ProcessAlarm( float fElapsedTime )
{
	for( std::list<SZoneAlarmInfo>::iterator iter = m_listAlarm.begin(); iter != m_listAlarm.end(); )
	{
		if( 0 > iter->fDurationTime )
		{
			DeleteControl( iter->pStaticAlarm );
			iter = m_listAlarm.erase( iter );
			continue;
		}
		else
		{
			int nDuration = (int)iter->fDurationTime;

			if( nDuration % 2 )
				iter->pStaticAlarm->Show( false );
			else
				iter->pStaticAlarm->Show( true );

			iter->fDurationTime -= fElapsedTime;
		}

		++iter;
	}
}

void CDnGuildWarZoneMapDlg::ProcessBoss()
{
	for( std::list<SZoneBossInfo>::iterator iter = m_listBoss.begin(); iter != m_listBoss.end(); )
	{
		CDnActor * pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );

		if( pActor )
		{
			iter->pStaticBoss->Show( true );
			iter->pStaticText->Show( true );

			float nPercent = (float)( (float)pActor->GetHP() / (float)pActor->GetMaxHP() ) * 100.f;
			WCHAR wszString[64];
			swprintf_s( wszString, _countof(wszString), L"(%.0f%%)", nPercent );
			iter->pStaticText->SetText( wszString );
		}
		else
		{
			iter->nSessionID = FindMonsterUniqueID( iter->nTableID );

			iter->pStaticBoss->Show( false );
			iter->pStaticText->Show( false );
		}

		++iter;
	}
}

void CDnGuildWarZoneMapDlg::ProcessDoor()
{
	for( std::list<SZoneDoorInfo>::iterator iter = m_listDoor.begin(); iter != m_listDoor.end(); )
	{
		CDnActor * pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );

		if( pActor )
		{
			iter->pStaticDoor->Show( true );
			iter->pStaticBrokenDoor->Show( false );
			iter->pStaticText->Show( true );

			float nPercent = (float)( (float)pActor->GetHP() / (float)pActor->GetMaxHP() ) * 100.f;
			WCHAR wszString[64];
			swprintf_s( wszString, _countof(wszString), L"(%.0f%%)", nPercent );
			iter->pStaticText->SetText( wszString );
		}
		else
		{
			iter->nSessionID = FindMonsterUniqueID( iter->nTableID );

			iter->pStaticDoor->Show( false );
			iter->pStaticBrokenDoor->Show( true );
			iter->pStaticText->Show( false );
		}

		++iter;
	}
}

void CDnGuildWarZoneMapDlg::ProcessBuff()
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalPlayer = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor).GetPointer();

	if( !pLocalPlayer )
		return;

	bool bEnemyShow = pLocalPlayer->GetStateBlow()->IsExistStateBlowFromBlowIndex( STATE_BLOW::BLOW_210 );

	std::list<SZonePartyButtonInfo>::iterator itor = m_listButtonEnemy.begin();
	for( ; itor != m_listButtonEnemy.end(); ++itor )
		itor->pButton->Show( bEnemyShow );
}

void CDnGuildWarZoneMapDlg::ProcessPosition( float fElapsedTime )
{
	if( !CDnWorld::IsActive()  || !CDnWorld::GetInstance().GetGrid() ) 
		return;

	if ( !CDnWorld::GetInstance().GetGrid() )
		return;

	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask ) return;

	CDnPartyTask::PartyStruct* pParty(NULL);

	SUICoord textureCoord;
	m_pTextureMinimap->GetUICoord(textureCoord);

	float fMapWidth = CDnWorld::GetInstance().GetGrid()->GetGridWidth() * 50.0f;
	float fMapHeight = CDnWorld::GetInstance().GetGrid()->GetGridHeight() * 50.0f;

	CEtUIButton *pButton(NULL);
	CDnActor *pActor(NULL);

	for( std::list<SZonePartyButtonInfo>::iterator iter = m_listButtonParty.begin(); iter != m_listButtonParty.end(); )
	{
		pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );
		pParty = pPartyTask->GetPartyDataFromSessionID( iter->nSessionID );

		if( !pParty || !pActor )
		{
			DeleteControl( iter->pButton );
			iter = m_listButtonParty.erase( iter );
			continue;
		}

		ControlPosition( iter->pButton, pActor->GetName(), *pActor->GetPosition() );
		++iter;
	}

	for( std::list<SZonePartyButtonInfo>::iterator iter = m_listButtonEnemy.begin(); iter != m_listButtonEnemy.end(); )
	{
		pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );
		pParty = pPartyTask->GetPartyDataFromSessionID( iter->nSessionID );

		if( !pParty || !pActor )
		{
			DeleteControl( iter->pButton );
			iter = m_listButtonEnemy.erase( iter );
			continue;
		}

		ControlPosition( iter->pButton, pActor->GetName(), *pActor->GetPosition() );
		++iter;
	}

	for( std::list<SZoneBossInfo>::iterator iter = m_listBoss.begin(); iter != m_listBoss.end(); ++iter )
	{
		if( -1 == iter->nSessionID )
			continue;

		pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );

		if( !pActor || !pActor->IsMonsterActor() )
			continue;

		ControlPosition( iter->pStaticBoss, pActor->GetName(), *pActor->GetPosition(), iter->pStaticText );
	}

	for( std::list<SZoneDoorInfo>::iterator iter = m_listDoor.begin(); iter != m_listDoor.end(); ++iter )
	{
		pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );

		if( pActor && pActor->IsMonsterActor() )
			iter->szName = pActor->GetName();
		else
			iter->szName = NULL;

		ControlPosition( iter->pStaticDoor, iter->szName, iter->vPosition, iter->pStaticText );
		ControlPosition( iter->pStaticBrokenDoor, iter->szName, iter->vPosition );
	}

	for( std::list<SZoneAlarmInfo>::iterator iter = m_listAlarm.begin(); iter != m_listAlarm.end(); ++iter )
	{
		if( -1 == iter->nSessionID )
			continue;

		pActor = (CDnActor*)CDnActor::FindActorFromUniqueID( iter->nSessionID );

		if( !pActor || !pActor->IsMonsterActor() )
			continue;

		ControlPosition( iter->pStaticAlarm, pActor->GetName(), *pActor->GetPosition() );
	}

	//나의 위치 정보
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalPlayer = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor).GetPointer();

	EtVector3 *pNpcPos = pLocalPlayer->GetPosition();
	if( !pNpcPos ) return;

	float fNpcX = pNpcPos->x + fMapWidth;
	float fNpcY = fabs(pNpcPos->z - fMapHeight);

	SUICoord UVCoord;
	m_pTextureMinimap->GetUVCoord(UVCoord);

	fNpcX = (((fNpcX / fMapWidth )*0.5f - UVCoord.fX) / UVCoord.fWidth) * textureCoord.fWidth;
	fNpcY = (((fNpcY / fMapHeight )*0.5f - UVCoord.fY) / UVCoord.fHeight) * textureCoord.fHeight;

	SUICoord uiCoord;
	m_pStaticPlayer->GetUICoord(uiCoord);
	m_pStaticPlayer->SetPosition( textureCoord.fX + fNpcX - (uiCoord.fWidth/2.0f), textureCoord.fY + fNpcY - (uiCoord.fHeight/2.0f) );
	m_pStaticPlayer->SetTooltipText( pLocalPlayer->GetName() );

	EtVector3 vZDir = CDnCamera::GetActiveCamera()->GetMatEx()->m_vZAxis;
	float fDegree = EtToDegree( atan2f( vZDir.x, vZDir.z ) );	
	m_pStaticPlayer->SetRotate( fDegree );
}

void CDnGuildWarZoneMapDlg::ProcessMousePointer( float fElapsedTime )
{
	if( m_fMousePointTime < 0.f )
	{
		m_fMousePointTime = 0.f;
		m_pButtonMousePoint->Show( false );
	}
	else if( m_pButtonMousePoint->IsShow() )
		m_fMousePointTime -= fElapsedTime;
}

void CDnGuildWarZoneMapDlg::AddUser( DnActorHandle hActor, bool bIsGuildWar )
{
	if( !hActor )
		return;

	std::list<SZonePartyButtonInfo>::iterator iter = m_listButtonParty.begin();
	for( ; iter != m_listButtonParty.end(); ++iter)
	{
		if( hActor->GetUniqueID() == iter->nSessionID )
		{
			OutputDebug("길드전 미니맵에 존재하는 유저 또 추가 함!!");
			return;
		}
	}

	if( bIsGuildWar )
		AddGuildWarUserButton( hActor );
	else
		AddOccupationUserButton( hActor );

	MoveToTail( m_pStaticPlayer );
}

void CDnGuildWarZoneMapDlg::AddGuildWarUserButton( DnActorHandle hActor )
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalPlayer = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor).GetPointer();
	CDnGuildWarSituationMng * pMng = GetInterface().GetGuildWarSituation();

	if( !hActor || !pLocalPlayer || !pMng )
		return;

	if (CommonUtil::IsValidCharacterClassId( hActor->GetClassID() ) == false)
		return;

	CEtUIButton *pButton(NULL);
	SUIControlProperty sUIProperty;
	SZonePartyButtonInfo zonePartyButtonInfo;

	if( pLocalPlayer->GetUniqueID() == hActor->GetUniqueID() )	//나 자신이라면 리턴한다.
		return;
	if( pMng->IsPartyMember( hActor ) )
	{
		m_pButtonPartyMember->GetProperty( sUIProperty );
		zonePartyButtonInfo.bPartyMember = true;
	}
	else if( PvPCommon::Team::A == hActor->GetTeam() )
	{
		m_pButtonParty->GetProperty( sUIProperty );
		zonePartyButtonInfo.bPartyMember = false;
	}
	else if( PvPCommon::Team::B == hActor->GetTeam() )
	{
		m_pButtonEnemy->GetProperty( sUIProperty );
		zonePartyButtonInfo.bPartyMember = false;
	}

	pButton = (CEtUIButton*)CEtUIDialog::CreateControl( &sUIProperty );
	pButton->Show( true );
	
	zonePartyButtonInfo.pButton = pButton;
	zonePartyButtonInfo.nSessionID = hActor->GetUniqueID();

	if( pLocalPlayer->GetTeam() == hActor->GetTeam() )
	{
		m_listButtonParty.push_back( zonePartyButtonInfo );
	}
	else if( PvPCommon::Team::A == hActor->GetTeam() || PvPCommon::Team::B == hActor->GetTeam() )
	{
		m_listButtonEnemy.push_back( zonePartyButtonInfo );
		zonePartyButtonInfo.pButton->Show( m_bEnemyShow );
	}
}

void CDnGuildWarZoneMapDlg::AddOccupationUserButton( DnActorHandle hActor )
{
	if( !CDnActor::s_hLocalActor ) return;

	CDnLocalPlayerActor * pLocalPlayer = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor).GetPointer();

	if( !hActor || !pLocalPlayer ) return;

	if (CommonUtil::IsValidCharacterClassId( hActor->GetClassID() ) == false) return;

	if( pLocalPlayer->GetUniqueID() == hActor->GetUniqueID() )	return;

	CEtUIButton *pButton(NULL);
	SUIControlProperty sUIProperty;
	SZonePartyButtonInfo zonePartyButtonInfo;

	if( pLocalPlayer->GetTeam() == hActor->GetTeam() )
	{
		m_pButtonParty->GetProperty( sUIProperty );
		zonePartyButtonInfo.bPartyMember = false;
	}
	else
		return;

	pButton = (CEtUIButton*)CEtUIDialog::CreateControl( &sUIProperty );
	pButton->Show( true );

	zonePartyButtonInfo.pButton = pButton;
	zonePartyButtonInfo.nSessionID = hActor->GetUniqueID();

	m_listButtonParty.push_back( zonePartyButtonInfo );
}

void CDnGuildWarZoneMapDlg::AddAlarm( DnActorHandle hActor )
{
	if( !hActor )
		return;

	SZoneAlarmInfo alarmInfo;
	SUIControlProperty sUIProperty;
	
	m_pStaticAlarm->GetProperty( sUIProperty );
	alarmInfo.pStaticAlarm = (CEtUIStatic *)CEtUIDialog::CreateControl( &sUIProperty );
	alarmInfo.pStaticAlarm->Show( true );
	alarmInfo.fDurationTime = ALARM_TIME;
	alarmInfo.nSessionID = hActor->GetUniqueID();

	m_listAlarm.push_back( alarmInfo );

	MoveToTail( alarmInfo.pStaticAlarm );
}

void CDnGuildWarZoneMapDlg::AddDoorStatic( int eTeamIndex, int nDoorID, int nAreaID )
{
	if( 0 == nDoorID )
		return;

	SZoneDoorInfo doorInfo;
	SUIControlProperty sUIDoorProperty, sUIDoorBrokenProperty, sUIDoorTextProperty;

	if( PvPCommon::TeamIndex::A == eTeamIndex )
	{
		m_pStaticBlueDoor->GetProperty( sUIDoorProperty );
		m_pStaticBlueBrokenDoor->GetProperty( sUIDoorBrokenProperty );
		m_pStaticBlueText->GetProperty( sUIDoorTextProperty );
	}
	else if( PvPCommon::TeamIndex::B == eTeamIndex )
	{
		m_pStaticRedDoor->GetProperty( sUIDoorProperty );
		m_pStaticRedBrokenDoor->GetProperty( sUIDoorBrokenProperty );
		m_pStaticRedText->GetProperty( sUIDoorTextProperty );
	}

	doorInfo.pStaticDoor = (CEtUIStatic *)CEtUIDialog::CreateControl( &sUIDoorProperty );
	doorInfo.pStaticBrokenDoor = (CEtUIStatic *)CEtUIDialog::CreateControl( &sUIDoorBrokenProperty );
	doorInfo.pStaticText = (CEtUIStatic *)CEtUIDialog::CreateControl( &sUIDoorTextProperty );
	doorInfo.nTableID = nDoorID;
	doorInfo.nAreaID = nAreaID;
	doorInfo.nSessionID = FindMonsterUniqueID( nDoorID );

	CEtWorldEventControl*	pControl	= NULL;
	CEtWorldSector*			pSector		= NULL;
	CEtWorldEventArea*		pArea		= NULL;

	std::vector<CEtWorldEventArea*> vRespawnArea;

	CDnWorld * pWorld = CDnWorld::GetInstancePtr();
	if( !pWorld )
		return;

	for( DWORD i=0; i<pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
	{
		pSector	 = pWorld->GetGrid()->GetActiveSector(i);
		if( !pSector )
			continue;

		pControl = pSector->GetControlFromUniqueID( ETE_EventArea );
		if( !pControl ) 
			continue;

		for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
		{
			pArea = pControl->GetAreaFromIndex(j);
			if( pArea && nAreaID == pArea->GetCreateUniqueID() )
				break;
		}

		if( pArea )
		{
			doorInfo.vPosition = pArea->GetOBB()->Center;
			break;
		}
	}

	m_listDoor.push_back( doorInfo );
}

void CDnGuildWarZoneMapDlg::AddBossStatic( int eTeamIndex, int nBossID )
{
	if( 0 == nBossID )
		return;

	SZoneBossInfo bossInfo;
	SUIControlProperty sUIProperty, sUITextProperty;
 	if( PvPCommon::TeamIndex::A == eTeamIndex )
	{
		m_pStaticBlueBoss->GetProperty( sUIProperty );
		m_pStaticBlueText->GetProperty( sUITextProperty );
	}
	else if( PvPCommon::TeamIndex::B == eTeamIndex )
	{
		m_pStaticRedBoss->GetProperty( sUIProperty );
		m_pStaticRedText->GetProperty( sUITextProperty );
	}

	bossInfo.pStaticBoss = (CEtUIStatic *)CEtUIDialog::CreateControl( &sUIProperty );
	bossInfo.pStaticText = (CEtUIStatic *)CEtUIDialog::CreateControl( &sUITextProperty ); 
	bossInfo.nTableID = nBossID;
	bossInfo.nSessionID = FindMonsterUniqueID( nBossID );

	m_listBoss.push_back( bossInfo );
}

void CDnGuildWarZoneMapDlg::OnMousePointer( EtVector3 vPos )
{
	m_fMousePointTime = ALARM_TIME;

	m_pButtonMousePoint->SetPosition( vPos.x, vPos.y );
	m_pButtonMousePoint->Show( true );

	MoveToTail( m_pButtonMousePoint );
}

void CDnGuildWarZoneMapDlg::ProcessFlag()
{
	if( CDnOccupationTask::IsActive() )
	{
		if( !CDnActor::s_hLocalActor ) return;
		CDnLocalPlayerActor * pLocalActor = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
		if( !pLocalActor ) return;

		std::list<SFlagButtonInfo>::iterator itor = m_listButtonFlag.begin();

		CDnOccupationZone * pOccupationZone = NULL;
		for(; itor != m_listButtonFlag.end(); ++itor)
		{
			itor->Clear();

			pOccupationZone = GetOccupationTask().GetFlag( itor->m_nAreaID );
			if( !pOccupationZone )
				continue;

			if( PvPCommon::OccupationState::MoveWait & pOccupationZone->m_eState )
			{
				if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
				{
					if( PvPCommon::Team::A == pOccupationZone->m_nMoveWaitTeamID )
						itor->pButton[BLUE_PROCESS_FLAG]->Show( true );
					else
						itor->pButton[RED_PROCESS_FLAG]->Show( true );
				}
				if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
				{
					if( PvPCommon::Team::Observer == pLocalActor->GetTeam() )
					{
						if( PvPCommon::Team::A == pOccupationZone->m_nTryTeamID )
							itor->pButton[BLUE_PROCESS_FLAG]->Show( true );
						else if( PvPCommon::Team::B == pOccupationZone->m_nTryTeamID )
							itor->pButton[RED_PROCESS_FLAG]->Show( true );
					}
					else
					{
						if( pLocalActor->GetTeam() == pOccupationZone->m_nTryTeamID )
							itor->pButton[BLUE_PROCESS_FLAG]->Show( true );
						else
							itor->pButton[RED_PROCESS_FLAG]->Show( true );
					}
				}
			}
			else if( PvPCommon::OccupationState::Try & pOccupationZone->m_eState &&
				CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
			{
				if( PvPCommon::Team::Observer == pLocalActor->GetTeam() )
				{
					if( PvPCommon::Team::A == pOccupationZone->m_nTryTeamID )
						itor->pButton[BLUE_PROCESS_FLAG]->Show( true );
					else if( PvPCommon::Team::B == pOccupationZone->m_nTryTeamID )
						itor->pButton[RED_PROCESS_FLAG]->Show( true );
				}
				else
				{
					if( pLocalActor->GetTeam() == pOccupationZone->m_nTryTeamID )
						itor->pButton[BLUE_PROCESS_FLAG]->Show( true );
					else
						itor->pButton[RED_PROCESS_FLAG]->Show( true );
				}
			}
			else if( PvPCommon::OccupationState::Own & pOccupationZone->m_eState )
			{
				if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
				{
					if( PvPCommon::Team::A == pOccupationZone->m_nOwnedTemID )
						itor->pButton[BLUE_SUCCESS_FLAG]->Show( true );
					else
						itor->pButton[RED_SUCCESS_FLAG]->Show( true );
				}			
				if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
				{
					if( PvPCommon::Team::Observer == pLocalActor->GetTeam() )
					{
						if( PvPCommon::Team::A == pOccupationZone->m_nOwnedTemID )
							itor->pButton[BLUE_SUCCESS_FLAG]->Show( true );
						else if( PvPCommon::Team::B == pOccupationZone->m_nOwnedTemID )
							itor->pButton[RED_SUCCESS_FLAG]->Show( true );
					}
					else
					{
						if( pLocalActor->GetTeam() == pOccupationZone->m_nOwnedTemID )
							itor->pButton[BLUE_SUCCESS_FLAG]->Show( true );
						else
							itor->pButton[RED_SUCCESS_FLAG]->Show( true );
					}
				}
			}
			else
				itor->pButton[NONE_FLAG]->Show( true );
		}
	}
}

void CDnGuildWarZoneMapDlg::CreateFlagControl()
{
	CEtUIButton *pButton(NULL);

	SUICoord textureCoord;
	m_pTextureMinimap->GetUICoord(textureCoord);

	float fMapWidth = CDnWorld::GetInstance().GetGrid()->GetGridWidth() * 50.0f;
	float fMapHeight = CDnWorld::GetInstance().GetGrid()->GetGridHeight() * 50.0f;

	std::vector<CDnOccupationZone *> & vFlag = GetOccupationTask().GetFlagVector();

	for( int itr = 0; itr < (int)vFlag.size(); ++itr )
	{
		SFlagButtonInfo flagButtonInfo;
		SUIControlProperty sUIProperty;
		SUICoord uiCoord;

		EtVector3 position = vFlag[itr]->m_OBB.Center;

		float fX = position.x + fMapWidth;
		float fY = fabs(position.z - fMapHeight);

		SUICoord UVCoord;
		m_pTextureMinimap->GetUVCoord(UVCoord);

		fX = (((fX / fMapWidth )*0.5f - UVCoord.fX) / UVCoord.fWidth) * textureCoord.fWidth;
		fY = (((fY / fMapHeight )*0.5f - UVCoord.fY) / UVCoord.fHeight) * textureCoord.fHeight;

		for( int jtr = 0; jtr < MAX_FLAG; ++jtr )
		{
			m_pButtonFlag[jtr]->GetProperty( sUIProperty );

			pButton = (CEtUIButton*)CEtUIDialog::CreateControl( &sUIProperty );
			pButton->Show( false );

			pButton->GetUICoord(uiCoord);
			pButton->SetPosition( textureCoord.fX + fX - (uiCoord.fWidth/2.0f), textureCoord.fY + fY - (uiCoord.fHeight/2.0f) );
			pButton->SetTooltipText( vFlag[itr]->m_wszName.c_str() );

			flagButtonInfo.pButton[jtr] = pButton;
		}

		flagButtonInfo.m_nAreaID = vFlag[itr]->m_nAreaID;
		flagButtonInfo.m_Position = vFlag[itr]->m_OBB.Center;

		m_listButtonFlag.push_back( flagButtonInfo );
	}
}

void CDnGuildWarZoneMapDlg::CreateDoorBossControl()
{
	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );

	int nBattleGroundID = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "_BattleGroundID" )->GetInteger();;

	pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDMODE );

	char szColumName[32];
	for( int itr = 0; itr < PvPCommon::TeamIndex::Max; ++itr )
	{
		if( PvPCommon::TeamIndex::A == itr )
		{
			for( int jtr = 0; jtr < PvPCommon::Common::DoorCount; ++jtr )
			{
				sprintf_s( szColumName, _countof(szColumName), "_CosmosGate%dID", jtr+1 );
				int nDoorID = pSox->GetFieldFromLablePtr( nBattleGroundID, szColumName)->GetInteger();

				sprintf_s( szColumName, _countof(szColumName), "_CosmosGate%dArea", jtr+1 );
				int nAreaID = pSox->GetFieldFromLablePtr( nBattleGroundID, szColumName)->GetInteger();

				AddDoorStatic( PvPCommon::TeamIndex::A, nDoorID, nAreaID );
			}

			sprintf_s( szColumName, _countof(szColumName), "_CosmosBossID" );
			int nBossID = pSox->GetFieldFromLablePtr( nBattleGroundID, szColumName)->GetInteger();
			AddBossStatic( PvPCommon::TeamIndex::A, nBossID );
		}
		else if( PvPCommon::TeamIndex::B == itr )
		{
			for( int jtr = 0; jtr < PvPCommon::Common::DoorCount; ++jtr )
			{
				sprintf_s( szColumName, _countof(szColumName), "_ChaosGate%dID", jtr+1 );
				int nDoorID = pSox->GetFieldFromLablePtr( nBattleGroundID, szColumName)->GetInteger();

				sprintf_s( szColumName, _countof(szColumName), "_ChaosGate%dArea", jtr+1 );
				int nAreaID = pSox->GetFieldFromLablePtr( nBattleGroundID, szColumName)->GetInteger();

				AddDoorStatic( PvPCommon::TeamIndex::B, nDoorID, nAreaID );
			}

			sprintf_s( szColumName, _countof(szColumName), "_ChaosBossID" );
			int nBossID = pSox->GetFieldFromLablePtr( nBattleGroundID, szColumName)->GetInteger();
			AddBossStatic( PvPCommon::TeamIndex::B, nBossID );
		}
	}
}

void CDnGuildWarZoneMapDlg::SetMap()
{
	int nCurMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;

	CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nCurMapIndex );

	if( pWorldData )
	{
		std::string strMiniMapName;
		pWorldData->GetMiniMapName( strMiniMapName );

		char szTemp[_MAX_PATH] = {0};
		sprintf_s( szTemp, _MAX_PATH, "%s\\Grid\\%s\\%s_Map.dds", CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str(), strMiniMapName.c_str(), strMiniMapName.c_str() );
		SAFE_RELEASE_SPTR( m_hMinimap );
		m_hMinimap = LoadResource( szTemp, RT_TEXTURE );
		if( !m_hMinimap ) {
			sprintf_s( szTemp, _MAX_PATH, "%s\\Grid\\%s\\%s.dds", CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str(), strMiniMapName.c_str(), strMiniMapName.c_str() );
			m_hMinimap = LoadResource( szTemp, RT_TEXTURE );
		}

		int nX = 0, nY = 0, nWidth = 1024, nHeight = 1024;			
		if( m_hMinimap ) {
			m_hMinimap->CalcFittingArea( 10, &nX, &nY, &nWidth, &nHeight);

			if( nWidth > nHeight ) {
				nY -= (nWidth-nHeight)/2;
				nHeight = nWidth;
			}
			if( nHeight > nWidth ) {
				nX -= (nHeight-nWidth)/2;
				nWidth = nHeight;
			}

			const int GabSize = 60;

			nX -= GabSize;
			nWidth += GabSize*2;
			nY -= GabSize;
			nHeight += GabSize*2;

			m_pTextureMinimap->SetTexture( m_hMinimap, nX, nY, nWidth, nHeight );
		}

		m_pTextureMinimap->GetUICoord(m_TextureCoord);
		m_pTextureMinimap->GetUVCoord(m_UVCoord);
		m_fMapWidth = CDnWorld::GetInstance().GetGrid()->GetGridWidth() * 50.0f;
		m_fMapHeight = CDnWorld::GetInstance().GetGrid()->GetGridHeight() * 50.0f;
	}
}

void CDnGuildWarZoneMapDlg::ControlPosition( CEtUIControl * pControl, const TCHAR * szName, EtVector3 vPosition, CEtUIControl * pTextControl )
{
	if( !pControl )
	{
		OutputDebug("GuildWarZoneMapDlg ControlPosition Error");
		return;
	}

	float fNpcX = vPosition.x + m_fMapWidth;
	float fNpcY = fabs(vPosition.z - m_fMapHeight);

	fNpcX = (((fNpcX / m_fMapWidth )*0.5f - m_UVCoord.fX) / m_UVCoord.fWidth) * m_TextureCoord.fWidth;
	fNpcY = (((fNpcY / m_fMapHeight )*0.5f - m_UVCoord.fY) / m_UVCoord.fHeight) * m_TextureCoord.fHeight;

	SUICoord uiCoord;
	pControl->GetUICoord(uiCoord);
	pControl->SetPosition( m_TextureCoord.fX + fNpcX - (uiCoord.fWidth/2.0f), m_TextureCoord.fY + fNpcY - (uiCoord.fHeight/2.0f) );

	if( szName )
		pControl->SetTooltipText( szName );

	if( pTextControl )
	{
		SUICoord uiTextCoord;
		pTextControl->GetUICoord(uiTextCoord);
		pTextControl->SetPosition( m_TextureCoord.fX + fNpcX - (uiTextCoord.fWidth/2.0f), 
			m_TextureCoord.fY + fNpcY - (uiCoord.fHeight) );
	}
}

int CDnGuildWarZoneMapDlg::FindMonsterUniqueID( int nTableID )
{
	for( int itr = 0; itr < (int)CDnActor::s_pVecProcessList.size(); ++itr )
	{
		if( CDnActor::s_pVecProcessList[itr] && CDnActor::s_pVecProcessList[itr]->IsMonsterActor() )
		{
			CDnMonsterActor * pMonster = reinterpret_cast<CDnMonsterActor *>(CDnActor::s_pVecProcessList[itr]);

			if( pMonster && pMonster->GetMonsterClassID() == nTableID )
				return pMonster->GetUniqueID();
		}
	}

	return -1;
}
