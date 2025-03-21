#pragma once

#include "DnItem.h"

#ifdef PRE_ADD_EQUIPLOCK

class CDnItemLockMgr
{
public:
	enum EquipLockInvenType
	{
		NORMAL,
		CASH,
		EQUIPLOCK_INVENTYPE_MAX
	};

	struct LockInfo
	{
		int equipSlotIndex;
		EquipItemLock::eLockStatus state;

		__time64_t lockOrReleaseStartTime;
		__time64_t releaseTime;

		std::wstring lockOrReleaseStartTimeString;
		std::wstring releaseTimeString;

		LockInfo()
		{
			equipSlotIndex = -1;
			state = EquipItemLock::None;
			lockOrReleaseStartTime = 0;
			releaseTime = 0;
		}

		void SetLockItemInfo(const EquipItemLock::TLockItemInfo& cur);
	};

	CDnItemLockMgr() {}

	void SetEquipLockItemList(const SCEquipItemLockList& packet);
	void AddEquipLockItem(const EquipItemLock::TLockItemInfo& cur, const EquipLockInvenType addInvenType);
	void RefreshEquipLockItem(const SCItemLockRes& res);

	void GetStateToolTipString(std::wstring& result, const CDnItem& item) const;
	void GetLockDateToolTipString(std::wstring& result, const CDnItem& item) const;
	void GetUnLockDateToolTipString(std::wstring& unlockStart, std::wstring& unlock, const CDnItem& item) const;

	EquipItemLock::eLockStatus GetLockState(const CDnItem& item) const;

	void RequestLock(const CDnItem& item) const;
	void RequestUnLock(const CDnItem& item) const;

private:
	void ClearEquipLockItemList();

	int GetEquipIndex(const CDnItem& item, bool bCheckLockItemList) const;
	const CDnItemLockMgr::LockInfo* GetLockInfo(CDnItemLockMgr::EquipLockInvenType type, int equipIndex) const;

	std::vector<LockInfo> m_EquipLockItemList[EQUIPLOCK_INVENTYPE_MAX];
};

#endif