#include "StdAfx.h"
#include "DnPetInventory.h"
#include "DnItem.h"
#include "DnInventoryDlg.h"
#include "DnInterface.h"
#include "DnMainDlg.h"
#include "DnItemTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 



CDnPetInventory::CDnPetInventory(void)
{
}

CDnPetInventory::~CDnPetInventory(void)
{
}

void CDnPetInventory::CreatePetItem( TVehicleCompact &item, int nSlotIndex )
{
	if( !m_pInvenDlg )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, m_pInvenDlg is NULL!" );
		return;
	}

	if( !m_pItemTask )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, m_pItemTask is NULL!" );
		return;
	}

	CDnItem *pItem = m_pItemTask->CreateItem( item.Vehicle[Pet::Slot::Body] );

	if( pItem == NULL )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, ������(%d) ������ ����!", 	item.Vehicle->nItemID );
		return;
	}

	pItem->SetSlotIndex( nSlotIndex );
	m_mapInventory[pItem->GetSlotIndex()] = pItem;
	m_pInvenDlg->SetItem( pItem );
}

bool CDnPetInventory::InsertItem( CDnItem *pItem, bool bMoving )
{
	if (pItem == NULL)
	{
		_ASSERT(0 && "CDnPetInventory::InsertItem - ITEM is NULL\n");
		return false;
	}

	if( !m_pInvenDlg )
	{
		CDebugSet::ToLogFile( "CDnPetInventory::InsertItem, m_pInvenDlg is NULL!" );
		return false;
	}

	int nExistCnt = GetItemCount(pItem->GetClassID());

	// �̹� �����ߴ� �������� ������ ���� �Ȱ��� Ȯ�� ����
	if( GetItem( pItem->GetSlotIndex() ) ) 
	{
		INVENTORY_MAP_ITER iter = m_mapInventory.find( pItem->GetSlotIndex() );
		if( iter != m_mapInventory.end() )
		{
			m_pInvenDlg->ResetSlot( iter->second );

			SAFE_DELETE( iter->second );
			m_mapInventory.erase( iter );
		}
	}

	// �̹� DnInvenSlotDlg ���� �Ҹ��� ���ְ� �־��..�ߺ����� ���ϴ�.
	//	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	if(pItem)
		pItem->SetSoulBound(true); // Ż���� �ͼ�<ĳ��> �������̴�.

	m_mapInventory[pItem->GetSlotIndex()] = pItem;
	m_pInvenDlg->SetItem( pItem );

	int nNowCount = GetItemCount(pItem->GetClassID());

	switch( pItem->GetItemType() ) {
		case ITEMTYPE_NORMAL:
		case ITEMTYPE_COOKING:
			GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
			break;
	}

	// ���� �����ϴ� �����ۺ��� �þ���� 
	int nDeltaCnt = nNowCount - nExistCnt;
	if ( nDeltaCnt > 0 && bMoving == false )
	{
		// �������� �� ����ٰ� �˶��޼����� ���ش�.
		if( pItem->GetItemType() == ITEMTYPE_QUEST )
		{
			GetInterface().ShowSymbolAlarmDialog( pItem->GetName(), pItem, nDeltaCnt, textcolor::WHITE, 3.0f );
		}
		else
		{
			GetInterface().ShowItemAlarmDialog( pItem->GetName(), pItem, nDeltaCnt, textcolor::WHITE, 3.0f );
		}
	}

	return true;
}

bool CDnPetInventory::RemoveItem( int nSlotIndex )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.find( nSlotIndex );
	if( iter != m_mapInventory.end() )
	{
		// �̹� DnInvenSlotDlg ���� �Ҹ��� ���ְ� �־��..�ߺ����� ���ϴ�.
		//		CEtSoundEngine::GetInstance().PlaySound( "2D", iter->second->GetDragSoundIndex() );

		eItemTypeEnum emItemType = iter->second->GetItemType();
		m_pInvenDlg->ResetSlot( iter->second );

		SAFE_DELETE( iter->second );
		m_mapInventory.erase( iter );

		switch( emItemType ) {
			case ITEMTYPE_NORMAL:
			case ITEMTYPE_COOKING:
				GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
				break;
		}

		return true;
	}

	return false;

	return true;
}

void CDnPetInventory::SetUsableSlotCount( int nCount )
{ 
	if (m_pInvenDlg == NULL) return;
	m_nUsableSlotCount = nCount;
}

void CDnPetInventory::DecreaseDurability( int nValue )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();

	for( iter; iter != iterEnd; ++iter )
	{
		CDnItem *pItem = iter->second;
		if( !pItem ) continue;
		switch( pItem->GetItemType() ) {
			case ITEMTYPE_WEAPON:
				{
					CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
					if( !pWeapon ) break;
					if( pWeapon->IsInfinityDurability() ) break;
					if( pWeapon->GetDurability() == 0 ) break;

					int nTemp = pWeapon->GetDurability() - nValue;
					if( nTemp < 0 ) nTemp = 0;
					pWeapon->SetDurability( nTemp );
				}
				break;
			case ITEMTYPE_PARTS:
				{
					CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
					if( !pParts ) break;
					if( pParts->IsInfinityDurability() ) break;
					if( pParts->GetDurability() == 0 ) break;

					int nTemp = pParts->GetDurability() - nValue;
					if( nTemp < 0 ) nTemp = 0;
					pParts->SetDurability( nTemp );
				}
				break;
		}
	}
}

void CDnPetInventory::DecreaseDurability( float fValue )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();

	for( iter; iter != iterEnd; ++iter )
	{
		CDnItem *pItem = iter->second;
		if( !pItem ) continue;
		switch( pItem->GetItemType() ) {
			case ITEMTYPE_WEAPON:
				{
					CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
					if( !pWeapon ) break;
					if( pWeapon->IsInfinityDurability() ) break;
					if( pWeapon->GetDurability() == 0 ) break;

					int nTemp = (int)( pWeapon->GetDurability() - ( pWeapon->GetMaxDurability() * fValue ) );
					if( nTemp < 0 ) nTemp = 0;
					pWeapon->SetDurability( nTemp );
				}
				break;
			case ITEMTYPE_PARTS:
				{
					CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
					if( !pParts ) break;
					if( pParts->IsInfinityDurability() ) break;
					if( pParts->GetDurability() == 0 ) break;

					int nTemp = (int)( pParts->GetDurability() - ( pParts->GetMaxDurability() * fValue ) );
					if( nTemp < 0 ) nTemp = 0;
					pParts->SetDurability( nTemp );
				}
				break;
		}
	}
}

