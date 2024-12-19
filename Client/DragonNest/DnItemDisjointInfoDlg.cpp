#include "StdAfx.h"
#include "DnItemDisjointInfoDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnCashShopPayDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnItemDisjointInfoDlg::CDnItemDisjointInfoDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
	, m_pItemSlotBG(NULL)
	, m_pNameStatic(NULL)
	, m_pSlotBtn(NULL)
	, m_pBase(NULL)
	, m_pCountStatic(NULL)
	, m_pItem(NULL)
	, m_Index(0)
{
}

CDnItemDisjointInfoDlg::~CDnItemDisjointInfoDlg(void)
{
	SAFE_DELETE(m_pItem);
}

void CDnItemDisjointInfoDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ItemDisjointListDlg.ui").c_str(), bShow);
}

void CDnItemDisjointInfoDlg::InitialUpdate()
{
	m_pBase = GetControl<CEtUIStatic>("ID_STATIC0");
	m_pNameStatic = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pCountStatic = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
}

void CDnItemDisjointInfoDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_BT_ITEM"))
		return;

	m_pSlotBtn = static_cast<CDnItemSlotButton*>(pControl);
	m_pSlotBtn->SetSlotType(ST_INVENTORY);
	m_pSlotBtn->SetSlotIndex(0);
}


void CDnItemDisjointInfoDlg::SetInfo(int nItemID)
{
	int nCount = 0;
	SAFE_DELETE(m_pItem);
	TItemInfo itemInfo;
	if (CDnItem::MakeItemInfo(nItemID, nCount, itemInfo))
	{
		m_pItem = GetItemTask().CreateItem(itemInfo);
		if (m_pItem)
		{
			m_pSlotBtn->SetItem(m_pItem, m_pItem->GetOverlapCount());
			m_pNameStatic->SetText(m_pItem->GetName());
		}
	}
}

void CDnItemDisjointInfoDlg::SetInfo(int nItemID, int nCount)
{
	SAFE_DELETE(m_pItem);
	TItemInfo itemInfo;
	if (CDnItem::MakeItemInfo(nItemID, nCount, itemInfo))
	{
		m_pItem = GetItemTask().CreateItem(itemInfo);
		if (m_pItem)
		{
			m_pSlotBtn->SetItem(m_pItem, m_pItem->GetOverlapCount());
			m_pNameStatic->SetText(m_pItem->GetName());
		}
	}
}

void CDnItemDisjointInfoDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());


	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}