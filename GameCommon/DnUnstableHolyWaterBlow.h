#pragma once
#include "dnblow.h"


class CDnUnstableHolyWaterBlow : public CDnBlow, public TBoostMemoryPool< CDnUnstableHolyWaterBlow >
{
protected:

	std::vector<int> m_vecTransformActorList;
	int m_nHolyWaterChance;

public:

	CDnUnstableHolyWaterBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnUnstableHolyWaterBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void SetHolyWaterInfo();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
