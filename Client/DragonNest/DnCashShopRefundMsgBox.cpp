#include "StdAfx.h"

#ifdef PRE_ADD_CASHSHOP_REFUND_CL

#include "DnCashShopRefundMsgBox.h"
#include "DnCashShopTask.h"
#include "DnSlotButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCashShopRefundMsgBox::CDnCashShopRefundMsgBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pOKBtn(NULL)
	, m_pCancelBtn(NULL)
	, m_pItemNameStatic(NULL)
	, m_pRefundCashStatic(NULL)
	, m_pSlotBtn(NULL)
{
}

CDnCashShopRefundMsgBox::~CDnCashShopRefundMsgBox(void)
{
}

void CDnCashShopRefundMsgBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSRefundResult_refund.ui").c_str(), bShow);
}

void CDnCashShopRefundMsgBox::InitialUpdate()
{
	m_pOKBtn		= GetControl<CEtUIButton>("ID_BUTTON_OK");
	m_pCancelBtn	= GetControl<CEtUIButton>("ID_BUTTON_CANCEL");

	m_pItemNameStatic = GetControl<CEtUIStatic>("UI_STATIC_ITEMNAME");
	m_pRefundCashStatic = GetControl<CEtUIStatic>("UI_STATIC_REFUNDCASH");
}

void CDnCashShopRefundMsgBox::SetMsgBoxText(CDnSlotButton* pSlotBtn, const std::wstring& itemName, int refundCash)
{
	std::wstring cashStr = FormatW(L"%d", refundCash);
	std::wstring countStr = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4625), pSlotBtn->GetSlotOriginalCount());
	std::wstring nameStr = FormatW(L"%s / %s", itemName.c_str(), countStr.c_str());
	m_pItemNameStatic->SetText(nameStr.c_str());
	m_pRefundCashStatic->SetText(cashStr.c_str());

	m_pSlotBtn = pSlotBtn;
}

void CDnCashShopRefundMsgBox::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (bShow == false)
	{
		if (m_pSlotBtn && m_pSlotBtn->IsRegist())
		{
			m_pSlotBtn->SetRegist(false);
			m_pSlotBtn->DisableSplitMode(true);
			drag::ReleaseControl();
		}
		m_pSlotBtn = NULL;
	}

	CEtUIDialog::Show(bShow);
}

void CDnCashShopRefundMsgBox::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (IsCmdControl("ID_BUTTON_OK"))
		{
			if (m_pSlotBtn)
			{
				const CDnItem* pItem = static_cast<CDnItem*>(m_pSlotBtn->GetItem());
				CDnCashShopTask::GetInstance().RequestCashShopRefund(pItem->GetSerialID(), CDnCashShopTask::GetInstance().IsPackageItem(pItem->GetCashItemSN()));
				m_pSlotBtn->SetRegist(false);
			}
			else
				_ASSERT(0);

			m_pSlotBtn = NULL;
		}

		Show(false);
		return;
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#endif // PRE_ADD_CASHSHOP_REFUND_CL