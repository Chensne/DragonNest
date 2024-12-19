#include "StdAfx.h"
#include "DnWorldServerStorageDrawConfirmDlg.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnWorldServerStorageDrawConfirmDlg::CDnWorldServerStorageDrawConfirmDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnWorldServerStorageChargeConfirmDlg(dialogType, pParentDialog, nID, pCallback)
{

}

void CDnWorldServerStorageDrawConfirmDlg::InitialUpdate()
{
	m_pItemName = GetControl<CEtUIStatic>("ID_TEXT_NAME"); 

	m_pOKButton = GetControl<CEtUIButton>("ID_OK");
	m_pCancelButton = GetControl<CEtUIButton>("ID_CANCEL");

	CEtUIStatic* pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	pTempStatic->Show(false);
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_NOTICE");
	pTempStatic->Show(false);

	m_SmartMove.SetControl(m_pCancelButton);
}

//////////////////////////////////////////////////////////////////////////

CDnWorldServerStorageDrawConfirmExDlg::CDnWorldServerStorageDrawConfirmExDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnWorldServerStorageChargeConfirmExDlg(dialogType, pParentDialog, nID, pCallback)
{
}


void CDnWorldServerStorageDrawConfirmExDlg::InitialUpdate()
{
	CDnWorldServerStorageChargeConfirmDlg::InitialUpdate();

	m_pEditBoxItemCount = GetControl<CEtUIEditBox>("ID_EDITBOX_COUNT");
	m_pItemUp = GetControl<CEtUIButton>("ID_ITEM_UP");
	m_pItemDown = GetControl<CEtUIButton>("ID_ITEM_DOWN");

	m_pButtonMin = GetControl<CEtUIButton>("ID_BT_MINI");
	m_pButtonMax = GetControl<CEtUIButton>("ID_BT_MAX");

	CEtUIStatic* pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	pTempStatic->Show(false);
	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_NOTICE");
	pTempStatic->Show(false);

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK2");
	pTempStatic->Show(true);
}