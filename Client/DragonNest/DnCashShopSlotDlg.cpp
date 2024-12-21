#include "StdAfx.h"
#include "DnCashShopSlotDlg.h"

#ifdef _CASHSHOP_UI

CDnCashShopSlotDlg::CDnCashShopSlotDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
{

}

CDnCashShopSlotDlg::~CDnCashShopSlotDlg(void)
{

}

void CDnCashShopSlotDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSCommodity.ui" ).c_str(), bShow );
}

void CDnCashShopSlotDlg::InitialUpdate()
{
	
}

void CDnCashShopSlotDlg::InitCustomControl( CEtUIControl *pControl )
{

}

#endif // _CASHSHOP_UI