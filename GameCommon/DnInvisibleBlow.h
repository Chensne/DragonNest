#pragma once
#include "dnblow.h"

// 투명화 상태효과
// 효과 인덱스: 73
// 효과 수치: 수치와 관련 없음
// 효과 내용:
// 효과시간 동안 시전자의 모습을 투명화(알파값 100%) 시킨다.
// 효과시간이 지나거나 버프를 삭제하는 효과에 맞으면 투명화 효과가 사라진다.
// 투명화 상태에서 이동과 Stand 외의 행동(공격, 스킬사용, 아이템사용)을 하면 그 액션 동안은 모습이 투명하게 보이는 상태(알파값 50%정도)로 보이고 액션이 끝나면 다시 투명화가 된다.
// 투명화 상태에서 공격을 받을 경우에도 공격할 때와 같이 모습이 투명하게 보였다가 경직모션이 끝나면 다시 투명화가 된다.
// 투명화 상태에서 예외로 [디버프]효과에 걸렸을 경우 해당 디버프에 대한 [상태이상 이펙트]는 투명해지지 않고 계속 유지된다. (그외의 상태이상 이펙트는 모두 같이 투명해 진다.)
// 투명화 상태 중에는 [그림자]가 출력되지 않는다.
// 투명화 상태라도 관련 Sound는 모두 출력된다.
// 시전자가 죽으면 모든 버프/디버프가 사라지므로 투명화효과도 사라진다.
class CDnInvisibleBlow : public CDnBlow,
						 public TBoostMemoryPool<CDnInvisibleBlow>
{
private:
#ifndef _GAMESERVER
	float m_fDestAlpha;
	float m_fNowAlpha;
	bool m_bEnded;
#endif

public:
	CDnInvisibleBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnInvisibleBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
