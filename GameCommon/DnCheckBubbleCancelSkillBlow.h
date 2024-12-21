#pragma once
#include "dnblow.h"



class CDnCheckBubbleCancelSkillBlow : public CDnBlow,
	public TBoostMemoryPool< CDnCheckBubbleCancelSkillBlow >
{
private:
	int m_nBubbleID;
	int m_nSkillID;
	int m_nBubbleCount;
	float m_nProcessTime;

public:
	CDnCheckBubbleCancelSkillBlow(DnActorHandle hActor, const char* szValue);
	virtual ~CDnCheckBubbleCancelSkillBlow(void);

	virtual void OnBegin(LOCAL_TIME LocalTime, float fDelta);
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnEnd(LOCAL_TIME LocalTime, float fDelta);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
