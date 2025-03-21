#pragma once
#include "dnblow.h"


// 효과 Index : 78
//
// 수치 Type : int
//
// 설정 : <효과 시간>동안 MP 소모 스킬 사용 불가
//
// 사용 방식 : 해당 속성이 포함된 스킬에 피격된 상대방은 <효과 시간>동안 MP 소모 스킬을 사용 못하는 디버프를 받습니다.

class CDnCantUseMPUsingSkillBlow : public CDnBlow, 
								   public TBoostMemoryPool< CDnCantUseMPUsingSkillBlow >
{
private:
	

public:
	CDnCantUseMPUsingSkillBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCantUseMPUsingSkillBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
