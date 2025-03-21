#include "StdAfx.h"
#include "DnStorageInventory.h"
#include "DnItem.h"
#include "DnInventoryDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStorageInventory::CDnStorageInventory(void)
{
}

CDnStorageInventory::~CDnStorageInventory(void)
{
}

bool CDnStorageInventory::InsertItem( CDnItem *pItem, bool bMoving )
{
	if( !m_pInvenDlg )
	{
		CDebugSet::ToLogFile( "CDnStorageInventory::InsertItem, m_pInvenDlg is NULL!" );
		return false;
	}

	if( GetItem( pItem->GetSlotIndex() ) ) 
	{
		RemoveItem( pItem->GetSlotIndex() );
	}

	// 사운드 처리는 서버응답 받을때 혹은 클라단에서 ui처리(집을때 등)할때에만 플레이합니다.
	//CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_mapInventory[pItem->GetSlotIndex()] = pItem;
	m_pInvenDlg->SetItem( pItem );

	return true;
}

bool CDnStorageInventory::RemoveItem( int nSlotIndex )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.find( nSlotIndex );
	if( iter != m_mapInventory.end() )
	{
		m_pInvenDlg->ResetSlot( iter->second );

		//CEtSoundEngine::GetInstance().PlaySound( "2D", iter->second->GetDragSoundIndex() );

		SAFE_DELETE( iter->second );
		m_mapInventory.erase( iter );

		return true;
	}

	return false;
}