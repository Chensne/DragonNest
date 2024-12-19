#include "StdAfx.h"

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
#include "DnPotentialCheckCancelDlg.h"
#include "DnMoneyInputDlg.h"
#include "DnInterfaceString.h"
#include "ItemSendPacket.h"
#include "DnPotentialJewelDlg.h"

CDnPotentialCheckCancelDlg::CDnPotentialCheckCancelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true ) 
, m_pInfoText( NULL )
{
}

CDnPotentialCheckCancelDlg::~CDnPotentialCheckCancelDlg()
{
}

void CDnPotentialCheckCancelDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPowerMessageBoxDlg.ui" ).c_str(), bShow );
}

void CDnPotentialCheckCancelDlg::InitialUpdate()
{
	m_pInfoText = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	m_pInfoText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9179)); // 잠재력을 보호하기 위해선 수수료가 필요합니다. 기존 잠재력을 보호 하시겠습니까?
}

void CDnPotentialCheckCancelDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show(bShow);
}

bool CDnPotentialCheckCancelDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;
		
	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			if(wParam == VK_RETURN)
			{
				//return true;
			}
			else if(wParam == VK_ESCAPE)
			{
				//return true;
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPotentialCheckCancelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_OK") )
		{
			CDnPotentialJewelDlg* pPotentialJewlDlg = dynamic_cast<CDnPotentialJewelDlg*>(GetParentDialog());
			if(!pPotentialJewlDlg) return;
			if(!pPotentialJewlDlg->GetCurrentItem() || !pPotentialJewlDlg->GetPotentialItem()) return;

			int				nSlotIndex		 = pPotentialJewlDlg->GetCurrentItem()->GetSlotIndex();
			INT64			nSlotSerialID	 = pPotentialJewlDlg->GetCurrentItem()->GetSerialID();
			INT64			nPotentialItemID = pPotentialJewlDlg->GetPotentialItemID();
			SendRollBackPotentialJewel(nSlotIndex, nSlotSerialID, nPotentialItemID);
			
			//Show(false);
		}
		else if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			//ShowChildDialog( this, false );
			//Show(false);
			//focus::UnsetFocus();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPotentialCheckCancelDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnPotentialCheckCancelDlg::SetMoney( int nOnePrice )
{
	INT64 nMoney = 0;
	std::wstring strString;
	wchar_t wszBuffer[16]={0,};

	CDnMoneyControl::GetStrMoneyG( nOnePrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_GOLD )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyS( nOnePrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_SILVER )->SetText( wszBuffer );	
	CDnMoneyControl::GetStrMoneyC( nOnePrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_BRONZE )->SetText( wszBuffer );	
}
#endif //PRE_MOD_POTENTIAL_JEWEL_RENEWAL