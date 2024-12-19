#pragma once

#include "DnWorldOperationProp.h"
#include "DnActor.h"
#include "DnDropItem.h"

class CDnWorldChestProp : public CDnWorldOperationProp
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

	CDnWorldChestProp();
	virtual ~CDnWorldChestProp();

protected:
	//int m_nCloseActionIndex;
	int m_iNeedKeyID;
	int m_iItemDropGroupTableID;
	bool m_bRemoveNeedKeyItem;
	int m_iNumNeedKeyItem;

	int m_iServerOperateResult;

	// #37265 레벨 구간별 보상 값.
	// m_iItemDropGroupTableID 이 설정되어있더라도 이 구간 값들이 제대로 셋팅되어있다면 이것이 우선으로 처리된다.
	struct S_LEVEL_BOUNDS_REWARD
	{
		int iLevelStart;
		int iLevelEnd;
		int iItemDropGroupTableID;
		S_LEVEL_BOUNDS_REWARD( void ) : iLevelStart( 0 ), iLevelEnd( 0 ), iItemDropGroupTableID( 0 ) {};
	};
	vector<S_LEVEL_BOUNDS_REWARD> m_vlLevelBoundsReward;
	vector<CDnItem::DropItemStruct> m_VecDropItemList;

protected:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{
		if( m_pData && m_bPostCreateCustomParam )
		{
			ChestStruct* pChestStruct = static_cast<ChestStruct*>( m_pData );
			SAFE_DELETE( pChestStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};
	//void DropItems();

public:
	//virtual bool CreateObject();

	virtual void CmdOperation();

	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );

	virtual int GetOperateState( void ) { return m_iServerOperateResult; };

	bool CanOpen( void );

	virtual int GetCrosshairType();
	//ChestStateEnum GetChestState();
};