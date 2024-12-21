#pragma once
#include "dnblow.h"

#if defined(PRE_ADD_BUFF_STATE_LIMIT)
class CDnBasicBlow;
#endif // PRE_ADD_BUFF_STATE_LIMIT

class CDnIntelligenceToPhysicAttackBlow : public CDnBlow, public TBoostMemoryPool< CDnIntelligenceToPhysicAttackBlow >
{
public:
	CDnIntelligenceToPhysicAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnIntelligenceToPhysicAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:
#if defined(PRE_ADD_BUFF_STATE_LIMIT)
	CDnBasicBlow* m_pBasicBlow;

#else
	bool NeedRefreshAll( void );

	CDnState m_State;

	bool m_bNeedRefreshState;
	CDnActorState::StateTypeEnum m_eRefreshState;
#endif // PRE_ADD_BUFF_STATE_LIMIT


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
