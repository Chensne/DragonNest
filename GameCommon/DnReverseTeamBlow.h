#pragma once
#include "dnblow.h"

// 팀 반전 상태효과
//효과 인덱스: 86
//효과 수치: 수치와 관련 없음
//효과 설명:
//효과시간 동안 강제로 팀속성을 반전시킨다.
//아군일 경우 적군으로, 적군일 경우 아군으로
class CDnReverseTeamBlow : public CDnBlow
{
public:
	// #13885 이슈 관련. 히트 시그널 반전 타입이면 모든 히트 시그널의 타겟이 반전됨.
	// 해당 효과에 걸린 대상은 사용하는 발사체와 액션의 모든 hit시그널의 TargetType과 DamageType이 반전된다.
	// Enemy -> Friend
	// Friend -> Enemy
	// all 일 경우에는 변동되지 않는다.
	// 세인트후반에 등장하는 하피 블랙(1197)이 사용할 예정입니다.
	enum
	{
		HIT_SIGNAL_TARGET_CHANGE,
		TEAM_NUMBER_CHANGE,
		HIT_SIGNAL_TARGET_ALL,			// 적 아군 구분없이 모두 타격됨. (#24337)
	};

private:
	int m_iType;


public:
	CDnReverseTeamBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnReverseTeamBlow(void);

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
