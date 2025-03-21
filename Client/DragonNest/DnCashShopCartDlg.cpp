#include "StdAfx.h"
#include "DnInvenSlotDlg.h"
#include "DnCashShopCartDlg.h"
#include "DnCashShopCartItemDlg.h"
#include "DnCashShopTask.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopCartDlg::CDnCashShopCartDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	//m_pInvenSlotDlg = NULL;

	m_pPrevBtn = NULL;
	m_pNextBtn = NULL;
	m_pPageNum = NULL;
	m_pEmptyCart = NULL;
	m_pBuyBtn = NULL;
	m_pPresentBtn = NULL;

	m_CurrentPage = 1;
	m_MaxPage = 0;
}

CDnCashShopCartDlg::~CDnCashShopCartDlg(void)
{
	Clear();
}

void CDnCashShopCartDlg::Initialize(bool bShow)
{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_Cart.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CSCart.ui").c_str(), bShow);
#endif // PRE_ADD_CASHSHOP_RENEWAL
}

void CDnCashShopCartDlg::Clear()
{
	std::list<SCartDlgUnit>::iterator iter = m_pItems.begin();
	for (; iter != m_pItems.end(); ++iter)
	{
		SCartDlgUnit& unit = *iter;
		SAFE_DELETE(unit.pItem);
	}
	m_pItems.clear();
}


void CDnCashShopCartDlg::EmptySlots()
{
	std::vector<CDnQuickSlotButton*>::iterator iter = m_pSlotBtns.begin();
	for (; iter != m_pSlotBtns.end(); ++iter)
	{
		CDnQuickSlotButton* pBtn = *iter;
		if (pBtn)
			pBtn->ResetSlot();
	}
}

void CDnCashShopCartDlg::InitialUpdate()
{
	m_pPrevBtn = GetControl<CEtUIButton>("ID_BUTTON_PGUP");
	m_pNextBtn = GetControl<CEtUIButton>("ID_BUTTON_PGDN");
	m_pPageNum = GetControl<CEtUIStatic>("ID_STATIC_PAGE");
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_STATIC10");
	if( m_pPrevBtn ) m_pPrevBtn->Show(false);
	if( m_pNextBtn ) m_pNextBtn->Show(false);
	if( m_pPageNum ) m_pPageNum->Show(false);
	if( pStatic ) pStatic->Show( false );

	m_pEmptyCart = GetControl<CEtUIButton>("ID_BUTTON_ALLDELETE");

	m_pBuyBtn		= GetControl<CEtUIButton>("ID_BUTTON_ALLBUY");
	m_pPresentBtn	= GetControl<CEtUIButton>("ID_BUTTON_ALLPRESENT");
}

void CDnCashShopCartDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_BUTTON_ITEM"))
		return;

	CDnQuickSlotButton* pQuickSlotBtn(NULL);
	pQuickSlotBtn = static_cast<CDnQuickSlotButton*>(pControl);
	pQuickSlotBtn->SetSlotType(ST_INVENTORY_CASHSHOP);
	pQuickSlotBtn->SetSlotIndex((int)m_pSlotBtns.size());

	m_pSlotBtns.push_back(pQuickSlotBtn);
}

void CDnCashShopCartDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if (bShow)
	{
		Update();
	}

	CEtUIDialog::Show(bShow);
}

void CDnCashShopCartDlg::Update()
{
	Clear();

	MakeItemList();
}

void CDnCashShopCartDlg::MakeItemList()
{
	const CART_ITEM_LIST& cartList = GetCashShopTask().GetCartList();

	int i = 0;
	CART_ITEM_LIST::const_iterator iter = cartList.begin();
	for (; iter != cartList.end(); ++iter, ++i)
	{
		const SCashShopCartItemInfo& itemInfo = *iter;
		AddItem(itemInfo.presentSN, i);
	}
}

void CDnCashShopCartDlg::SetCurrentPageToMaxPage()
{
	m_MaxPage = (int(m_pItems.size()) / _MAX_INVEN_SLOT_PER_LINE);
	if ((int(m_pItems.size()) % _MAX_INVEN_SLOT_PER_LINE) != 0)
		m_MaxPage++;
	m_CurrentPage = m_MaxPage;
}

void CDnCashShopCartDlg::AddItem(CASHITEM_SN sn, int cartItemId)
{
	const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo(sn);
	if (pItemInfo != NULL && pItemInfo->presentItemId != ITEMCLSID_NONE)
	{
		TItemInfo itemInfo;
		if( CDnItem::MakeItemInfo( pItemInfo->presentItemId, pItemInfo->count, itemInfo ) == false )
			return;

		SCartDlgUnit unit;
		unit.id = cartItemId;
		unit.pItem = GetItemTask().CreateItem(itemInfo);
		unit.pItem->SetCashItemSN(sn);
		m_pItems.push_back(unit);

		SetCurrentPageToMaxPage();
	}
}

