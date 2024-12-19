#pragma once
#include "idnskillprocessor.h"
#include "DnSkill.h"

#ifdef _GAMESERVER

// 아이템 접두어 시스템에서 사용하는 스킬에서 사용할 발현타입. (#23818)
// 평타로 피격된 대상에게 일정 확률로 스킬의 Target 으로 지정된 상태효과들을 부여한다.
// 추후 쿨타임 개념이 포함될 수도 있다.
class CDnApplySEWhenTargetNormalHitProcessor : public IDnSkillProcessor, 
											   public TBoostMemoryPool< CDnApplySEWhenTargetNormalHitProcessor >
{
private:
	float m_fProbability;

public:
	CDnApplySEWhenTargetNormalHitProcessor( DnActorHandle hActor, float fProbability );
	virtual ~CDnApplySEWhenTargetNormalHitProcessor( void );

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void OnNormalHitSuccess( DnActorHandle hHittedTarget );		// 일반 평타가 성공했을 때.

	int GetNumArgument( void ) { return 0; };
};
#endif // #ifdef _GAMESERVER
