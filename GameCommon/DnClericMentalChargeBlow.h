#pragma once
#include "dnblow.h"


// 클러릭(몽크)의 임팩트 블로우에서만 쓰이는 기 모으기 상태효과
class CDnClericMentalChargeBlow : public CDnBlow, public TBoostMemoryPool< CDnClericMentalChargeBlow >
{
private:
	

public:
	CDnClericMentalChargeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnClericMentalChargeBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
