#pragma once
#include "dnblow.h"

// 공격자 HP 절대
// 효과 인덱스: 91
// 효과 수치: Int
// 효과 내용:
// 효과시간 (효과시간이 0일 경우에는 해당 액션이 실행되는 동안) 시전자를 공격한 대상의 현재 HP를 수치만큼 증/감 시킨다.
// 효과 수치가 [+]면 증가, [-]면 감소된다.
// 공격자의 공격력, 데미지 등에 관련없이 입력된 수치만큼만 영향을 준다.
class CDnAttackerHPAbsolute : public CDnBlow
{
private:
	

public:
	CDnAttackerHPAbsolute( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAttackerHPAbsolute(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

protected:
	virtual void OnSetParentSkillInfo();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
