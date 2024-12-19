#pragma once
#include "dnblow.h"

class CDnMechanicModeBlow : public CDnBlow, public TBoostMemoryPool< CDnMechanicModeBlow >
{
private:
	DNVector(int) m_pVecFindSkills;
	DNVector(int) m_pVecNewSkills;

public:
	CDnMechanicModeBlow(DnActorHandle hActor, const char* szValue);
	virtual ~CDnMechanicModeBlow(void);

	virtual void OnBegin(LOCAL_TIME LocalTime, float fDelta);
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnEnd(LOCAL_TIME LocalTime, float fDelta);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
	bool CanUseSkill(int nSkillID);
	int GetReplaceSkill(int nSkillID);
};
