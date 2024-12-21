#pragma once
#include "dnblow.h"


class CDnGhoulTransformBlow : public CDnBlow, public TBoostMemoryPool< CDnGhoulTransformBlow >
{
public:
	CDnGhoulTransformBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnGhoulTransformBlow(void);
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void SetTransformActorList();

protected:

	std::vector<int> m_vecTransformActorList;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
