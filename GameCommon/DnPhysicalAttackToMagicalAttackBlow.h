#pragma once
#include "dnblow.h"


class CDnPhysicalAttackToMagicalAttackBlow : public CDnBlow, public TBoostMemoryPool< CDnPhysicalAttackToMagicalAttackBlow >
{
public:
	CDnPhysicalAttackToMagicalAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPhysicalAttackToMagicalAttackBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
