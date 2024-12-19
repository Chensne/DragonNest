
#include "Stdafx.h"
#include "DNLimitedCashItemRepository.h"
#include "Log.h"

#ifdef PRE_ADD_LIMITED_CASHITEM
#if !defined(_VILLAGESERVER) && !defined(_GAMESERVER)
#include "DNSQLManager.h"
#include "DNSQLMembership.h"
#endif		//#if !defined(_VILLAGESERVER) && !defined(_GAMESERVER)

extern TCashConfig g_Config;

CDNLimitedCashItemRepository * g_pLimitedCashItemRepository = NULL;

CDNLimitedCashItemRepository::CDNLimitedCashItemRepository() : m_bInit(false), m_lReferenced(0), m_dwLastUpdateTime(0)
{
}

CDNLimitedCashItemRepository::~CDNLimitedCashItemRepository()
{
}

void CDNLimitedCashItemRepository::DoUpdate(int nThreadID)
{
#if !defined(_VILLAGESERVER) && !defined(_GAMESERVER)
	if (InterlockedCompareExchange(&m_lReferenced, 1, 0) == 0)	
	{
		DWORD dwCurTime = timeGetTime();
		if (dwCurTime > m_dwLastUpdateTime + LimitedCashItem::Common::DitributingTerm)
		{
			CDNSQLMembership * pMembership = g_pSQLManager->FindMembershipDB(nThreadID);
			if (pMembership)
				pMembership->QueryGetLimitedItem();
			m_dwLastUpdateTime = dwCurTime;
		}
		InterlockedExchange(&m_lReferenced, 0);
	}
#endif		//#if !defined(_VILLAGESERVER) && !defined(_GAMESERVER)
}

void CDNLimitedCashItemRepository::UpdateLimitedItemList(std::vector<LimitedCashItem::TLimitedQuantityCashItem> &vList)
{
	if (vList.empty()) return;
	ScopeLock <CSyncLock> sync(m_Sync);
	m_vCashItemList = vList;
	m_bInit = true;
}

void CDNLimitedCashItemRepository::UpdateLimitedItemList(TAGetLimitedItemList * pPacket)
{
	if (pPacket->cCount == 0) return;
	ScopeLock <CSyncLock> sync(m_Sync);
	for (int i = 0; i < pPacket->cCount; i++)
		m_vCashItemTempList.push_back(pPacket->Limited[i]);

	if (pPacket->cIsLast >= 1)
	{
		m_vCashItemList = m_vCashItemTempList;
		m_vCashItemTempList.clear();
		m_bInit = true;
	}
}

bool CDNLimitedCashItemRepository::GetLimitedItemList(std::vector<LimitedCashItem::TLimitedQuantityCashItem> &vList)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	if (m_bInit == false) return false;
	vList = m_vCashItemList;
	return true;
}

bool CDNLimitedCashItemRepository::GetChangedLimitedItemList(std::vector<LimitedCashItem::TChangedLimitedQuantity> &vList)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	if (m_bInit == false) return false;
	vList = m_vChangedItemMaxList;
	return true;
}

bool CDNLimitedCashItemRepository::UpdateChangedLimitedItemMax(int nSN, int nLimitMax)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	bool bCheck = false;
	std::vector<LimitedCashItem::TChangedLimitedQuantity>::iterator ii;
	for (ii = m_vChangedItemMaxList.begin(); ii != m_vChangedItemMaxList.end(); ii++)
	{
		if ((*ii).nProductID == nSN)
		{
			(*ii).nMaxCount = nLimitMax;
			bCheck = true;
		}
	}

	if (bCheck == false)
	{
		LimitedCashItem::TChangedLimitedQuantity Limit;
		memset(&Limit, 0, sizeof(LimitedCashItem::TChangedLimitedQuantity));

		Limit.nProductID = nSN;
		Limit.nMaxCount = nLimitMax;

		m_vChangedItemMaxList.push_back(Limit);
	}
	return true;
}

bool CDNLimitedCashItemRepository::GetChangedLimitedItemMax(int nSN, int &nLimitMax)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector<LimitedCashItem::TChangedLimitedQuantity>::iterator ii;
	for (ii = m_vChangedItemMaxList.begin(); ii != m_vChangedItemMaxList.end(); ii++)
	{
		if ((*ii).nProductID == nSN)
		{
			nLimitMax = (*ii).nMaxCount;
			return true;
		}
	}
	return false;
}

void CDNLimitedCashItemRepository::QueryGetLimitedItem()
{
#if !defined(_VILLAGESERVER) && !defined(_GAMESERVER)
	CDNSQLMembership * pMembership = g_pSQLManager->FindMembershipDB(0);
	if (pMembership)
	{
		if (pMembership->QueryGetLimitedItem() != ERROR_NONE)
			g_Log.Log(LogType::_ERROR, L"QueryGetLimitedItem Failed! Ip:%S Port:%d DBName:%s DBID:%s\r\n", g_Config.MembershipDB.szIP, g_Config.MembershipDB.nPort, g_Config.MembershipDB.wszDBName, g_Config.MembershipDB.wszDBID);
	}
#endif		//#if !defined(_VILLAGESERVER) && !defined(_GAMESERVER)
}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM