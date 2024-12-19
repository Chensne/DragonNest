
#pragma once

#ifdef PRE_ADD_LIMITED_CASHITEM
class CDNLimitedCashItemRepository
{
public:
	CDNLimitedCashItemRepository();
	~CDNLimitedCashItemRepository();

	void DoUpdate(int nThreadID);

	void UpdateLimitedItemList(std::vector<LimitedCashItem::TLimitedQuantityCashItem> &vList);
	void UpdateLimitedItemList(TAGetLimitedItemList * pPacket);

	bool GetLimitedItemList(std::vector<LimitedCashItem::TLimitedQuantityCashItem> &vList);
	bool GetChangedLimitedItemList(std::vector<LimitedCashItem::TChangedLimitedQuantity> &vList);

	bool UpdateChangedLimitedItemMax(int nSN, int nLimitMax);
	bool GetChangedLimitedItemMax(int nSN, int &nLimitMax);

	void QueryGetLimitedItem();

private:
	volatile bool m_bInit;
	long m_lReferenced;
	DWORD m_dwLastUpdateTime;

	CSyncLock m_Sync;
	std::vector<LimitedCashItem::TLimitedQuantityCashItem> m_vCashItemList;
	std::vector<LimitedCashItem::TLimitedQuantityCashItem> m_vCashItemTempList;

	std::vector<LimitedCashItem::TChangedLimitedQuantity> m_vChangedItemMaxList;
};

extern CDNLimitedCashItemRepository * g_pLimitedCashItemRepository;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM