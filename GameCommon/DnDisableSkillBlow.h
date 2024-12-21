#pragma once
#include "dnblow.h"


class CDnDisableSkillBlow : public CDnBlow, public TBoostMemoryPool< CDnDisableSkillBlow >
{
protected:
	std::vector<int> m_vecDisabledSkills;

public:

	CDnDisableSkillBlow(DnActorHandle hActor, const char* szValue);
	virtual ~CDnDisableSkillBlow(void);

	virtual void OnBegin(LOCAL_TIME LocalTime, float fDelta);
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnEnd(LOCAL_TIME LocalTime, float fDelta);

	bool IsDisabledSkill(int nSkillID);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
