#pragma once
#include "dnblow.h"

class CDnTransformMonsterBlow : public CDnBlow, public TBoostMemoryPool< CDnTransformMonsterBlow >
{

protected:
	int m_nTransformActorID;
	int m_nOriginalActorID;

#if defined(_GAMESERVER)
	int m_nTransformSkillID;
	bool m_bOrderUseSkill;
#endif

public:
	CDnTransformMonsterBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnTransformMonsterBlow(void);
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

};
