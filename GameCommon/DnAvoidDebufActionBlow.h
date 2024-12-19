#pragma once
#include "dnblow.h"

// 이슈 #51048
// 빙결, 감전 등의 상태효과에서 액션이 끊기지 않도록 하는 상태효과.
// 현재 네스트 네임드 급 이상의 몬스터일 경우 그렇게 처리 되지만 이 상태효과를 
// 갖고 있는 actor 도 동일하게 처리된다.
class CDnAvoidDebufActionBlow : public CDnBlow,
						 public TBoostMemoryPool< CDnAvoidDebufActionBlow >
{
private:

public:
	CDnAvoidDebufActionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAvoidDebufActionBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
};
