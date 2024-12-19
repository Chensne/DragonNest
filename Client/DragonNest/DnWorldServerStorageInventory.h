#pragma once

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnInventory.h"

class CDnItem;

class CDnWorldServerStorageInventory : public CDnInventory
{
public:
	CDnWorldServerStorageInventory(void);
	virtual ~CDnWorldServerStorageInventory(void);

	void CreateItem(TItem &item, int nSlotIndex);
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	int GetMaxStorageCount(bool bCash) const;
};

#endif // PRE_ADD_SERVER_WAREHOUSE