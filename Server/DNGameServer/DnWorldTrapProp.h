#pragma once

#include "DnWorldActProp.h"
#include "DnActor.h"
#include "DnDropItem.h"
#include "DnDamageBase.h"
#include "MAActorProp.h"
#include "DnHitPropComponent.h"


class CDnWorldTrapProp : public CDnWorldActProp, public CDnDamageBase, public MAActorProp, public TBoostMemoryPool< CDnWorldTrapProp >
{
public:
	CDnWorldTrapProp( CMultiRoom *pRoom );
	virtual ~CDnWorldTrapProp();

protected:
	virtual bool InitializeTable( int nTableID );
	virtual bool CreateObject();


protected:
	// 트랩 발동 조건 관련.
	enum
	{
		LOOP,					// 정해진 액션을 계속 실행 -> 이제 사용 안함. 트리거에서 직접 활성 액션 제어를 하도록.. TRIGGER_HANDLING 과 동일함.
		BOUNDING_BOX_CHECK,		// 바운딩 박스 체크해서 Activate 됨
		TRIGGER_HANDLING,		// 트리거에서 완전히 제어함.. 보통 액션 프랍과 마찬가지가 되지만 액션 프랍에서는 hit 처리가 안됨.
	};

	int m_nIdleElementIndex;
	int m_nActivateElementIndex;
	int m_nNowActionElementIndex;

	CDnHitPropComponent m_HitComponent;

	int m_iActivateType;

	// idle 되어야 하는 상황이라면, (SetActionQueue("Idle")이 호출되었다면)
	bool m_bIdle;

public:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f , float fStartFrame = 0.f );
	virtual void OnFinishAction( const char* szPrevAction, LOCAL_TIME time );

	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }
	//DnActorHandle GetMonsterHandle() { return m_hMonster; }

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };

	virtual void OnChangeAction( const char *szPrevAction );
};