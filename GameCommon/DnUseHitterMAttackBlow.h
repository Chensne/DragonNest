#pragma once
#include "dnblow.h"

//
//수치 Type : float
//
//효과 Index
//- 피격자 마법 공격력 데미지 적용 : 110
//
//설정 : 시전자의 물리 or 마법 공격력이 아닌 실제 <효과를 받는 대상>의 물리 or 마법 공격력을 이용한 공격 컨셉
//	 - 피격자 마법 공격력 데미지 적용 : <효과가 적용되는 대상의 마법 공격력>*<효과수치>+<시전자의 마법공격력> 만큼의 마법공격력으로 마법데미지를 입힘
//
//	 사용 방식
//	 - 해당 스킬에 피격된 상대방의 물리/마법 공격력에 따라 데미지 상승

class CDnUseHitterMAttackBlow : public CDnBlow, 
								public TBoostMemoryPool< CDnUseHitterMAttackBlow >
{
private:
	

public:
	CDnUseHitterMAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnUseHitterMAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
