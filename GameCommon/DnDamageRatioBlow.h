#pragma once
#include "DnBlow.h"

class CDnDamageRatioBlow : public CDnBlow, public TBoostMemoryPool< CDnDamageRatioBlow >
{
private:

public:
	CDnDamageRatioBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnDamageRatioBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
