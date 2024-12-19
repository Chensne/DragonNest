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

	// bMoving - 인벤내 이동인가.
	// 실제로 서버에서 인벤,장비창,창고 안에서 아이템 돌리는건 다른 메세지를 사용한다.
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	virtual void SetUsableSlotCount( int nCount );

	void DecreaseDurability( int nValue );
	void DecreaseDurability( float fValue );
};