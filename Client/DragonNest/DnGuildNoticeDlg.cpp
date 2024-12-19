#include "StdAfx.h"
#include "DnGuildNoticeDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildNoticeDlg::CDnGuildNoticeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pEditBoxNotice(NULL)
, m_pButtonOK(NULL)
{
}

CDnGuildNoticeDlg::~CDnGuildNoticeDlg(void)
{
}

void CDnGuildNoticeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildNoticeDlg.ui" ).c_str(), bShow );
}

void CDnGuildNoticeDlg::InitialUpdate()
{
	m_pEditBoxNotice = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NOTICE");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
}

void CDnGuildNoticeDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonOK->Enable( false );
		m_pEditBoxNotice->ClearText();
		RequestFocus( m_pEditBoxNotice );
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildNoticeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			std::wstring szStr = m_pEditBoxNotice->GetText();
			if( DN_INTERFACE::UTIL::CheckChat( szStr ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			if( wcscmp( m_pEditBoxNotice->GetText(), GetGuildTask().GetGuildInfo()->wszGuildNotice ) != 0 )
				GetGuildTask().RequestChangeGuildInfo( GUILDUPDATE_TYPE_NOTICE, 0, 0, 0, m_pEditBoxNotice->GetText() );
			Show(false);
			return;
		}

		if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl("ID_IMEEDITBOX_NOTICE") )
		{
			if( m_pEditBoxNotice->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 )
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

void CDnGuildNoticeDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			RequestFocus( m_pEditBoxNotice );
		}
	}
}