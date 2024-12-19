#pragma once
#include "DNPacket.h"
#include "DnItem.h"

class CDnItemTask;
class CDnInventoryDlg;

class CDnInventory
{
public:
	typedef std::map<int,CDnItem*>		INVENTORY_MAP;
	typedef INVENTORY_MAP::iterator		INVENTORY_MAP_ITER;
	typedef INVENTORY_MAP::const_iterator	INVENTORY_MAP_ITER_CONST;

	// 캐시 인벤토리 개선에 따라 서버는 더 이상 인덱스 기반으로 처리되지 않는다.
	// 하지만 클라이언트 경우엔 인벤토리 모양이 일반 인벤토리와 같고,
	// 지금까지 사용되던 코드를 어느정도 유지해야하기때문에,
	// 슬롯인덱스를 키로 갖는 std::map을 그대로 사용할 것이다.

	CDnInventory(void);
	virtual ~CDnInventory(void);

protected:
	INVENTORY_MAP m_mapInventory;
	int m_nUsableSlotCount;			// Note : 사용가능한 창고 슬롯의 갯수
	
	CDnInventoryDlg *m_pInvenDlg;
	CDnItemTask *m_pItemTask;

	// FindFirstEmptyIndex함수에서 사용하는 임시 데이터
	std::vector<bool> m_vecTempCheck;

	// 정렬 데이터
	std::vector<CDnItem*> m_vecSortItem;
	TSortSlot m_SortSlotInfo[INVENTORYMAX];		// INVENTORYMAX가 150으로 제일 커서 넉넉히 들고있겠다.
	bool m_bSortRequesting;

#if defined(PRE_PERIOD_INVENTORY)
	INVENTORY_MAP m_mapNotSortItem;
	bool m_bSortPeriod;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

public:
	void SetInventoryDialog( CDnInventoryDlg *pDialog ) { m_pInvenDlg = pDialog; }
	void SetItemTask( CDnItemTask *pItemTask ) { m_pItemTask = pItemTask; }

public:
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false ) = 0;
	virtual bool RemoveItem( int nSlotIndex ) = 0;

public:
	void CreateItem( TItemInfo &itemInfo );
	CDnItem *GetItem( int nSlotIndex );
	CDnItem* FindItem( int nItemTableID, char cOption = -1 );
	int FindItemSlotIndex( int nItemTableID );
	CDnItem* FindItemFromSerialID( INT64 nSerialID );
	void FindItemFromItemType( eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult );
	void FindItemFromItemType( eItemTypeEnum Type, std::vector<CDnItem *> &pVecItem, std::vector<int> &pVecIndex );
	void BeginSameSkillCoolTime( const CDnItem *pItem, int nSkillID );
	int GetItemCount( int nItemTableID, char cOption = -1 );
	//blondy 인벤을 돌며 현재 디세이블 해야 되는 아이템을 셋팅
	void SetDisableItemInven( int iGameMode );
	//blondyend
	int FindItemList( int nItemTableID, char cOption, std::vector<CDnItem *> &pVecResultList );

	INVENTORY_MAP& GetInventoryItemList();

	void RefreshInventory();
	void ClearInventory();

	int ScanItemFromID( int nItemTableID, std::vector<CDnItem *> *pVecResult );
	bool RequestUseItem( int nSlotIndex, char cType = 0 );

	// 캐시인벤 리프레쉬 될때 비어있는 슬롯 찾기위해 만든 함수입니다.
	// 인벤토리 슬롯의 닫혀있음 등은 고려하지 않고 그냥 map에서 빈 곳을 찾으니,
	// 일반 인벤토리에서는 사용하지 않는 것을 권장합니다.
	int FindFirstEmptyIndex();

	virtual void SetUsableSlotCount( int nCount );
	int GetUsableSlotCount() const { return m_nUsableSlotCount; }
	// 인벤안에 있는 아이템들의 수리비를 계산한다. 현재 CharInven에서만 사용된다.
	int GetRepairInvenPrice();

	// 시리얼ID같은 아이템 얻기. 현재 사용하지 않는다.
	//CDnItem *GetItemFromSerialID( INT64 nSerialID );

	// 정렬
	bool PrepareSort( bool bPeriod = false );
	bool Sort( bool bServerSucceed );
	TSortSlot *GetSortArray() { return m_SortSlotInfo; }
	int GetSortCount() { return (int)m_vecSortItem.size(); }

	// 캐시용 정렬. 일반템과 그냥 따로 구분해두겠다.
	bool SortCashInventory();

public:

#ifdef PRE_ADD_CASHREMOVE
	// 아이템의 CoolTime 을 돌리지 않는 Dlg 를 위해서 가상함수로 변경.
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
#else
	void Process( LOCAL_TIME LocalTime, float fDelta );
#endif

};
