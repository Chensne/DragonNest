#include "StdAfx.h"
#include "DnGuildIntroduceDlg.h"
#include "DnGuildTask.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildIntroduceDlg::CDnGuildIntroduceDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pEditBoxIntroduce(NULL)
, m_pButtonOK(NULL)
{
}

CDnGuildIntroduceDlg::~CDnGuildIntroduceDlg(void)
{
}

void CDnGuildIntroduceDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildAuthDlg.ui" ).c_str(), bShow );
}

void CDnGuildIntroduceDlg::InitialUpdate()
{
	m_pEditBoxIntroduce = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_MESSAGE");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
}

void CDnGuildIntroduceDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			std::wstring szStr = m_pEditBoxIntroduce->GetText();
			if( DN_INTERFACE::UTIL::CheckChat( szStr ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			TGuildMember *pMyGuildMemberInfo = GetGuildTask().GetMyGuildMemberInfo();
			if( pMyGuildMemberInfo && wcscmp( m_pEditBoxIntroduce->GetText(), pMyGuildMemberInfo->wszGuildMemberIntroduce ) != 0 )
				GetGuildTask().RequestChangeGuildMemberInfo( GUILDMEMBUPDATE_TYPE_INTRODUCE, 0, 0, 0, m_pEditBoxIntroduce->GetText(), pMyGuildMemberInfo->nAccountDBID, pMyGuildMemberInfo->nCharacterDBID );
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
		if( IsCmdControl("ID_IMEEDITBOX_MESSAGE") )
		{
			if( m_pEditBoxIntroduce->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 )
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

void CDnGuildIntroduceDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonOK->Enable( false );
		m_pEditBoxIntroduce->ClearText();
		RequestFocus( m_pEditBoxIntroduce );
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildIntroduceDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			RequestFocus( m_pEditBoxIntroduce );
		}
	}
}