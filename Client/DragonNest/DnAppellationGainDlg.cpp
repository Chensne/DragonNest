#include "StdAfx.h"
#include "DnAppellationGainDlg.h"
#include "DnItem.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"


CDnAppellationGainDlg::CDnAppellationGainDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pStaticText = NULL;
	m_pItem = NULL;
}

CDnAppellationGainDlg::~CDnAppellationGainDlg()
{
}

void CDnAppellationGainDlg::SetAppellationGainItem( CDnItem *pItem )
{
	m_pItem = pItem;
	WCHAR wszStr[256];

	swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8053 ), ( m_pItem ) ? m_pItem->GetName() : L"" );
	m_pStaticText->SetText( wszStr );
	
}

void CDnAppellationGainDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSMessageBoxAppellation.ui" ).c_str(), bShow );
}

void CDnAppellationGainDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_MESSAGE");

#ifdef PRE_MOD_LEVELUP_REWARD_SMARTMOVE_APP	
	if( GetControl<CEtUIButton>("ID_YES") )
		m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_YES") );
	
#endif
}

void CDnAppellationGainDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;
	

	CEtUIDialog::Show( bShow );
	
#ifdef PRE_MOD_LEVELUP_REWARD_SMARTMOVE_APP
	if( bShow )
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}
#endif 

}

void CDnAppellationGainDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_YES") )
		{
			if( !m_pItem ) {
				Show( false );
				return;
			}
			char cType = 0;
			cType = ( m_pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
			CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
			if( pPlayerActor ) pPlayerActor->UseItemFromSlotIndex( m_pItem->GetSlotIndex(), cType );

			Show( false );
			return;
		}
		else if( IsCmdControl("ID_NO") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show( false );
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

}
