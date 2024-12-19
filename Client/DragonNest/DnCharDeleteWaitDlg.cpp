#include "StdAfx.h"
#include "DnCharDeleteWaitDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharDeleteWaitDlg::CDnCharDeleteWaitDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pButtonDeleteCancel( NULL )
, m_pStaticDeleteRemainTime( NULL )
, m_pStaticSlotBlock( NULL )
, m_ExpectTime( 0 )
{
}

CDnCharDeleteWaitDlg::~CDnCharDeleteWaitDlg()
{
}

void CDnCharDeleteWaitDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharDelet.ui" ).c_str(), bShow );
}

void CDnCharDeleteWaitDlg::InitialUpdate()
{
	m_pStaticDeleteRemainTime = GetControl<CEtUIStatic>("ID_TEXT_TIME");
	m_pButtonDeleteCancel = GetControl<CEtUIButton>("ID_BUTTON_DELET");
	m_pStaticSlotBlock = GetControl<CEtUIStatic>("ID_STATIC_NOTICE");
}

void CDnCharDeleteWaitDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl("ID_BUTTON_DELET" ) ) {
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 113 ), MB_YESNO, m_nDialogID, this );
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCharDeleteWaitDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	if( !IsShow() ) return;

	UpdateDeleteRemainTime();
}


void CDnCharDeleteWaitDlg::SetRemainTime( __time64_t RemainTime )
{
	m_ExpectTime = RemainTime;

	UpdateDeleteRemainTime();
}

void CDnCharDeleteWaitDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow ) {
		m_pButtonDeleteCancel->Enable( true );
	}
	CEtUIDialog::Show( bShow );
}

void CDnCharDeleteWaitDlg::UpdateDeleteRemainTime()
{
#ifdef PRE_FIX_CHARDELWAITDLG_TIME_DUMP
	tm* pExpect = localtime( &m_ExpectTime );
	if (pExpect == NULL)
	{
		m_pStaticDeleteRemainTime->SetText( L"" );
		return;
	}

	__time64_t CurTime;
	time( &CurTime );

	const tm ExpectTime = (*pExpect);
	tm* pCurrentTime = localtime( &CurTime );
	if (pCurrentTime == NULL)
	{
		m_pStaticDeleteRemainTime->SetText( L"" );
		return;
	}

	const tm CurrentTime = (*pCurrentTime);
#else
	tm *ptmTemp = localtime( &m_ExpectTime );
	if( ptmTemp == NULL ) {
		m_pStaticDeleteRemainTime->SetText( L"" );
		return;
	}

	__time64_t CurTime;
	time( &CurTime );

	tm ExpectTime = *localtime( &m_ExpectTime );
	tm CurrentTime = *localtime( &CurTime );
#endif

	__int64 nExpectTime = ( GetDateValue( 1900 + ExpectTime.tm_year, ExpectTime.tm_mon + 1, ExpectTime.tm_mday ) * 24 * 60 ) + ( ExpectTime.tm_hour * 60 ) + ExpectTime.tm_min;
	__int64 nCurrentTime = ( GetDateValue( 1900 + CurrentTime.tm_year, CurrentTime.tm_mon + 1, CurrentTime.tm_mday ) * 24 * 60 ) + ( CurrentTime.tm_hour * 60 ) + CurrentTime.tm_min;
	__int64 nRemainTime = nExpectTime - nCurrentTime;
	if( nRemainTime < 0 ) {
		m_pStaticDeleteRemainTime->SetText( L"" );
		return;
	}

	WCHAR wszTemp[256];
	if( nRemainTime >= 60 * 24 ) {
		int nDay = (int)( nRemainTime / ( 60 * 24 ) );
		int nHour = (int)( ( nRemainTime % ( 60 * 24 ) ) / 60 );
		int nMin = (int)( ( nRemainTime % ( 60 * 24 ) )  % 60 );
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 116 ), nDay, nHour, nMin );
	}
	else {
		int nHour = (int)( nRemainTime / 60 );
		int nMin = (int)( nRemainTime % 60 );
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1986 ), nHour, nMin );
	}
	m_pStaticDeleteRemainTime->SetText( wszTemp );
}

void CDnCharDeleteWaitDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl("ID_YES" ) ) {
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pTask ) pTask->ReviveCharacter( nID - PLAYERDELETEWAIT_DIALOG );
			m_pButtonDeleteCancel->Enable( false );
		}
	}
}

void CDnCharDeleteWaitDlg::ShowSlotBlock( bool bShow )
{
	m_pStaticDeleteRemainTime->Show( !bShow );
	m_pButtonDeleteCancel->Show( !bShow );
	m_pStaticSlotBlock->Show( bShow );

	if( bShow )
		m_pStaticSlotBlock->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3293 ) );
	else
		m_pStaticSlotBlock->SetText( L"" );

	CEtUIStatic* pStatic = GetControl<CEtUIStatic>( "ID_TEXT" );
	if( pStatic )
		pStatic->Show( !bShow );

	pStatic = GetControl<CEtUIStatic>( "ID_STATIC_BOARD" );
	if( pStatic )
		pStatic->Show( !bShow );

	CEtUIDialog::Show( bShow );
}
