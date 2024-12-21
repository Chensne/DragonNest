#include "StdAfx.h"
#include "DnCashShopGiftReplyDlg.h"
#include "strsafe.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopGiftReplyDlg::CDnCashShopGiftReplyDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
, m_pTitle( NULL )
, m_pImpressionComboBox( NULL )
, m_pReplyEditBox( NULL )
, m_pCancelBtn( NULL )
, m_pOkBtn( NULL )
{
}

void CDnCashShopGiftReplyDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSPresentRE.ui").c_str(), bShow);
}

void CDnCashShopGiftReplyDlg::InitialUpdate()
{
	m_pTitle = GetControl<CEtUIStatic>("ID_STATIC2");
	m_pImpressionComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_OPINION");
	m_pReplyEditBox = GetControl<CEtUILineIMEEditBox>("ID_LINEEDITBOX_REPLY");
#ifdef PRE_REMOVE_CASHSHOP_GIFT_CADGE_MSGMAX
#else
	m_pReplyEditBox->SetMaxEditLength(GIFTMESSAGEMAX);
#endif

	m_pOkBtn = GetControl<CEtUIButton>("ID_BUTTON_SEND");
	m_pCancelBtn = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

	AddComboItem();
}

void CDnCashShopGiftReplyDlg::AddComboItem()
{
	m_pImpressionComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4676), NULL, 0);
	m_pImpressionComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4677), NULL, 1);
	m_pImpressionComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4678), NULL, 2);

	m_pImpressionComboBox->SetSelectedByIndex(0);
}

void CDnCashShopGiftReplyDlg::SetInfo(const WCHAR* pSenderName)
{
	std::wstring str;
	str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4629), pSenderName);
	m_pTitle->SetText(str.c_str());
}

void CDnCashShopGiftReplyDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_CLOSE"))
		{
			Show(false);
			return;
		}

		if (IsCmdControl("ID_BUTTON_SEND"))
		{
			//CSCashShopGiftReply reply;
			//reply.cComboBoxIndex = m_pImpressionComboBox->GetSelectedIndex();
			//StringCchCopyW(reply.wszReplyMessage, GIFTREPLYMESSAGEMAX, m_pReplyEditBox->GetText());

			//GetCashShopTask().RequestCashShopRecvGiftImpression(reply);
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}