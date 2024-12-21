#include "StdAfx.h"
#include "DnPetNamingDlg.h"
#include "DNCountryUnicodeSet.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnPetTask.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPetNamingDlg::CDnPetNamingDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pEditBoxPetName( NULL )
, m_pButtonOK( NULL )
, m_ChangeItemSerial(0)
, m_iPetSerial( 0 )
, m_bSetGestureQuickSlot( false )
{
}

CDnPetNamingDlg::~CDnPetNamingDlg()
{
}

void CDnPetNamingDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pEditBoxPetName = GetControl<CEtUIIMEEditBox>( "ID_IMEEDITBOX_NAME" );
	m_pButtonOK = GetControl<CEtUIButton>( "ID_OK" );
}

void CDnPetNamingDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenPetNameDlg.ui" ).c_str(), bShow );
}

void CDnPetNamingDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonOK->Enable( false );
		m_pEditBoxPetName->ClearText();
		RequestFocus( m_pEditBoxPetName );
	}
	else
	{
	}

	CEtUIDialog::Show( bShow );
}

void CDnPetNamingDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			std::wstring szName = m_pEditBoxPetName->GetText();

			if( !g_CountryUnicodeSet.Check( szName.c_str() ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0 );
				return;
			}

			if( DN_INTERFACE::UTIL::CheckAccount( szName ) || DN_INTERFACE::UTIL::CheckChat( szName ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0 );
				return;
			}

			GetPetTask().RequestPetNaming(m_ChangeItemSerial, m_iPetSerial, szName.c_str());

			m_pButtonOK->Enable( false );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_CLOSE" ) || IsCmdControl( "ID_CANCEL" ) )
			Show( false );
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( m_bSetGestureQuickSlot )	// GestureQuickSlot에서 콜된 경우 최초 단축키에 의한 입력은 무시한다.
		{
			m_pEditBoxPetName->ClearText();
			m_bSetGestureQuickSlot = false;
			return;
		}

		if( IsCmdControl( "ID_IMEEDITBOX_NAME" ) )
		{
			bool bCreatable = true;
			std::wstring szName = m_pEditBoxPetName->GetText();
			szName = boost::algorithm::trim_copy( szName );
			if( szName.empty() )
				bCreatable = false;

			if( m_pEditBoxPetName->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 && bCreatable )
				m_pButtonOK->Enable( true );
			else
				m_pButtonOK->Enable( false );
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		focus::ReleaseControl();
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

