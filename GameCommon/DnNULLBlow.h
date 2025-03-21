#pragma once
#include "dnblow.h"


// 이펙트만 나오게 하는 아무것도 하지 않는 상태효과.
class CDnNULLBlow : public CDnBlow, public TBoostMemoryPool< CDnNULLBlow >
{
protected:
#ifdef _CLIENT
	bool m_bUseCommonEffect;
	std::string m_strCommonEffectName;
#endif
	
public:
	CDnNULLBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnNULLBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
};
