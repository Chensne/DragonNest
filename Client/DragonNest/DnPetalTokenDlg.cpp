#include "StdAfx.h"
#include "DnPetalTokenDlg.h"
#include "DnItem.h"
#include "DnPlayerActor.h"


CDnPetalTokenDlg::CDnPetalTokenDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pStaticText = NULL;
	m_pItem = NULL;
}

CDnPetalTokenDlg::~CDnPetalTokenDlg()
{
}

void CDnPetalTokenDlg::SetPetalTokenItem( CDnItem *pItem )
{
	m_pItem = pItem;
	WCHAR wszStr[256];
	int nPetal = 0;

	if( pItem ) {
		nPetal = pItem->GetTypeParam(0);
	}
	swprintf_s( wszStr, L"%d %s", nPetal, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4614 ) );
	m_pStaticText->SetText( wszStr );
}

void CDnPetalTokenDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSMessageBoxPetal.ui" ).c_str(), bShow );
}

void CDnPetalTokenDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_PETAL");
}

void CDnPetalTokenDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	CEtUIDialog::Show( bShow );
}

void CDnPetalTokenDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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
