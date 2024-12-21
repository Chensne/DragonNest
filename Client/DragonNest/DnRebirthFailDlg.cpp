#include "StdAfx.h"
#include "DnRebirthFailDlg.h"
#include "GameSendPacket.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRebirthFailDlg::CDnRebirthFailDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnRebirthFailDlg::~CDnRebirthFailDlg(void)
{
}

void CDnRebirthFailDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RebirthFailDlg.ui" ).c_str(), bShow );
}

void CDnRebirthFailDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BTN_OUT" ) )
		{
			Show( false );
			SendRequestDungeonFailed();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
