#pragma once
#include "DnBlow.h"

class CDnFrameBlow : public CDnBlow, public TBoostMemoryPool< CDnFrameBlow >
{
private:

public:
	CDnFrameBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnFrameBlow(void);

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
