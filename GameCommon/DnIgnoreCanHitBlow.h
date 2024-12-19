#pragma once
#include "dnblow.h"

// 이슈 #19619
// 타임리스트릭션 스킬처럼 데미지 없는 히트시그널을 사용해서 상태효과를 부여하는 스킬들의 경우
// 대상의 CanHit 상태에 따라 적용되고 안될 수가 있다. 
// 이 상태효과는 해당 상태효과가 걸려있는 주체의 CDnActor::IsHittable() 함수에서 CanHit 상태를 
// 무시하고 무조건 true 로 처리하도록 한다.
class CDnIgnoreCanHitBlow : public CDnBlow,
							public TBoostMemoryPool< CDnIgnoreCanHitBlow >
{
private:

public:
	CDnIgnoreCanHitBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnIgnoreCanHitBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};
