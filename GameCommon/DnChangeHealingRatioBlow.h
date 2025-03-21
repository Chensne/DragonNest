#pragma once
#include "dnblow.h"


//치료비율변경(HealingAdd)
//- 스킬 효과 ID : 141
//- 수치 단위 : float
//- 설명 
//1)HP가 회복되는 행동(물약 섭취, 힐링 스킬 발동 등)에 의해 HP를 회복 시킬 경우 <효과 비율>만큼 회복량이 늘어납니다.

class CDnChangeHealingRatioBlow : public CDnBlow, 
								   public TBoostMemoryPool< CDnChangeHealingRatioBlow >
{
private:
	
public:
	CDnChangeHealingRatioBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeHealingRatioBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:
	virtual void OnSetParentSkillInfo();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
