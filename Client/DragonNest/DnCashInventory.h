#pragma once
#include "DnInventory.h"

class CDnItem;

class CDnCashInventory : public CDnInventory
{
public:
	CDnCashInventory(void);
	virtual ~CDnCashInventory(void);

protected:

public:
	void CreateCashItem( TItem &item, int nSlotIndex );

	// bMoving - �κ��� �̵��ΰ�.
	// ������ �������� �κ�,���â,â�� �ȿ��� ������ �����°� �ٸ� �޼����� ����Ѵ�.
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	virtual void SetUsableSlotCount( int nCount );

	void DecreaseDurability( int nValue );
	void DecreaseDurability( float fValue );
};