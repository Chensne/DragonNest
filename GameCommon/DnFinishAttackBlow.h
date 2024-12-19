#pragma once
#include "dnblow.h"


class CDnFinishAttackBlow : public CDnBlow, public TBoostMemoryPool< CDnFinishAttackBlow >
{
public:
	CDnFinishAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFinishAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	virtual void OnTargetHit( DnActorHandle hTargetActor );
#endif

protected:
	float m_fDestHpRate;
	float m_fAddDamageRate;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
