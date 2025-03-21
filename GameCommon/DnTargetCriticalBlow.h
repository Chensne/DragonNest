#pragma once
#include "dnblow.h"

//히트 되는 대상이 이 상태효과를 가지고 있으면 타격자의 크리티컬 확률을 증가 시켜 준다.
class CDnTargetCriticalBlow  : public CDnBlow, public TBoostMemoryPool< CDnTargetCriticalBlow >
{
public:
	CDnTargetCriticalBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnTargetCriticalBlow(void);
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
