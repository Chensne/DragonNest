#pragma once
#include "dnblow.h"

class CDnAddStateBySkillGroupBlow : public CDnBlow, public TBoostMemoryPool< CDnAddStateBySkillGroupBlow >
{
protected: 

#ifdef _GAMESERVER
	bool m_bStateEffectApplied;
	int m_nServerBlowID;
	int m_nStateBlowIndex;
	std::string m_szAdditionalBlowParam;
	std::vector<int> m_vecAvailableSkillIndex;
#endif

public:
	CDnAddStateBySkillGroupBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddStateBySkillGroupBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	void ApplyAddtionalStateBlowFromSkill( int nSkillIndex );
	void RemoveAdditionalStateBlow();
#endif

};