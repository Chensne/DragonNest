#pragma once
#include "dnworldbrokenprop.h"
#include "DnHitPropComponent.h"
#include "DnSkillPropComponent.h"
#include "MAActorProp.h"


class CDnWorldBrokenDamageProp : public CDnWorldBrokenProp,
								 public MAActorProp,
								 public CDnDamageBase
{
protected:
	// 스킬(안의 상태효과)을 적용할 타입
	enum
	{
		APPLY_SKILL_HITSIGNAL_RANGE,
		APPLY_SKILL_LASTHITTER,
	};

	int m_iSkillApplyType;

	CDnHitPropComponent m_HitComponent;
	CDnSkillPropComponent m_SkillComponent;

	bool m_bEnableHitSignal;		// 라이프 타임 다 된 경우 히트 시그널 비활성화 시킴.

protected:
	//virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( int nTableID );
	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			BrokenDamageStruct* pBrokenDamageStruct = static_cast<BrokenDamageStruct*>( m_pData );
			SAFE_DELETE( pBrokenDamageStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

	virtual void _OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta );

public:
	CDnWorldBrokenDamageProp( CMultiRoom* pRoom );
	virtual ~CDnWorldBrokenDamageProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	// from CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	// from CDnWorldBrokenProp
	virtual void OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam );
};


class IBoostPoolDnWorldBrokenDamageProp : public CDnWorldBrokenDamageProp, public TBoostMemoryPool< IBoostPoolDnWorldBrokenDamageProp >
{
public:
	IBoostPoolDnWorldBrokenDamageProp( CMultiRoom *pRoom ):CDnWorldBrokenDamageProp(pRoom){}
	virtual ~IBoostPoolDnWorldBrokenDamageProp(){}
};
