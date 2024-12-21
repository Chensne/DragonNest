#pragma once
#include "dnblow.h"


class CDnDisableActionBlow : public CDnBlow, public TBoostMemoryPool< CDnDisableActionBlow >
{
public:
	CDnDisableActionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnDisableActionBlow(void);
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	std::vector<std::string> m_vecActionList;
	void SetBlowInfo();
	bool IsMatchedAction(const char* strAction);

protected:

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
