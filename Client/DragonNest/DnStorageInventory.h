#pragma once
#include "DnInventory.h"

class CDnItem;

class CDnStorageInventory : public CDnInventory
{
public:
	CDnStorageInventory(void);
	virtual ~CDnStorageInventory(void);

public:
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );
};
