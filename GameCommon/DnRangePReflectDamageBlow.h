#pragma once
#include "dnblow.h"

// 수치 Type : float
//
// 효과 Index 
// - 물리레인지 데미지 반사 : 112
//
// - 물리레인지 데미지 반사 : <효과 시간>동안 효과를 지닌 대상을 물리&레인지 타입으로 공격한 상대는 <공격한 상대의 물리 공격력>*<효과 수치>만큼의 물리 데미지를 입는다. 

class CDnRangePReflectDamageBlow : public CDnBlow, 
								   public TBoostMemoryPool< CDnRangePReflectDamageBlow >
{
private:
#ifdef _GAMESERVER
	float m_fReflectRatio;
	float m_fReflectRatioMax;
#endif // #ifdef _GAMESERVER

public:
	CDnRangePReflectDamageBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnRangePReflectDamageBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual bool OnDefenseAttack( DnActorHandle hActor, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
