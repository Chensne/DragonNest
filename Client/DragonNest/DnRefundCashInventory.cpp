#include "StdAfx.h"

#ifdef PRE_ADD_CASHSHOP_REFUND_CL

#include "DnRefundCashInventory.h"
#include "DnItem.h"
#include "DnInventoryDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

void CDnRefundCashInventory::CreateCashItem( TItem &item, int nSlotIndex, CASHITEM_SN sn, INT64 dbid )
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

	CDnItem *pItem = m_pItemTask->CreateItem( item );

	if( pItem == NULL )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, ������(%d) ������ ����!", item.nItemID );
		return;
	}

	pItem->SetSlotIndex( nSlotIndex );
	pItem->SetCashItemSN(sn);
	pItem->SetSerialID(dbid);	//	note by kalliste : Use DBID as SerialID at refund inventroy.

	m_mapInventory[pItem->GetSlotIndex()] = pItem;
	m_pInvenDlg->SetItem( pItem );
}

bool CDnRefundCashInventory::InsertItem( CDnItem *pItem, bool bMoving )
{
	_ASSERT(0);
	return false;
}

bool CDnRefundCashInventory::RemoveItem( int nSlotIndex )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.find( nSlotIndex );
	if( iter != m_mapInventory.end() )
	{
		CDnItem* pItem = iter->second;
		if (pItem)
		{
			m_pInvenDlg->ResetSlot(pItem);

			SAFE_DELETE(pItem);
			m_mapInventory.erase( iter );

			SortRefundCashInventory();
		}

		return true;
	}
	else
	{
		_ASSERT(0);
		return false;
	}

	return false;
}

bool CDnRefundCashInventory::RemoveItem(INT64 dbid, std::wstring& removeItemName)
{
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for (; iter != m_mapInventory.end(); ++iter)
	{
		CDnItem* pItem = (*iter).second;
		if (pItem && pItem->GetSerialID() == dbid)
		{
			m_pInvenDlg->ResetSlot( pItem );

			if (CDnCashShopTask::GetInstance().IsPackageItem(pItem->GetCashItemSN()))
			{
				const SCashShopItemInfo* pInfo = CDnCashShopTask::GetInstance().GetItemInfo(pItem->GetCashItemSN());
				if (pInfo)
					removeItemName = pInfo->nameString;
			}
			else
			{
				removeItemName = pItem->GetName();
			}

			SAFE_DELETE( pItem );
			m_mapInventory.erase( iter );

			SortRefundCashInventory();

			return true;
		}
	}

	return false;
}

void CDnRefundCashInventory::SetUsableSlotCount( int nCount )
{ 
	if (m_pInvenDlg == NULL) return;
	m_nUsableSlotCount = nCount;
}

static bool CompareRefundCashInvenItem( CDnItem *s1, CDnItem *s2 )
{
	return (s1->GetSerialID() < s2->GetSerialID());
}

bool CDnRefundCashInventory::SortRefundCashInventory()
{
	std::vector<CDnItem*> vecSortItem;
	vecSortItem.clear();

	// �������� �Ѱ��� ������,
	if( (int)m_mapInventory.size() == 0 )
		return false;

	// �� ��ȯ
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();
	vecSortItem.reserve(m_mapInventory.size());
	for( iter; iter != iterEnd; ++iter )
		vecSortItem.push_back(iter->second);

	std::sort( vecSortItem.begin(), vecSortItem.end(), CompareRefundCashInvenItem );

	bool bChanged = false;
	for( int i = 0; i < (int)vecSortItem.size(); ++i )
	{
		if( vecSortItem[i]->GetSlotIndex() != i )
		{
			bChanged = true;
			break;
		}
	}

	// ���� �� ���Թ�ȣ�� �ϳ��� �ٲ��� �ʾҴٸ�,
	if( bChanged == false )
		return false;

	// �ڿ������� ���� �� �� ��(�̷��� ���� �ڿ� ���� ������ ���� ���� ������ �� �ִ�.)
	std::map<int,CDnItem*>::reverse_iterator riter = m_mapInventory.rbegin();
	while( riter != m_mapInventory.rend() )
	{
		INVENTORY_MAP_ITER iter2 = m_mapInventory.find( riter->first );
		if( iter2 != m_mapInventory.end() )
		{
			m_pInvenDlg->ResetSlot( iter2->second );
			m_mapInventory.erase( iter2 );
		}
	}

	// ���ĵ� ����� �籸��
	for( int i = 0; i < (int)vecSortItem.size(); ++i )
	{
		vecSortItem[i]->SetSlotIndex(i);
		m_mapInventory[i] = vecSortItem[i];
		m_pInvenDlg->SetItem(vecSortItem[i]);
	}

	return true;
}

INT64 CDnRefundCashInventory::GetDBID(int slotIdx) const
{
	INVENTORY_MAP_ITER_CONST iter = m_mapInventory.find(slotIdx);
	if( iter != m_mapInventory.end() )
	{
		const CDnItem* pItem = (*iter).second;
		return pItem->GetSerialID();
	}

	return -1;
}

#endif // PRE_ADD_CASHSHOP_REFUND_CL