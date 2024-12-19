#pragma once
#include "dnblow.h"

// 물리 밀리 방어력 비율 변경
class CDnRtMeleePDefenseBlow : public CDnBlow, 
							   public TBoostMemoryPool< CDnRtMeleePDefenseBlow >
{
private:
	

public:
	CDnRtMeleePDefenseBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnRtMeleePDefenseBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
