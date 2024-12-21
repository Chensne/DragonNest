#pragma once
#include "dnblow.h"


class CDnStealBuffBlow : public CDnBlow, public TBoostMemoryPool< CDnStealBuffBlow >
{
public:
	CDnStealBuffBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnStealBuffBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	void FindSkillBlowList(DnActorHandle hActor, int skillID, DNVector(DnBlowHandle)& vlBlowList);
	void StealBuffProcess();

	void StealBuffProcess(LOCAL_TIME LocalTime);
	void StealBuffFromActor( DnActorHandle hTargetActor );
#endif // _GAMESERVER

protected:
	int m_nCount;
	int m_nNewDelayTime;
	float m_fScanRange;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
