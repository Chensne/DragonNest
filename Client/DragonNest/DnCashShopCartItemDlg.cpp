#include "StdAfx.h"
#include "DnCashShopCartItemDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopCartItemDlg::CDnCashShopCartItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_Index = 0;

	int i = 0;
	for (; i < _MAX_INVEN_SLOT_PER_LINE; ++i)
	{
		m_pItem[i] = NULL;
	}
}

CDnCashShopCartItemDlg::~CDnCashShopCartItemDlg(void)
{
	Clear();
}

void CDnCashShopCartItemDlg::Clear()
{
	int i = 0;
	for (; i < _MAX_INVEN_SLOT_PER_LINE; ++i)
	{
		SAFE_DELETE(m_pItem[i]);
	}
}

void CDnCashShopCartItemDlg::Initialize(bool bShow)
{
	m_SlotBtnList.clear();
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSCInventory_Bar.ui").c_str(), bShow);
}

void CDnCashShopCartItemDlg::InitCustomControl(CEtUIControl* pControl)
{
	CDnItemSlotButton*	pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
	m_SlotBtnList.push_back(pItemSlotButton);
}

void CDnCashShopCartItemDlg::SetInfo(int itemDlgIdx)
{
	m_Index = itemDlgIdx;

	Clear();

	const CART_ITEM_LIST& cartList = GetCashShopTask().GetCartList();
	int startIdx = itemDlgIdx * _MAX_INVEN_SLOT_PER_LINE;
	if ((int)cartList.size() < startIdx)
	{
		_ASSERT(0);
		return;
	}

	int i = 0;
	std::vector<CDnItemSlotButton*>::iterator iter = m_SlotBtnList.begin();
	for (; iter != m_SlotBtnList.end(); ++iter, ++i)
	{
		CDnItemSlotButton* pBtn = *iter;

		int curIdx = startIdx + i;
		if (curIdx >= (int)cartList.size())
			break;

		const SCashShopCartItemInfo* pCartItem = GetCashShopTask().GetCartListItemInfo(cartList, curIdx);
		if (pCartItem)
		{
			const CASHITEM_SN& sn = pCartItem->presentSN;
			const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo(sn);
			if (pItemInfo != NULL && pItemInfo->presentItemId != ITEMCLSID_NONE)
			{
				TItemInfo itemInfo;
				if( CDnItem::MakeItemInfo( pItemInfo->presentItemId, 1, itemInfo ) == false ) return;
				m_pItem[i] = GetItemTask().CreateItem(itemInfo);
				m_pItem[i]->SetCashItemSN(sn);

				if (m_pItem[i] != NULL)
					pBtn->SetItem(m_pItem[i], m_pItem[i]->GetOverlapCount());
			}
		}
	}
}