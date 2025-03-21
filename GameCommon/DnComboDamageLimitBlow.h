#pragma once
#include "dnblow.h"


class CDnComboDamageLimitBlow : public CDnBlow, public TBoostMemoryPool< CDnComboDamageLimitBlow >
{
public:
	CDnComboDamageLimitBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnComboDamageLimitBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
	void UpdateHitCount(CDnSkill::SkillInfo* pSkillInfo, CDnDamageBase::SHitParam& HitParam);
#endif // _GAMESERVER

	void SetSkillStartTime(LOCAL_TIME startTime) { m_SkillStartTime = startTime; }
	LOCAL_TIME GetSkillStartTime() { return m_SkillStartTime; }

protected:
	int m_nHitLimitCount;	//Hit제한 수
	float m_fDamageRate;	//데미지 제한 수치
	bool m_bActivated;		//Hit제한 횟수가 다 되었을 경우 활성화 시킴.

	LOCAL_TIME m_SkillStartTime;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
