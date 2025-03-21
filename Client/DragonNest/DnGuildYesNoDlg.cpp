#include "StdAfx.h"
#include "DnGuildYesNoDlg.h"
#include "DnInterface.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildYesNoDlg::CDnGuildYesNoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pTitle( NULL )
, m_pMessage( NULL )
, m_eType( GuildYesNo_None )
{
}

CDnGuildYesNoDlg::~CDnGuildYesNoDlg(void)
{
}

void CDnGuildYesNoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildYesNoDlg.ui" ).c_str(), bShow );
}

void CDnGuildYesNoDlg::InitialUpdate()
{
	m_pTitle = GetControl<CEtUIStatic>("ID_TITLE");
	m_pMessage = GetControl<CEtUIStatic>("ID_NAME");
}

void CDnGuildYesNoDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		GetInterface().CloseBlind();
		if( m_emDialogType != UI_TYPE_MODAL )
			GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildYesNoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			switch( m_eType )
			{
			case GuildYesNo_Dismiss:	GetGuildTask().RequestDismissGuild();	break;
			case GuildYesNo_Leave:		GetGuildTask().RequestLeaveGuild();		break;
			case GuildYesNo_GuildWarEnroll:	GetGuildTask().RequestEnrollGuildWar();		break;
			}
			Show(false);
			return;
		}

		if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildYesNoDlg::SetGuildYesNoType( eGuildYesNoType eType )
{
	m_eType = eType;

	switch( eType )
	{
	case GuildYesNo_Dismiss:
		m_pTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3776 ) );
		m_pMessage->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3786 ) );
		break;
	case GuildYesNo_Leave:
		m_pTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1508 ) );
		m_pMessage->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3799 ) );
		break;
	case GuildYesNo_GuildWarEnroll:
		m_pTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126026 ) );
		m_pMessage->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126023 ) );
		break;
	}
}