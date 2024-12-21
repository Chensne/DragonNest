#pragma once
#include "dnblow.h"

// 물리 레인지 방어력 절대 변경
class CDnAbRangePDefenseBlow : public CDnBlow, 
							   public TBoostMemoryPool< CDnAbRangePDefenseBlow >
{
private:
	

public:
	CDnAbRangePDefenseBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAbRangePDefenseBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
