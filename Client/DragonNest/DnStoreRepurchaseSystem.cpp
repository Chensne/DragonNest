#include "StdAfx.h"
#include "DnStoreRepurchaseSystem.h"
#include "DnStoreTabDlg.h"
#include "DnItemTask.h"
#include "TradeSendPacket.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnStoreRepurchaseSystem::CDnStoreRepurchaseSystem() : m_pStoreDialog(NULL)
{
}

CDnStoreRepurchaseSystem::~CDnStoreRepurchaseSystem(void)
{
	DeleteStoreData();
}

bool CDnStoreRepurchaseSystem::Initialize()
{
	return true;
}

bool CDnStoreRepurchaseSystem::SetRepurchaseInfo(const int& nInfoCount, const TRepurchaseItemInfo* pInfoArray)
{
	DeleteStoreData();

	char cSlotIndexer = 0;
	int i = 0;
	for (; i < nInfoCount; ++i)
	{
		if (i >= Shop::Repurchase::MaxList)
		{
			_ASSERT(0);
			return false;
		}

		const TRepurchaseItemInfo& curInfo = pInfoArray[i];
		const SRepurchaseInfo* pExistInfo = GetRepurchaseInfo(curInfo.iRepurchaseID);
		if (pExistInfo != NULL)
		{
			_ASSERT(0);
			continue;
		}

		TItemInfo itemInfo;
		itemInfo.cSlotIndex = cSlotIndexer;
		itemInfo.Item = curInfo.Item;
		cSlotIndexer++;

		CDnItem* pItem = CDnItemTask::GetInstance().CreateItem( itemInfo );
		if (pItem == NULL)
			continue;

		SRepurchaseInfo info;
		info.nSellPrice = curInfo.iSellPrice;
		info.tSellDate = curInfo.tSellDate;
		info.pItem = pItem;

		int repurchaseTabID = m_pStoreDialog->GetRepurchasableTabID();
		if (repurchaseTabID != CDnStoreTabDlg::INVALID_TABID)
		{
			m_mapRepurchaseInfo.insert(std::make_pair(curInfo.iRepurchaseID, info));
			if (m_pStoreDialog)
				m_pStoreDialog->SetStoreItem(repurchaseTabID, pItem);
		}
	}

	return true;
}

void CDnStoreRepurchaseSystem::DeleteStoreData()
{
	std::map<int, SRepurchaseInfo>::iterator rpsIter = m_mapRepurchaseInfo.begin();
	for(; rpsIter != m_mapRepurchaseInfo.end(); ++rpsIter)
	{
		SRepurchaseInfo& info = (*rpsIter).second;
		SAFE_DELETE(info.pItem);
	}

	m_mapRepurchaseInfo.clear();
}

void CDnStoreRepurchaseSystem::DeleteStoreData(int nRepurchaseID)
{
	std::map<int, SRepurchaseInfo>::iterator rpsIter = m_mapRepurchaseInfo.find(nRepurchaseID);
	if (rpsIter != m_mapRepurchaseInfo.end())
	{
		SRepurchaseInfo& info = (*rpsIter).second;
		SAFE_DELETE(info.pItem);

		m_mapRepurchaseInfo.erase(rpsIter);
	}
}

int CDnStoreRepurchaseSystem::GetRepurchasePrice(const CDnItem& item) const
{
	const SRepurchaseInfo* pInfo = GetRepurchaseInfo(item);
	if (pInfo)
		return int(pInfo->nSellPrice * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Shop_Repurchase_Fee ));

	return eINVALID_PRICE;
}

const CDnStoreRepurchaseSystem::SRepurchaseInfo* CDnStoreRepurchaseSystem::GetRepurchaseInfo(const CDnItem& item) const
{
	std::map<int, SRepurchaseInfo>::const_iterator rpsIter = m_mapRepurchaseInfo.begin();
	for(; rpsIter != m_mapRepurchaseInfo.end(); ++rpsIter)
	{
		const SRepurchaseInfo& info = (*rpsIter).second;
		if (IsSameItem(*(info.pItem), item))
			return &info;
	}
	
	return NULL;
}

const CDnStoreRepurchaseSystem::SRepurchaseInfo* CDnStoreRepurchaseSystem::GetRepurchaseInfo(int nRepurchaseID) const
{
	std::map<int, SRepurchaseInfo>::const_iterator found = m_mapRepurchaseInfo.find(nRepurchaseID);
	if (found != m_mapRepurchaseInfo.end())
	{
		return &((*found).second);
	}

	return NULL;
}

int CDnStoreRepurchaseSystem::GetRepurchaseID(const CDnItem& item) const
{
	std::map<int, SRepurchaseInfo>::const_iterator rpsIter = m_mapRepurchaseInfo.begin();
	for(; rpsIter != m_mapRepurchaseInfo.end(); ++rpsIter)
	{
		const SRepurchaseInfo& info = (*rpsIter).second;
		if (IsSameItem(*(info.pItem), item))
			return (*rpsIter).first;
	}

	return eINVALID_REPURCHASE_ID;
}

bool CDnStoreRepurchaseSystem::IsSameItem(const CDnItem& source, const CDnItem& target) const
{
	return (source.GetClassID() == target.GetClassID() && source.GetSlotIndex() == target.GetSlotIndex());
}

CDnStoreRepurchaseSystem::eEnableRepurchaseResult CDnStoreRepurchaseSystem::CheckEnableRepurchase(const CDnItem& item) const
{
	int nNeedMoney = GetRepurchasePrice(item);
	if (nNeedMoney > GetItemTask().GetCoin())
	{
		GetInterface().MessageBox( 1706, MB_OK ); // UISTRING : 돈이 부족하여 구입 할 수 없습니다.
		return eNO_NOT_ENOUGH_MONEY;
	}

	if( !GetInterface().IsEmptySlot(CDnMainMenuDlg::INVENTORY_DIALOG, ITEM_SLOT_TYPE::ST_INVENTORY))
	{
		GetInterface().MessageBox( 1925, MB_OK ); // UISTRING : 인벤토리 공간이 부족합니다.
		return eNO_NOT_ENOUGH_INVENSLOT;
	}

	return eOK;
}

bool CDnStoreRepurchaseSystem::RequestRepurchase(const CDnItem& item) const
{
	int rpsID = GetRepurchaseID(item);
	if (rpsID != eINVALID_REPURCHASE_ID)
	{
		SendShopRepurchase(rpsID);
		return true;
	}

	return false;
}

void CDnStoreRepurchaseSystem::OnRepurchaseSuccess(int nRepurchaseID)
{
	const SRepurchaseInfo* pInfo = GetRepurchaseInfo(nRepurchaseID);
	if (pInfo == NULL || pInfo->pItem == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (m_pStoreDialog == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pStoreDialog->ResetSlot(pInfo->pItem->GetSlotIndex());
	DeleteStoreData(nRepurchaseID);
}
