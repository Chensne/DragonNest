#pragma once
#include "DnBlow.h"

class CDnHPIncBlow : public CDnBlow, public TBoostMemoryPool< CDnHPIncBlow >
{
public:
	CDnHPIncBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnHPIncBlow(void);

protected:
	float m_fElapsedTime;
	bool m_bFromRebirth;
	bool m_bKillingBlow;

#if defined(PRE_FIX_66687)
	bool m_bPartsHP;
#endif // PRE_FIX_66687

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void FromRebirth( bool bFromRebirth ) { m_bFromRebirth = bFromRebirth; };

protected:
	virtual void OnSetParentSkillInfo();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
