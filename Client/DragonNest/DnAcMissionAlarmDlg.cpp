#include "StdAfx.h"

#ifdef PRE_ADD_ACTIVEMISSION

#include "DnItemTask.h"
#include "DnTableDB.h"
#include "DnItem.h"
#include "GameOption.h"

#include "DnAcMissionAlarmDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const int NPC_FACE_TEXTURE_SIZE = 64;

CDnAcMissionAlarmDlg::CDnAcMissionAlarmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
, m_fShowTime(0.0f)
{
	m_pStaticDesc = NULL;	
}


void CDnAcMissionAlarmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MissionActiveAlarmDlg.ui" ).c_str(), bShow );	
}

void CDnAcMissionAlarmDlg::InitialUpdate()
{
	m_pStaticDesc = GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_TEXT_MAINTITLE");
	if( pStatic )
		pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9170 ) );
}

void CDnAcMissionAlarmDlg::Process( float fElapsedTime )
{
	if( !IsShow() )
		return;

	CEtUIDialog::Process( fElapsedTime );

	m_fShowTime -= fElapsedTime;		

	if( m_fShowTime <= 0.0f )
		Show( false );
	
}

void CDnAcMissionAlarmDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow ) return;

	if( bShow )
	{
	//	if( false == CGameOption::GetInstance().bSimpleGuide )
	//		return;
	}
	else
	{
		m_fShowTime = 0.0f;
	}

	CEtUIDialog::Show( bShow );
}


void CDnAcMissionAlarmDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl( "ID_BT_CLOSE" ) ) {
			Show( false );
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnAcMissionAlarmDlg::SetAlarm( std::wstring & str, float fadeTime )
{
	m_fShowTime = fadeTime;	
	if( m_pStaticDesc )
		m_pStaticDesc->SetText( str.c_str() );
}

bool CDnAcMissionAlarmDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show( false );	
			return true;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

#endif // PRE_ADD_ACTIVEMISSION