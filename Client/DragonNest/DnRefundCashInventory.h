#pragma once

#include "DnInventory.h"

#ifdef PRE_ADD_CASHSHOP_REFUND_CL

class CDnItem;

class CDnRefundCashInventory : public CDnInventory
{
public:
	CDnRefundCashInventory(void) {}
	virtual ~CDnRefundCashInventory(void) {}

	void CreateCashItem( TItem &item, int nSlotIndex, CASHITEM_SN sn, INT64 dbid );

	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );
	bool RemoveItem(INT64 dbid, std::wstring& removeItemName);
	virtual void SetUsableSlotCount( int nCount );

	bool SortRefundCashInventory();
	INT64 GetDBID(int slotIdx) const;
};

#endif // PRE_ADD_CASHSHOP_REFUND_CL