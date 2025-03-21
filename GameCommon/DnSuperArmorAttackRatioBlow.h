#pragma once
#include "dnblow.h"


//슈퍼아머 공격력 비율
//- 스킬 효과 ID : 142
//- 수치 단위 : float
//- 설명 
//1) <효과 시간>동안 해당 스킬 효과가 적용된 대상의 슈퍼아머1에 대한 공격력 비율이 <효과 비율>만큼 증가합니다.
class CDnSuperArmorAttackRatioBlow : public CDnBlow, public TBoostMemoryPool< CDnSuperArmorAttackRatioBlow >
{
private:
	

public:
	CDnSuperArmorAttackRatioBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnSuperArmorAttackRatioBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
