#pragma once
#include "dnblow.h"



class CDnShieldBlow : public CDnBlow, public TBoostMemoryPool< CDnShieldBlow >
{
private:
	int m_iDurability;				// 내구도. 모두 소진되면 상태효과가 풀리게 된다.
	bool m_bCalcDamage;				// OnCalcDamage에서 데미지 보정 계산을 할지 말지 여부용 Flag
public:
	CDnShieldBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnShieldBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
	float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
