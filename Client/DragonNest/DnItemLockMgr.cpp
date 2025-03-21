#include "stdafx.h"
#include "DnItemLockMgr.h"
#include "DnInterfaceString.h"
#include "DnItemTask.h"
#include "DnCharStatusDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "ItemSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_ADD_EQUIPLOCK

void CDnItemLockMgr::LockInfo::SetLockItemInfo(const EquipItemLock::TLockItemInfo& cur)
{
	equipSlotIndex = cur.cItemSlotIndex;
	state = cur.ItemData.eItemLockStatus;
	if (state == EquipItemLock::Lock)
	{
		lockOrReleaseStartTime = cur.ItemData.tUnLockDate;
		DN_INTERFACE::STRING::GetTimeText2(lockOrReleaseStartTimeString, lockOrReleaseStartTime, 8414);
	}
	else if (state == EquipItemLock::RequestUnLock)
	{
		releaseTime = cur.ItemData.tUnLockDate;
		DN_INTERFACE::STRING::GetTimeText2(releaseTimeString, releaseTime, 8414);

		lockOrReleaseStartTime = cur.ItemData.tUnLockRequestDate;
		DN_INTERFACE::STRING::GetTimeText2(lockOrReleaseStartTimeString, lockOrReleaseStartTime, 8414);
	}
}

void CDnItemLockMgr::SetEquipLockItemList(const SCEquipItemLockList& packet)
{
	ClearEquipLockItemList();

	int i = 0;
	for (; i < packet.nEquipCount; ++i)
	{
		const EquipItemLock::TLockItemInfo& cur = packet.LockItems[i];
		AddEquipLockItem(cur, NORMAL);
	}

	for (i = packet.nEquipCount; i < packet.nEquipCount + packet.nCashEquipCount; ++i)
	{
		const EquipItemLock::TLockItemInfo& cur = packet.LockItems[i];
		AddEquipLockItem(cur, CASH);
	}
}

void CDnItemLockMgr::AddEquipLockItem(const EquipItemLock::TLockItemInfo& cur, const EquipLockInvenType type)
{
	if (type < NORMAL || type >= EQUIPLOCK_INVENTYPE_MAX)
		return;

	LockInfo info;
	info.SetLockItemInfo(cur);

	m_EquipLockItemList[type].push_back(info);
}

void CDnItemLockMgr::RefreshEquipLockItem(const SCItemLockRes& res)
{
	EquipLockInvenType type = (res.IsCashEquip) ? CASH : NORMAL;
	const int& equipSlotIdx = res.LockItems.cItemSlotIndex;
	if (equipSlotIdx < 0)
		return;

	const LockInfo* pInfo = GetLockInfo(type, equipSlotIdx);
	if (pInfo == NULL)
	{
		AddEquipLockItem(res.LockItems, type);
		return;
	}

	std::vector<LockInfo>::iterator iter = m_EquipLockItemList[type].begin();
	for (; iter != m_EquipLockItemList[type].end(); ++iter)
	{
		LockInfo& info = (*iter);
		if (info.equipSlotIndex == equipSlotIdx)
		{
			info.SetLockItemInfo(res.LockItems);
			break;
		}
	}
}

void CDnItemLockMgr::ClearEquipLockItemList()
{
	int i = 0;
	for (; i < EQUIPLOCK_INVENTYPE_MAX; ++i)
	{
		m_EquipLockItemList[i].clear();
	}
}

void CDnItemLockMgr::GetStateToolTipString(std::wstring& result, const CDnItem& item) const
{
	EquipLockInvenType invenType = item.IsCashItem() ? CASH : NORMAL;

	int equipIndex = GetEquipIndex(item, true);
	if (equipIndex < 0)
		return;

	int stringIndex = 0;
	const LockInfo* pInfo = GetLockInfo(invenType, equipIndex);
	if (pInfo == NULL)
		return;

	if (pInfo->state == EquipItemLock::Lock)
	{
		result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8384);
	}
	else if (pInfo->state == EquipItemLock::RequestUnLock)
	{
		result = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8392);
	}
	else
	{
		return;
	}
}

