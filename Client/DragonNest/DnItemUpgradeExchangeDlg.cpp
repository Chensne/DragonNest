#include "StdAfx.h"
#include "DnItemUpgradeExchangeDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnTableDB.h"
#include "ItemSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_EXCHANGE_ENCHANT

CDnItemUpgradeExchangeDlg::CDnItemUpgradeExchangeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pTargetSlot = NULL;
	m_pSourceSlot = NULL;

	m_pStaticGold = NULL;
	m_pStaticSilver = NULL;
	m_pStaticBronze = NULL;
	m_pButtonOK = NULL;
	m_pButtonCancel = NULL;

	int i = 0;
	for (; i < USERITEM_SLOT_COUNT; ++i)
	{
		m_pInvenSlotCache[i] = NULL;
	}

	for (i = 0; i < STUFF_ITEMSLOT_MAX; ++i)
	{
		m_pStuffItemSlots[i] = NULL;
	}

	m_dwColorGold = 0;
	m_dwColorSilver = 0;
	m_dwColorBronze = 0;
}

CDnItemUpgradeExchangeDlg::~CDnItemUpgradeExchangeDlg(void)
{
	ClearAllSourceAndTargetSlot();
}

void CDnItemUpgradeExchangeDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ItemUpgradeChangeDlg.ui").c_str(), bShow);
}

void CDnItemUpgradeExchangeDlg::InitialUpdate()
{
	m_pTargetSlot = GetControl<CDnItemSlotButton>("ID_ITEM_UPGRADE1");
	m_pTargetSlot->SetSlotIndex(TARGET_SLOT_INDEX);
	m_pTargetSlot->SetSlotType(ST_ITEM_UPGRADE_EXCHANGE_TARGET);

	m_pSourceSlot = GetControl<CDnItemSlotButton>("ID_ITEM_UPGRADE0");
	m_pSourceSlot->SetSlotIndex(SOURCE_SLOT_INDEX);
	m_pSourceSlot->SetSlotType(ST_ITEM_UPGRADE_EXCHANGE_SOURCE);

	int i = 0;
	std::string ctrlName;
	for (; i < USERITEM_SLOT_COUNT; ++i)
	{
		ctrlName = FormatA("ID_ITEM%d", i);
		m_pInvenSlotCache[i] = GetControl<CDnItemSlotButton>(ctrlName.c_str());
	}

	for( int i = 0; i < STUFF_ITEMSLOT_MAX; ++i )
	{
		ctrlName = FormatA("ID_ITEM%d", i);
		m_pStuffItemSlots[i] = GetControl<CDnItemSlotButton>(ctrlName.c_str());
		m_pStuffItemSlots[i]->SetSlotType(ST_ITEM_UPGRADE_EXCHANGE_STUFF);
	}

	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");
	m_dwColorGold = m_pStaticGold->GetTextColor();
	m_dwColorSilver = m_pStaticSilver->GetTextColor();
	m_dwColorBronze = m_pStaticBronze->GetTextColor();

	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");
}

void CDnItemUpgradeExchangeDlg::Show(bool bShow)
{ 
	if (m_bShow == bShow)
		return;

	if (bShow == false)
	{
		ClearAllSourceAndTargetSlot();

		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if (pDragButton)
			{
				pDragButton->DisableSplitMode(false);
				drag::ReleaseControl();
			}
		}

		GetInterface().CloseBlind();
	}

	CEtUIDialog::Show(bShow);
}

void CDnItemUpgradeExchangeDlg::AddAttachItem(CDnSlotButton* pFromButton)
{
	if (pFromButton->IsEmptySlot())
		return;

	if (m_pSourceSlot->IsEmptySlot())
	{
		if (AddAttachItem(pFromButton, m_pSourceSlot) == false)
			drag::Command(UI_DRAG_CMD_CANCEL);
	}
	else if (m_pTargetSlot->IsEmptySlot())
	{
		if (AddAttachItem(pFromButton, m_pTargetSlot) == false)
			drag::Command(UI_DRAG_CMD_CANCEL);
	}

	drag::ReleaseControl();

	return;
}

