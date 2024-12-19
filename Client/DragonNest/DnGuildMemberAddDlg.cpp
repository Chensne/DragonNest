#include "StdAfx.h"
#include "DnGuildMemberAddDlg.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMemberAddDlg::CDnGuildMemberAddDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pEditBoxName(NULL)
, m_pButtonOK(NULL)
{
}

CDnGuildMemberAddDlg::~CDnGuildMemberAddDlg(void)
{
}

void CDnGuildMemberAddDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildMemberAddDlg.ui" ).c_str(), bShow );
}

void CDnGuildMemberAddDlg::InitialUpdate()
{
	m_pEditBoxName = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
}

void CDnGuildMemberAddDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			GetGuildTask().RequestInviteGuildMember( m_pEditBoxName->GetText() );
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
		if( IsCmdControl("ID_IMEEDITBOX_NAME") )
		{
			if( m_pEditBoxName->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 )
				m_pButtonOK->Enable( true );
			else
				m_pButtonOK->Enable( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildMemberAddDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonOK->Enable( false );
		m_pEditBoxName->ClearText();
		RequestFocus( m_pEditBoxName );
	}

	CEtUIDialog::Show( bShow );
}