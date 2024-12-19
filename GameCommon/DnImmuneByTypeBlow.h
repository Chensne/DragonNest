#pragma once
#include "dnblow.h"

class CDnImmuneByType : public CDnBlow, public TBoostMemoryPool< CDnImmuneByType >
{
protected:
	

public:
	CDnImmuneByType( DnActorHandle hActor, const char* szValue );
	virtual ~CDnImmuneByType(void);

private:
	int m_nImmuneType;

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	int GetImmuneTypeValue() { return m_nImmuneType; }

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
