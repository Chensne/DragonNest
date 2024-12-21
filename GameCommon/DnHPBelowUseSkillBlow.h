#pragma once
#include "dnblow.h"


// 스킬 효과 ID : 281
// 특정HP 이하가 되면 정해진 스킬을 발동 함

#ifdef PRE_ADD_HPBELOWDOSKILLBLOW

class CDnHPBelowUseSkillBlow : public CDnBlow, public TBoostMemoryPool< CDnHPBelowUseSkillBlow >
{
public:
	CDnHPBelowUseSkillBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnHPBelowUseSkillBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	float CalcDamage( float fOriginalDamage );
	float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

private:
	INT64 m_nHP;
	int m_nSkillIndex;
};

#endif // PRE_ADD_HPBELOWDOSKILLBLOW