#pragma once
#include "dnblow.h"


// #33316 관련 물리 공격력 최종 데미지 비율 적용 상태효과. 0.3 이면 30% 증가.
class CDnFinalPDamageBlow : public CDnBlow, public TBoostMemoryPool< CDnFinalPDamageBlow >
{
private:
	

public:
	CDnFinalPDamageBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFinalPDamageBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
