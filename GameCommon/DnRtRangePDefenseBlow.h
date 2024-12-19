#pragma once
#include "dnblow.h"


// 물리 레인지 방어력 비율 변경
class CDnRtRangePDefenseBlow : public CDnBlow, 
							   public TBoostMemoryPool< CDnRtRangePDefenseBlow >
{
private:
	

public:
	CDnRtRangePDefenseBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnRtRangePDefenseBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
