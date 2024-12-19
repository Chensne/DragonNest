#include "StdAfx.h"
#include "DnCashShopGiftAnswerDlg.h"
#include "strsafe.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopGiftAnswerDlg::CDnCashShopGiftAnswerDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
				:CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pTitle = NULL;
	m_pTextBox = NULL;
	m_pCancelBtn = NULL;
}

void CDnCashShopGiftAnswerDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSPresentANS.ui").c_str(), bShow);
}

void CDnCashShopGiftAnswerDlg::InitialUpdate()
{
	m_pTitle = GetControl<CEtUIStatic>("ID_STATIC_REACT");
	m_pTextBox = GetControl<CEtUITextBox>("ID_TEXTBOX_MEMO");
	m_pCancelBtn = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
}

void CDnCashShopGiftAnswerDlg::SetInfo(const WCHAR* pSenderName, int comboBoxIndex, const WCHAR* pMsg)
{
	int index = -1;
	if (comboBoxIndex == 0)			index = 4676;
	else if (comboBoxIndex == 1)		index = 4677;
	else if (comboBoxIndex == 2)		index = 4678;

	std::wstring str;
	str = FormatW(L"%s %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4675), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, index));
	m_pTitle->SetText(str.c_str());

	m_pTextBox->SetText(pMsg);
}

void CDnCashShopGiftAnswerDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_CLOSE"))
		{
			Show(false);
			return;
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}