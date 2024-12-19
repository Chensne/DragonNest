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

	// bMoving - �κ��� �̵��ΰ�.
	// ������ �������� �κ�,���â,â�� �ȿ��� ������ �����°� �ٸ� �޼����� ����Ѵ�.
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	virtual void SetUsableSlotCount( int nCount );

	void DecreaseDurability( int nValue );  //
	void DecreaseDurability( float fValue );// �ϴ� ������ ������ ������ �ֽ��ϴ�.
};

