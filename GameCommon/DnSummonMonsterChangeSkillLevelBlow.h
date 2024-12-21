#pragma once
#include "dnblow.h"


// 몬스터 소환시 해당 몬스터의 모든 소유 스킬의 레벨을 이 상태효과에 정의된 레벨로 바꿈.
class CDnSummonMonsterChangeSkillLevelBlow : public CDnBlow, 
											 public TBoostMemoryPool< CDnSummonMonsterChangeSkillLevelBlow >
{
public:
	CDnSummonMonsterChangeSkillLevelBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnSummonMonsterChangeSkillLevelBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:
	float m_fRate;	//확률값

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};
