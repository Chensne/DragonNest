#pragma once
#include "dnblow.h"

// 셋팅된 상태효과가 면역 됨. 게임서버에서만 쓰임.. 
// 면역되 상태효과는 DnActor::CmdAddStateEffect 에서 체크해서 패킷 쏘지 않음..
class CDnImmuneBlow : public CDnBlow, public TBoostMemoryPool< CDnImmuneBlow >
{
private:
	float m_fResistPercent;

	float m_fReduceTimeValue;	//지속시간 감소 비율 값(0 ~ 1)

public:
	CDnImmuneBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnImmuneBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

	bool IsImmuned( const CDnSkill::SkillInfo* pParentSkillInfo, STATE_BLOW::emBLOW_INDEX BlowIndex );

	bool IsImmuned( STATE_BLOW::emBLOW_INDEX BlowIndex );

	float GetReduceTimeValue() { return m_fReduceTimeValue; }

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
