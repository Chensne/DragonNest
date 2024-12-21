#pragma once

#include "DnWorldActProp.h"
#include "DnActor.h"
//#include "DnDropItem.h"

class CDnPropStateDoAction;
class CDnPropStateTrigger;

class CDnWorldBrokenProp : public CDnWorldActProp
{
public:
	CDnWorldBrokenProp( CMultiRoom *pRoom );
	virtual ~CDnWorldBrokenProp();

protected:
	//int m_nBreakActionCount;
	int m_nDurability;
	bool m_bHittable;
	bool m_bBroken;
	bool m_bNoDamage;
	LOCAL_TIME m_LastDamageTime;
	CDnDamageBase *m_pLastHitObject;
	int m_nItemDropGroupTableID;

	DNVector(CDnItem::DropItemStruct) m_VecDropItemList;

	int m_nLastHitUniqueID;

	int m_iLastRandomSeed;
	bool m_bHitted;			// 한번 폴링 되고 리셋됨.

	CDnPropStateDoAction* m_pBrokenActionState;
	CDnPropStateTrigger* m_pTriggerActionState;

protected:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( int nTableID );
	virtual bool CreateObject();
	void ReleasePostCustomParam( void ) 
	{
		if( m_pData && m_bPostCreateCustomParam )
		{
			BrokenStruct* pBrokenStruct = static_cast<BrokenStruct*>( m_pData );
			SAFE_DELETE( pBrokenStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

	void _OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined( PRE_TRIGGER_TEST )
public:
	virtual void CalcDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam );
protected:
#else // #if defined( PRE_TRIGGER_TEST )
	virtual void CalcDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam );
#endif // #if defined( PRE_TRIGGER_TEST )
	//void DropItems();

	virtual void OnDamageWriteAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream ) {}
	virtual void OnDamageReadAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream ) {}

public:
	// CDnWorldProp Message
	virtual void OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam );

	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	bool IsHittable( CDnDamageBase *pHitter, LOCAL_TIME LocalTime, int iHitUniqueID = -1 );
	bool IsBroken() { return m_bBroken; }
	void EnableNoDamage( bool bEnable ) { m_bNoDamage = bEnable; }

	virtual int GetDurability( void ) { return m_nDurability; };
	virtual int GetLastHitRandomSeed( void ) { return m_iLastRandomSeed; };
	virtual bool IsHittedAndReset( void );

	virtual int GetNumDropItem( void ) { return (int)m_VecDropItemList.size(); };
	virtual const CDnItem::DropItemStruct* GetDropItemByIndex( int iIndex ) { return &m_VecDropItemList.at(iIndex); };
	virtual void ClearDropItemList( void ) { SAFE_DELETE_VEC( m_VecDropItemList ); };

	virtual bool bIsCanBroken(){ return true; }
	virtual void OnSyncComplete( CDNUserSession* pBreakIntoGameSession=NULL );

	virtual bool IsBrokenType( void ) { return true; };

	void UpdatePropBreakToHitter( CDnDamageBase* pHitter );
};

class IBoostPoolDnWorldBrokenProp : public CDnWorldBrokenProp, public TBoostMemoryPool< IBoostPoolDnWorldBrokenProp >
{
public:
	IBoostPoolDnWorldBrokenProp( CMultiRoom *pRoom ):CDnWorldBrokenProp(pRoom){}
	virtual ~IBoostPoolDnWorldBrokenProp(){}
};
