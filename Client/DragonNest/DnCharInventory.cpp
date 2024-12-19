#include "StdAfx.h"
#include "DnCharInventory.h"
#include "DnItem.h"
#include "DnInventoryDlg.h"
#include "DnInterface.h"
#include "DnMainDlg.h"
#include "DnMasterTask.h"
#include "DnMainMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCharInventory::CDnCharInventory(void)
	: m_emInvenType(typeItemInven)
{
}

CDnCharInventory::~CDnCharInventory(void)
{
}

bool CDnCharInventory::InsertItem( CDnItem *pItem, bool bMoving )
{
	if (pItem == NULL)
	{
		_ASSERT(0 && "CDnCharInventory::InsertItem - ITEM is NULL\n");
		return false;
	}

	if( !m_pInvenDlg )
	{
		CDebugSet::ToLogFile( "CDnCharInventory::InsertItem, m_pInvenDlg is NULL!" );
		return false;
	}

	int nExistCnt = GetItemCount(pItem->GetClassID());

	// 이미 존재했던 아이템이 갯수만 변경 된건지 확인 여부
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

	// 이미 DnInvenSlotDlg 에서 소리를 내주고 있어요..중복으로 납니다.
//	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	// 인벤토리 아이템 관련 사운드에 대해 잠깐 적어두겠다. by hayannal2009
	//
	// 우선 기본적으로 서버응답이 있을때(SC_MOVEITEM, SC_REFRESHINVEN, SC_REFRESHCASHINVEN, SC_PICKUP, SC_USEITEM)
	// 적당한 타입의 사운드를 플레이하는게 원칙이다.
	// 그리고 그 외 ui단에서 처리하는건 로컬에서 마우스로 집었을때 등인데, 이건 일일이 다이얼로그에서 해주는 방법 외엔 없다.
	//
	// 마지막으로 부연 설명을 달자면,
	// 위의 패킷 받을때 사운드 플레이를 하지 않고,
	// CharInventory의 Insert, Remove에서 할 경우 -> SC_PICKUP에서 루팅사운드 내고, 인벤 들어올때 또 나서 안된다. 게다가 템을 옆자리로 옮길때 두번 소리나게 된다.
	// 이런 이유로 CharInventory에서 처리하지 않는거니 여기에다 사운드 플레이 추가하지 않길 바란다.

	m_mapInventory[pItem->GetSlotIndex()] = pItem;
	m_pInvenDlg->SetItem( pItem );

	int nNowCount = GetItemCount(pItem->GetClassID());

	switch( pItem->GetItemType() ) {
		case ITEMTYPE_NORMAL:
		case ITEMTYPE_COOKING:
		case ITEMTYPE_RETURN_HOME:
			GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
			break;
	}

	// 원래 존재하던 아이템보다 늘어났을때 
	int nDeltaCnt = nNowCount - nExistCnt;
	if ( nDeltaCnt > 0 && bMoving == false )
	{
		// 아이템을 더 얻었다고 알람메세지를 해준다.
		if( pItem->GetItemType() == ITEMTYPE_QUEST )
		{
			GetInterface().ShowSymbolAlarmDialog( pItem->GetName(), pItem, nDeltaCnt, textcolor::WHITE, 3.0f );
		}
		else
		{
			GetInterface().ShowItemAlarmDialog( pItem->GetName(), pItem, nDeltaCnt, textcolor::WHITE, 3.0f );
		}
	}

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
	if( pMainMenuDlg )
	{
		pMainMenuDlg->RefreshCompoundNotifyPercentage();
	}
#endif

	return true;
}

bool CDnCharInventory::RemoveItem( int nSlotIndex )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.find( nSlotIndex );
	if( iter != m_mapInventory.end() )
	{
		// 이미 DnInvenSlotDlg 에서 소리를 내주고 있어요..중복으로 납니다.
//		CEtSoundEngine::GetInstance().PlaySound( "2D", iter->second->GetDragSoundIndex() );

		eItemTypeEnum emItemType = iter->second->GetItemType();
		m_pInvenDlg->ResetSlot( iter->second );

		SAFE_DELETE( iter->second );
		m_mapInventory.erase( iter );

		switch( emItemType ) {
			case ITEMTYPE_NORMAL:
			case ITEMTYPE_COOKING:
			case ITEMTYPE_RETURN_HOME:
				GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
				break;
		}

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg )
		{
			pMainMenuDlg->RefreshCompoundNotifyPercentage();
		}
#endif

		return true;
	}

	return false;
}

void CDnCharInventory::SetUsableSlotCount( int nCount )
{ 
	if (m_pInvenDlg == NULL) return;
	m_nUsableSlotCount = nCount;

	if( m_emInvenType == typeItemInven )
	{
		m_pInvenDlg->SetUseItemCnt( nCount );
	}
	else
	{
		m_pInvenDlg->SetUseQuestItemCnt( nCount );
	}
}


void CDnCharInventory::DecreaseDurability( int nValue )
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

void CDnCharInventory::DecreaseDurability( float fValue )
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
