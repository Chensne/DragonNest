#include "Stdafx.h"


#ifdef PRE_ADD_NEWCOMEBACK

#include "DnComeBackMsgDlg.h"



CDnComeBackMsgDlg::CDnComeBackMsgDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
:CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
{}

// Override - CEtUIDialog //
void CDnComeBackMsgDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("CharCreate_ReturnDlg.ui").c_str(), bShow );
}

void CDnComeBackMsgDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	// Button.
	if( nCommand == EVENT_BUTTON_CLICKED )
	{	
		// ´Ý±â.
		if( IsCmdControl("ID_BT_OK") )
			Show( false );
	}
	
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


#endif // PRE_ADD_NEWCOMEBACK
