#pragma once
#include "dnblow.h"


//효과 Index : 56
//
//수치 Type : float
//
//설정 : <효과 시간>동안 <효과 수치>만큼 패링 확률 증가
//
//	 사용 방식 : 특정 스텐스로 변경이나 버프 또는 패시브 스킬을 통해 패링 확률 상승


class CDnParryingProbBlow : public CDnBlow, public TBoostMemoryPool< CDnParryingProbBlow >
{
private:
	

public:
	CDnParryingProbBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnParryingProbBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
