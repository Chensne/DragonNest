#pragma once
#include "dnblow.h"



class CDnCoolTimeChangeBlow : public CDnBlow, public TBoostMemoryPool< CDnCoolTimeChangeBlow >
{
private:
	int m_nSkillType;		//액티브/패시브
	float m_fCoolTimeRate;	//쿨타임 비율

	std::map<int, float> m_OrigSkillCoolTimeAdjustValues;

public:
	CDnCoolTimeChangeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCoolTimeChangeBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void ResetCoolTime();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
