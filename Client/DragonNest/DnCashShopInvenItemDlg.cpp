#include "StdAfx.h"
#include "DnCashShopInvenItemDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"

#ifdef _CASHSHOP_UI

CDnCashShopInvenItemDlg::CDnCashShopInvenItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_Index = 0;
}

CDnCashShopInvenItemDlg::~CDnCashShopInvenItemDlg(void)
{
}

void CDnCashShopInvenItemDlg::Initialize(bool bShow)
{
	m_SlotBtnList.clear();
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSCInventory_Bar.ui").c_str(), bShow);
}

void CDnCashShopInvenItemDlg::InitCustomControl(CEtUIControl* pControl)
{
	CDnItemSlotButton*	pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
	m_SlotBtnList.push_back(pItemSlotButton);
}

void CDnCashShopInvenItemDlg::SetSlotIndex(int itemDlgIdx)
{
	m_Index = itemDlgIdx;

	// cashshop temp
	TItemInfo info;
	TItem& item			= info.Item;

	item.nItemID		= 280004;
	item.nSerial		= 648799822575987218;
	item.nRandomSeed	= 1879640279;
	item.wDur			= 0;
	item.wCount			= 1;
	item.nCoolTime		= 0;
	item.dwColor		= 0;
	item.cLevel			= 0;
	item.cPotential		= 0;
	item.bSoulbound		= false;
	item.cOption		= 0;
	item.cSealCount		= 0;

	int i = 0;
	std::vector<CDnItemSlotButton*>::iterator iter = m_SlotBtnList.begin();
	for (; iter != m_SlotBtnList.end(); ++iter, ++i)
	{
		CDnItemSlotButton* pBtn = *iter;

		int slotIndex = (itemDlgIdx * _MAX_INVEN_SLOT_PER_LINE) + i;
		pBtn->SetSlotIndex(slotIndex);

		//	cashshop temp
		info.Item.nItemID += (slotIndex > 10) ? slotIndex % 10 : slotIndex;
		info.cSlotIndex = (BYTE)slotIndex;
		CDnItem* pTempItem = CDnItemTask::GetInstance().CreateItem(info);
		pBtn->SetItem(pTempItem, pTempItem->GetOverlapCount());
	}
}

#endif // _CASHSHOP_UI