#pragma once
#include "DnInventory.h"

class CDnItem;

class CDnGuildInventory : public CDnInventory
{
public:
	CDnGuildInventory(void);
	virtual ~CDnGuildInventory(void);

public:
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	void CheckInvalidSlot( int nSlotIndex );
};
