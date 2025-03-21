#pragma once
#include "dnblow.h"

// 팀 반전 상태효과
//효과 인덱스: 86
//효과 수치: 수치와 관련 없음
//효과 설명:
//효과시간 동안 강제로 팀속성을 반전시킨다.
//아군일 경우 적군으로, 적군일 경우 아군으로
class CDnAggroResetBlow : public CDnBlow
{
public:
	

private:
	int m_iType;


public:
	CDnAggroResetBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAggroResetBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	int GetType( void ) { return m_iType; };

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
