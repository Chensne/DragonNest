#pragma once
#include "dnblow.h"


// 인선씨가 스테이지 클리어 점수 나오는 동안 사용할 공격 불가 상태효과.
class CDnCantAttackBlow : public CDnBlow, public TBoostMemoryPool< CDnCantAttackBlow >
{
private:
	

public:
	CDnCantAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCantAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};