void CDnItemLockMgr::GetLockDateToolTipString(std::wstring& result, const CDnItem& item) const
{
	EquipLockInvenType invenType = item.IsCashItem() ? CASH : NORMAL;

	int equipIndex = GetEquipIndex(item, true);
	if (equipIndex < 0)
		return;

	int stringIndex = 0;
	const LockInfo* pInfo = GetLockInfo(invenType, equipIndex);
	if (pInfo == NULL)
		return;

	if (pInfo->state == EquipItemLock::Lock)
		result = pInfo->lockOrReleaseStartTimeString;
}

void CDnItemLockMgr::GetUnLockDateToolTipString(std::wstring& unlockStart, std::wstring& unlock, const CDnItem& item) const
{
	EquipLockInvenType invenType = item.IsCashItem() ? CASH : NORMAL;

	int equipIndex = GetEquipIndex(item, true);
	if (equipIndex < 0)
		return;

	int stringIndex = 0;
	const LockInfo* pInfo = GetLockInfo(invenType, equipIndex);
	if (pInfo == NULL)
		return;

	if (pInfo->state == EquipItemLock::RequestUnLock)
	{
		unlockStart = pInfo->lockOrReleaseStartTimeString;
		unlock = pInfo->releaseTimeString;
	}
}

EquipItemLock::eLockStatus CDnItemLockMgr::GetLockState(const CDnItem& item) const
{
	int equipIndex = GetEquipIndex(item, true);
	if (equipIndex < 0)
		return EquipItemLock::None;

	if (item.IsCashItem())
	{
		const LockInfo* pInfo = GetLockInfo(CASH, equipIndex);
		if (pInfo != NULL)
			return pInfo->state;
		return EquipItemLock::None;
	}

	const LockInfo* pInfo = GetLockInfo(NORMAL, equipIndex);
	if (pInfo != NULL)
		return pInfo->state;

	return EquipItemLock::None;
}

const CDnItemLockMgr::LockInfo* CDnItemLockMgr::GetLockInfo(CDnItemLockMgr::EquipLockInvenType type, int equipIndex) const
{
	if (type < 0 || type >= EQUIPLOCK_INVENTYPE_MAX)
		return NULL;

	std::vector<LockInfo>::const_iterator iter =  m_EquipLockItemList[type].begin();
	for (; iter != m_EquipLockItemList[type].end(); ++iter)
	{
		const LockInfo& info = (*iter);
		if (info.equipSlotIndex == equipIndex)
			return (&info);
	}

	return NULL;
}

int CDnItemLockMgr::GetEquipIndex(const CDnItem& item, bool bCheckLockItemList) const
{
	if (CDnItemTask::IsActive() == false)
		return -1;

	CDnCharStatusDlg* pCharStatusDlg = static_cast<CDnCharStatusDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG));
	if (pCharStatusDlg == NULL)
		return -1;

	int equipIndex = item.GetSlotIndex();
	CDnItemLockMgr::EquipLockInvenType invenType = CDnItemLockMgr::NORMAL;
	if (item.IsCashItem())
		invenType = CDnItemLockMgr::CASH;
	else
		invenType = CDnItemLockMgr::NORMAL;

	if (bCheckLockItemList)
	{
		int invenSize = (int)m_EquipLockItemList[invenType].size();
		if (invenSize <= 0)
			return -1;
	}

	return equipIndex;
}

void CDnItemLockMgr::RequestLock(const CDnItem& item) const
{
	int equipIndex = GetEquipIndex(item, false);
	if (equipIndex < 0)
	{
		_ASSERT(0);
		return;
	}

	SendItemEquipLock(item.IsCashItem(), equipIndex, item.GetSerialID());
}

void CDnItemLockMgr::RequestUnLock(const CDnItem& item) const
{
	int equipIndex = GetEquipIndex(item, false);
	if (equipIndex < 0)
	{
		_ASSERT(0);
		return;
	}

	SendItemEquipUnLock(item.IsCashItem(), equipIndex, item.GetSerialID());
}

#endif // PRE_ADD_EQUIPLOCK