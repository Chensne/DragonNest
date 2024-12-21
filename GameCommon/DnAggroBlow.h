#pragma once
#include "dnblow.h"

// 어그로 조절 상태효과. 
// 몬스터 어그로 시스템 쪽에서 폴링하면서 인자로 정해진 값으로 어그로를 조절함.
// 인자로 받는 값은 곧바로 곱셈되는 값. (1.0 이면 그대로)
class CDnAggroBlow : public CDnBlow, 
					 public TBoostMemoryPool< CDnAggroBlow >
{
private:
	float m_fRange;
	float m_fRangeSQ;
	float m_fRangeAngleByRadian;
	DNVector(DnActorHandle) m_vlhApplyMonsters;

public:
	CDnAggroBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAggroBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
