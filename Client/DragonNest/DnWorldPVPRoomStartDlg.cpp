#include "StdAfx.h"
#include "DnWorldPVPRoomStartDlg.h"
#include "PvPSendPacket.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_WORLDCOMBINE_PVP

CDnWorldPVPRoomStartDlg::CDnWorldPVPRoomStartDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: m_uiPVPIndex( -1 )
{
}

CDnWorldPVPRoomStartDlg::~CDnWorldPVPRoomStartDlg()
{
}

void CDnWorldPVPRoomStartDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "WorldPVPRoomStart.ui" ).c_str(), bShow );
}

void CDnWorldPVPRoomStartDlg::InitialUpdate()
{
	CEtUIStatic* pStatic = GetControl<CEtUIStatic>( "ID_TEXT_MESSAGE" );
	if( pStatic )
		pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120218 ) );
}

void CDnWorldPVPRoomStartDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		m_uiPVPIndex = -1;
	}

	CEtUIDialog::Show( bShow );
}

void CDnWorldPVPRoomStartDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_BT_START" ) )
		{
			if( m_uiPVPIndex != -1 )
				SendWorldPVPRoomStart( m_uiPVPIndex );

			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#endif // PRE_WORLDCOMBINE_PVP