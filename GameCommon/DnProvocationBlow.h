#pragma once
#include "dnblow.h"

// 도발 상태효과 (#11916)
class CDnProvocationBlow : public CDnBlow, 
						   public TBoostMemoryPool< CDnProvocationBlow >
{
private:
	//float m_fRange;
	//float m_fRangeSQ;
	//float m_fRangeAngleByRadian;

public:
	CDnProvocationBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnProvocationBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