void CDnCashShopCartDlg::RemoveItem(int cartItemId)
{
	std::list<SCartDlgUnit>::iterator iter = m_pItems.begin();
	for (; iter != m_pItems.end(); ++iter)
	{
		SCartDlgUnit& unit = (*iter);
		if (unit.pItem && unit.id == cartItemId)
		{
			m_pItems.erase(iter);
			break;
		}
	}
}

void CDnCashShopCartDlg::UpdateItem(eRetCartAction action, const SCartActionParam& param)
{
	if (action == eRETCART_ADD)
		AddItem(param.presentSN, param.cartItemId);
	else if (action == eRETCART_REMOVE)
		RemoveItem(param.cartItemId);
	else if (action == eRETCART_CLEAR)
		Clear();
	else
		_ASSERT(0);

	RefreshItems();
}

void CDnCashShopCartDlg::RefreshItems()
{
	m_MaxPage = (int(m_pItems.size()) / _MAX_INVEN_SLOT_PER_LINE);
	if ((int(m_pItems.size()) % _MAX_INVEN_SLOT_PER_LINE) != 0)
		m_MaxPage++;

	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	int startIndex = (m_CurrentPage - 1) * _MAX_INVEN_SLOT_PER_LINE;

	std::wstring page;
	page = FormatW(L"%d/%d", m_CurrentPage, m_MaxPage);
	m_pPageNum->SetText(page);

	// exception handling
	if (startIndex > (int)m_pItems.size())
		startIndex = 0;

	EmptySlots();

	int listCount = 0;
	int slotIdx = 0;
	std::list<SCartDlgUnit>::iterator iter = m_pItems.begin();
	for (; listCount < startIndex + _MAX_INVEN_SLOT_PER_LINE && iter != m_pItems.end(); ++listCount, ++iter)
	{
		if (listCount >= startIndex)
		{
			SCartDlgUnit& unit = *iter;
			CDnItem* pItem = unit.pItem;
			if (pItem != NULL)
			{
				m_pSlotBtns[slotIdx]->SetQuickItem(pItem);
				slotIdx++;
			}
		}
	}
}

void CDnCashShopCartDlg::PrevPage()
{
	if (m_pItems.empty() || m_MaxPage <= 1)
		return;

	m_CurrentPage--;
	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	RefreshItems();
}

void CDnCashShopCartDlg::NextPage()
{
	if (m_pItems.empty() || m_MaxPage <= 1)
		return;

	m_CurrentPage++;
	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	RefreshItems();
}

void CDnCashShopCartDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (IsCmdControl("ID_BUTTON_PGUP"))
		{
			PrevPage();
			return;
		}

		if (IsCmdControl("ID_BUTTON_PGDN"))
		{
			NextPage();
			return;
		}

		if (IsCmdControl("ID_BUTTON_ALLBUY"))
		{
			if (GetCashShopTask().GetCartItemCount() > 0)
				GetCashShopTask().BuyCart();
			return;
		}

		if (IsCmdControl("ID_BUTTON_ALLPRESENT"))
		{
			if (GetCashShopTask().GetCartItemCount() > 0)
				GetCashShopTask().GiftCart(true);
			return;
		}

		if (IsCmdControl("ID_BUTTON_ALLDELETE"))
		{
			CDnCashShopTask::GetInstance().ClearCartItems(false);
			return;
		}

		const std::string& cmdName = GetCmdControlName();
		std::string::size_type numberPos = cmdName.find("ID_BUTTON_ITEM");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_ITEM");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONUP)
				{
					int page = m_CurrentPage - 1;
					CommonUtil::ClipNumber(page, 0, page);
					int cartListIndex = page * _MAX_INVEN_SLOT_PER_LINE + slotIdx;
					if (cartListIndex >= int(m_pItems.size()))
					{
						_ASSERT(0);
						return;
					}

					int i = 0;
					std::list<SCartDlgUnit>::iterator iter = m_pItems.begin();
					for (; iter != m_pItems.end(); ++iter, ++i)
					{
						if (cartListIndex == i)
						{
							SCartDlgUnit& unit = *iter;
							if (uMsg == WM_RBUTTONUP)
								GetCashShopTask().RemoveCartItem(unit.id);
							else if (uMsg == WM_LBUTTONDOWN)
							{
								if (unit.pItem->GetItemType() == ITEMTYPE_WEAPON ||
									unit.pItem->GetItemType() == ITEMTYPE_PARTS ||
									unit.pItem->GetItemType() == ITEMTYPE_HAIRDYE)
								{
									GetCashShopTask().AttachPartsToPreview(unit.pItem->GetCashItemSN(), unit.pItem->GetClassID());
								}
							}
							return;
						}
					}
				}
			}
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopCartDlg::Process(float fElapsedTime)
{
	if (m_bShow == false || GetCashShopTask().IsActive() == false)
	{
		return;
	}

	bool bEnable = (GetCashShopTask().GetCartItemCount() > 0);
	m_pBuyBtn->Enable(bEnable);
#if defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
	m_pPresentBtn->Enable(false);
#else
	m_pPresentBtn->Enable(bEnable);
#endif

	m_pEmptyCart->Enable(m_pItems.empty() == false);

	CEtUIDialog::Process(fElapsedTime);
}