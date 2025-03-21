#pragma once
#include "dnblow.h"

// #32426 이슈 관련. 내가 소환한 몬스터들에게 특정 스킬을 사용하라고 하는 
// OrderMySummonedMonster 시그널과 연동되는 상태효과.
// 몬스터가 사용할 스킬/스킬레벨 정보를 갖고 있다.
// 시그널에 다 담지 않은 이유는 스킬 레벨별로 액션을 따로 만들어야 해서 편의성 문제로
// 기획팀에서 요청했기 때문.
// 게임서버에서만 쓰인다.
class CDnOrderMySummonedMonsterBlow : public CDnBlow, public TBoostMemoryPool< CDnOrderMySummonedMonsterBlow >
{
private:
	int m_iSkillID;

public:
	CDnOrderMySummonedMonsterBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnOrderMySummonedMonsterBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	int GetSkillID( void ) { return m_iSkillID; };

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif

};
