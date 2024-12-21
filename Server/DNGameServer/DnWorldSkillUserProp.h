#pragma once
#include "DnWorldActProp.h"
#include "MAActorProp.h"


// 스킬 쓰는 프랍.. 액터측에서 사용하는 MASkillUser 는 사용하지 않는다.
// 일단 그냥 구현해서 가능성을 타진해보고 FSM으로 조각내보도록 하자.
class CDnWorldSkillUserProp : public CDnWorldActProp,
							  public MAActorProp
{
private:
	enum
	{
		ONCE,
		CONTINUOUS,
	};

	DnSkillHandle m_hSkill;
	int m_iUseType;
	float m_fCheckRadius;
	set<DWORD> m_setAffectedActorsID;

	int m_iActivateElementIndex;
	int m_iIdleElementIndex;
	int m_iNowActionElementIndex;


protected:
	bool InitializeTable( int nTableID );
	bool CreateObject( void );

public:
	CDnWorldSkillUserProp( CMultiRoom* pRoom );
	virtual ~CDnWorldSkillUserProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnSignal( SignalTypeEnum Type, void* pPtr, LOCAL_TIME LocalTime, 
						   LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

};