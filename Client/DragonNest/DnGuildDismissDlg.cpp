#include "StdAfx.h"
#include "DnGuildDismissDlg.h"
#include "DnInterface.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildDismissDlg::CDnGuildDismissDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pButtonYes( NULL )
{
}

CDnGuildDismissDlg::~CDnGuildDismissDlg(void)
{
}

void CDnGuildDismissDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildBreakDlg.ui" ).c_str(), bShow );
}

void CDnGuildDismissDlg::InitialUpdate()
{
}

void CDnGuildDismissDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildDismissDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			GetGuildTask().RequestDismissGuild();
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
