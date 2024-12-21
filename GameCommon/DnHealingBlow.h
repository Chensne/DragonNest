#pragma once
#include "dnblow.h"


//치료(Healing)
//- 스킬 효과 ID : 140
//- 수치 단위 : float
//- 설명 
//1)사용자의 <마법 공격력>*<효과 비율>만큼 대상의 HP를 회복시켜준다.

class CDnHealingBlow : public CDnBlow, 
					   public TBoostMemoryPool< CDnHealingBlow >
{
private:
	

public:
	CDnHealingBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnHealingBlow(void);

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