bool CDnItemUpgradeExchangeDlg::CheckAttachItemWithOtherItemSlot(const CDnItem* pFromItem, ITEM_SLOT_TYPE type)
{
	CDnItemSlotButton* pCompareItemSlot = NULL;
	if (type == ST_ITEM_UPGRADE_EXCHANGE_SOURCE)
		pCompareItemSlot = m_pTargetSlot;
	else if (type == ST_ITEM_UPGRADE_EXCHANGE_TARGET)
		pCompareItemSlot = m_pSourceSlot;
	else
	{
		_ASSERT(0);
		return false;
	}

	if (pCompareItemSlot->IsEmptySlot() == false)
	{
		const MIInventoryItem* pCompareItem = pCompareItemSlot->GetItem();
		if (pCompareItem->GetType() == MIInventoryItem::Item)
		{
			ITEM_SLOT_TYPE existItemSlotType = pCompareItemSlot->GetSlotType();
			eERR_ITEM_UPGRADE_EXC err = IsSlotValidExchangeCondition(existItemSlotType, *pFromItem);
			if (err != eEIU_NONE)
			{
				HandleError(err);
				return false;
			}
		}
		else
		{
			_ASSERT(0);
			return false;
		}
	}

	return true;
}

bool CDnItemUpgradeExchangeDlg::AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot)
{
	if (pFromSlot == NULL || pAttachSlot == NULL)
	{
		_ASSERT(0);
		return false;
	}

	if (m_pSourceSlot == NULL || m_pTargetSlot == NULL)
	{
		_ASSERT(0);
		return false;
	}

	int i = 0;
	for (; i < USERITEM_SLOT_COUNT; ++i)
	{
		if (m_pInvenSlotCache[i] != NULL && m_pInvenSlotCache[i] == pFromSlot)
			return false;
	}

	CDnItem* pItem = static_cast<CDnItem *>(pFromSlot->GetItem());
	if (pItem == NULL)
		return false;

	if (pItem->GetEnchantID() <= 0)
	{
		HandleError(eEIU_INVALID_SOURCE_OR_TARGET_ITEM);
		return false;
	}

	std::vector<int> nVecJobList;
	if (CDnActor::s_hLocalActor)
	{
		((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );
		if (pItem->IsPermitPlayer(nVecJobList) == false)
		{
			HandleError(eEIU_CANT_EXCHANGE_OTHER_CLASS);
			return false;
		}
	}

	ITEM_SLOT_TYPE attachSlotType = pAttachSlot->GetSlotType();

	if (attachSlotType == ST_ITEM_UPGRADE_EXCHANGE_SOURCE)
	{
		if (pItem->GetEnchantLevel() <= 0)
		{
			HandleError(eEIU_INVALID_SOURCE_OR_TARGET_ITEM);
			return false;
		}
	}

	bool bCheckAttachItem = CheckAttachItemWithOtherItemSlot(pItem, attachSlotType);
	if (bCheckAttachItem == false)
		return false;

	ITEM_SLOT_TYPE fromSlotType = pFromSlot->GetSlotType();
	if (pAttachSlot->IsEmptySlot() == false)
		ReleaseItemSlotBtn(pAttachSlot);

	if (fromSlotType == ST_ITEM_UPGRADE_EXCHANGE_TARGET || fromSlotType == ST_ITEM_UPGRADE_EXCHANGE_SOURCE)
	{
		ReleaseItemSlotBtn(pAttachSlot, pFromSlot->GetSlotIndex());

		pFromSlot->ResetSlot();
	}
	else
	{
		pFromSlot->SetRegist(true);

		int slotIdx = pAttachSlot->GetSlotIndex();
		if (slotIdx >= SOURCE_SLOT_INDEX && slotIdx < USERITEM_SLOT_COUNT)
			m_pInvenSlotCache[slotIdx] = pFromSlot;
	}

	pAttachSlot->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	CEtSoundEngine::GetInstance().PlaySound("2D", pItem->GetDragSoundIndex());

	CheckUpgradeItem();

	return true;
}

bool CDnItemUpgradeExchangeDlg::ReleaseItemSlotBtn(CDnItemSlotButton* pReleaseSlotBtn, int slotIdxIfSlotChange /*= NOITEMS_IN_USERSLOT*/)
{
	if (pReleaseSlotBtn == NULL)
	{
		_ASSERT(0);
		return false;
	}

	ITEM_SLOT_TYPE type = pReleaseSlotBtn->GetSlotType();
	if (type != ST_ITEM_UPGRADE_EXCHANGE_SOURCE && type != ST_ITEM_UPGRADE_EXCHANGE_TARGET)
		return false;

	pReleaseSlotBtn->ResetSlot();

	int slotIdx = pReleaseSlotBtn->GetSlotIndex();
	if (slotIdx >= SOURCE_SLOT_INDEX && slotIdx < USERITEM_SLOT_COUNT)
	{
		if (m_pInvenSlotCache[slotIdx])
		{
			m_pInvenSlotCache[slotIdx]->SetRegist(false);
			m_pInvenSlotCache[slotIdx]->DisableSplitMode(true);
			m_pInvenSlotCache[slotIdx] = NULL;
		}
	}

	if (slotIdxIfSlotChange != NOITEMS_IN_USERSLOT && 
		(slotIdxIfSlotChange >= 0 && slotIdxIfSlotChange < USERITEM_SLOT_COUNT) &&
		(slotIdx >= SOURCE_SLOT_INDEX && slotIdx < USERITEM_SLOT_COUNT) )
	{
		m_pInvenSlotCache[slotIdx] = m_pInvenSlotCache[slotIdxIfSlotChange];
		m_pInvenSlotCache[slotIdxIfSlotChange] = NULL;
	}

	CheckUpgradeItem();

	return true;
}

void CDnItemUpgradeExchangeDlg::HandleError(CDnItemUpgradeExchangeDlg::eERR_ITEM_UPGRADE_EXC code)
{
	std::wstring str;
	switch (code)
	{
	case eEIU_GENERAL:
	case eEIU_CANT_EXCHANGE_OTHER_CLASS:
	case eEIU_EMPTY_SOURCE_OR_TARGET_SLOT:
	case eEIU_INVALID_SOURCE_OR_TARGET_ITEM:
		{
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4936); // UISTRING : 강화이동이 불가능한 아이템입니다.
		}
		break;

	case eEIU_DOESNOTMATCH_RANK:
	case eEIU_DOESNOTMATCH_LEVELLIMIT:
		{
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4937); // UISTRING : 원본아이템과 등급 및 레벨이 다른 아이템입니다.
		}
		break;

	case eEIU_SAME_OR_HIGH_TARGET_ENCHANT_LEVEL:
		{
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4938); // UISTRING : 원본아이템보다 등급이 높거나 같은 아이템입니다.
		}
		break;

	default:
		str = FormatW(L"%s (CODE:%d)", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 130310), code);	break;	// UISTRING : 코스튬 합성 중 문제가 발생했습니다.
	}

	GetInterface().MessageBox(str.c_str(), MB_OK);
}

void CDnItemUpgradeExchangeDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (strstr(pControl->GetControlName(), "ID_ITEM_UPGRADE"))
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			CDnItemSlotButton *pPressedButton = static_cast<CDnItemSlotButton*>(pControl);

			if (uMsg == WM_RBUTTONUP)
			{
				if (ReleaseItemSlotBtn(pPressedButton) == false)
					return;
			}

			if (pDragButton)
			{
				if (pDragButton != pPressedButton)
				{
					if (AddAttachItem(pDragButton, pPressedButton) == false)
						drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					return;
				}
				else
				{
					drag::ReleaseControl();
					return;
				}
			}
		}
		else if (IsCmdControl("ID_OK"))
		{
			if (CDnItemTask::IsActive())
			{
				if (m_pSourceSlot->IsEmptySlot() || m_pTargetSlot->IsEmptySlot())
				{
					HandleError(eEIU_EMPTY_SOURCE_OR_TARGET_SLOT);
					return;
				}

				const MIInventoryItem* pSourceItem = m_pSourceSlot->GetItem();
				if (pSourceItem == NULL || pSourceItem->GetType() != MIInventoryItem::Item)
				{
					HandleError(eEIU_INVALID_SOURCE_OR_TARGET_ITEM);
					return;
				}

				const MIInventoryItem* pTargetItem = m_pTargetSlot->GetItem();
				if (pTargetItem == NULL || pTargetItem->GetType() != MIInventoryItem::Item)
				{
					HandleError(eEIU_INVALID_SOURCE_OR_TARGET_ITEM);
					return;
				}

				const CDnItem* pExtractItem = static_cast<const CDnItem*>(pSourceItem);
				const CDnItem* pInjectItem = static_cast<const CDnItem*>(pTargetItem);

				if (pExtractItem == NULL || pInjectItem == NULL)
				{
					HandleError(eEIU_INVALID_SOURCE_OR_TARGET_ITEM);
					return;
				}

				CDnItemTask::GetInstance().RequestExchangeEnchant(pExtractItem, pInjectItem);
			}
		}
		else if (IsCmdControl("ID_CANCEL"))
		{
			ProcessCommand(EVENT_BUTTON_CLICKED, false, GetControl("ID_CLOSE_DIALOG"), 0);
			return;
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

bool CDnItemUpgradeExchangeDlg::IsFullSourceAndTargetItemSlot() const
{
	return (m_pSourceSlot && m_pSourceSlot->IsEmptySlot() == false && m_pTargetSlot && m_pTargetSlot->IsEmptySlot() == false);
}

CDnItemUpgradeExchangeDlg::eERR_ITEM_UPGRADE_EXC CDnItemUpgradeExchangeDlg::IsItemValidExchangeCondition(const CDnItem& source, const CDnItem& target)
{
	if (source.GetItemRank() != target.GetItemRank())
		return eEIU_DOESNOTMATCH_RANK;

	if (source.GetLevelLimit() != target.GetLevelLimit())
		return eEIU_DOESNOTMATCH_LEVELLIMIT;

	return eEIU_NONE;
}

CDnItemUpgradeExchangeDlg::eERR_ITEM_UPGRADE_EXC CDnItemUpgradeExchangeDlg::IsSlotValidExchangeCondition(ITEM_SLOT_TYPE existItemSlotType, const CDnItem& fromItem)
{
	CDnItemSlotButton* pExistItemSlot = NULL;
	if (existItemSlotType == ST_ITEM_UPGRADE_EXCHANGE_TARGET)
	{
		pExistItemSlot = m_pTargetSlot;
	}
	else if (existItemSlotType == ST_ITEM_UPGRADE_EXCHANGE_SOURCE)
	{
		pExistItemSlot = m_pSourceSlot;
	}
	else
	{
		_ASSERT(0);
		return eEIU_GENERAL;
	}

	if (pExistItemSlot == NULL || pExistItemSlot->IsEmptySlot())
		return eEIU_GENERAL;

	const MIInventoryItem* pExistItem = pExistItemSlot->GetItem();
	if (pExistItem)
	{
		if (pExistItem->GetType() != MIInventoryItem::Item)
		{
			_ASSERT(0);
			return eEIU_GENERAL;
		}

		const CDnItem* pExistDnItem = static_cast<const CDnItem*>(pExistItem);
		if (pExistDnItem == NULL)
		{
			_ASSERT(0);
			return eEIU_GENERAL;
		}

		eERR_ITEM_UPGRADE_EXC ret = IsItemValidExchangeCondition(fromItem, *pExistDnItem);
		if (ret != eEIU_NONE)
			return ret;

		if (existItemSlotType == ST_ITEM_UPGRADE_EXCHANGE_SOURCE)
		{
			if (pExistDnItem->GetEnchantLevel() <= fromItem.GetEnchantLevel())
				return eEIU_SAME_OR_HIGH_TARGET_ENCHANT_LEVEL;
		}
		else if (existItemSlotType == ST_ITEM_UPGRADE_EXCHANGE_TARGET)
		{
			if (pExistDnItem->GetEnchantLevel() >= fromItem.GetEnchantLevel())
				return eEIU_SAME_OR_HIGH_TARGET_ENCHANT_LEVEL;
		}
	}
	else
	{
		_ASSERT(0);
		return eEIU_GENERAL;
	}

	return eEIU_NONE;
}

void CDnItemUpgradeExchangeDlg::CheckUpgradeItem()
{
	if (CDnItemTask::IsActive() == false)
		return;

	int nPrice = 0;
	bool bEmpty = false;

	if (IsFullSourceAndTargetItemSlot() == false)
		bEmpty = true;

	if (bEmpty)
	{
		m_pButtonOK->Enable(false);
		ClearAllStuffItemSlot();
	}
	else
	{
		// NULL일리 없겠지만, 그래도 검사.
		if (m_pSourceSlot && m_pTargetSlot)
		{
			const MIInventoryItem* pMISourceItem = m_pSourceSlot->GetItem();
			const MIInventoryItem* pMITargetItem = m_pTargetSlot->GetItem();
			if (pMISourceItem == NULL || pMITargetItem == NULL || pMISourceItem->GetType() != MIInventoryItem::Item || pMITargetItem->GetType() != MIInventoryItem::Item)
			{
				_ASSERT(0);
				return;
			}

			const CDnItem* pSourceItem = static_cast<const CDnItem*>(pMISourceItem);
			const CDnItem* pTargetItem = static_cast<const CDnItem*>(pMITargetItem);

			if (pSourceItem && pTargetItem)
			{
				eERR_ITEM_UPGRADE_EXC err = IsItemValidExchangeCondition(*pSourceItem, *pTargetItem);
				if (err != eEIU_NONE)
				{
					HandleError(err);
					return;
				}

				std::vector<SNeedStuffItems> needStuffItemInfos;
				GetNeedStuffs(nPrice, needStuffItemInfos, *pSourceItem);
				m_pButtonOK->Enable( true );

				if (nPrice > GetItemTask().GetCoin())
					m_pButtonOK->Enable(false);

				bool bCheckAndSet = CheckAndSetNeedItemInfo(needStuffItemInfos);
				if (bCheckAndSet == false)
					m_pButtonOK->Enable(false);
			}
		}
	}

	int nGold = nPrice/10000;
	int nSilver = (nPrice%10000)/100;
	int nBronze = nPrice%100;

	m_pStaticGold->SetIntToText( nGold );
	m_pStaticSilver->SetIntToText( nSilver );
	m_pStaticBronze->SetIntToText( nBronze );
	if( nPrice > GetItemTask().GetCoin() )
	{
		if( nPrice >= 10000 ) m_pStaticGold->SetTextColor( 0xFFFF0000, true );
		if( nPrice >= 100 ) m_pStaticSilver->SetTextColor( 0xFFFF0000, true );
		m_pStaticBronze->SetTextColor( 0xFFFF0000, true );
	}
	else
	{
		m_pStaticGold->SetTextColor( m_dwColorGold );
		m_pStaticSilver->SetTextColor( m_dwColorSilver );
		m_pStaticBronze->SetTextColor( m_dwColorBronze );
	}
}

bool CDnItemUpgradeExchangeDlg::CheckAndSetNeedItemInfo(const std::vector<SNeedStuffItems>& needStuffItems)
{
	bool bOK = true;

	ClearAllStuffItemSlot();

	int slotIndex = 0;
	std::vector<SNeedStuffItems>::const_iterator iter = needStuffItems.begin();
	for (; iter != needStuffItems.end(); ++iter)
	{
		const SNeedStuffItems& item = (*iter);
		if (item.itemId <= 0)
			continue;

		int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount(item.itemId);
		if (nCurItemCount < item.count)
			bOK = false;

		if (slotIndex >= STUFF_ITEMSLOT_MAX)
		{
			_ASSERT(0);
			return false;
		}

		SetStuffItemSlot(slotIndex, item.itemId, item.count, nCurItemCount);
		slotIndex++;
	}

	return bOK;
}

void CDnItemUpgradeExchangeDlg::SetStuffItemSlot(int slotIndex, int itemId, int count, int curItemCount)
{
	if (slotIndex < 0 || slotIndex >= STUFF_ITEMSLOT_MAX)
	{
		_ASSERT(0);
		return;
	}

	ClearStuffItemSlot(slotIndex);

	TItemInfo itemInfo;	
	if (CDnItem::MakeItemInfo(itemId, 1, itemInfo) == false)
		return;
	CDnItem* pItem = GetItemTask().CreateItem(itemInfo);

	if (m_pStuffItemSlots[slotIndex])
	{
		m_pStuffItemSlots[slotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pStuffItemSlots[slotIndex]->SetJewelCount(count, curItemCount);

		m_pStuffItemSlots[slotIndex]->SetRegist(curItemCount < count);
	}
}

void CDnItemUpgradeExchangeDlg::ClearAllSourceAndTargetSlot()
{
	ReleaseItemSlotBtn(m_pSourceSlot);
	ReleaseItemSlotBtn(m_pTargetSlot);
}

void CDnItemUpgradeExchangeDlg::ClearAllStuffItemSlot()
{
	int i = 0;
	for (; i < STUFF_ITEMSLOT_MAX; ++i)
	{
		ClearStuffItemSlot(i);
	}
}

void CDnItemUpgradeExchangeDlg::ClearStuffItemSlot(int slotIndex)
{
	if (slotIndex < 0 || slotIndex >= STUFF_ITEMSLOT_MAX)
	{
		_ASSERT(0);
		return;
	}

	if (m_pStuffItemSlots[slotIndex])
	{
		const MIInventoryItem* pItem = m_pStuffItemSlots[slotIndex]->GetItem();
		if (pItem != NULL)
			SAFE_DELETE(pItem);

		m_pStuffItemSlots[slotIndex]->ResetSlot();
	}
}

void CDnItemUpgradeExchangeDlg::GetNeedStuffs(int& nNeedCoin, std::vector<SNeedStuffItems>& needStuffItems, const CDnItem& source) const
{
	eItemRank itemRank = source.GetItemRank();
	char cEnchantLevel = source.GetEnchantLevel();
	int nLevelLimit = source.GetLevelLimit();

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TENCHANTTRANSFER);
	if (pSox == NULL)
	{
		_ASSERT(0);
		return;
	}

	int nEnchantTableItemID = 0;
	std::vector<int> nVecItemID;
	pSox->GetItemIDListFromField("_EnchantLevel", cEnchantLevel, nVecItemID);
	if (nVecItemID.empty())
		return;

	int i = 0;
	for (; i < (int)nVecItemID.size(); i++)
	{
		DNTableCell* pRankCell = pSox->GetFieldFromLablePtr(nVecItemID[i], "_Rank");
		if (pRankCell == NULL || pRankCell->GetInteger() != (int)itemRank)
			continue;

		DNTableCell* pLevelLimitCell = pSox->GetFieldFromLablePtr(nVecItemID[i], "_LevelLimit");
		if (pLevelLimitCell == NULL || pLevelLimitCell->GetInteger() != nLevelLimit)
			continue;

		SNeedStuffItems item;
		DNTableCell* pNeedCoinCell = pSox->GetFieldFromLablePtr(nVecItemID[i], "_NeedCoin");
		if (pNeedCoinCell == NULL)
		{
			_ASSERT(0);
			continue;
		}

		nNeedCoin = pNeedCoinCell->GetInteger();

		int j = 1;
		std::string stuffItemIdString;
		std::string stuffItemCountString;
		for (; j <= MAX_STUFFITEM; ++j)
		{
			SNeedStuffItems item;

			stuffItemIdString = FormatA("_NeedItemID%d", j);
			DNTableCell* pStuffCell = pSox->GetFieldFromLablePtr(nVecItemID[i], stuffItemIdString.c_str());
			if (pStuffCell != NULL && pStuffCell->GetInteger() > 0)
				item.itemId = pStuffCell->GetInteger();

			stuffItemCountString = FormatA("_NeedItemCount%d", j);
			DNTableCell* pStuffCountCell = pSox->GetFieldFromLablePtr(nVecItemID[i], stuffItemCountString.c_str());
			if (pStuffCountCell != NULL && pStuffCountCell->GetInteger() > 0)
				item.count = pStuffCountCell->GetInteger();

			needStuffItems.push_back(item);
		}
	}
}

void CDnItemUpgradeExchangeDlg::OnComplete()
{
	ClearAllSourceAndTargetSlot();
}

#endif // PRE_ADD_EXCHANGE_ENCHANT