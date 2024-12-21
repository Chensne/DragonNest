#pragma once

#include "DnWorldActProp.h"
#include "DnActor.h"
//#include "DnDropItem.h"
//#include "DnDamageBase.h"

class CDnPropStateDoAction;
class CDnWorldBrokenProp : public CDnWorldActProp
{
public:
	CDnWorldBrokenProp();
	virtual ~CDnWorldBrokenProp();

protected:
	int m_nBreakActionCount;

	int m_nDurability;
	int m_nHitParticle;
	std::vector<int> m_nVecHitSoundList;
	float m_fSoundRange;
	float m_fRollOff;

	int m_nItemDropGroupTableID;
	std::vector<CDnItem::DropItemStruct> m_VecDropItemList;

	int m_iLastRandomSeed;
	bool m_bHitted;
	CDnPropStateDoAction* m_pBrokenActionState;
	bool m_bBroken;
	bool m_bLifeTimeEnd;

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

	virtual void OnDamageWriteAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream ) {}
	virtual void OnDamageReadAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream ) {}
	//void DropItems();

	void LoadHitEffectData( int nTableID );
public:
	// CDnWorldProp Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, CDnActor::SHitParam &HitParam );
	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );

	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	virtual int GetDurability( void ) { return m_nDurability; };
	virtual int GetLastHitRandomSeed( void ) { return m_iLastRandomSeed; };
	virtual bool IsHittedAndReset( void );

	virtual int GetNumDropItem( void ) { return (int)m_VecDropItemList.size(); };
	virtual const CDnItem::DropItemStruct* GetDropItemByIndex( int iIndex ) { return &m_VecDropItemList.at(iIndex); };
	virtual void ClearDropItemList( void ) { SAFE_DELETE_VEC( m_VecDropItemList ); };

	//
	virtual bool bIsCanBroken(){ return true; }

	void UpdatePropBreakToHitter( CDnDamageBase* pHitter );
};