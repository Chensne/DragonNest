#include "StdAfx.h"
#include "DnDungeonClearMoveDlg.h"
#include "DnWorld.h"
#include "DnBridgeTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnDungeonClearMoveDlg::CDnDungeonClearMoveDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_fElapsedTime(0.0f)
	, m_nCount(0)
	, m_pStaticTimer(NULL)
	, m_pButtonClose(NULL)
	, m_pButtonToWorld(NULL)
	, m_pButtonToVillage(NULL)
	, m_bMissionFailMode(true)
	, m_pStaticMissionFailGuide(NULL)
	, m_pStaticNormalGuide(NULL)
	, m_bTriggeredByUser_DisableWorld( false )
{
	memset( m_wszTimer, 0, sizeof( m_wszTimer ) );
}

CDnDungeonClearMoveDlg::~CDnDungeonClearMoveDlg(void)
{
}

void CDnDungeonClearMoveDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonClearMoveDlg.ui" ).c_str(), bShow );
}

void CDnDungeonClearMoveDlg::InitialUpdate()
{
	m_pStaticTimer = GetControl<CEtUIStatic>("ID_STATIC_TIMER");
	m_pButtonToWorld = GetControl<CEtUIButton>("ID_TO_WORLD");
	m_pButtonToVillage = GetControl<CEtUIButton>("ID_TO_VILLAGE");
	m_pButtonClose = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
	m_pStaticMissionFailGuide = GetControl<CEtUIStatic>("ID_STATIC");
	m_pStaticNormalGuide = GetControl<CEtUIStatic>("ID_STATIC_CENTER");
	m_pStaticMissionFailGuide->Show(false);
	m_pStaticNormalGuide->Show(false);
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
	m_pStaticVillageName = GetControl<CEtUIStatic>("ID_TEXT_VILLAGENAME");
	m_pStaticVillageName->Show(false);
#endif
	if (m_bMissionFailMode)
	{
		m_pButtonClose->Enable(false);
	}
}

void CDnDungeonClearMoveDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if (m_bMissionFailMode)
	{
		if( m_nCount > 0 )
		{
			m_fElapsedTime += fElapsedTime;

			if( m_fElapsedTime > 1.0f )
			{
				m_nCount--;
				SetTime( m_nCount );

				m_fElapsedTime -= 1.0f;
			}
		}
		else
		{
			if( IsShow() )
			{
				CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonToVillage, 0 );
				Show(false);
			}
		}
	}
}

void CDnDungeonClearMoveDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( CDnBridgeTask::IsActive() && CDnBridgeTask::GetInstance().IsZeroPopulation() )
		{
			if( IsCmdControl( "ID_TO_WORLD" ) )
			{
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3190 ), true );
				return;
			}
			else if( IsCmdControl( "ID_TO_VILLAGE" ) )
			{
				CDnBridgeTask::GetInstance().SetZeroPopulation( false );
			}
		}
		Show(false);
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDungeonClearMoveDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonClose->Enable(m_bMissionFailMode == false);

		m_pStaticMissionFailGuide->Show(m_bMissionFailMode);
		m_pStaticNormalGuide->Show(m_bMissionFailMode == false);
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
		m_pStaticVillageName->Show( true );
#endif 
		m_pStaticTimer->ClearText();
//		m_pButtonToWorld->Enable(true);

		if (m_bMissionFailMode)
		{
			m_nCount = 10;
			SetTime( m_nCount );
		}

		if( true == m_bTriggeredByUser_DisableWorld )
			m_pButtonToWorld->Enable( false );
	}
	else
	{
		m_fElapsedTime = 0.0f;
	}

	CEtUIDialog::Show( bShow );

	if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap ) {
		m_pButtonToWorld->Enable( false );
	}
	//CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnDungeonClearMoveDlg::SetTime( int nTime )
{
	swprintf_s( m_wszTimer, 80, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2349 ), nTime );
	m_pStaticTimer->SetText( m_wszTimer ); 
}

void CDnDungeonClearMoveDlg::SetMissionFailMode(bool bSet)
{
	m_bMissionFailMode = bSet;
}
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
void CDnDungeonClearMoveDlg::SetReturnVillage( int nVillage )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if ( !pSox || !pSox->IsExistItem( nVillage ) ) return;

	int nID = pSox->GetFieldFromLablePtr( nVillage, "_MapNameID" )->GetInteger();
	m_pStaticVillageName->ClearText();

	wchar_t wszMapName[128] = {0, };
	swprintf_s( wszMapName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 804 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nID ) ); // MID : 마을위치 :%s
	m_pStaticVillageName->SetText( wszMapName );
}
#endif 