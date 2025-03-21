#pragma once
#include "dnblow.h"


// #33316 관련 마법 공격력 최종 데미지 비율 적용 상태효과. 0.3이면 30% 증가.
class CDnFinalMDamageBlow : public CDnBlow, public TBoostMemoryPool< CDnFinalMDamageBlow >
{
private:


public:
	CDnFinalMDamageBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFinalMDamageBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
