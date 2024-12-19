#pragma once
#include "idnskillprocessor.h"


class CDnState;

// 클러릭의 임팩트 블로우를 위한 전용 발현 타입임. 에너지 차지 스킬 버프가 활성화 되어있는지 체크해서 해당 수치만큼 공격력을 높임
class CDnImpactBlowProcessor : public IDnSkillProcessor, public TBoostMemoryPool< CDnImpactBlowProcessor >
{
private:
	CDnState*					m_pState;
	int							m_iNumChargeBlow;

public:
	CDnImpactBlowProcessor( DnActorHandle hActor );
	virtual ~CDnImpactBlowProcessor(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	int GetNumArgument( void ) { return 0; };
};
