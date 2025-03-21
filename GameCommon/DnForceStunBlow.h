#pragma once
#include "dnblow.h"

// 이슈 #12200
// 스턴확률관련 스탯한계치(스턴의경우 35%)때문에 [강제스턴]스킬을 스킬효과로 설정할수가 없습니다.(그래서 지금 있는 스턴스킬들 다 병신;;;)
// 액션툴에서 강제스턴으로 설정하자니 보스몬스터의 경우에도 무조건 스턴에 걸리게되어 너무 좋은스킬이 됩니다. 따라서 아래와 같이 처리해야할것 같습니다.
// 시전자의 스턴확률 능력치에 상관없이 효과수치에 설정된 확률로 대상을 [스턴]상태로 만드는 스킬효과를 추가해야합니다.
class CDnForceStunBlow : public CDnBlow,
						 public TBoostMemoryPool< CDnForceStunBlow >
{
private:

public:
	CDnForceStunBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnForceStunBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
