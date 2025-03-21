#pragma once


#ifdef PRE_ADD_CASHREMOVE


#include "DnInventory.h"

class CDnItem;

class CDnCashRemoveInventory : public CDnInventory
{
private:

	std::map< INT64, __time64_t > m_mapBeginExpireTimes; // 아이템이 캐시대기탭으로 들어온 시간 - 대기시간계산용.

public:
	CDnCashRemoveInventory(void);
	virtual ~CDnCashRemoveInventory(void);

protected:

public:

	// Override - CDnInventory //
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ){} // 아이템의 CoolTime 을 돌리지 않기때문에 재정의함.


	void CreateCashItem( TItem &item, int nSlotIndex, __time64_t beginTime, __time64_t expireTime, bool bExpireComplete );

	// bMoving - 인벤내 이동인가.
	// 실제로 서버에서 인벤,장비창,창고 안에서 아이템 돌리는건 다른 메세지를 사용한다.
	virtual bool InsertItem( CDnItem *pItem, bool bMoving = false );
	virtual bool RemoveItem( int nSlotIndex );

	virtual void SetUsableSlotCount( int nCount );

	void DecreaseDurability( int nValue );
	void DecreaseDurability( float fValue );

	__time64_t GetTimeBySN( INT64 nSerial );
};


#endif