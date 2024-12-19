#pragma once
#include "dnblow.h"


class CDnManaShieldBlow : public CDnBlow, public TBoostMemoryPool< CDnManaShieldBlow >
{
public:
	CDnManaShieldBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnManaShieldBlow(void);
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void CalcManaShield( const float fOriginalDamage , float &fAbsorbDamage, int &nAbsorbSP );

protected:

	float m_fAbsorbDamageRatio;
	float m_fAbsorbMPRatio;


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
