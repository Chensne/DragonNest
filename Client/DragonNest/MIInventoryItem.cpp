#include "StdAfx.h"
#include "MIInventoryItem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MIInventoryItem::MIInventoryItem( InvenItemTypeEnum Type )
{
	m_Type = Type;
	m_nSlotIndex = -1;
	m_nIconImageIndex = 0;
	m_CashItemSerial = -1;
}

MIInventoryItem::~MIInventoryItem()
{	
}

void MIInventoryItem::SetIconImageIndex( int nValue )
{
	m_nIconImageIndex = nValue;
}