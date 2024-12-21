#pragma once

#include "DnWorldOperationProp.h"
#include "DnActor.h"
#include "DnDropItem.h"

class CDnPropCondiNeedItem;
class CDnPropStateItemDrop;

class CDnWorldChestProp : public CDnWorldOperationProp, public TBoostMemoryPool< CDnWorldChestProp >
{
public:
	//enum ChestStateEnum {
	//	None = 0,
	//	Close = 0x01,
	//	Open = 0x02,
	//	CanOpen = 0x04,
	//	CantOpen = 0x08,
	//	Opening = 0x10,
	//};

	enum
	{
		CLOSED,
		TRY_OPEN,
		OPEN_SUCCESS,		// 추후에 OPENNING 시점까지 필요하게 되면 그 때 만들자.
		OPEN_FAILED,
	};

	CDnWorldChestProp( CMultiRoom *pRoom );
	virtual ~CDnWorldChestProp();

protected:
	//int m_nCloseActionIndex;
	int m_iNeedKeyID;
	int m_iItemDropGroupTableID;
	int m_iOperateState;
	bool m_bRemoveNeedKeyItem;
	int m_iNumNeedKeyItem;
	bool m_bEnableOwnership;

	// #37265 레벨 구간별 보상 값.
	// m_iItemDropGroupTableID 이 설정되어있더라도 이 구간 값들이 제대로 셋팅되어있다면 이것이 우선으로 처리된다.
	struct S_LEVEL_BOUNDS_REWARD
	{
		int iLevelStart;
		int iLevelEnd;
		int iItemDropGroupTableID;
		S_LEVEL_BOUNDS_REWARD( void ) : iLevelStart( 0 ), iLevelEnd( 0 ), iItemDropGroupTableID( 0 ) {};
	};
	DNVector( S_LEVEL_BOUNDS_REWARD ) m_vlLevelBoundsReward;

	CDnPropCondiNeedItem* m_pNeedItemCondition;
	CDnPropStateItemDrop* m_pItemDropState;

	DNVector(CDnItem::DropItemStruct) m_VecDropItemList;

protected:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( int nTableID );

	void CalcDropItemList();

	void ReleasePostCustomParam( void ) 
	{
		if( m_pData && m_bPostCreateCustomParam )
		{
			ChestStruct* pChestStruct = static_cast<ChestStruct*>( m_pData );
			SAFE_DELETE( pChestStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

public:
	//virtual bool CreateObject();
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket );

	virtual void CmdOperation( int iOperateID );

	// CDnPropOperatedCondition 에서 호출하는 함수임. 
	virtual int GetOperateState( void ) { return m_iOperateState; };

	virtual int GetNumDropItem( void ) { return (int)m_VecDropItemList.size(); };
	virtual const CDnItem::DropItemStruct* GetDropItemByIndex( int iIndex ) { return &m_VecDropItemList.at(iIndex); };
	virtual void ClearDropItemList( void ) { SAFE_DELETE_VEC( m_VecDropItemList ); };

	//void DropItems();
	//ChestStateEnum GetChestState();
};