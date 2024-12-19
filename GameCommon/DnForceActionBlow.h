#pragma once
#include "dnblow.h"

class CDnCantMoveBlow;
class CDnCantAttackBlow;


// 강제 상태 이행 상태효과. 84번
class CDnForceActionBlow : public CDnBlow, public TBoostMemoryPool< CDnForceActionBlow >
{
private:
	CDnCantMoveBlow*			m_pCantMoveBlow;
	CDnCantAttackBlow*			m_pCantAttackBlow;
	LOCAL_TIME					m_StartTime;
	float						m_fOriAddedHeight;

public:
	CDnForceActionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnForceActionBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
