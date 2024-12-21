#pragma once
#include "DnInventory.h"

class CDnItem;

class CDnPetInventory : public CDnInventory
{
public:
	CDnPetInventory(void);
	virtual ~CDnPetInventory(void);

protected:

public:
	void CreatePetItem( TVehicleCompact &item, int nSlotIndex );

	// bMoving - 인벤내 이동인가.
	// 실제로 서버에서 인벤,장비창,창고 안에서 아이템 돌리는건 다른 메세지를 사용한다.
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	virtual void SetUsableSlotCount( int nCount );

	void DecreaseDurability( int nValue );  //
	void DecreaseDurability( float fValue );// 일단 쓸일이 없지만 가지고 있습니다.
};

