#include "StdAfx.h"
#include "DnTHOTPDlg.h"
#include "LoginSendPacket.h"
#include "DnInterface.h"
#include "DnLoginDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(_TH) && defined(_AUTH)

CDnTHOTPDlg::CDnTHOTPDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pEditBoxOTPCode( NULL )
, m_pButtonOK( NULL )
, m_pButtonCancel( NULL )
{
}

CDnTHOTPDlg::~CDnTHOTPDlg(void)
{
}

void CDnTHOTPDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LoginOTP.ui" ).c_str(), bShow );
}

void CDnTHOTPDlg::InitialUpdate()
{
	m_pEditBoxOTPCode = GetControl<CEtUIEditBox>( "ID_EDITBOX_OTP" );
	m_pButtonOK = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pButtonCancel = GetControl<CEtUIButton>( "ID_BT_CANCEL" );
}

void CDnTHOTPDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_OK" ) )
		{
			if( m_pEditBoxOTPCode->GetTextLength() == 0 ) 
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3378 ) );
				return;
			}

			SendAsiaSoftOTP( m_pEditBoxOTPCode->GetText() );

			m_pButtonOK->Enable( false );
			m_pButtonCancel->Enable( false );
			return;
		}
		else if( IsCmdControl( "ID_BT_CANCEL" ) )
		{
			if( GetInterface().GetLoginDlg() )
				GetInterface().GetLoginDlg()->Show( true );

			Show( false );
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnTHOTPDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		RequestFocus( m_pEditBoxOTPCode );
		m_pButtonOK->Enable( true );
		m_pButtonCancel->Enable( true );
	}
	else
	{
		m_pEditBoxOTPCode->ClearTextMemory();
	}
}

#endif	// _TH && _AUTH