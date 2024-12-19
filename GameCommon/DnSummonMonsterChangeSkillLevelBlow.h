#pragma once
#include "dnblow.h"


// ���� ��ȯ�� �ش� ������ ��� ���� ��ų�� ������ �� ����ȿ���� ���ǵ� ������ �ٲ�.
class CDnSummonMonsterChangeSkillLevelBlow : public CDnBlow, 
											 public TBoostMemoryPool< CDnSummonMonsterChangeSkillLevelBlow >
{
public:
	CDnSummonMonsterChangeSkillLevelBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnSummonMonsterChangeSkillLevelBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:
	float m_fRate;	//Ȯ����

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};
