#include "StdAfx.h"
#include "DnGuildMarkInfoDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMarkInfoDlg::CDnGuildMarkInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnGuildMarkInfoDlg::~CDnGuildMarkInfoDlg(void)
{
}

void CDnGuildMarkInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildMarkInfoDlg.ui" ).c_str(), bShow );
}

void CDnGuildMarkInfoDlg::InitialUpdate()
{
	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_BT_OK") );
}

void CDnGuildMarkInfoDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		m_SmartMove.MoveCursor();
	else
		m_SmartMove.ReturnCursor();

	CEtUIDialog::Show( bShow );
}

void CDnGuildMarkInfoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		Show(false);
		return;
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}